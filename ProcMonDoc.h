#pragma once

#include "ProcessCollector.h"
#include "ThreadCollector.h"
#include "ModuleCollector.h"

#include <set>

// Jedan redak u prikazu. Osim podataka o procesu nosi i podatke potrebne za
// crtanje stabla: dubinu u hijerarhiji, ima li proces djecu i je li rasiren.
struct CProcessRow
{
    CProcessInfo info;
    int  depth       = 0;
    bool hasChildren = false;
    bool expanded    = true;
};

// CProcMonDoc - dokument u arhitekturi dokument/pogled. Sadrzi sve podatke,
// postavke prikaza i obradu naredbi s Ribbon trake. Sva tri pogleda rade nad
// istim dokumentom.
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
        colCount
    };

    const std::vector<CProcessRow>&  GetVisibleProcesses() const { return m_visible; }
    const std::vector<CThreadInfo>&  GetThreads() const { return m_threads.GetAll(); }
    const std::vector<CModuleInfo>&  GetModules() const { return m_modules.GetAll(); }

    bool AreModulesAccessible() const { return m_modules.IsAccessible(); }

    DWORD   GetSelectedPid() const { return m_selectedPid; }
    CString GetSelectedProcessName() const;
    bool    IsAutoRefresh() const { return m_bAutoRefresh; }
    bool    IsTreeMode() const { return m_bTreeMode; }

    // Ocitava stanje sustava i obavjescuje poglede.
    void RefreshData();

    void SetSelectedPid(DWORD pid);
    void SetFilter(const CString& filter);

    // Ponovni klik na isti stupac obrce redoslijed.
    void SortByColumn(int column);

    void ToggleExpand(DWORD pid);

    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

protected:
    afx_msg void OnRefresh();
    afx_msg void OnAutoRefresh();
    afx_msg void OnUpdateAutoRefresh(CCmdUI* pCmdUI);
    afx_msg void OnTreeMode();
    afx_msg void OnUpdateTreeMode(CCmdUI* pCmdUI);
    afx_msg void OnKillProcess();
    afx_msg void OnUpdateNeedsSelection(CCmdUI* pCmdUI);

    DECLARE_MESSAGE_MAP()

private:
    // Iz svih ocitanih procesa gradi popis pripremljen za prikaz.
    void BuildVisibleList();
    void BuildFlatList();
    void BuildTreeList();
    void AddSubtree(size_t parentIndex, int depth, std::set<DWORD>& visited);

    bool CompareProcesses(const CProcessInfo& left, const CProcessInfo& right) const;
    bool IsLess(const CProcessInfo& left, const CProcessInfo& right) const;
    bool MatchesFilter(const CProcessInfo& info) const;
    bool IsRealParent(const CProcessInfo& child) const;

    // Ocitava dretve i module odabranog procesa.
    void RefreshDetails();

    void UpdateStatusBar();
    void ReportKillError(const CString& name, DWORD pid, DWORD dwError) const;
    void CollectDescendants(DWORD pid, std::vector<DWORD>& result) const;
    bool TerminateOne(DWORD pid, DWORD& dwError) const;

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
