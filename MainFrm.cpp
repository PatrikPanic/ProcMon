#include "pch.h"
#include "framework.h"
#include "ProcMon.h"
#include "MainFrm.h"
#include "ProcMonDoc.h"
#include "SysUtil.h"
#include "Commands.h"
#include "StringIDs.h"

#include "afxvisualmanageroffice2007.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
    ON_WM_CREATE()
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_COMMAND(ID_CMD_FILTER, &CMainFrame::OnFilterChanged)
END_MESSAGE_MAP()

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Djeca MDI okvira prikazuju se kao kartice.
    // Krizic se prikazuje na aktivnoj kartici. Zatvaranje prozora s popisom
    // procesa svejedno nije moguce jer ga odbija CChildFrame::OnClose.
    CMDITabInfo tabInfo;
    tabInfo.m_style                  = CMFCTabCtrl::STYLE_3D_ONENOTE;
    tabInfo.m_bActiveTabCloseButton  = TRUE;
    tabInfo.m_bTabIcons              = FALSE;
    tabInfo.m_bAutoColor             = TRUE;
    tabInfo.m_bDocumentMenu          = TRUE;
    EnableMDITabbedGroups(TRUE, tabInfo);

    CreateRibbon();

    if (!m_wndStatusBar.Create(this))
        return -1;

    m_wndStatusBar.AddElement(
        new CMFCRibbonStatusBarPane(ID_CMD_STATUS_PANE,
                                    CSysUtil::LoadStr(IDS_STATUS_READY), TRUE),
        _T(""));

    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
    CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);

    // Mjerac vremena za osvjezavanje u stvarnom vremenu drzi glavni okvir, a ne
    // pojedini pogled, pa osvjezavanje radi neovisno o tome koji su prozori
    // otvoreni.
    SetTimer(timerRefresh, refreshIntervalMs, NULL);

    return 0;
}

void CMainFrame::OnDestroy()
{
    KillTimer(timerRefresh);
    CMDIFrameWndEx::OnDestroy();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == timerRefresh)
    {
        CProcMonDoc* pDoc = GetInspectorDoc();
        if (pDoc != NULL && pDoc->IsAutoRefresh())
            pDoc->RefreshData();
    }

    CMDIFrameWndEx::OnTimer(nIDEvent);
}

void CMainFrame::CreateRibbon()
{
    m_wndRibbonBar.Create(this);
    m_wndRibbonBar.SetWindows7Look(FALSE);

    // Kategorija bez slikovnih resursa - gumbi su tekstualni.
    CMFCRibbonCategory* pCategory =
        m_wndRibbonBar.AddCategory(CSysUtil::LoadStr(IDS_RIBBON_CATEGORY), 0, 0);

    // Ploca s naredbama za osvjezavanje
    CMFCRibbonPanel* pPanelRefresh = pCategory->AddPanel(CSysUtil::LoadStr(IDS_PANEL_REFRESH));
    pPanelRefresh->Add(new CMFCRibbonButton(ID_CMD_REFRESH,
                                            CSysUtil::LoadStr(IDS_CMD_REFRESH), -1, -1));
    pPanelRefresh->Add(new CMFCRibbonCheckBox(ID_CMD_AUTOREFRESH,
                                              CSysUtil::LoadStr(IDS_CMD_AUTOREFRESH)));

    // Ploca s naredbama nad odabranim procesom
    CMFCRibbonPanel* pPanelDetails = pCategory->AddPanel(CSysUtil::LoadStr(IDS_PANEL_DETAILS));
    pPanelDetails->Add(new CMFCRibbonButton(ID_CMD_SHOW_THREADS,
                                            CSysUtil::LoadStr(IDS_CMD_THREADS), -1, -1));
    pPanelDetails->Add(new CMFCRibbonButton(ID_CMD_SHOW_MODULES,
                                            CSysUtil::LoadStr(IDS_CMD_MODULES), -1, -1));
    pPanelDetails->Add(new CMFCRibbonButton(ID_CMD_KILL_PROCESS,
                                            CSysUtil::LoadStr(IDS_CMD_KILL), -1, -1));

    // Ploca s postavkama prikaza
    CMFCRibbonPanel* pPanelView = pCategory->AddPanel(CSysUtil::LoadStr(IDS_PANEL_VIEW));
    pPanelView->Add(new CMFCRibbonCheckBox(ID_CMD_TREE,
                                           CSysUtil::LoadStr(IDS_CMD_TREE)));

    // Ploca s poljem za filtriranje popisa procesa
    CMFCRibbonPanel* pPanelFilter = pCategory->AddPanel(CSysUtil::LoadStr(IDS_PANEL_FILTER));
    pPanelFilter->Add(new CMFCRibbonEdit(ID_CMD_FILTER, 120,
                                         CSysUtil::LoadStr(IDS_FILTER_LABEL)));
}

void CMainFrame::OnFilterChanged()
{
    CMFCRibbonEdit* pEdit =
        DYNAMIC_DOWNCAST(CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_CMD_FILTER));
    if (pEdit == NULL)
        return;

    CProcMonDoc* pDoc = GetInspectorDoc();
    if (pDoc != NULL)
        pDoc->SetFilter(pEdit->GetEditText());
}

void CMainFrame::SetStatusText(LPCTSTR lpszText)
{
    CMFCRibbonStatusBarPane* pPane =
        DYNAMIC_DOWNCAST(CMFCRibbonStatusBarPane, m_wndStatusBar.FindElement(ID_CMD_STATUS_PANE));

    if (pPane != NULL)
    {
        pPane->SetText(lpszText);
        m_wndStatusBar.Invalidate();
        m_wndStatusBar.UpdateWindow();
    }
}

CProcMonDoc* CMainFrame::GetInspectorDoc() const
{
    // Svi MDI prozori dijele isti dokument, pa je dovoljno pitati aktivni.
    CMDIChildWnd* pChild = const_cast<CMainFrame*>(this)->MDIGetActive();
    if (pChild == NULL)
        return NULL;

    return DYNAMIC_DOWNCAST(CProcMonDoc, pChild->GetActiveDocument());
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CMDIFrameWndEx::Dump(dc);
}
#endif
