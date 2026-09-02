#pragma once

#ifndef __AFXWIN_H__
    #error "ukljucite 'pch.h' prije ove datoteke"
#endif

#include "resource.h"

// CProcMonApp - klasa aplikacije. Stvara tri predloska dokumenta: jedan za
// popis procesa (registriran kao glavni) te po jedan za prikaz dretvi i modula.
// Sve tri kartice otvaraju se pri pokretanju, nad istim dokumentom.
class CProcMonApp : public CWinAppEx
{
public:
    CProcMonApp();

    virtual BOOL InitInstance();
    virtual int  ExitInstance();

protected:
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()

private:
    // Otvara dodatnu karticu s pogledom iz zadanog predloska, nad vec
    // postojecim dokumentom.
    void CreateAdditionalView(CMultiDocTemplate* pTemplate, CDocument* pDoc);

    CMultiDocTemplate* m_pProcessTemplate;
    CMultiDocTemplate* m_pThreadTemplate;
    CMultiDocTemplate* m_pModuleTemplate;
};

extern CProcMonApp theApp;
