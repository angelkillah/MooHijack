#pragma once

#include <Windows.h>
#include <stdio.h>

#include "database.h"
#include "util.h"

// CPS1
#define PATH_OKI_FILE			"rom.oki"
#define PATH_Z80_FILE			"rom.z80"
#define PATH_68K_FILE			"rom.68k"
#define PATH_VROM_FILE			"rom.vrom"

// CPS2
#define PATH_QS_FILE			"rom.qs"
#define PATH_68Y_FILE			"rom.68y"

// ALL SYSTEMS
#define PATH_LOGO_FILE			"logo.png"
#define PATH_SAVESTATE_FILE		"multi.sav"

#define SF2HF_68K_SIZE		    0x180000  
#define SF2HF_VROM_SIZE			0x600000
#define SF2HF_OKI_SIZE			0x40000
#define SF2HF_Z80_SIZE			0x10000
#define SF2HF_LOGO_SIZE			0x35D84   
#define SF2HF_SAVESTATE_SIZE	0x5487A

#define SSF2_68K_SIZE			0x280000
#define SSF2_68Y_SIZE			0x280000
#define SSF2_VROM_SIZE			0xC00000
#define SSF2_Z80_SIZE			0x20000
#define SSF2_QS_SIZE			0x400000

#define SSF2X_68K_SIZE			0x380000
#define SSF2X_68Y_SIZE			0x380000
#define SSF2X_VROM_SIZE			0x1000000
#define SSF2X_Z80_SIZE			0x40000

#define SFA3_LOGO_SIZE			0x12E3C
#define SFA3_SAVESTATE_SIZE		0x5B57F

#define SF2CE_ID				2
#define SF2HF_ID				3
#define SFA2_ID					7
#define SFA3_ID					8
#define SSF2_ID					4
#define SSF2X_ID				5

#define SSF2X_PATCHED_ONLINE_ID		'2'

int	OFFSET_LOAD_STATE = 0x1CFE30;
int OFFSET_CPS1 = 0x2BD520;

#define OFFSET_CLOCK				OFFSET_CPS1 + 8
#define OFFSET_CPSB					OFFSET_CLOCK + 8	
#define OFFSET_CTRL					OFFSET_CPSB + 8
#define OFFSET_PALCTRL				OFFSET_CTRL + 8
#define OFFSET_PRIORITY_MASK		OFFSET_PALCTRL + 8
#define OFFSET_MULTIPLY_PROTECTION	OFFSET_PRIORITY_MASK + 32
#define OFFSET_IN2					OFFSET_MULTIPLY_PROTECTION + 32

#define OFFSET_ID				OFFSET_IN2 + 28
#define OFFSET_LAYER_MASK       OFFSET_ID + 4

#define OFFSET_GFX_MAPPER		OFFSET_LAYER_MASK + 24
#define OFFSET_GFX_SPRITES		0
#define	OFFSET_GFX_SPRITES_END	48
#define OFFSET_GFX_SCROLL1		OFFSET_GFX_SPRITES_END + 32
#define OFFSET_GFX_SCROLL2		OFFSET_GFX_SCROLL1 + 80
#define OFFSET_GFX_SCROLL3		OFFSET_GFX_SCROLL2 + 80

int OFFSET_GETSIZE = 0x148238;
int OFFSET_GETDATA = 0x14827A;
int OFFSET_CPS1_CALLBACKS = 0x1A9E55;
int OFFSET_SWITCH_GAMES = 0x1CEFE;
int OFFSET_SSF2_END_MATCH_CALLBACK = 0x1B0D4A;

int OFFSET_SPECTATOR_MODE = 0x4E962;
PCHAR patchSpectator = "\xb0\x01\x90\x90\x90\x90\x90";

int OFFSET_GAME_VERSION = 0x2471D0;
int OFFSET_CREATE_LOBBY = 0x223A4;
int OFFSET_FIND_LOBBY = 0x4B0D5;

PCHAR currentPath;
PVOID VEHhandler;

PVOID Orig_GetSize, Orig_GetData, Orig_SwitchGames, Orig_CreateLobby, Orig_FindLobby;
BYTE OrigByte_GetSize, OrigByte_GetData, OrigByte_SwitchGames, OrigByte_CreateLobby, OrigByte_FindLobby;
BYTE int3[] = "\xcc";

DWORD dwDataSize = 0;
BOOL bDoLoadCPS2 = FALSE;