#pragma once

// CSysUtil - pomocne staticke metode koje koriste ostali dijelovi programa.
// Sve je smjesteno u klasu kako u programu ne bi bilo globalnih funkcija.
class CSysUtil
{
public:
    // Podize ovlast SeDebugPrivilege za trenutni proces. Bez nje se ne moze
    // otvoriti veci dio sistemskih procesa. Vraca true ako je ovlast dobivena.
    static bool EnableDebugPrivilege();

    static CString LoadStr(UINT nID);

    // Pretvara broj bajtova u citljiv oblik, npr. "128,4 MB".
    static CString FormatBytes(ULONGLONG bytes);

    static CString FormatTimeStamp(const FILETIME& ft);

    // Pretvara trajanje izrazeno u jedinicama od 100 ns u oblik "h:mm:ss.mmm".
    static CString FormatDuration(ULONGLONG time100ns);

    // Pretvara oznaku greske sustava (GetLastError) u citljiv opis.
    static CString FormatSystemError(DWORD dwError);

    // Spaja dva dijela FILETIME strukture u jedan 64-bitni broj.
    static ULONGLONG ToUInt64(const FILETIME& ft);
};
