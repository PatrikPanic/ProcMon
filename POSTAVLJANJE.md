# Upute za postavljanje projekta

Ova datoteka služi samo tebi tijekom sastavljanja projekta — ne mora ići u repozitorij.

## 1. Novi projekt

*Create a new project → MFC App*, naziv projekta **točno `ProcMon`** (o nazivu ovise
imena generiranih klasa i oznake resursa `IDR_ProcMonTYPE`).

Postavke u čarobnjaku:

| Stranica | Postavka |
|---|---|
| Application Type | **Multiple documents**, Document/View architecture support **uključeno** |
| Project Style | **Office** |
| Visual Style | **Office 2007 (Blue theme)** |
| Use of MFC | Use MFC in a shared DLL |
| User Interface Features | isključi *Toolbar* (zamjenjuje ga Ribbon), ostavi *Status bar* |
| Advanced Features | isključi *Printing and print preview*, *ActiveX controls*, *Common Control Manifest* ostavi uključen |
| Generated Classes | ostaviti kako jest |

## 2. Uklanjanje nepotrebne klase

Iz projekta i s diska obriši `ProcMonView.h` i `ProcMonView.cpp` — zamjenjuje ih
`ProcessView`.

## 3. Kopiranje datoteka

Kopiraj u mapu projekta (uz `ProcMon.vcxproj`):

```
Commands.h            SysUtil.h / SysUtil.cpp
StringIDs.h           ProcessCollector.h / .cpp
ProcMon.h / .cpp      ThreadCollector.h / .cpp
MainFrm.h / .cpp      ModuleCollector.h / .cpp
ProcMonDoc.h / .cpp   ProcessView.h / .cpp
README.md             ThreadView.h / .cpp
.gitignore            ModuleView.h / .cpp
```

`ProcMon.h/.cpp`, `MainFrm.h/.cpp` i `ProcMonDoc.h/.cpp` prepisuju datoteke koje je
napravio čarobnjak — to je u redu.

Datoteku `res\ProcMon.rc2` prepiši onom iz mape `res`.

U *Solution Exploreru* zatim *Add → Existing Item…* i dodaj sve nove `.h` i `.cpp`
datoteke (one koje su prepisane već su u projektu).

## 4. Provjera resursa

- Otvori `ProcMon.rc` desnim klikom → *View Code* i provjeri da u odjeljku
  `TEXTINCLUDE 3` stoji `#include "res\ProcMon.rc2"` (čarobnjak to postavlja sam).
- U *String Table* promijeni `IDR_MAINFRAME` u `Process & Memory Viewer` — to je
  naslov glavnog prozora.
- Resurs `IDR_RIBBON` koji je napravio čarobnjak ostaje neiskorišten jer se Ribbon
  gradi u kodu. Možeš ga obrisati ili ostaviti.

## 5. Postavke projekta

- Platforma: **x64** (32-bitni program ne može čitati module 64-bitnih procesa).
- *Properties → Linker → Manifest File → UAC Execution Level* →
  **requireAdministrator**.
- Ako se pojavi upozorenje C4819 zbog kodne stranice, provjeri da su izvorne
  datoteke spremljene kao UTF-8 (*File → Advanced Save Options*). Komentari su
  namjerno pisani bez dijakritike, a svi tekstovi s dijakritikom su u `res\ProcMon.rc2`.

## 6. Prevođenje i pokretanje

Nakon prevođenja aplikacija traži ovlasti administratora. Ako se pokrene bez njih,
prikazat će obavijest da će podaci o sistemskim procesima biti nepotpuni.

## 7. Repozitorij

```
git init
git add .
git commit -m "Process & Memory Viewer - seminarski rad"
```

Prije prve objave provjeri da `git status` ne prijavljuje `.obj`, `.exe`, `.pdb`,
`.aps` ni mapu `x64` — `.gitignore` ih pokriva.
