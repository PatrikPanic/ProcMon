#include "pch.h"
#include "framework.h"
#include "ProcMon.h"
#include "ProcMonDoc.h"
#include "MainFrm.h"
#include "SysUtil.h"
#include "Commands.h"
#include "StringIDs.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CProcMonDoc, CDocument)

BEGIN_MESSAGE_MAP(CProcMonDoc, CDocument)
    ON_COMMAND(ID_CMD_REFRESH, &CProcMonDoc::OnRefresh)
    ON_COMMAND(ID_CMD_AUTOREFRESH, &CProcMonDoc::OnAutoRefresh)
    ON_UPDATE_COMMAND_UI(ID_CMD_AUTOREFRESH, &CProcMonDoc::OnUpdateAutoRefresh)
    ON_COMMAND(ID_CMD_TREE, &CProcMonDoc::OnTreeMode)
    ON_UPDATE_COMMAND_UI(ID_CMD_TREE, &CProcMonDoc::OnUpdateTreeMode)
    ON_COMMAND(ID_CMD_SHOW_THREADS, &CProcMonDoc::OnShowThreads)
    ON_UPDATE_COMMAND_UI(ID_CMD_SHOW_THREADS, &CProcMonDoc::OnUpdateNeedsSelection)
    ON_COMMAND(ID_CMD_SHOW_MODULES, &CProcMonDoc::OnShowModules)
    ON_UPDATE_COMMAND_UI(ID_CMD_SHOW_MODULES, &CProcMonDoc::OnUpdateNeedsSelection)
    ON_COMMAND(ID_CMD_KILL_PROCESS, &CProcMonDoc::OnKillProcess)
    ON_UPDATE_COMMAND_UI(ID_CMD_KILL_PROCESS, &CProcMonDoc::OnUpdateNeedsSelection)
END_MESSAGE_MAP()

CProcMonDoc::CProcMonDoc()
    : m_selectedPid(0),
      m_sortColumn(colCpu),
      m_bSortAscending(false),      // najzahtjevniji procesi na vrhu
      m_bAutoRefresh(true),
      m_bTreeMode(true)
{
}

CProcMonDoc::~CProcMonDoc()
{
}

BOOL CProcMonDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    SetTitle(CSysUtil::LoadStr(IDS_TITLE_PROCESSES));
    RefreshData();

    return TRUE;
}

void CProcMonDoc::Serialize(CArchive& /*ar*/)
{
    // Aplikacija prikazuje trenutno stanje sustava i nema sadrzaj koji bi se
    // spremao u datoteku, pa je serijalizacija namjerno prazna.
}

// ---------------------------------------------------------------------------
// Ocitavanje podataka
// ---------------------------------------------------------------------------

void CProcMonDoc::RefreshData()
{
    m_processes.Refresh();

    // Ako je odabrani proces u meduvremenu zavrsio, odabir se ponistava.
    if (m_selectedPid != 0 && m_processes.Find(m_selectedPid) == NULL)
        m_selectedPid = 0;

    BuildVisibleList();
    RefreshDetails();
    UpdateStatusBar();

    UpdateAllViews(NULL, HINT_PROCESSES);
}

void CProcMonDoc::RefreshDetails()
{
    m_threads.Refresh(m_selectedPid);
    m_modules.Refresh(m_selectedPid);
}

// ---------------------------------------------------------------------------
// Gradnja popisa za prikaz
// ---------------------------------------------------------------------------

void CProcMonDoc::BuildVisibleList()
{
    m_visible.clear();
    m_visible.reserve(m_processes.GetAll().size());

    // Dok je filtar upisan, prikazuje se ravan popis pogodaka. Stablo tada nema
    // smisla jer bi roditelji pogodaka morali biti prikazani iako ne odgovaraju
    // uvjetu filtriranja.
    if (m_bTreeMode && m_filter.IsEmpty())
        BuildTreeList();
    else
        BuildFlatList();
}

void CProcMonDoc::BuildFlatList()
{
    const std::vector<CProcessInfo>& all = m_processes.GetAll();

    for (size_t i = 0; i < all.size(); ++i)
    {
        if (!MatchesFilter(all[i]))
            continue;

        CProcessRow row;
        row.info = all[i];
        m_visible.push_back(row);
    }

    std::sort(m_visible.begin(), m_visible.end(),
              [this](const CProcessRow& left, const CProcessRow& right)
              {
                  return CompareProcesses(left.info, right.info);
              });
}

void CProcMonDoc::BuildTreeList()
{
    // Krece se od procesa koji nemaju vidljivog roditelja, a zatim se rekurzivno
    // dodaju njihova djeca. Skup obidenih procesa sprjecava beskonacnu rekurziju
    // u slucaju neocekivanih podataka.
    const std::vector<CProcessInfo>& all = m_processes.GetAll();
    std::set<DWORD> visited;

    std::vector<const CProcessInfo*> roots;
    for (size_t i = 0; i < all.size(); ++i)
    {
        if (!IsRealParent(all[i]))
            roots.push_back(&all[i]);
    }

    std::sort(roots.begin(), roots.end(),
              [this](const CProcessInfo* left, const CProcessInfo* right)
              {
                  return CompareProcesses(*left, *right);
              });

    for (size_t i = 0; i < roots.size(); ++i)
    {
        if (visited.find(roots[i]->pid) != visited.end())
            continue;

        visited.insert(roots[i]->pid);

        CProcessRow row;
        row.info     = *roots[i];
        row.depth    = 0;
        row.expanded = (m_collapsed.find(roots[i]->pid) == m_collapsed.end());

        const size_t index = m_visible.size();
        m_visible.push_back(row);

        AddSubtree(index, 1, visited);
    }
}

void CProcMonDoc::AddSubtree(size_t parentIndex, int depth, std::set<DWORD>& visited)
{
    const DWORD parentPid = m_visible[parentIndex].info.pid;
    const std::vector<CProcessInfo>& all = m_processes.GetAll();

    // Djeca zadanog procesa
    std::vector<const CProcessInfo*> children;
    for (size_t i = 0; i < all.size(); ++i)
    {
        if (all[i].parentPid == parentPid && all[i].pid != parentPid &&
            IsRealParent(all[i]) && visited.find(all[i].pid) == visited.end())
        {
            children.push_back(&all[i]);
        }
    }

    if (children.empty())
        return;

    // Cvor ima djecu bez obzira na to jesu li trenutno prikazana, jer o tome
    // ovisi hoce li se ispred naziva nacrtati oznaka za sklapanje.
    m_visible[parentIndex].hasChildren = true;

    if (!m_visible[parentIndex].expanded)
        return;     // sklopljen cvor: djeca se ne prikazuju

    // Djeca se sortiraju po istom pravilu kao i ostatak popisa.
    std::sort(children.begin(), children.end(),
              [this](const CProcessInfo* left, const CProcessInfo* right)
              {
                  return CompareProcesses(*left, *right);
              });

    for (size_t i = 0; i < children.size(); ++i)
    {
        visited.insert(children[i]->pid);

        CProcessRow row;
        row.info     = *children[i];
        row.depth    = depth;
        row.expanded = (m_collapsed.find(children[i]->pid) == m_collapsed.end());

        const size_t index = m_visible.size();
        m_visible.push_back(row);

        AddSubtree(index, depth + 1, visited);
    }
}

bool CProcMonDoc::IsRealParent(const CProcessInfo& child) const
{
    // Proces bez roditelja ili s roditeljem koji vise ne postoji prikazuje se
    // kao korijen stabla.
    if (child.parentPid == 0 || child.parentPid == child.pid)
        return false;

    const CProcessInfo* pParent = m_processes.Find(child.parentPid);
    if (pParent == NULL)
        return false;

    // Windows ponovno dodjeljuje identifikatore zavrsenih procesa, pa se moze
    // dogoditi da "roditelj" bude noviji od svojeg "djeteta". Takva veza nije
    // stvarna i takav se proces prikazuje kao korijen.
    const ULONGLONG parentTime = CSysUtil::ToUInt64(pParent->creationTime);
    const ULONGLONG childTime  = CSysUtil::ToUInt64(child.creationTime);

    if (parentTime != 0 && childTime != 0 && parentTime > childTime)
        return false;

    return true;
}

bool CProcMonDoc::MatchesFilter(const CProcessInfo& info) const
{
    if (m_filter.IsEmpty())
        return true;

    CString name = info.name;
    CString filter = m_filter;

    name.MakeLower();
    filter.MakeLower();

    return (name.Find(filter) >= 0);
}

// ---------------------------------------------------------------------------
// Sortiranje
// ---------------------------------------------------------------------------

bool CProcMonDoc::CompareProcesses(const CProcessInfo& left, const CProcessInfo& right) const
{
    // Silazni redoslijed dobiva se zamjenom argumenata, a ne negacijom
    // rezultata: negacija bi kod jednakih vrijednosti dala da je istovremeno
    // left < right i right < left, sto nije dopusteno.
    return m_bSortAscending ? IsLess(left, right) : IsLess(right, left);
}

bool CProcMonDoc::IsLess(const CProcessInfo& left, const CProcessInfo& right) const
{
    switch (m_sortColumn)
    {
    case colName:
        {
            const int result = left.name.CompareNoCase(right.name);
            if (result != 0)
                return result < 0;
        }
        break;

    case colCpu:
        if (left.cpuPercent != right.cpuPercent)
            return left.cpuPercent < right.cpuPercent;
        break;

    case colWorkingSet:
        if (left.workingSet != right.workingSet)
            return left.workingSet < right.workingSet;
        break;

    case colPrivate:
        if (left.privateBytes != right.privateBytes)
            return left.privateBytes < right.privateBytes;
        break;

    case colThreadCount:
        if (left.threadCount != right.threadCount)
            return left.threadCount < right.threadCount;
        break;

    case colPath:
        {
            const int result = left.path.CompareNoCase(right.path);
            if (result != 0)
                return result < 0;
        }
        break;

    case colPid:
    default:
        break;
    }

    // Procesi s jednakom vrijednoscu u odabranom stupcu razvrstavaju se po
    // PID-u, cime je redoslijed jednoznacan i popis ne poskakuje pri
    // osvjezavanju.
    return left.pid < right.pid;
}

// ---------------------------------------------------------------------------
// Postavke prikaza
// ---------------------------------------------------------------------------

void CProcMonDoc::SetSelectedPid(DWORD pid)
{
    if (m_selectedPid == pid)
        return;

    m_selectedPid = pid;
    RefreshDetails();

    UpdateAllViews(NULL, HINT_SELECTION);
}

void CProcMonDoc::SetFilter(const CString& filter)
{
    if (m_filter == filter)
        return;

    m_filter = filter;
    BuildVisibleList();
    UpdateStatusBar();

    UpdateAllViews(NULL, HINT_PROCESSES);
}

void CProcMonDoc::SortByColumn(int column)
{
    if (column < 0 || column >= colCount)
        return;

    if (m_sortColumn == column)
    {
        m_bSortAscending = !m_bSortAscending;
    }
    else
    {
        m_sortColumn = column;
        // Tekstualni stupci se prirodno citaju uzlazno, brojcani silazno.
        m_bSortAscending = (column == colName || column == colPath || column == colPid);
    }

    BuildVisibleList();
    UpdateAllViews(NULL, HINT_PROCESSES);
}

void CProcMonDoc::ToggleExpand(DWORD pid)
{
    if (pid == 0)
        return;

    std::set<DWORD>::iterator it = m_collapsed.find(pid);
    if (it == m_collapsed.end())
        m_collapsed.insert(pid);    // sklopi
    else
        m_collapsed.erase(it);      // rasiri

    BuildVisibleList();
    UpdateAllViews(NULL, HINT_PROCESSES);
}

CString CProcMonDoc::GetSelectedProcessName() const
{
    const CProcessInfo* pInfo = m_processes.Find(m_selectedPid);
    return (pInfo != NULL) ? pInfo->name : CString();
}

// ---------------------------------------------------------------------------
// Naredbe s Ribbon trake
// ---------------------------------------------------------------------------

void CProcMonDoc::OnRefresh()
{
    RefreshData();
}

void CProcMonDoc::OnAutoRefresh()
{
    m_bAutoRefresh = !m_bAutoRefresh;
}

void CProcMonDoc::OnUpdateAutoRefresh(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bAutoRefresh ? 1 : 0);
}

void CProcMonDoc::OnTreeMode()
{
    m_bTreeMode = !m_bTreeMode;

    BuildVisibleList();
    UpdateAllViews(NULL, HINT_PROCESSES);
}

void CProcMonDoc::OnUpdateTreeMode(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_bTreeMode ? 1 : 0);
}

void CProcMonDoc::OnUpdateNeedsSelection(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_selectedPid != 0);
}

void CProcMonDoc::OnShowThreads()
{
    OpenAdditionalView(theApp.GetThreadTemplate());
}

void CProcMonDoc::OnShowModules()
{
    OpenAdditionalView(theApp.GetModuleTemplate());
}

void CProcMonDoc::OpenAdditionalView(CMultiDocTemplate* pTemplate)
{
    if (pTemplate == NULL)
        return;

    // Nad istim dokumentom otvara se dodatni MDI prozor s drugom vrstom pogleda.
    CFrameWnd* pFrame = pTemplate->CreateNewFrame(this, NULL);
    if (pFrame != NULL)
        pTemplate->InitialUpdateFrame(pFrame, this);
}

void CProcMonDoc::OnKillProcess()
{
    const CProcessInfo* pInfo = m_processes.Find(m_selectedPid);
    if (pInfo == NULL)
    {
        AfxMessageBox(CSysUtil::LoadStr(IDS_ERR_NO_SELECTION), MB_OK | MB_ICONINFORMATION);
        return;
    }

    const CString name = pInfo->name;
    const DWORD   pid  = pInfo->pid;

    // Programi poput preglednika ili glazbenih aplikacija sastoje se od vise
    // procesa. Prekid samo glavnog procesa ostavio bi ostale i dalje pokrenute,
    // pa se prikupljaju svi potomci odabranog procesa.
    std::vector<DWORD> descendants;
    CollectDescendants(pid, descendants);

    CString message;
    if (descendants.empty())
    {
        message.Format(CSysUtil::LoadStr(IDS_CONFIRM_KILL), (LPCTSTR)name, pid);
    }
    else
    {
        message.Format(CSysUtil::LoadStr(IDS_CONFIRM_KILL_TREE),
                       (LPCTSTR)name, pid, static_cast<int>(descendants.size()));
    }

    if (AfxMessageBox(message, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2) != IDYES)
        return;

    // Potomci se prekidaju od najdubljih prema gore, kako roditelj ne bi stigao
    // pokrenuti zamjenu za dijete koje je upravo prekinuto.
    for (size_t i = descendants.size(); i > 0; --i)
    {
        DWORD dwChildError = ERROR_SUCCESS;
        if (TerminateOne(descendants[i - 1], dwChildError))
            m_processes.Remove(descendants[i - 1]);
    }

    // Neuspjeh se prijavljuje samo za odabrani proces. Pojedini potomak moze
    // zavrsiti sam od sebe cim mu roditelj nestane, pa to nije greska.
    DWORD dwError = ERROR_SUCCESS;
    if (!TerminateOne(pid, dwError))
    {
        ReportKillError(name, pid, dwError);
        RefreshData();
        return;
    }

    // TerminateProcess samo zatrazi prekid i odmah se vraca, pa bi procesi u
    // sljedecoj snimci sustava jos uvijek bili vidljivi. Umjesto cekanja, koje
    // bi zaustavilo sucelje, uklanjaju se odmah iz ocitanog popisa. Ako prekid
    // ipak ne bi uspio, redak ce se vratiti pri sljedecem ocitanju i prikaz ce
    // opet odgovarati stvarnom stanju.
    m_processes.Remove(pid);
    m_selectedPid = 0;

    BuildVisibleList();
    RefreshDetails();
    UpdateStatusBar();

    UpdateAllViews(NULL, HINT_PROCESSES);
}

void CProcMonDoc::CollectDescendants(DWORD pid, std::vector<DWORD>& result) const
{
    const std::vector<CProcessInfo>& all = m_processes.GetAll();

    for (size_t i = 0; i < all.size(); ++i)
    {
        if (all[i].parentPid != pid || all[i].pid == pid)
            continue;

        // Provjera stvarnog roditeljstva sprjecava da se zbog ponovno
        // dodijeljenog identifikatora prekine posve nepovezan proces.
        if (!IsRealParent(all[i]))
            continue;

        // Zastita od ponavljanja u slucaju neocekivanih podataka.
        bool bAlreadyListed = false;
        for (size_t j = 0; j < result.size(); ++j)
        {
            if (result[j] == all[i].pid)
            {
                bAlreadyListed = true;
                break;
            }
        }

        if (bAlreadyListed)
            continue;

        result.push_back(all[i].pid);
        CollectDescendants(all[i].pid, result);
    }
}

bool CProcMonDoc::TerminateOne(DWORD pid, DWORD& dwError) const
{
    dwError = ERROR_SUCCESS;

    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL)
    {
        dwError = GetLastError();
        return false;
    }

    const BOOL bTerminated = TerminateProcess(hProcess, 0);
    if (!bTerminated)
        dwError = GetLastError();

    CloseHandle(hProcess);
    return (bTerminated != FALSE);
}

void CProcMonDoc::ReportKillError(const CString& name, DWORD pid, DWORD dwError) const
{
    CString message;
    message.Format(CSysUtil::LoadStr(IDS_ERR_KILL_FAILED),
                   (LPCTSTR)name, pid, dwError,
                   (LPCTSTR)CSysUtil::FormatSystemError(dwError));

    AfxMessageBox(message, MB_OK | MB_ICONEXCLAMATION);
}

// ---------------------------------------------------------------------------
// Statusna traka
// ---------------------------------------------------------------------------

void CProcMonDoc::UpdateStatusBar()
{
    ULONGLONG totalWorkingSet = 0;
    const std::vector<CProcessInfo>& all = m_processes.GetAll();

    for (size_t i = 0; i < all.size(); ++i)
        totalWorkingSet += all[i].workingSet;

    CString text;
    text.Format(CSysUtil::LoadStr(IDS_STATUS_FORMAT),
                static_cast<int>(all.size()),
                (LPCTSTR)CSysUtil::FormatBytes(totalWorkingSet),
                (LPCTSTR)CTime::GetCurrentTime().Format(_T("%H:%M:%S")));

    CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
    if (pFrame != NULL)
        pFrame->SetStatusText(text);
}

// ---------------------------------------------------------------------------
// Dijagnostika
// ---------------------------------------------------------------------------

#ifdef _DEBUG
void CProcMonDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CProcMonDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif
