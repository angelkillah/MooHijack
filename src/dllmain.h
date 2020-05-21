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

// CPS3
#define PATH_BIOS_FILE			"rom.bios"
#define PATH_S1_FILE			"rom.s1"
#define PATH_S2_FILE			"rom.s2"
#define PATH_S3_FILE			"rom.s3"
#define PATH_S4_FILE			"rom.s4"
#define PATH_S5_FILE			"rom.s5"

// ALL SYSTEMS
#define PATH_LOGO_FILE			"logo.png"
#define PATH_SAVESTATE_FILE		"multi.sav"

#define SF2HF_68K_SIZE		    0x180000  
#define SF2HF_VROM_SIZE			0x600000
#define SF2HF_OKI_SIZE			0x40000
#define SF2HF_Z80_SIZE			0x10000
#define SF2HF_LOGO_SIZE			0x35D84  
#define SF2HF_JAP_LOGO_SIZE     0x20B3F
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
#define SSF2X_QS_SIZE			0x400000

#define SFA3_LOGO_SIZE			0x12E3C
#define SSF2_SAVESTATE_SIZE		0x5B57F

#define SF33_LOGO_SIZE			0x37D1C
#define SF32_SAVESTATE_SIZE		0x95B007 
#define SF32_BIOS_SIZE			0x80000
#define SF32_S1_SIZE			0x800000
#define SF32_S2_SIZE			0x800000
#define SF32_S3_SIZE			0x1000000
#define SF32_S4_SIZE			0x1000000
#define SF32_S5_SIZE			0x1000000

#define SF2CE_ID				2
#define SF2HF_ID				3
#define SSF2_ID					4
#define SSF2X_ID				5
#define SFA2_ID					7
#define SFA3_ID					8
#define SF32_ID					10
#define SF33_ID					11

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

int OFFSET_SSF2_P1WIN_CALLBACK = 0x1B0D40;
int OFFSET_SSF2_TIE_CALLBACK = 0x1B0D4A;
int OFFSET_SSF2_P2WIN_CALLBACK = 0x1B0D54;

int OFFSET_SF32_TIE_CALLBACK = 0x1BB019;

int OFFSET_SPECTATOR_MODE = 0x4E962;
PCHAR patchSpectator = "\xb0\x01\x90\x90\x90\x90\x90";

int OFFSET_SSF2_NV = 0x2C1F00;
int OFFSET_SSF2_VROM = 0x2C1F08;
int OFFSET_SSF2X_NV = 0x2C2AD0;
int OFFSET_SSF2X_VROM = 0x2C2AD8;  

int OFFSET_GAME_VERSION = 0x2471D0;
int OFFSET_CREATE_LOBBY = 0x223A4;
int OFFSET_FIND_LOBBY = 0x4B0D5;

int OFFSET_CPS1_SETUP = 0x1CF060;
int OFFSET_CPS2_SETUP = 0x1E9CE0;
int OFFSET_CPS3_SETUP = 0x1C3ED0;

int OFFSET_SF32_KEYS = 0x2C5670;

PCHAR currentPath;
PVOID VEHhandler;

PVOID Orig_GetSize, Orig_GetData, Orig_SwitchGames, Orig_CreateLobby, Orig_FindLobby, Orig_CPS1, Orig_CPS2, Orig_CPS3;
BYTE OrigByte_GetSize, OrigByte_GetData, OrigByte_SwitchGames, OrigByte_CreateLobby, OrigByte_FindLobby, OrigByte_CPS1, OrigByte_CPS2, OrigByte_CPS3;
BYTE int3[] = "\xcc";

DWORD dwCurrentSystem = 3;
DWORD dwDataSize = 0;
BOOL bDoLoadCPS2 = FALSE;

BYTE hash[SHA1_HASH_SIZE];

// 68k, vrom, oki, z80
BOOL bDataSF2HFound[4] = { FALSE };

// z80, qs, 68k, 68y, vrom
BOOL bDataSSF2Found[5] = { FALSE };
BOOL bDataSSF2XFound[5] = { FALSE };
