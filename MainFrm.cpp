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

    // Gumb za zatvaranje kartice je iskljucen jer su sve tri kartice stalno
    // otvorene.
    CMDITabInfo tabInfo;
    tabInfo.m_style                  = CMFCTabCtrl::STYLE_3D_ONENOTE;
    tabInfo.m_bActiveTabCloseButton  = FALSE;
    tabInfo.m_bTabIcons              = FALSE;
    tabInfo.m_bAutoColor             = TRUE;
    tabInfo.m_bDocumentMenu          = FALSE;
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

    // Mjerac vremena drzi glavni okvir, a ne pojedini pogled, pa osvjezavanje
    // radi neovisno o tome koja je kartica prikazana.
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

    // Kategorija bez slikovnih resursa, gumbi su tekstualni.
    CMFCRibbonCategory* pCategory =
        m_wndRibbonBar.AddCategory(CSysUtil::LoadStr(IDS_RIBBON_CATEGORY), 0, 0);

    CMFCRibbonPanel* pPanelRefresh = pCategory->AddPanel(CSysUtil::LoadStr(IDS_PANEL_REFRESH));
    pPanelRefresh->Add(new CMFCRibbonButton(ID_CMD_REFRESH,
                                            CSysUtil::LoadStr(IDS_CMD_REFRESH), -1, -1));
    pPanelRefresh->Add(new CMFCRibbonCheckBox(ID_CMD_AUTOREFRESH,
                                              CSysUtil::LoadStr(IDS_CMD_AUTOREFRESH)));

    CMFCRibbonPanel* pPanelProcess = pCategory->AddPanel(CSysUtil::LoadStr(IDS_PANEL_PROCESS));
    pPanelProcess->Add(new CMFCRibbonButton(ID_CMD_KILL_PROCESS,
                                            CSysUtil::LoadStr(IDS_CMD_KILL), -1, -1));

    CMFCRibbonPanel* pPanelView = pCategory->AddPanel(CSysUtil::LoadStr(IDS_PANEL_VIEW));
    pPanelView->Add(new CMFCRibbonCheckBox(ID_CMD_TREE,
                                           CSysUtil::LoadStr(IDS_CMD_TREE)));

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
    // Sve tri kartice dijele isti dokument, pa je dovoljno pitati aktivnu.
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
