#pragma once

#include <vector>

// Podaci o jednoj dretvi.
struct CThreadInfo
{
    DWORD     tid           = 0;
    DWORD     ownerPid      = 0;
    LONG      basePriority  = 0;
    ULONGLONG kernelTime    = 0;    // u jedinicama od 100 ns
    ULONGLONG userTime      = 0;
    FILETIME  creationTime  = {};
    bool      accessible    = false;
};

// CThreadCollector - dohvaca popis dretvi zadanog procesa.
class CThreadCollector
{
public:
    // Ako je pid jednak nuli, popis se samo prazni.
    void Refresh(DWORD pid);

    const std::vector<CThreadInfo>& GetAll() const { return m_items; }

private:
    // Dopunjava podatke koji zahtijevaju otvaranje dretve.
    void ReadThreadTimes(CThreadInfo& info) const;

    std::vector<CThreadInfo> m_items;
};
