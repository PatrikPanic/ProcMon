#pragma once

#include <afxcview.h>

class CProcMonDoc;

// CThreadView - pogled s popisom dretvi odabranog procesa.
class CThreadView : public CListView
{
    DECLARE_DYNCREATE(CThreadView)

protected:
    CThreadView();

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
        colTid = 0,
        colPriority,
        colKernelTime,
        colUserTime,
        colCreated
    };

    void InsertColumns();
    void FillList();

public:
    virtual ~CThreadView();
};
