#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "ProcMon.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "ProcMonDoc.h"
#include "ProcessView.h"
#include "ThreadView.h"
#include "ModuleView.h"
#include "SysUtil.h"
#include "StringIDs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CProcMonApp, CWinAppEx)
    ON_COMMAND(ID_APP_ABOUT, &CProcMonApp::OnAppAbout)
END_MESSAGE_MAP()

// Jedini primjerak aplikacije; MFC zahtijeva da bude globalan.
CProcMonApp theApp;

CProcMonApp::CProcMonApp()
    : m_pProcessTemplate(NULL), m_pThreadTemplate(NULL), m_pModuleTemplate(NULL)
{
    SetAppID(_T("Vsite.ProcMon.1"));
}

BOOL CProcMonApp::InitInstance()
{
    INITCOMMONCONTROLSEX initCtrls = {};
    initCtrls.dwSize = sizeof(initCtrls);
    initCtrls.dwICC  = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&initCtrls);

    CWinAppEx::InitInstance();

    EnableTaskbarInteraction(FALSE);

    SetRegistryKey(_T("Vsite"));

    InitContextMenuManager();
    InitKeyboardManager();
    InitTooltipManager();

    CMFCToolTipInfo params;
    params.m_bVislManagerTheme = TRUE;
    GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
                                          RUNTIME_CLASS(CMFCToolTipCtrl), &params);

    // Bez ovlasti SeDebugPrivilege vidljiv je samo dio podataka o procesima.
    if (!CSysUtil::EnableDebugPrivilege())
        AfxMessageBox(CSysUtil::LoadStr(IDS_WARN_NO_DEBUG_PRIV), MB_OK | MB_ICONINFORMATION);

    // Glavni predlozak: dokument s podacima i pogled s popisom procesa.
    m_pProcessTemplate = new CMultiDocTemplate(
        IDR_ProcMonTYPE,
        RUNTIME_CLASS(CProcMonDoc),
        RUNTIME_CLASS(CChildFrame),
        RUNTIME_CLASS(CProcessView));
    if (m_pProcessTemplate == NULL)
        return FALSE;
    AddDocTemplate(m_pProcessTemplate);

    // Pomocni predlosci se namjerno ne dodaju u popis predlozaka jer se iz njih
    // ne otvaraju novi dokumenti, nego samo dodatni pogledi na postojeci.
    m_pThreadTemplate = new CMultiDocTemplate(
        IDR_ProcMonTYPE,
        RUNTIME_CLASS(CProcMonDoc),
        RUNTIME_CLASS(CChildFrame),
        RUNTIME_CLASS(CThreadView));

    m_pModuleTemplate = new CMultiDocTemplate(
        IDR_ProcMonTYPE,
        RUNTIME_CLASS(CProcMonDoc),
        RUNTIME_CLASS(CChildFrame),
        RUNTIME_CLASS(CModuleView));

    CMainFrame* pMainFrame = new CMainFrame;
    if (pMainFrame == NULL || !pMainFrame->LoadFrame(IDR_MAINFRAME))
    {
        delete pMainFrame;
        return FALSE;
    }
    m_pMainWnd = pMainFrame;

    // Aplikacija ne otvara datoteke, nego odmah prikazuje stanje sustava.
    m_pProcessTemplate->OpenDocumentFile(NULL);

    pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
    pMainFrame->UpdateWindow();

    return TRUE;
}

int CProcMonApp::ExitInstance()
{
    // Predlosci koji nisu dodani u popis moraju se osloboditi rucno.
    delete m_pThreadTemplate;
    delete m_pModuleTemplate;

    m_pThreadTemplate = NULL;
    m_pModuleTemplate = NULL;

    return CWinAppEx::ExitInstance();
}

// ---------------------------------------------------------------------------
// Dijalog "O programu"
// ---------------------------------------------------------------------------

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}

    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX) { CDialogEx::DoDataExchange(pDX); }
    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

void CProcMonApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}
