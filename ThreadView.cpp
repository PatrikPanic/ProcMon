#include "pch.h"
#include "framework.h"
#include "ProcMon.h"
#include "ProcMonDoc.h"
#include "ThreadView.h"
#include "SysUtil.h"
#include "Commands.h"
#include "StringIDs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CThreadView, CListView)

BEGIN_MESSAGE_MAP(CThreadView, CListView)
END_MESSAGE_MAP()

CThreadView::CThreadView()
{
}

CThreadView::~CThreadView()
{
}

CProcMonDoc* CThreadView::GetDocument() const
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CProcMonDoc)));
    return (CProcMonDoc*)m_pDocument;
}

BOOL CThreadView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;
    return CListView::PreCreateWindow(cs);
}

void CThreadView::OnInitialUpdate()
{
    CListView::OnInitialUpdate();

    GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |
                                   LVS_EX_DOUBLEBUFFER);
    InsertColumns();
    FillList();
}

void CThreadView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
    // Popis se osvjezava i kod promjene odabira i kod redovnog osvjezavanja
    // podataka, jer se dretve procesa stalno stvaraju i zavrsavaju.
    FillList();
}

void CThreadView::InsertColumns()
{
    CListCtrl& list = GetListCtrl();

    list.InsertColumn(colTid,        CSysUtil::LoadStr(IDS_COL_TID),        LVCFMT_RIGHT,  90);
    list.InsertColumn(colPriority,   CSysUtil::LoadStr(IDS_COL_PRIORITY),   LVCFMT_RIGHT,  80);
    list.InsertColumn(colKernelTime, CSysUtil::LoadStr(IDS_COL_KERNELTIME), LVCFMT_RIGHT, 120);
    list.InsertColumn(colUserTime,   CSysUtil::LoadStr(IDS_COL_USERTIME),   LVCFMT_RIGHT, 120);
    list.InsertColumn(colCreated,    CSysUtil::LoadStr(IDS_COL_CREATED),    LVCFMT_LEFT,  150);
}

void CThreadView::FillList()
{
    CListCtrl& list = GetListCtrl();
    CProcMonDoc* pDoc = GetDocument();

    const std::vector<CThreadInfo>& items = pDoc->GetThreads();
    const int topIndex = list.GetTopIndex();

    // Odabir se izricito ponistava prije praznjenja popisa, inace obojeni redak
    // ostaje nacrtan i nakon brisanja stavke.
    list.SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);
    list.DeleteAllItems();

    // Kartica je otvorena i prije nego je proces odabran, pa se umjesto praznog
    // popisa ispisuje uputa.
    if (pDoc->GetSelectedPid() == 0)
    {
        list.InsertItem(0, CSysUtil::LoadStr(IDS_THREADS_NO_SELECTION));
        return;
    }

    CString text;

    for (size_t i = 0; i < items.size(); ++i)
    {
        const CThreadInfo& info = items[i];

        text.Format(_T("%u"), info.tid);
        const int index = list.InsertItem(static_cast<int>(i), text);
        if (index < 0)
            continue;

        list.SetItemData(index, info.tid);

        text.Format(_T("%ld"), info.basePriority);
        list.SetItemText(index, colPriority, text);

        if (info.accessible)
        {
            list.SetItemText(index, colKernelTime, CSysUtil::FormatDuration(info.kernelTime));
            list.SetItemText(index, colUserTime,   CSysUtil::FormatDuration(info.userTime));
            list.SetItemText(index, colCreated,    CSysUtil::FormatTimeStamp(info.creationTime));
        }
        else
        {
            const CString notAvailable = CSysUtil::LoadStr(IDS_NOT_AVAILABLE);
            list.SetItemText(index, colKernelTime, notAvailable);
            list.SetItemText(index, colUserTime,   notAvailable);
            list.SetItemText(index, colCreated,    notAvailable);
        }
    }

    if (topIndex > 0 && topIndex < list.GetItemCount())
        list.EnsureVisible(topIndex, FALSE);

    list.RedrawWindow(nullptr, nullptr,
                      RDW_INVALIDATE | RDW_ERASE | RDW_FRAME |
                      RDW_ALLCHILDREN | RDW_UPDATENOW);
}
