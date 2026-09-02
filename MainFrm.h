#pragma once

class CProcMonDoc;

//
// CMainFrame - glavni MDI okvir aplikacije.
// Gradi Ribbon traku i statusnu traku te prosljeduje sadrzaj polja za
// filtriranje dokumentu.
//
class CMainFrame : public CMDIFrameWndEx
{
    DECLARE_DYNAMIC(CMainFrame)

public:
    CMainFrame();

    // Postavlja tekst u statusnu traku (poziva ga dokument nakon osvjezavanja).
    void SetStatusText(LPCTSTR lpszText);

protected:
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnFilterChanged();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();

    DECLARE_MESSAGE_MAP()

private:
    // Identifikator je namjerno velik broj. Standardne kontrole sustava koriste
    // vlastite mjerace vremena s malim identifikatorima, pa bi mjerac postavljen
    // na prozor kontrole mogao doci u sukob s njima.
    enum { timerRefresh = 1001, refreshIntervalMs = 1000 };

    void CreateRibbon();
    CProcMonDoc* GetInspectorDoc() const;

    CMFCRibbonBar       m_wndRibbonBar;
    CMFCRibbonStatusBar m_wndStatusBar;

public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
};
