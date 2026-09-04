#include "pch.h"
#include "ThreadCollector.h"
#include "SysUtil.h"

#include <tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CThreadCollector::Refresh(DWORD pid)
{
    m_items.clear();

    if (pid == 0)
        return;

    // Snimka dretvi uvijek obuhvaca cijeli sustav, pa se rezultat filtrira
    // po identifikatoru procesa vlasnika.
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return;

    THREADENTRY32 te = {};
    te.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hSnapshot, &te))
    {
        do
        {
            if (te.th32OwnerProcessID != pid)
                continue;

            CThreadInfo info;
            info.tid          = te.th32ThreadID;
            info.ownerPid     = te.th32OwnerProcessID;
            info.basePriority = te.tpBasePri;

            ReadThreadTimes(info);
            m_items.push_back(info);
        }
        while (Thread32Next(hSnapshot, &te));
    }

    CloseHandle(hSnapshot);
}

void CThreadCollector::ReadThreadTimes(CThreadInfo& info) const
{
    HANDLE hThread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, info.tid);
    if (hThread == nullptr)
        return;

    info.accessible = true;

    FILETIME ftCreation = {}, ftExit = {}, ftKernel = {}, ftUser = {};
    if (GetThreadTimes(hThread, &ftCreation, &ftExit, &ftKernel, &ftUser))
    {
        info.creationTime = ftCreation;
        info.kernelTime   = CSysUtil::ToUInt64(ftKernel);
        info.userTime     = CSysUtil::ToUInt64(ftUser);
    }

    CloseHandle(hThread);
}
