#pragma once

// Identifikatori stringova iz datoteke res\ProcMon.rc2. Raspon 40000+ je
// odabran da se ne sudari s onim sto generira Visual Studio u resource.h
// niti sa standardnim MFC porukama (61000+).

// Ribbon
#define IDS_RIBBON_CATEGORY        40000
#define IDS_PANEL_REFRESH          40001
#define IDS_PANEL_PROCESS          40002
#define IDS_PANEL_FILTER           40003
#define IDS_CMD_REFRESH            40004
#define IDS_CMD_AUTOREFRESH        40005
#define IDS_CMD_KILL               40006
#define IDS_FILTER_LABEL           40007
#define IDS_STATUS_READY           40008
#define IDS_PANEL_VIEW             40009
#define IDS_CMD_TREE               40010
#define IDS_CMD_ABOUT              40011

// Naslovi kartica
#define IDS_TITLE_PROCESSES        40020
#define IDS_TITLE_THREADS          40021
#define IDS_TITLE_MODULES          40022

// Stupci: procesi
#define IDS_COL_PID                40030
#define IDS_COL_NAME               40031
#define IDS_COL_CPU                40032
#define IDS_COL_WORKINGSET         40033
#define IDS_COL_PRIVATE            40034
#define IDS_COL_THREADCOUNT        40035
#define IDS_COL_PATH               40036

// Stupci: dretve
#define IDS_COL_TID                40040
#define IDS_COL_PRIORITY           40041
#define IDS_COL_KERNELTIME         40042
#define IDS_COL_USERTIME           40043
#define IDS_COL_CREATED            40044

// Stupci: moduli
#define IDS_COL_MODULE             40050
#define IDS_COL_BASEADDRESS        40051
#define IDS_COL_SIZE               40052
#define IDS_COL_MODULEPATH         40053

// Poruke i formati
#define IDS_STATUS_FORMAT          40060
#define IDS_STATUS_SELECTED        40061
#define IDS_STATUS_NO_SELECTION    40062
#define IDS_NOT_AVAILABLE          40063
#define IDS_CONFIRM_KILL           40064
#define IDS_CONFIRM_KILL_TREE      40065
#define IDS_ERR_KILL_FAILED        40066
#define IDS_ERR_NO_SELECTION       40067
#define IDS_WARN_NO_DEBUG_PRIV     40068
#define IDS_MODULES_DENIED         40069
#define IDS_THREADS_NO_SELECTION   40070
