#include "pch.h"
#include "ProcessCollector.h"
#include "SysUtil.h"

#include <tlhelp32.h>
#include <psapi.h>

#pragma comment(lib, "Psapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CProcessCollector::CProcessCollector()
    : m_previousTick(0), m_processorCount(1)
{
    SYSTEM_INFO si = {};
    GetSystemInfo(&si);
    if (si.dwNumberOfProcessors > 0)
        m_processorCount = si.dwNumberOfProcessors;
}

void CProcessCollector::Refresh()
{
    const ULONGLONG currentTick = GetTickCount64();
    const ULONGLONG elapsedMs   = (m_previousTick == 0) ? 0 : (currentTick - m_previousTick);

    std::vector<CProcessInfo>  items;
    std::map<DWORD, ULONGLONG> cpuTimes;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return;

    PROCESSENTRY32 pe = {};
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe))
    {
        do
        {
            CProcessInfo info;
            info.pid         = pe.th32ProcessID;
            info.parentPid   = pe.th32ParentProcessID;
            info.threadCount = pe.cntThreads;
            info.name        = pe.szExeFile;

            const ULONGLONG cpuTime = ReadProcessDetails(info);
            cpuTimes[info.pid] = cpuTime;

            // Iskoristenje procesora = potroseno procesorsko vrijeme podijeljeno
            // s proteklim stvarnim vremenom i brojem jezgri.
            std::map<DWORD, ULONGLONG>::const_iterator it = m_previousCpuTimes.find(info.pid);
            if (it != m_previousCpuTimes.end() && elapsedMs > 0 && cpuTime >= it->second)
            {
                const double busyMs = (cpuTime - it->second) / 10000.0;
                double percent = busyMs / (static_cast<double>(elapsedMs) * m_processorCount) * 100.0;

                if (percent < 0.0)   percent = 0.0;
                if (percent > 100.0) percent = 100.0;

                info.cpuPercent = percent;
            }

            items.push_back(info);
        }
        while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);

    m_items.swap(items);
    m_previousCpuTimes.swap(cpuTimes);
    m_previousTick = currentTick;
}

ULONGLONG CProcessCollector::ReadProcessDetails(CProcessInfo& info) const
{
    // PROCESS_QUERY_LIMITED_INFORMATION je dovoljan za putanju, memoriju i
    // vremena, a uspijeva na vise procesa nego PROCESS_QUERY_INFORMATION.
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, info.pid);
    if (hProcess == nullptr)
        return 0;

    info.accessible = true;
    ULONGLONG cpuTime = 0;

    TCHAR szPath[MAX_PATH] = {};
    DWORD cchPath = MAX_PATH;
    if (QueryFullProcessImageName(hProcess, 0, szPath, &cchPath))
        info.path = szPath;

    PROCESS_MEMORY_COUNTERS_EX pmc = {};
    pmc.cb = sizeof(pmc);
    if (GetProcessMemoryInfo(hProcess, reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)))
    {
        info.workingSet   = pmc.WorkingSetSize;
        info.privateBytes = pmc.PrivateUsage;
    }

    FILETIME ftCreation = {}, ftExit = {}, ftKernel = {}, ftUser = {};
    if (GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser))
    {
        info.creationTime = ftCreation;
        cpuTime = CSysUtil::ToUInt64(ftKernel) + CSysUtil::ToUInt64(ftUser);
    }

    CloseHandle(hProcess);
    return cpuTime;
}

const CProcessInfo* CProcessCollector::Find(DWORD pid) const
{
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        if (m_items[i].pid == pid)
            return &m_items[i];
    }
    return nullptr;
}

void CProcessCollector::Remove(DWORD pid)
{
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        if (m_items[i].pid == pid)
        {
            m_items.erase(m_items.begin() + i);
            break;
        }
    }

    m_previousCpuTimes.erase(pid);
}
