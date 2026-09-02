#pragma once

#include <vector>

//
// Podaci o jednom ucitanom modulu (izvrsna datoteka ili DLL).
//
struct CModuleInfo
{
    ULONGLONG baseAddress = 0;
    DWORD     size        = 0;      // velicina u bajtovima
    CString   name;
    CString   path;
};

//
// CModuleCollector - dohvaca popis modula ucitanih u zadani proces.
//
class CModuleCollector
{
public:
    // Ocitava module procesa s zadanim identifikatorom.
    // Ako je pid jednak nuli, popis se samo prazni.
    void Refresh(DWORD pid);

    const std::vector<CModuleInfo>& GetAll() const { return m_items; }

    // Je li zadnje ocitanje uspjelo (kod nedostatka ovlasti ne uspije).
    bool IsAccessible() const { return m_accessible; }

private:
    std::vector<CModuleInfo> m_items;
    bool                     m_accessible = false;
};
