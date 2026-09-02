# Process & Memory Viewer

Seminarski rad (NWP projekt) – aplikacija za pregled procesa, dretvi i učitanih modula
u operacijskom sustavu Windows, s osvježavanjem podataka u stvarnom vremenu.

## Funkcionalnost

- popis svih aktivnih procesa: PID, naziv, iskorištenje procesora, radni skup,
  privatna memorija, broj dretvi i puna putanja izvršne datoteke
- sortiranje popisa klikom na zaglavlje stupca i filtriranje po nazivu procesa
- pregled dretvi odabranog procesa: identifikator dretve, osnovni prioritet,
  vrijeme provedeno u jezgri i u korisničkom načinu rada, vrijeme nastanka
- pregled učitanih modula i DLL-ova odabranog procesa: naziv, bazna adresa,
  veličina i putanja
- osvježavanje podataka svake sekunde, uz mogućnost isključivanja
- prekid odabranog procesa uz potvrdu korisnika

## Zahtjevi

- Windows 10 ili noviji, 64-bitni
- Visual Studio 2022 s instaliranom komponentom *C++ MFC for latest v143 build tools (x86 & x64)*

## Prevođenje

1. Otvoriti `ProcMon.sln` u razvojnom okruženju Visual Studio 2022.
2. Odabrati konfiguraciju `Debug` ili `Release` i platformu `x64`.
3. Pokrenuti *Build → Build Solution*.

Aplikacija u manifestu traži ovlasti administratora, pa se pri pokretanju
prikazuje upit sustava za kontrolu korisničkih računa (UAC). Bez tih ovlasti
aplikacija se pokreće, ali podaci o sistemskim procesima ostaju nepotpuni.

## Korištenje

Nakon pokretanja otvara se prozor s popisom svih procesa koji se osvježava
svake sekunde.

- **Osvježi** – ručno očitavanje trenutnog stanja sustava.
- **Automatski (1 s)** – uključuje ili isključuje osvježavanje u stvarnom vremenu.
  Isključivanje je korisno kad se popis pregledava ili sortira.
- **Naziv** – u polje se upisuje dio naziva procesa; prikazuju se samo procesi
  čiji naziv sadrži upisani tekst.
- Klik na zaglavlje stupca sortira popis po tom stupcu; ponovni klik na isti
  stupac obrće redoslijed.
- Odabirom procesa u popisu i pritiskom na **Dretve** ili **Moduli** otvara se
  novi prozor s pripadajućim podacima. Ti se prozori automatski osvježavaju
  zajedno s glavnim popisom i prate promjenu odabranog procesa.
- **Prekini proces** prekida izvođenje odabranog procesa nakon potvrde.

Svi prozori prikazuju se kao kartice unutar glavnog prozora aplikacije.

## Struktura projekta

| Datoteka | Opis |
|---|---|
| `ProcessCollector.h/.cpp` | očitavanje popisa procesa i izračun iskorištenja procesora |
| `ThreadCollector.h/.cpp` | očitavanje dretvi zadanog procesa |
| `ModuleCollector.h/.cpp` | očitavanje modula zadanog procesa |
| `SysUtil.h/.cpp` | pomoćne metode (ovlasti, oblikovanje brojeva i vremena) |
| `ProcMonDoc.h/.cpp` | dokument: podaci, filtriranje, sortiranje i obrada naredbi |
| `ProcessView.h/.cpp` | pogled s popisom procesa i mjeračem vremena |
| `ThreadView.h/.cpp` | pogled s popisom dretvi |
| `ModuleView.h/.cpp` | pogled s popisom modula |
| `MainFrm.h/.cpp` | glavni okvir, Ribbon traka i statusna traka |
| `ProcMon.h/.cpp` | klasa aplikacije i predlošci dokumenata |
| `StringIDs.h`, `res\ProcMon.rc2` | identifikatori i tekstovi sučelja u resursima |
| `Commands.h` | oznake naredbi i obavijesti prema pogledima |

Sloj za dohvat podataka (`*Collector`) ne ovisi o sučelju, pa se isti podaci
mogu prikazati u bilo kojem pogledu.
