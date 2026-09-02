#include "pch.h"
#include "framework.h"
#include "ProcMon.h"
#include "ChildFrm.h"
#include "ProcessView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CMDIChildWndEx::PreCreateWindow(cs))
        return FALSE;

    return TRUE;
}

void CChildFrame::ActivateFrame(int nCmdShow)
{
    CMDIChildWndEx::ActivateFrame(nCmdShow);

    // Iz izbornika prozora uklanja se naredba za zatvaranje kako korisniku ne
    // bi bila ponudena mogucnost koja ionako nece biti izvrsena. Pogled je
    // dostupan tek nakon sto je okvir aktiviran, pa se provjera radi ovdje.
    if (IsMainProcessWindow())
    {
        CMenu* pSysMenu = GetSystemMenu(FALSE);
        if (pSysMenu != NULL)
        {
            pSysMenu->RemoveMenu(SC_CLOSE, MF_BYCOMMAND);
            pSysMenu->RemoveMenu(SC_MOVE, MF_BYCOMMAND);
        }
    }
}

void CChildFrame::OnClose()
{
    // Popis procesa je sredisnji dio aplikacije. Kad bi se zatvorio, zatvorio bi
    // se i dokument sa svim podacima, pa se zahtjev za zatvaranjem zanemaruje.
    if (IsMainProcessWindow())
        return;

    CMDIChildWndEx::OnClose();
}

bool CChildFrame::IsMainProcessWindow() const
{
    CView* pView = const_cast<CChildFrame*>(this)->GetActiveView();
    if (pView == NULL)
        return false;

    return (pView->IsKindOf(RUNTIME_CLASS(CProcessView)) != 0);
}

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
    CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
    CMDIChildWndEx::Dump(dc);
}
#endif
