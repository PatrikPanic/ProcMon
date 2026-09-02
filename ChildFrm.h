#pragma once

//
// CChildFrame - okvir jednog MDI prozora.
// Prozor s popisom procesa je glavni prozor aplikacije i ne moze se zatvoriti,
// dok se prozori s dretvama i modulima zatvaraju normalno.
//
class CChildFrame : public CMDIChildWndEx
{
    DECLARE_DYNCREATE(CChildFrame)

public:
    CChildFrame();

protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void ActivateFrame(int nCmdShow);

    afx_msg void OnClose();

    DECLARE_MESSAGE_MAP()

private:
    // Vraca true ako ovaj okvir sadrzi pogled s popisom procesa.
    bool IsMainProcessWindow() const;

public:
    virtual ~CChildFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
};
