//
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2014 Fabian Greffrath
// Copyright(C) 2016-2019 Julian Nechaevsky
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//
// Parses [STRINGS] sections in BEX files
//



#include <stdio.h>
#include <string.h>

#include "deh_defs.h"
#include "deh_io.h"
#include "deh_main.h"

#include "rd_lang.h"

typedef struct {
    char *macro;
    char *string;
} bex_string_t;

// mnemonic keys table
static const bex_string_t bex_stringtable[] = {
    // part 1 - general initialization and prompts
    {"D_DEVSTR", D_DEVSTR},
    {"D_CDROM", D_CDROM},
    {"QUITMSG", QUITMSG},
    {"LOADNET", LOADNET},
    {"QLOADNET", QLOADNET},
    {"QSAVESPOT", QSAVESPOT},
    {"SAVEDEAD", SAVEDEAD},
    {"QSPROMPT", QSPROMPT},
    {"QLPROMPT", QLPROMPT},
    {"NEWGAME", NEWGAME},
    {"NIGHTMARE", NIGHTMARE},
    {"SWSTRING", SWSTRING},
    {"MSGOFF", MSGOFF},
    {"MSGON", MSGON},
    {"NETEND", NETEND},
    {"ENDGAME", ENDGAME},
    {"DETAILHI", DETAILHI},
    {"DETAILLO", DETAILLO},
    //  [JN] Should not be translated
    // {"GAMMALVL0", GAMMALVL0},
    // {"GAMMALV02", GAMMALV02},
    // {"GAMMALV05", GAMMALV05},
    // {"GAMMALV07", GAMMALV07},
    // {"GAMMALVL1", GAMMALVL1},
    // {"GAMMALV12", GAMMALV12},
    // {"GAMMALV15", GAMMALV15},
    // {"GAMMALVL2", GAMMALVL2},
    // {"GAMMALV22", GAMMALV22},
    // {"GAMMALV25", GAMMALV25},
    // {"GAMMALV27", GAMMALV27},
    // {"GAMMALVL3", GAMMALVL3},
    // {"GAMMALV32", GAMMALV32},
    // {"GAMMALV35", GAMMALV35},
    // {"GAMMALV37", GAMMALV37},
    // {"GAMMALVL4", GAMMALVL4},
    {"EMPTYSTRING", EMPTYSTRING},
    {"GGSAVED", GGSAVED},
    {"GGLOADED", GGLOADED},
    {"SAVEGAMENAME", SAVEGAMENAME},
    // part 2 - messages when the player gets things
    {"GOTARMOR", GOTARMOR},
    {"GOTMEGA", GOTMEGA},
    {"GOTHTHBONUS", GOTHTHBONUS},
    {"GOTARMBONUS", GOTARMBONUS},
    {"GOTSTIM", GOTSTIM},
    {"GOTMEDINEED", GOTMEDINEED},
    {"GOTMEDIKIT", GOTMEDIKIT},
    {"GOTSUPER", GOTSUPER},
    {"GOTBLUECARD", GOTBLUECARD},
    {"GOTYELWCARD", GOTYELWCARD},
    {"GOTREDCARD", GOTREDCARD},
    {"GOTBLUESKUL", GOTBLUESKUL},
    {"GOTYELWSKUL", GOTYELWSKUL},
    {"GOTREDSKULL", GOTREDSKULL},
    {"GOTINVUL", GOTINVUL},
    {"GOTBERSERK", GOTBERSERK},
    {"GOTINVIS", GOTINVIS},
    {"GOTSUIT", GOTSUIT},
    {"GOTMAP", GOTMAP},
    {"GOTVISOR", GOTVISOR},
    {"GOTMSPHERE", GOTMSPHERE},
    {"GOTCLIP", GOTCLIP},
    {"GOTCLIPBOX", GOTCLIPBOX},
    {"GOTROCKET", GOTROCKET},
    {"GOTROCKET2", GOTROCKET2}, // [JN] Получено две ракеты.
    {"GOTROCKBOX", GOTROCKBOX},
    {"GOTCELL", GOTCELL},
    {"GOTCELLBOX", GOTCELLBOX},
    {"GOTSHELLS", GOTSHELLS},
    {"GOTSHELLS8", GOTSHELLS8}, // [JN] Получено 8 патронов для дробовика.
    {"GOTSHELLBOX", GOTSHELLBOX},
    {"GOTBACKPACK", GOTBACKPACK},
    {"GOTBFG9000", GOTBFG9000},
    {"GOTCHAINGUN", GOTCHAINGUN},
    {"GOTCHAINSAW", GOTCHAINSAW},
    {"GOTLAUNCHER", GOTLAUNCHER},
    {"GOTPLASMA", GOTPLASMA},
    {"GOTSHOTGUN", GOTSHOTGUN},
    {"GOTSHOTGUN2", GOTSHOTGUN2},
    // part 3 - messages when keys are needed
    {"PD_BLUEO", PD_BLUEO},
    {"PD_REDO", PD_REDO},
    {"PD_YELLOWO", PD_YELLOWO},
    {"PD_BLUEK", PD_BLUEK},
    {"PD_REDK", PD_REDK},
    {"PD_YELLOWK", PD_YELLOWK},
    // part 4 - multiplayer messaging
    {"HUSTR_MSGU", HUSTR_MSGU},
    {"HUSTR_MESSAGESENT", HUSTR_MESSAGESENT},
    {"HUSTR_CHATMACRO0", HUSTR_CHATMACRO0},
    {"HUSTR_CHATMACRO1", HUSTR_CHATMACRO1},
    {"HUSTR_CHATMACRO2", HUSTR_CHATMACRO2},
    {"HUSTR_CHATMACRO3", HUSTR_CHATMACRO3},
    {"HUSTR_CHATMACRO4", HUSTR_CHATMACRO4},
    {"HUSTR_CHATMACRO5", HUSTR_CHATMACRO5},
    {"HUSTR_CHATMACRO6", HUSTR_CHATMACRO6},
    {"HUSTR_CHATMACRO7", HUSTR_CHATMACRO7},
    {"HUSTR_CHATMACRO8", HUSTR_CHATMACRO8},
    {"HUSTR_CHATMACRO9", HUSTR_CHATMACRO9},
    {"HUSTR_TALKTOSELF1", HUSTR_TALKTOSELF1},
    {"HUSTR_TALKTOSELF2", HUSTR_TALKTOSELF2},
    {"HUSTR_TALKTOSELF3", HUSTR_TALKTOSELF3},
    {"HUSTR_TALKTOSELF4", HUSTR_TALKTOSELF4},
    {"HUSTR_TALKTOSELF5", HUSTR_TALKTOSELF5},
    {"HUSTR_PLRGREEN", HUSTR_PLRGREEN},
    {"HUSTR_PLRINDIGO", HUSTR_PLRINDIGO},
    {"HUSTR_PLRBROWN", HUSTR_PLRBROWN},
    {"HUSTR_PLRRED", HUSTR_PLRRED},
    // part 5 - level names in the automap
    {"HUSTR_E1M1", HUSTR_E1M1},
    {"HUSTR_E1M2", HUSTR_E1M2},
    {"HUSTR_E1M3", HUSTR_E1M3},
    {"HUSTR_E1M4", HUSTR_E1M4},
    {"HUSTR_E1M5", HUSTR_E1M5},
    {"HUSTR_E1M6", HUSTR_E1M6},
    {"HUSTR_E1M7", HUSTR_E1M7},
    {"HUSTR_E1M8", HUSTR_E1M8},
    {"HUSTR_E1M9", HUSTR_E1M9},
    {"HUSTR_E2M1", HUSTR_E2M1},
    {"HUSTR_E2M2", HUSTR_E2M2},
    {"HUSTR_E2M3", HUSTR_E2M3},
    {"HUSTR_E2M4", HUSTR_E2M4},
    {"HUSTR_E2M5", HUSTR_E2M5},
    {"HUSTR_E2M6", HUSTR_E2M6},
    {"HUSTR_E2M7", HUSTR_E2M7},
    {"HUSTR_E2M8", HUSTR_E2M8},
    {"HUSTR_E2M9", HUSTR_E2M9},
    {"HUSTR_E3M1", HUSTR_E3M1},
    {"HUSTR_E3M2", HUSTR_E3M2},
    {"HUSTR_E3M3", HUSTR_E3M3},
    {"HUSTR_E3M4", HUSTR_E3M4},
    {"HUSTR_E3M5", HUSTR_E3M5},
    {"HUSTR_E3M6", HUSTR_E3M6},
    {"HUSTR_E3M7", HUSTR_E3M7},
    {"HUSTR_E3M8", HUSTR_E3M8},
    {"HUSTR_E3M9", HUSTR_E3M9},
    {"HUSTR_E4M1", HUSTR_E4M1},
    {"HUSTR_E4M2", HUSTR_E4M2},
    {"HUSTR_E4M3", HUSTR_E4M3},
    {"HUSTR_E4M4", HUSTR_E4M4},
    {"HUSTR_E4M5", HUSTR_E4M5},
    {"HUSTR_E4M6", HUSTR_E4M6},
    {"HUSTR_E4M7", HUSTR_E4M7},
    {"HUSTR_E4M8", HUSTR_E4M8},
    {"HUSTR_E4M9", HUSTR_E4M9},
    {"HUSTR_1", HUSTR_1},
    {"HUSTR_2", HUSTR_2},
    {"HUSTR_3", HUSTR_3},
    {"HUSTR_4", HUSTR_4},
    {"HUSTR_5", HUSTR_5},
    {"HUSTR_6", HUSTR_6},
    {"HUSTR_7", HUSTR_7},
    {"HUSTR_8", HUSTR_8},
    {"HUSTR_9", HUSTR_9},
    {"HUSTR_10", HUSTR_10},
    {"HUSTR_11", HUSTR_11},
    {"HUSTR_12", HUSTR_12},
    {"HUSTR_13", HUSTR_13},
    {"HUSTR_14", HUSTR_14},
    {"HUSTR_15", HUSTR_15},
    {"HUSTR_16", HUSTR_16},
    {"HUSTR_17", HUSTR_17},
    {"HUSTR_18", HUSTR_18},
    {"HUSTR_19", HUSTR_19},
    {"HUSTR_20", HUSTR_20},
    {"HUSTR_21", HUSTR_21},
    {"HUSTR_22", HUSTR_22},
    {"HUSTR_23", HUSTR_23},
    {"HUSTR_24", HUSTR_24},
    {"HUSTR_25", HUSTR_25},
    {"HUSTR_26", HUSTR_26},
    {"HUSTR_27", HUSTR_27},
    {"HUSTR_28", HUSTR_28},
    {"HUSTR_29", HUSTR_29},
    {"HUSTR_30", HUSTR_30},
    {"HUSTR_31", HUSTR_31},
    {"HUSTR_32", HUSTR_32},
    {"PHUSTR_1", PHUSTR_1},
    {"PHUSTR_2", PHUSTR_2},
    {"PHUSTR_3", PHUSTR_3},
    {"PHUSTR_4", PHUSTR_4},
    {"PHUSTR_5", PHUSTR_5},
    {"PHUSTR_6", PHUSTR_6},
    {"PHUSTR_7", PHUSTR_7},
    {"PHUSTR_8", PHUSTR_8},
    {"PHUSTR_9", PHUSTR_9},
    {"PHUSTR_10", PHUSTR_10},
    {"PHUSTR_11", PHUSTR_11},
    {"PHUSTR_12", PHUSTR_12},
    {"PHUSTR_13", PHUSTR_13},
    {"PHUSTR_14", PHUSTR_14},
    {"PHUSTR_15", PHUSTR_15},
    {"PHUSTR_16", PHUSTR_16},
    {"PHUSTR_17", PHUSTR_17},
    {"PHUSTR_18", PHUSTR_18},
    {"PHUSTR_19", PHUSTR_19},
    {"PHUSTR_20", PHUSTR_20},
    {"PHUSTR_21", PHUSTR_21},
    {"PHUSTR_22", PHUSTR_22},
    {"PHUSTR_23", PHUSTR_23},
    {"PHUSTR_24", PHUSTR_24},
    {"PHUSTR_25", PHUSTR_25},
    {"PHUSTR_26", PHUSTR_26},
    {"PHUSTR_27", PHUSTR_27},
    {"PHUSTR_28", PHUSTR_28},
    {"PHUSTR_29", PHUSTR_29},
    {"PHUSTR_30", PHUSTR_30},
    {"PHUSTR_31", PHUSTR_31},
    {"PHUSTR_32", PHUSTR_32},
    {"THUSTR_1", THUSTR_1},
    {"THUSTR_2", THUSTR_2},
    {"THUSTR_3", THUSTR_3},
    {"THUSTR_4", THUSTR_4},
    {"THUSTR_5", THUSTR_5},
    {"THUSTR_6", THUSTR_6},
    {"THUSTR_7", THUSTR_7},
    {"THUSTR_8", THUSTR_8},
    {"THUSTR_9", THUSTR_9},
    {"THUSTR_10", THUSTR_10},
    {"THUSTR_11", THUSTR_11},
    {"THUSTR_12", THUSTR_12},
    {"THUSTR_13", THUSTR_13},
    {"THUSTR_14", THUSTR_14},
    {"THUSTR_15", THUSTR_15},
    {"THUSTR_16", THUSTR_16},
    {"THUSTR_17", THUSTR_17},
    {"THUSTR_18", THUSTR_18},
    {"THUSTR_19", THUSTR_19},
    {"THUSTR_20", THUSTR_20},
    {"THUSTR_21", THUSTR_21},
    {"THUSTR_22", THUSTR_22},
    {"THUSTR_23", THUSTR_23},
    {"THUSTR_24", THUSTR_24},
    {"THUSTR_25", THUSTR_25},
    {"THUSTR_26", THUSTR_26},
    {"THUSTR_27", THUSTR_27},
    {"THUSTR_28", THUSTR_28},
    {"THUSTR_29", THUSTR_29},
    {"THUSTR_30", THUSTR_30},
    {"THUSTR_31", THUSTR_31},
    {"THUSTR_32", THUSTR_32},
    // part 6 - messages as a result of toggling states
    {"AMSTR_FOLLOWON", AMSTR_FOLLOWON},
    {"AMSTR_FOLLOWOFF", AMSTR_FOLLOWOFF},
    {"AMSTR_GRIDON", AMSTR_GRIDON},
    {"AMSTR_GRIDOFF", AMSTR_GRIDOFF},
    {"AMSTR_MARKEDSPOT", AMSTR_MARKEDSPOT},
    {"AMSTR_MARKSCLEARED", AMSTR_MARKSCLEARED},
    {"STSTR_MUS", STSTR_MUS},
    {"STSTR_NOMUS", STSTR_NOMUS},
    {"STSTR_DQDON", STSTR_DQDON},
    {"STSTR_DQDOFF", STSTR_DQDOFF},
    {"STSTR_KFAADDED", STSTR_KFAADDED},
    {"STSTR_FAADDED", STSTR_FAADDED},
    {"STSTR_KAADDED", STSTR_KAADDED},
    {"STSTR_NCON", STSTR_NCON},
    {"STSTR_NCOFF", STSTR_NCOFF},
    {"STSTR_BEHOLD", STSTR_BEHOLD},
    {"STSTR_BEHOLDX", STSTR_BEHOLDX},
    {"STSTR_CHOPPERS", STSTR_CHOPPERS},
    {"STSTR_CLEV", STSTR_CLEV},
    // part 7 - episode intermission texts
    {"E1TEXT", E1TEXT},
    {"E2TEXT", E2TEXT},
    {"E3TEXT", E3TEXT},
    {"E4TEXT", E4TEXT},
    {"C1TEXT", C1TEXT},
    {"C2TEXT", C2TEXT},
    {"C3TEXT", C3TEXT},
    {"C4TEXT", C4TEXT},
    {"C5TEXT", C5TEXT},
    {"C6TEXT", C6TEXT},
    {"P1TEXT", P1TEXT},
    {"P2TEXT", P2TEXT},
    {"P3TEXT", P3TEXT},
    {"P4TEXT", P4TEXT},
    {"P5TEXT", P5TEXT},
    {"P6TEXT", P6TEXT},
    {"T1TEXT", T1TEXT},
    {"T2TEXT", T2TEXT},
    {"T3TEXT", T3TEXT},
    {"T4TEXT", T4TEXT},
    {"T5TEXT", T5TEXT},
    {"T6TEXT", T6TEXT},
    // part 8 - creature names for the finale
    {"CC_ZOMBIE", CC_ZOMBIE},
    {"CC_SHOTGUN", CC_SHOTGUN},
    {"CC_HEAVY", CC_HEAVY},
    {"CC_IMP", CC_IMP},
    {"CC_DEMON", CC_DEMON},
    {"CC_LOST", CC_LOST},
    {"CC_CACO", CC_CACO},
    {"CC_HELL", CC_HELL},
    {"CC_BARON", CC_BARON},
    {"CC_ARACH", CC_ARACH},
    {"CC_PAIN", CC_PAIN},
    {"CC_REVEN", CC_REVEN},
    {"CC_MANCU", CC_MANCU},
    {"CC_ARCH", CC_ARCH},
    {"CC_SPIDER", CC_SPIDER},
    {"CC_CYBER", CC_CYBER},
    {"CC_HERO", CC_HERO},
    // part 9 - intermission tiled backgrounds
    {"BGFLATE1", "FLOOR4_8"},
    {"BGFLATE2", "SFLR6_1"},
    {"BGFLATE3", "MFLR8_4"},
    {"BGFLATE4", "MFLR8_3"},
    {"BGFLAT06", "SLIME16"},
    {"BGFLAT11", "RROCK14"},
    {"BGFLAT20", "RROCK07"},
    {"BGFLAT30", "RROCK17"},
    {"BGFLAT15", "RROCK13"},
    {"BGFLAT31", "RROCK19"},
    {"BGCASTCALL", "BOSSBACK"},
    // -------------------------------------------
    // [JN] Russian strings:
    // -------------------------------------------
    // part 1 - general initialization and prompts
    {"D_DEVSTR_RUS", D_DEVSTR_RUS},
    {"D_CDROM_RUS", D_CDROM_RUS},
    {"QUITMSG_RUS", QUITMSG_RUS},
    {"LOADNET_RUS", LOADNET_RUS},
    {"QLOADNET_RUS", QLOADNET_RUS},
    {"QSAVESPOT_RUS", QSAVESPOT_RUS},
    {"SAVEDEAD_RUS", SAVEDEAD_RUS},
    {"QSPROMPT_RUS", QSPROMPT_RUS},
    {"QLPROMPT_RUS", QLPROMPT_RUS},
    {"NEWGAME_RUS", NEWGAME_RUS},
    {"NIGHTMARE_RUS", NIGHTMARE_RUS},
    {"SWSTRING_RUS", SWSTRING_RUS},
    {"MSGOFF_RUS", MSGOFF_RUS},
    {"MSGON_RUS", MSGON_RUS},
    {"NETEND_RUS", NETEND_RUS},
    {"ENDGAME_RUS", ENDGAME_RUS},
    {"DETAILHI_RUS", DETAILHI_RUS},
    {"DETAILLO_RUS", DETAILLO_RUS},
    //  [JN] Should not be translated
    // {"GAMMALVL0_RUS", GAMMALVL0_RUS},
    // {"GAMMALV02_RUS", GAMMALV02_RUS},
    // {"GAMMALV05_RUS", GAMMALV05_RUS},
    // {"GAMMALV07_RUS", GAMMALV07_RUS},
    // {"GAMMALVL1_RUS", GAMMALVL1_RUS},
    // {"GAMMALV12_RUS", GAMMALV12_RUS},
    // {"GAMMALV15_RUS", GAMMALV15_RUS},
    // {"GAMMALVL2_RUS", GAMMALVL2_RUS},
    // {"GAMMALV22_RUS", GAMMALV22_RUS},
    // {"GAMMALV25_RUS", GAMMALV25_RUS},
    // {"GAMMALV27_RUS", GAMMALV27_RUS},
    // {"GAMMALVL3_RUS", GAMMALVL3_RUS},
    // {"GAMMALV32_RUS", GAMMALV32_RUS},
    // {"GAMMALV35_RUS", GAMMALV35_RUS},
    // {"GAMMALV37_RUS", GAMMALV37_RUS},
    // {"GAMMALVL4_RUS", GAMMALVL4_RUS},
    // {"EMPTYSTRING_RUS", EMPTYSTRING_RUS},
    {"GGSAVED_RUS", GGSAVED_RUS},
    {"GGLOADED_RUS", GGLOADED_RUS},
    // part 2 - messages when the player gets things
    {"GOTARMOR_RUS", GOTARMOR_RUS},
    {"GOTMEGA_RUS", GOTMEGA_RUS},
    {"GOTHTHBONUS_RUS", GOTHTHBONUS_RUS},
    {"GOTARMBONUS_RUS", GOTARMBONUS_RUS},
    {"GOTSTIM_RUS", GOTSTIM_RUS},
    {"GOTMEDINEED_RUS", GOTMEDINEED_RUS},
    {"GOTMEDIKIT_RUS", GOTMEDIKIT_RUS},
    {"GOTSUPER_RUS", GOTSUPER_RUS},
    {"GOTBLUECARD_RUS", GOTBLUECARD_RUS},
    {"GOTYELWCARD_RUS", GOTYELWCARD_RUS},
    {"GOTREDCARD_RUS", GOTREDCARD_RUS},
    {"GOTBLUESKUL_RUS", GOTBLUESKUL_RUS},
    {"GOTYELWSKUL_RUS", GOTYELWSKUL_RUS},
    {"GOTREDSKULL_RUS", GOTREDSKULL_RUS},
    {"GOTINVUL_RUS", GOTINVUL_RUS},
    {"GOTBERSERK_RUS", GOTBERSERK_RUS},
    {"GOTINVIS_RUS", GOTINVIS_RUS},
    {"GOTSUIT_RUS", GOTSUIT_RUS},
    {"GOTMAP_RUS", GOTMAP_RUS},
    {"GOTVISOR_RUS", GOTVISOR_RUS},
    {"GOTMSPHERE_RUS", GOTMSPHERE_RUS},
    {"GOTCLIP_RUS", GOTCLIP_RUS},
    {"GOTCLIPBOX_RUS", GOTCLIPBOX_RUS},
    {"GOTROCKET_RUS", GOTROCKET_RUS},
    {"GOTROCKET2_RUS", GOTROCKET2_RUS},
    {"GOTROCKBOX_RUS", GOTROCKBOX_RUS},
    {"GOTCELL_RUS", GOTCELL_RUS},
    {"GOTCELLBOX_RUS", GOTCELLBOX_RUS},
    {"GOTSHELLS_RUS", GOTSHELLS_RUS},
    {"GOTSHELLS8_RUS", GOTSHELLS8_RUS},
    {"GOTSHELLBOX_RUS", GOTSHELLBOX_RUS},
    {"GOTBACKPACK_RUS", GOTBACKPACK_RUS},
    {"GOTBFG9000_RUS", GOTBFG9000_RUS},
    {"GOTCHAINGUN_RUS", GOTCHAINGUN_RUS},
    {"GOTCHAINSAW_RUS", GOTCHAINSAW_RUS},
    {"GOTLAUNCHER_RUS", GOTLAUNCHER_RUS},
    {"GOTPLASMA_RUS", GOTPLASMA_RUS},
    {"GOTSHOTGUN_RUS", GOTSHOTGUN_RUS},
    {"GOTSHOTGUN_RUS", GOTSHOTGUN_RUS},
    {"GOTSHOTGUN2_RUS", GOTSHOTGUN2_RUS},
    // part 3 - messages when keys are needed
    {"PD_BLUEO_RUS", PD_BLUEO_RUS},
    {"PD_REDO_RUS", PD_REDO_RUS},
    {"PD_YELLOWO_RUS", PD_YELLOWO_RUS},
    {"PD_BLUEK_RUS", PD_BLUEK_RUS},
    {"PD_REDK_RUS", PD_REDK_RUS},
    {"PD_YELLOWK_RUS", PD_YELLOWK_RUS},
    // part 4 - multiplayer messaging
    {"HUSTR_MSGU_RUS", HUSTR_MSGU_RUS},
    {"HUSTR_MESSAGESENT_RUS", HUSTR_MESSAGESENT_RUS},
    {"HUSTR_CHATMACRO0_RUS", HUSTR_CHATMACRO0_RUS},
    {"HUSTR_CHATMACRO1_RUS", HUSTR_CHATMACRO1_RUS},
    {"HUSTR_CHATMACRO2_RUS", HUSTR_CHATMACRO2_RUS},
    {"HUSTR_CHATMACRO3_RUS", HUSTR_CHATMACRO3_RUS},
    {"HUSTR_CHATMACRO4_RUS", HUSTR_CHATMACRO4_RUS},
    {"HUSTR_CHATMACRO5_RUS", HUSTR_CHATMACRO5_RUS},
    {"HUSTR_CHATMACRO6_RUS", HUSTR_CHATMACRO6_RUS},
    {"HUSTR_CHATMACRO7_RUS", HUSTR_CHATMACRO7_RUS},
    {"HUSTR_CHATMACRO8_RUS", HUSTR_CHATMACRO8_RUS},
    {"HUSTR_CHATMACRO9_RUS", HUSTR_CHATMACRO9_RUS},
    {"HUSTR_TALKTOSELF1_RUS", HUSTR_TALKTOSELF1_RUS},
    {"HUSTR_TALKTOSELF2_RUS", HUSTR_TALKTOSELF2_RUS},
    {"HUSTR_TALKTOSELF3_RUS", HUSTR_TALKTOSELF3_RUS},
    {"HUSTR_TALKTOSELF4_RUS", HUSTR_TALKTOSELF4_RUS},
    {"HUSTR_TALKTOSELF5_RUS", HUSTR_TALKTOSELF5_RUS},
    {"HUSTR_PLRGREEN_RUS", HUSTR_PLRGREEN_RUS},
    {"HUSTR_PLRINDIGO_RUS", HUSTR_PLRINDIGO_RUS},
    {"HUSTR_PLRBROWN_RUS", HUSTR_PLRBROWN_RUS},
    {"HUSTR_PLRRED_RUS", HUSTR_PLRRED_RUS},
    // part 5 - level names in the automap
    {"HUSTR_E1M1_RUS", HUSTR_E1M1_RUS},
    {"HUSTR_E1M2_RUS", HUSTR_E1M2_RUS},
    {"HUSTR_E1M3_RUS", HUSTR_E1M3_RUS},
    {"HUSTR_E1M4_RUS", HUSTR_E1M4_RUS},
    {"HUSTR_E1M5_RUS", HUSTR_E1M5_RUS},
    {"HUSTR_E1M6_RUS", HUSTR_E1M6_RUS},
    {"HUSTR_E1M7_RUS", HUSTR_E1M7_RUS},
    {"HUSTR_E1M8_RUS", HUSTR_E1M8_RUS},
    {"HUSTR_E1M9_RUS", HUSTR_E1M9_RUS},
    {"HUSTR_E2M1_RUS", HUSTR_E2M1_RUS},
    {"HUSTR_E2M2_RUS", HUSTR_E2M2_RUS},
    {"HUSTR_E2M3_RUS", HUSTR_E2M3_RUS},
    {"HUSTR_E2M4_RUS", HUSTR_E2M4_RUS},
    {"HUSTR_E2M5_RUS", HUSTR_E2M5_RUS},
    {"HUSTR_E2M6_RUS", HUSTR_E2M6_RUS},
    {"HUSTR_E2M7_RUS", HUSTR_E2M7_RUS},
    {"HUSTR_E2M8_RUS", HUSTR_E2M8_RUS},
    {"HUSTR_E2M9_RUS", HUSTR_E2M9_RUS},
    {"HUSTR_E3M1_RUS", HUSTR_E3M1_RUS},
    {"HUSTR_E3M2_RUS", HUSTR_E3M2_RUS},
    {"HUSTR_E3M3_RUS", HUSTR_E3M3_RUS},
    {"HUSTR_E3M4_RUS", HUSTR_E3M4_RUS},
    {"HUSTR_E3M5_RUS", HUSTR_E3M5_RUS},
    {"HUSTR_E3M6_RUS", HUSTR_E3M6_RUS},
    {"HUSTR_E3M7_RUS", HUSTR_E3M7_RUS},
    {"HUSTR_E3M8_RUS", HUSTR_E3M8_RUS},
    {"HUSTR_E3M9_RUS", HUSTR_E3M9_RUS},
    {"HUSTR_E4M1_RUS", HUSTR_E4M1_RUS},
    {"HUSTR_E4M2_RUS", HUSTR_E4M2_RUS},
    {"HUSTR_E4M3_RUS", HUSTR_E4M3_RUS},
    {"HUSTR_E4M4_RUS", HUSTR_E4M4_RUS},
    {"HUSTR_E4M5_RUS", HUSTR_E4M5_RUS},
    {"HUSTR_E4M6_RUS", HUSTR_E4M6_RUS},
    {"HUSTR_E4M7_RUS", HUSTR_E4M7_RUS},
    {"HUSTR_E4M8_RUS", HUSTR_E4M8_RUS},
    {"HUSTR_E4M9_RUS", HUSTR_E4M9_RUS},
    {"HUSTR_1_RUS", HUSTR_1_RUS},
    {"HUSTR_2_RUS", HUSTR_2_RUS},
    {"HUSTR_3_RUS", HUSTR_3_RUS},
    {"HUSTR_4_RUS", HUSTR_4_RUS},
    {"HUSTR_5_RUS", HUSTR_5_RUS},
    {"HUSTR_6_RUS", HUSTR_6_RUS},
    {"HUSTR_7_RUS", HUSTR_7_RUS},
    {"HUSTR_8_RUS", HUSTR_8_RUS},
    {"HUSTR_9_RUS", HUSTR_9_RUS},
    {"HUSTR_10_RUS", HUSTR_10_RUS},
    {"HUSTR_11_RUS", HUSTR_11_RUS},
    {"HUSTR_12_RUS", HUSTR_12_RUS},
    {"HUSTR_13_RUS", HUSTR_13_RUS},
    {"HUSTR_14_RUS", HUSTR_14_RUS},
    {"HUSTR_15_RUS", HUSTR_15_RUS},
    {"HUSTR_16_RUS", HUSTR_16_RUS},
    {"HUSTR_17_RUS", HUSTR_17_RUS},
    {"HUSTR_18_RUS", HUSTR_18_RUS},
    {"HUSTR_19_RUS", HUSTR_19_RUS},
    {"HUSTR_20_RUS", HUSTR_20_RUS},
    {"HUSTR_21_RUS", HUSTR_21_RUS},
    {"HUSTR_22_RUS", HUSTR_22_RUS},
    {"HUSTR_23_RUS", HUSTR_23_RUS},
    {"HUSTR_24_RUS", HUSTR_24_RUS},
    {"HUSTR_25_RUS", HUSTR_25_RUS},
    {"HUSTR_26_RUS", HUSTR_26_RUS},
    {"HUSTR_27_RUS", HUSTR_27_RUS},
    {"HUSTR_28_RUS", HUSTR_28_RUS},
    {"HUSTR_29_RUS", HUSTR_29_RUS},
    {"HUSTR_30_RUS", HUSTR_30_RUS},
    {"HUSTR_31_RUS", HUSTR_31_RUS},
    {"HUSTR_32_RUS", HUSTR_32_RUS},
    {"PHUSTR_1_RUS", PHUSTR_1_RUS},
    {"PHUSTR_2_RUS", PHUSTR_2_RUS},
    {"PHUSTR_3_RUS", PHUSTR_3_RUS},
    {"PHUSTR_4_RUS", PHUSTR_4_RUS},
    {"PHUSTR_5_RUS", PHUSTR_5_RUS},
    {"PHUSTR_6_RUS", PHUSTR_6_RUS},
    {"PHUSTR_7_RUS", PHUSTR_7_RUS},
    {"PHUSTR_8_RUS", PHUSTR_8_RUS},
    {"PHUSTR_9_RUS", PHUSTR_9_RUS},
    {"PHUSTR_10_RUS", PHUSTR_10_RUS},
    {"PHUSTR_11_RUS", PHUSTR_11_RUS},
    {"PHUSTR_12_RUS", PHUSTR_12_RUS},
    {"PHUSTR_13_RUS", PHUSTR_13_RUS},
    {"PHUSTR_14_RUS", PHUSTR_14_RUS},
    {"PHUSTR_15_RUS", PHUSTR_15_RUS},
    {"PHUSTR_16_RUS", PHUSTR_16_RUS},
    {"PHUSTR_17_RUS", PHUSTR_17_RUS},
    {"PHUSTR_18_RUS", PHUSTR_18_RUS},
    {"PHUSTR_19_RUS", PHUSTR_19_RUS},
    {"PHUSTR_20_RUS", PHUSTR_20_RUS},
    {"PHUSTR_21_RUS", PHUSTR_21_RUS},
    {"PHUSTR_22_RUS", PHUSTR_22_RUS},
    {"PHUSTR_23_RUS", PHUSTR_23_RUS},
    {"PHUSTR_24_RUS", PHUSTR_24_RUS},
    {"PHUSTR_25_RUS", PHUSTR_25_RUS},
    {"PHUSTR_26_RUS", PHUSTR_26_RUS},
    {"PHUSTR_27_RUS", PHUSTR_27_RUS},
    {"PHUSTR_28_RUS", PHUSTR_28_RUS},
    {"PHUSTR_29_RUS", PHUSTR_29_RUS},
    {"PHUSTR_30_RUS", PHUSTR_30_RUS},
    {"PHUSTR_31_RUS", PHUSTR_31_RUS},
    {"PHUSTR_32_RUS", PHUSTR_32_RUS},
    {"THUSTR_1_RUS", THUSTR_1_RUS},
    {"THUSTR_2_RUS", THUSTR_2_RUS},
    {"THUSTR_3_RUS", THUSTR_3_RUS},
    {"THUSTR_4_RUS", THUSTR_4_RUS},
    {"THUSTR_5_RUS", THUSTR_5_RUS},
    {"THUSTR_6_RUS", THUSTR_6_RUS},
    {"THUSTR_7_RUS", THUSTR_7_RUS},
    {"THUSTR_8_RUS", THUSTR_8_RUS},
    {"THUSTR_9_RUS", THUSTR_9_RUS},
    {"THUSTR_10_RUS", THUSTR_10_RUS},
    {"THUSTR_11_RUS", THUSTR_11_RUS},
    {"THUSTR_12_RUS", THUSTR_12_RUS},
    {"THUSTR_13_RUS", THUSTR_13_RUS},
    {"THUSTR_14_RUS", THUSTR_14_RUS},
    {"THUSTR_15_RUS", THUSTR_15_RUS},
    {"THUSTR_16_RUS", THUSTR_16_RUS},
    {"THUSTR_17_RUS", THUSTR_17_RUS},
    {"THUSTR_18_RUS", THUSTR_18_RUS},
    {"THUSTR_19_RUS", THUSTR_19_RUS},
    {"THUSTR_20_RUS", THUSTR_20_RUS},
    {"THUSTR_21_RUS", THUSTR_21_RUS},
    {"THUSTR_22_RUS", THUSTR_22_RUS},
    {"THUSTR_23_RUS", THUSTR_23_RUS},
    {"THUSTR_24_RUS", THUSTR_24_RUS},
    {"THUSTR_25_RUS", THUSTR_25_RUS},
    {"THUSTR_26_RUS", THUSTR_26_RUS},
    {"THUSTR_27_RUS", THUSTR_27_RUS},
    {"THUSTR_28_RUS", THUSTR_28_RUS},
    {"THUSTR_29_RUS", THUSTR_29_RUS},
    {"THUSTR_30_RUS", THUSTR_30_RUS},
    {"THUSTR_31_RUS", THUSTR_31_RUS},
    {"THUSTR_32_RUS", THUSTR_32_RUS},
    // part 6 - messages as a result of toggling states
    {"AMSTR_FOLLOWON_RUS", AMSTR_FOLLOWON_RUS},
    {"AMSTR_FOLLOWOFF_RUS", AMSTR_FOLLOWOFF_RUS},
    {"AMSTR_GRIDON_RUS", AMSTR_GRIDON_RUS},
    {"AMSTR_GRIDOFF_RUS", AMSTR_GRIDOFF_RUS},
    {"AMSTR_MARKEDSPOT_RUS", AMSTR_MARKEDSPOT_RUS},
    {"AMSTR_MARKSCLEARED_RUS", AMSTR_MARKSCLEARED_RUS},
    {"STSTR_MUS_RUS", STSTR_MUS_RUS},
    {"STSTR_NOMUS_RUS", STSTR_NOMUS_RUS},
    {"STSTR_DQDON_RUS", STSTR_DQDON_RUS},
    {"STSTR_DQDOFF_RUS", STSTR_DQDOFF_RUS},
    {"STSTR_KFAADDED_RUS", STSTR_KFAADDED_RUS},
    {"STSTR_FAADDED_RUS", STSTR_FAADDED_RUS},
    {"STSTR_KAADDED_RUS", STSTR_KAADDED_RUS},
    {"STSTR_NCON_RUS", STSTR_NCON_RUS},
    {"STSTR_NCOFF_RUS", STSTR_NCOFF_RUS},
    {"STSTR_BEHOLD_RUS", STSTR_BEHOLD_RUS},
    {"STSTR_BEHOLDX_RUS", STSTR_BEHOLDX_RUS},
    {"STSTR_CHOPPERS_RUS", STSTR_CHOPPERS_RUS},
    {"STSTR_CLEV_RUS", STSTR_CLEV_RUS},
    // part 7 - episode intermission texts
    {"E1TEXT_RUS", E1TEXT_RUS},
    {"E2TEXT_RUS", E2TEXT_RUS},
    {"E3TEXT_RUS", E3TEXT_RUS},
    {"E4TEXT_RUS", E4TEXT_RUS},
    {"C1TEXT_RUS", C1TEXT_RUS},
    {"C2TEXT_RUS", C2TEXT_RUS},
    {"C3TEXT_RUS", C3TEXT_RUS},
    {"C4TEXT_RUS", C4TEXT_RUS},
    {"C5TEXT_RUS", C5TEXT_RUS},
    {"C6TEXT_RUS", C6TEXT_RUS},
    {"P1TEXT_RUS", P1TEXT_RUS},
    {"P2TEXT_RUS", P2TEXT_RUS},
    {"P3TEXT_RUS", P3TEXT_RUS},
    {"P4TEXT_RUS", P4TEXT_RUS},
    {"P5TEXT_RUS", P5TEXT_RUS},
    {"P6TEXT_RUS", P6TEXT_RUS},
    {"T1TEXT_RUS", T1TEXT_RUS},
    {"T2TEXT_RUS", T2TEXT_RUS},
    {"T3TEXT_RUS", T3TEXT_RUS},
    {"T4TEXT_RUS", T4TEXT_RUS},
    {"T5TEXT_RUS", T5TEXT_RUS},
    {"T6TEXT_RUS", T6TEXT_RUS},
    // part 8 - creature names for the finale
    {"CC_ZOMBIE_RUS", CC_ZOMBIE_RUS},
    {"CC_SHOTGUN_RUS", CC_SHOTGUN_RUS},
    {"CC_HEAVY_RUS", CC_HEAVY_RUS},
    {"CC_IMP_RUS", CC_IMP_RUS},
    {"CC_DEMON_RUS", CC_DEMON_RUS},
    {"CC_LOST_RUS", CC_LOST_RUS},
    {"CC_CACO_RUS", CC_CACO_RUS},
    {"CC_HELL_RUS", CC_HELL_RUS},
    {"CC_BARON_RUS", CC_BARON_RUS},
    {"CC_ARACH_RUS", CC_ARACH_RUS},
    {"CC_PAIN_RUS", CC_PAIN_RUS},
    {"CC_REVEN_RUS", CC_REVEN_RUS},
    {"CC_MANCU_RUS", CC_MANCU_RUS},
    {"CC_ARCH_RUS", CC_ARCH_RUS},
    {"CC_SPIDER_RUS", CC_SPIDER_RUS},
    {"CC_CYBER_RUS", CC_CYBER_RUS},
    {"CC_HERO_RUS", CC_HERO_RUS},
};

static void *DEH_BEXStrStart(deh_context_t *context, char *line)
{
    char s[10];

    if (sscanf(line, "%9s", s) == 0 || strncmp("[STRINGS]", s, sizeof(s)))
    {
	DEH_Warning(context, "Parse error on section start");
    }

    return NULL;
}

static void DEH_BEXStrParseLine(deh_context_t *context, char *line, void *tag)
{
    char *variable_name, *value;
    int i;

    if (!DEH_ParseAssignment(line, &variable_name, &value))
    {
	DEH_Warning(context, "Failed to parse assignment");
	return;
    }

    for (i = 0; i < arrlen(bex_stringtable); i++)
    {
	if (!strcmp(bex_stringtable[i].macro, variable_name))
	{
	    DEH_AddStringReplacement(bex_stringtable[i].string, value);
	}
    }
}

deh_section_t deh_section_bexstr =
{
    "[STRINGS]",
    NULL,
    DEH_BEXStrStart,
    DEH_BEXStrParseLine,
    NULL,
    NULL,
};
