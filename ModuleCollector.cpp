#include "pch.h"
#include "ModuleCollector.h"

#include <tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CModuleCollector::Refresh(DWORD pid)
{
    m_items.clear();
    m_accessible = false;

    if (pid == 0)
        return;

    // TH32CS_SNAPMODULE32 je dodan da bi 64-bitni program vidio i module
    // 32-bitnih procesa koji se izvode kroz WOW64.
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return;

    m_accessible = true;

    MODULEENTRY32 me = {};
    me.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &me))
    {
        do
        {
            CModuleInfo info;
            info.baseAddress = reinterpret_cast<ULONGLONG>(me.modBaseAddr);
            info.size        = me.modBaseSize;
            info.name        = me.szModule;
            info.path        = me.szExePath;

            m_items.push_back(info);
        }
        while (Module32Next(hSnapshot, &me));
    }

    CloseHandle(hSnapshot);
}
