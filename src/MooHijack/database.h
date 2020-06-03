#pragma once

#include <Windows.h>

#include "util.h"

#define NUMBER_OF_GAMES		11

typedef enum { CPS1, CPS2, CPS3, UNKNOWN } SYSTEM;

typedef struct _GFX_SPRITES {
	DWORD	dwGfxSpritesStart;
	DWORD	dwGfxSpritesEnd;
	DWORD	dwBankStart;
	DWORD	dwBankEnd;
} GFX_SPRITES;

typedef struct _GFX_SCROLL {
	DWORD	dwGfxScrollStart;
	DWORD	dwGfxScrollEnd;
	DWORD	dwBankStart;
	DWORD	dwBankEnd;
	DWORD   dwEndMarker;
} GFX_SCROLL;

typedef struct _MAPPER_GFX {
	GFX_SPRITES	GfxSpritesInfo[3];
	DWORD		dwGfxSpritesEndMarker;
	GFX_SCROLL	GfxScrollInfo[3];
} MAPPER_GFX;

typedef struct _CPSB_INFO {
	DWORD	dwCpuClockRate;
	DWORD	dwCPSB;
	DWORD	dwCtrl;
	DWORD	dwPalctrl;
	DWORD	dwPriorityMask[4];
	DWORD   dwMultiplyProtection[4];
	DWORD	dwIn2;
	DWORD	dwID;
	DWORD	dwLayerEnableMask[5];
} CPSB_INFO;

typedef struct _GAME_INFO {
	CPSB_INFO	CpsbInfo;
	MAPPER_GFX	MapperGfx;
} GAME_INFO;

typedef struct _ROMS_INFO_CPS1 {
	DWORD	dw68kSize;
	DWORD	dwVromSize;
	DWORD	dwOkiSize;
	DWORD	dwZ80Size;
	DWORD	dwLogoSize;
} ROMS_INFO_CPS1;

typedef struct _ROMS_INFO_CPS2 {
	DWORD	dw68kSize;
	DWORD	dw68ySize;
	DWORD	dwVromSize;
	DWORD	dwQsSize;
	DWORD	dwZ80Size;
	DWORD	dwLogoSize;
	DWORD   dwBorderBottom;
	DWORD   dwBorderTop;
	DWORD   dwBorderLeft;
	DWORD   dwBorderRight;
} ROMS_INFO_CPS2;

typedef struct _ROMS_INFO_CPS3 {
	DWORD	dwBiosSize;
	DWORD	dwS1Size;
	DWORD	dwS2Size;
	DWORD	dwS3Size;
	DWORD	dwS4Size;
	DWORD	dwS5Size;
	DWORD	dwLogoSize;
	DWORD   dwBorderBottom;
	DWORD	dwBorderTop;
	DWORD   dwBorderLeft;
	DWORD	dwBorderRight;
} ROMS_INFO_CPS3;

typedef struct _ONLINE_INFO {
	PCHAR		Slot;
	ULONGLONG	Id;
} ONLINE_INFO;

typedef struct _CALLBACKS_INFO {
	PCHAR		OffsetEndMatch;
	PCHAR		OffsetP2WinCount;
} CALLBACKS_INFO;

typedef struct _GAME_LIST {
	GAME_INFO       GameInfo;
	union {
		ROMS_INFO_CPS1  RomsInfoCPS1;
		ROMS_INFO_CPS2  RomsInfoCPS2;
		ROMS_INFO_CPS3  RomsInfoCPS3;
	} RomsInfo;
	SYSTEM			System;
	ONLINE_INFO     OnlineInfo;
	CALLBACKS_INFO	CallbacksInfo;
	PCHAR			Name;
	PCHAR			Hash;
} GAME_LIST;

GAME_LIST GameList[NUMBER_OF_GAMES];
INT dwCurrentGameID[4];

VOID DB_Init();