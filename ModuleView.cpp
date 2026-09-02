#include "pch.h"
#include "framework.h"
#include "ProcMon.h"
#include "ProcMonDoc.h"
#include "ModuleView.h"
#include "SysUtil.h"
#include "Commands.h"
#include "StringIDs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CModuleView, CListView)

BEGIN_MESSAGE_MAP(CModuleView, CListView)
END_MESSAGE_MAP()

CModuleView::CModuleView()
{
}

CModuleView::~CModuleView()
{
}

CProcMonDoc* CModuleView::GetDocument() const
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProcMonDoc)));
    return (CProcMonDoc*)m_pDocument;
}

BOOL CModuleView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;
    return CListView::PreCreateWindow(cs);
}

void CModuleView::OnInitialUpdate()
{
    CListView::OnInitialUpdate();

    GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |
                                   LVS_EX_DOUBLEBUFFER);
    InsertColumns();
    FillList();
    UpdateFrameTitle();
}

void CModuleView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/)
{
    // Popis modula mijenja se rijetko, pa se osvjezava samo kod promjene
    // odabranog procesa i kod izricitog osvjezavanja podataka.
    if (lHint != HINT_SELECTION && lHint != HINT_PROCESSES)
        return;

    FillList();
    UpdateFrameTitle();
}

void CModuleView::InsertColumns()
{
    CListCtrl& list = GetListCtrl();

    list.InsertColumn(colModule,      CSysUtil::LoadStr(IDS_COL_MODULE),      LVCFMT_LEFT,  200);
    list.InsertColumn(colBaseAddress, CSysUtil::LoadStr(IDS_COL_BASEADDRESS), LVCFMT_RIGHT, 150);
    list.InsertColumn(colSize,        CSysUtil::LoadStr(IDS_COL_SIZE),        LVCFMT_RIGHT, 100);
    list.InsertColumn(colPath,        CSysUtil::LoadStr(IDS_COL_MODULEPATH),  LVCFMT_LEFT,  450);
}

void CModuleView::FillList()
{
    CListCtrl& list = GetListCtrl();
    const std::vector<CModuleInfo>& items = GetDocument()->GetModules();

    // Odabir se izricito ponistava prije praznjenja popisa, inace obojeni
    // redak ostaje nacrtan i nakon sto je stavka obrisana.
    list.SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);
    list.DeleteAllItems();

    CString text;

    for (size_t i = 0; i < items.size(); ++i)
    {
        const CModuleInfo& info = items[i];

        const int index = list.InsertItem(static_cast<int>(i), info.name);
        if (index < 0)
            continue;

        text.Format(_T("0x%016llX"), info.baseAddress);
        list.SetItemText(index, colBaseAddress, text);

        list.SetItemText(index, colSize, CSysUtil::FormatBytes(info.size));
        list.SetItemText(index, colPath, info.path);
    }

    list.RedrawWindow(NULL, NULL,
                      RDW_INVALIDATE | RDW_ERASE | RDW_FRAME |
                      RDW_ALLCHILDREN | RDW_UPDATENOW);
}

void CModuleView::UpdateFrameTitle()
{
    CProcMonDoc* pDoc = GetDocument();
    CString title;

    if (pDoc->GetSelectedPid() == 0)
    {
        title = CSysUtil::LoadStr(IDS_CMD_MODULES);
    }
    else
    {
        title.Format(CSysUtil::LoadStr(IDS_TITLE_MODULES),
                     (LPCTSTR)pDoc->GetSelectedProcessName(),
                     pDoc->GetSelectedPid());
    }

    if (GetParentFrame() != NULL)
        GetParentFrame()->SetWindowText(title);
}
