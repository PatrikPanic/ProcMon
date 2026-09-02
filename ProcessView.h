#pragma once

#include <afxcview.h>

class CProcMonDoc;
struct CProcessRow;

//
// CProcessView - glavni pogled: popis svih procesa u sustavu.
// Kad je ukljucen prikaz stabla, uvlaka i oznaka za sklapanje ispisuju se kao
// dio teksta u stupcu s nazivom, pa nije potrebna posebna kontrola stabla.
//
class CProcessView : public CListView
{
    DECLARE_DYNCREATE(CProcessView)

protected:
    CProcessView();

public:
    CProcMonDoc* GetDocument() const;

protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

    afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    DECLARE_MESSAGE_MAP()

private:
    void    InsertColumns();
    void    FillList();
    CString BuildNameText(const CProcessRow& row) const;
    bool    IsClickOnMarker(int index, const POINT& point) const;
    void    ToggleRow(int index);

    bool m_bFilling;    // sprjecava reakciju na promjenu odabira tijekom punjenja

public:
    virtual ~CProcessView();
};
