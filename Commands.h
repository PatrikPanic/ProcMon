#pragma once

// Vlastite oznake naredbi (Ribbon gumbi).
// Namjerno su u rasponu 33000+ da se ne sudare s onima koje generira
// Visual Studio u resource.h (_APS_NEXT_COMMAND_VALUE obicno krece od 32771).

#define ID_CMD_REFRESH          33000   // rucno osvjezavanje
#define ID_CMD_AUTOREFRESH      33001   // ukljuci/iskljuci automatsko osvjezavanje
#define ID_CMD_SHOW_THREADS     33002   // otvori prozor s threadovima
#define ID_CMD_SHOW_MODULES     33003   // otvori prozor s modulima
#define ID_CMD_KILL_PROCESS     33004   // prekini oznaceni proces
#define ID_CMD_TREE             33005   // prikaz stabla umjesto ravnog popisa
#define ID_CMD_FILTER           33006   // polje za filtriranje po nazivu
#define ID_CMD_STATUS_PANE      33010   // polje statusne trake

// Oznake (hintovi) koje dokument salje pogledima kroz UpdateAllViews
#define HINT_PROCESSES          1       // osvjezena lista procesa
#define HINT_SELECTION          2       // promijenjen odabrani proces
