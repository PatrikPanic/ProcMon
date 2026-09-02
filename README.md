# Process & Memory Viewer

Seminarski rad (NWP projekt) – aplikacija za pregled procesa, dretvi i učitanih modula
u operacijskom sustavu Windows, s osvježavanjem podataka u stvarnom vremenu.

## Funkcionalnost

- popis svih aktivnih procesa: PID, naziv, iskorištenje procesora, radni skup,
  privatna memorija, broj dretvi i puna putanja izvršne datoteke
- prikaz procesa u obliku stabla, po procesu koji ih je pokrenuo, uz sklapanje
  i širenje pojedinog čvora
- sortiranje popisa klikom na zaglavlje stupca i filtriranje po nazivu procesa
- pregled dretvi odabranog procesa: identifikator dretve, osnovni prioritet,
  vrijeme provedeno u jezgri i u korisničkom načinu rada, vrijeme nastanka
- pregled učitanih modula i DLL-ova odabranog procesa: naziv, bazna adresa,
  veličina i putanja
- osvježavanje podataka svake sekunde, uz mogućnost isključivanja
- prekid odabranog procesa, zajedno sa svim procesima koje je pokrenuo, uz
  potvrdu korisnika

## Zahtjevi

- Windows 10 ili noviji, 64-bitni
- Visual Studio 2022 s instaliranom komponentom *C++ MFC for latest v143 build tools (x86 & x64)*

## Prevođenje

1. Otvoriti `ProcMon.sln` u razvojnom okruženju Visual Studio 2022.
2. Odabrati konfiguraciju `Debug` ili `Release`; platforma je `x64`
   (32-bitni program ne može čitati module 64-bitnih procesa, pa se ne gradi).
3. Pokrenuti *Build → Build Solution*.

Aplikacija u manifestu traži ovlasti administratora, pa se pri pokretanju
prikazuje upit sustava za kontrolu korisničkih računa (UAC). Bez tih ovlasti
aplikacija se pokreće, ali podaci o sistemskim procesima ostaju nepotpuni.

## Korištenje

Nakon pokretanja otvaraju se tri kartice: **Procesi**, **Dretve** i **Moduli**.
Kartice su stalno otvorene i ne mogu se zatvoriti, a sve tri prikazuju podatke
iz istog očitanja, koje se osvježava svake sekunde.

U kartici **Procesi**:

- klik na proces odabire ga; kartice **Dretve** i **Moduli** odmah prikazuju
  podatke tog procesa, a njegov naziv i PID ispisani su u statusnoj traci
- klik na zaglavlje stupca sortira popis po tom stupcu; ponovni klik na isti
  stupac obrće redoslijed
- oznaka `[+]` ili `[-]` ispred naziva sklapa i širi čvor stabla; isto rade
  dvostruki klik na redak te tipke `+` i `-`

Naredbe na Ribbon traci:

- **Osvježi** – ručno očitavanje trenutnog stanja sustava
- **Automatski (1 s)** – uključuje ili isključuje osvježavanje u stvarnom
  vremenu; isključivanje je korisno kad se popis pregledava ili sortira
- **Stablo procesa** – prebacuje između hijerarhijskog i ravnog popisa
- **Prekini proces** – prekida odabrani proces nakon potvrde
- **Naziv:** – u polje se upisuje dio naziva procesa; prikazuju se samo procesi
  čiji naziv sadrži upisani tekst (dok je filtar upisan, popis je ravan)

Za procese koje štiti sam operacijski sustav pojedini podaci nisu dostupni; u
popisu je tada ispisano `nedostupno`, a umjesto popisa modula prikazuje se
odgovarajuća poruka.

## Struktura projekta

| Datoteka                         | Opis                                                       |
| -------------------------------- | ---------------------------------------------------------- |
| `ProcessCollector.h/.cpp`        | očitavanje popisa procesa i izračun iskorištenja procesora |
| `ThreadCollector.h/.cpp`         | očitavanje dretvi zadanog procesa                          |
| `ModuleCollector.h/.cpp`         | očitavanje modula zadanog procesa                          |
| `SysUtil.h/.cpp`                 | pomoćne metode (ovlasti, oblikovanje brojeva i vremena)    |
| `ProcMonDoc.h/.cpp`              | dokument: podaci, filtriranje, sortiranje i obrada naredbi |
| `ProcessView.h/.cpp`             | pogled s popisom procesa i stablom                         |
| `ThreadView.h/.cpp`              | pogled s popisom dretvi                                    |
| `ModuleView.h/.cpp`              | pogled s popisom modula                                    |
| `ChildFrm.h/.cpp`                | okvir kartice: naslov kartice i zabrana zatvaranja         |
| `MainFrm.h/.cpp`                 | glavni okvir, Ribbon traka, statusna traka i mjerač vremena |
| `ProcMon.h/.cpp`                 | klasa aplikacije i predlošci dokumenata                    |
| `StringIDs.h`, `res\ProcMon.rc2` | identifikatori i tekstovi sučelja u resursima              |
| `Commands.h`                     | oznake naredbi i obavijesti prema pogledima                |

Sloj za dohvat podataka (`*Collector`) ne ovisi o sučelju, pa se isti podaci
mogu prikazati u bilo kojem pogledu.
