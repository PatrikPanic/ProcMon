#pragma once

#ifndef __AFXWIN_H__
    #error "ukljucite 'pch.h' prije ove datoteke"
#endif

#include "resource.h"

//
// CProcMonApp - klasa aplikacije.
// Osim uobicajene inicijalizacije stvara tri predloska dokumenta: jedan za
// popis procesa (registriran kao glavni) te po jedan za prikaz dretvi i
// modula, koji sluze za otvaranje dodatnih MDI prozora nad istim dokumentom.
//
class CProcMonApp : public CWinAppEx
{
public:
    CProcMonApp();

    CMultiDocTemplate* GetThreadTemplate() const { return m_pThreadTemplate; }
    CMultiDocTemplate* GetModuleTemplate() const { return m_pModuleTemplate; }

    virtual BOOL InitInstance();
    virtual int  ExitInstance();

protected:
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()

private:
    CMultiDocTemplate* m_pProcessTemplate;
    CMultiDocTemplate* m_pThreadTemplate;
    CMultiDocTemplate* m_pModuleTemplate;
};

extern CProcMonApp theApp;
