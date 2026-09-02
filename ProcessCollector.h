#pragma once

#include <vector>
#include <map>

// Podaci o jednom procesu.
struct CProcessInfo
{
    DWORD     pid          = 0;
    DWORD     parentPid    = 0;
    DWORD     threadCount  = 0;
    double    cpuPercent   = 0.0;   // udio procesorskog vremena od zadnjeg osvjezavanja
    ULONGLONG workingSet   = 0;     // radni skup u bajtovima
    ULONGLONG privateBytes = 0;     // privatno zauzece u bajtovima
    FILETIME  creationTime = {};
    CString   name;                 // naziv izvrsne datoteke
    CString   path;                 // puna putanja (prazno ako nemamo ovlasti)
    bool      accessible   = false; // je li proces uopce bilo moguce otvoriti
};

// CProcessCollector - dohvaca popis svih procesa u sustavu i racuna
// iskoristenje procesora usporedbom s prethodnim ocitanjem.
class CProcessCollector
{
public:
    CProcessCollector();

    void Refresh();

    const std::vector<CProcessInfo>& GetAll() const { return m_items; }

    // Vraca NULL ako procesa nema u zadnjem ocitanju.
    const CProcessInfo* Find(DWORD pid) const;

    // Uklanja proces iz zadnjeg ocitanja. Koristi se nakon uspjesnog prekida
    // procesa, kako bi redak nestao odmah, bez cekanja sljedeceg ocitanja.
    void Remove(DWORD pid);

private:
    // Popunjava podatke koji zahtijevaju otvaranje procesa.
    // Vraca ukupno potroseno procesorsko vrijeme u jedinicama od 100 ns.
    ULONGLONG ReadProcessDetails(CProcessInfo& info) const;

    std::vector<CProcessInfo>  m_items;
    std::map<DWORD, ULONGLONG> m_previousCpuTimes;  // PID -> procesorsko vrijeme
    ULONGLONG                  m_previousTick;      // trenutak zadnjeg ocitanja
    DWORD                      m_processorCount;
};
