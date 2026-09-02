#pragma once

#include "ProcessCollector.h"
#include "ThreadCollector.h"
#include "ModuleCollector.h"

#include <set>

//
// Jedan redak u prikazu. Osim podataka o procesu nosi i podatke potrebne za
// crtanje stabla: dubinu u hijerarhiji, ima li proces djecu i je li rasiren.
//
struct CProcessRow
{
    CProcessInfo info;
    int  depth       = 0;
    bool hasChildren = false;
    bool expanded    = true;
};

//
// CProcMonDoc - dokument u arhitekturi dokument/pogled.
// Sadrzi sve podatke (procesi, dretve, moduli), postavke prikaza (filtar,
// sortiranje, stablo, automatsko osvjezavanje) i obraduje naredbe s Ribbon
// trake. Svi pogledi rade nad istim dokumentom.
//
class CProcMonDoc : public CDocument
{
protected: // dokument stvara samo okvir aplikacije
    CProcMonDoc();
    DECLARE_DYNCREATE(CProcMonDoc)

public:
    // Redoslijed stupaca u popisu procesa; koristi se i za sortiranje.
    enum ProcessColumn
    {
        colPid = 0,
        colName,
        colCpu,
        colWorkingSet,
        colPrivate,
        colThreadCount,
        colPath,
        colCount        // ukupan broj stupaca
    };

    // --- podaci za poglede ---
    const std::vector<CProcessRow>&  GetVisibleProcesses() const { return m_visible; }
    const std::vector<CThreadInfo>&  GetThreads() const { return m_threads.GetAll(); }
    const std::vector<CModuleInfo>&  GetModules() const { return m_modules.GetAll(); }

    DWORD   GetSelectedPid() const { return m_selectedPid; }
    CString GetSelectedProcessName() const;
    bool    IsAutoRefresh() const { return m_bAutoRefresh; }
    bool    IsTreeMode() const { return m_bTreeMode; }

    // --- radnje ---
    void RefreshData();                 // ocitava stanje sustava i obavjescuje poglede
    void SetSelectedPid(DWORD pid);     // mijenja odabrani proces
    void SetFilter(const CString& filter);
    void SortByColumn(int column);      // ponovni klik na isti stupac obrce redoslijed
    void ToggleExpand(DWORD pid);       // sklapa ili rasiruje cvor stabla

    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

protected:
    afx_msg void OnRefresh();
    afx_msg void OnAutoRefresh();
    afx_msg void OnUpdateAutoRefresh(CCmdUI* pCmdUI);
    afx_msg void OnTreeMode();
    afx_msg void OnUpdateTreeMode(CCmdUI* pCmdUI);
    afx_msg void OnShowThreads();
    afx_msg void OnShowModules();
    afx_msg void OnKillProcess();
    afx_msg void OnUpdateNeedsSelection(CCmdUI* pCmdUI);

    DECLARE_MESSAGE_MAP()

private:
    void BuildVisibleList();            // iz svih procesa gradi popis za prikaz
    void BuildFlatList();               // ravan popis (kad je stablo iskljuceno ili se filtrira)
    void BuildTreeList();               // hijerarhijski popis po roditeljskom procesu
    void AddSubtree(size_t parentIndex, int depth, std::set<DWORD>& visited);

    bool CompareProcesses(const CProcessInfo& left, const CProcessInfo& right) const;
    bool IsLess(const CProcessInfo& left, const CProcessInfo& right) const;
    bool MatchesFilter(const CProcessInfo& info) const;
    bool IsRealParent(const CProcessInfo& child) const;

    void RefreshDetails();              // ocitava dretve i module odabranog procesa
    void UpdateStatusBar();
    void ReportKillError(const CString& name, DWORD pid, DWORD dwError) const;
    void CollectDescendants(DWORD pid, std::vector<DWORD>& result) const;
    bool TerminateOne(DWORD pid, DWORD& dwError) const;
    void OpenAdditionalView(CMultiDocTemplate* pTemplate);

    CProcessCollector m_processes;
    CThreadCollector  m_threads;
    CModuleCollector  m_modules;

    std::vector<CProcessRow> m_visible;     // redci pripremljeni za prikaz
    std::set<DWORD>          m_collapsed;   // sklopljeni cvorovi stabla

    DWORD   m_selectedPid;
    CString m_filter;
    int     m_sortColumn;
    bool    m_bSortAscending;
    bool    m_bAutoRefresh;
    bool    m_bTreeMode;

public:
    virtual ~CProcMonDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
};
