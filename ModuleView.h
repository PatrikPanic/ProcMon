#pragma once

#include <afxcview.h>

class CProcMonDoc;

//
// CModuleView - pogled s popisom modula (DLL-ova) odabranog procesa.
// Otvara se kao dodatni MDI prozor nad istim dokumentom.
//
class CModuleView : public CListView
{
    DECLARE_DYNCREATE(CModuleView)

protected:
    CModuleView();

public:
    CProcMonDoc* GetDocument() const;

protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

    DECLARE_MESSAGE_MAP()

private:
    enum Column
    {
        colModule = 0,
        colBaseAddress,
        colSize,
        colPath
    };

    void InsertColumns();
    void FillList();
    void UpdateFrameTitle();

public:
    virtual ~CModuleView();
};
