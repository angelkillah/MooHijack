#include <Windows.h>
#include <psapi.h>
#include <stdio.h>

#include "database.h"
#include "util.h"

#define MAX_MODULES				500

// CPS1
#define PATH_OKI_FILE			".\\roms\\rom.oki"
#define PATH_Z80_FILE			".\\roms\\rom.z80"
#define PATH_68K_FILE			".\\roms\\rom.68k"
#define PATH_VROM_FILE			".\\roms\\rom.vrom"

// CPS2
#define PATH_QS_FILE			".\\roms\\rom.qs"
#define PATH_NV_FILE			".\\roms\\rom.nv"
#define PATH_68Y_FILE			".\\roms\\rom.68y"

// ALL SYSTEMS
#define PATH_LOGO_FILE			".\\roms\\logo.png"
#define PATH_SAVESTATE_FILE		".\\roms\\multi.sav"

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

#define NV_SIZE					0x80

#define SFA3_LOGO_SIZE			0x12E3C
#define SFA3_SAVESTATE_SIZE		0x5B57F

#define SF2CE_ID				2
#define SF2HF_ID				3
#define SFA2_ID					7
#define SFA3_ID					8
#define SSF2_ID					4

#define SSF2X_PATCHED_ONLINE_ID		'2'

#define OFFSET_PATCH_COINS			0x220CF6   // patch number of coins in memory 

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
int OFFSET_CPS1_CODE_TO_PATCH = 0x1A9E55;
int OFFSET_SWITCH_GAMES = 0x1CEFE;
int OFFSET_SSF2_END_MATCH_CALLBACK = 0x1B0D4A;

int OFFSET_SPECTATOR_MODE = 0x4E962;
PCHAR patchSpectator = "\xb0\x01\x90\x90\x90\x90\x90";

int OFFSET_TRAINING_ALPHA2 = 0x1AB740;
int OFFSET_TRAINING_SF2CE = 0x1A5F95;

int OFFSET_GAME_VERSION = 0x2471D0;
int OFFSET_CREATE_LOBBY = 0x223A4; 
int OFFSET_FIND_LOBBY = 0x4D08D0;

PCHAR currentPath;
PVOID VEHhandler;

PVOID Orig_GetSize, Orig_GetData, Orig_PatchCoins, Orig_SwitchGames, Orig_CreateLobby, Orig_FindLobby;
BYTE OrigByte_GetSize, OrigByte_GetData, OrigByte_PatchCoins, OrigByte_SwitchGames, OrigByte_CreateLobby, OrigByte_FindLobby;
BYTE int3[] = "\xcc";

DWORD dwDataSize = 0;
BOOL bIsMulti = FALSE;
BOOL bPatchCoinsApplied = FALSE;

VOID __declspec(dllexport) _()
{
}

PVOID GetModuleBaseAddress(PWCHAR moduleName)
{
	HMODULE hModules[MAX_MODULES];
	MODULEINFO ModuleInfo = { 0 };
	DWORD dwBytesNeeded, i;

	HANDLE hCurrentProcess = GetCurrentProcess();

	while (1)
	{
		if (EnumProcessModules(hCurrentProcess, hModules, sizeof(hModules), &dwBytesNeeded))
		{
			for (i = 0; i < (dwBytesNeeded / sizeof(HMODULE)); i++)
			{
				TCHAR szModName[MAX_PATH];
				if (GetModuleFileNameEx(hCurrentProcess, hModules[i], szModName,
					sizeof(szModName) / sizeof(TCHAR)))
				{
					if (wcsstr(szModName, moduleName) != NULL)
					{
						if (GetModuleInformation(hCurrentProcess, hModules[i], &ModuleInfo, sizeof(ModuleInfo)) == TRUE)
							return ModuleInfo.lpBaseOfDll;
					}
				}
			}
		}
		Sleep(500);
	}
}

VOID PatchCoins(PVOID CoinAddr)
{
	DWORD dwOldProtect;
	VirtualProtect(CoinAddr, sizeof(WORD), PAGE_READWRITE, &dwOldProtect);
	memcpy(CoinAddr, "\x20\x20", sizeof(WORD));
	VirtualProtect(CoinAddr, sizeof(WORD), dwOldProtect, &dwOldProtect);
}

VOID PatchCPS1GameSettings(PVOID GameBaseAddress)
{
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_CLOCK), (PCHAR)&GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwCpuClockRate, sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_CPSB), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwCPSB), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_CTRL), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwCtrl), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PALCTRL), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwPalctrl), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PRIORITY_MASK), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwPriorityMask[0]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PRIORITY_MASK + 8), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwPriorityMask[1]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PRIORITY_MASK + 16), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwPriorityMask[2]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PRIORITY_MASK + 24), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwPriorityMask[3]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_MULTIPLY_PROTECTION), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwMultiplyProtection[0]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_MULTIPLY_PROTECTION + 8), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwMultiplyProtection[1]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_MULTIPLY_PROTECTION + 16), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwMultiplyProtection[2]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_MULTIPLY_PROTECTION + 24), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwMultiplyProtection[3]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_IN2), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwIn2), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_ID), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwID), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_LAYER_MASK), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwLayerEnableMask), sizeof(DWORD) * 5);

	PVOID GFXMapper = *(PVOID*)((LPBYTE)GameBaseAddress + OFFSET_GFX_MAPPER);
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SPRITES), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.MapperGfx.GfxSpritesInfo), sizeof(GFX_SPRITES) * 3);
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SPRITES_END), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.MapperGfx.dwGfxSpritesEndMarker), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SCROLL1), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.MapperGfx.GfxScrollInfo[0]), sizeof(GFX_SCROLL));
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SCROLL2), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.MapperGfx.GfxScrollInfo[1]), sizeof(GFX_SCROLL));
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SCROLL3), (PCHAR) & (GameList[dwCurrentGameID].GameInfo.MapperGfx.GfxScrollInfo[2]), sizeof(GFX_SCROLL));
}

VOID InstallHook(PVOID addr, BYTE* bOrig)
{
	DWORD dwOldProtect;
	VirtualProtect(addr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	memcpy(bOrig, addr, 1);
	memcpy(addr, &int3, 1);

	VirtualProtect(addr, 1, dwOldProtect, &dwOldProtect);
}

LONG CALLBACK ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
	DWORD dwOldProtect;

	PVOID pExceptionAddr = ExceptionInfo->ExceptionRecord->ExceptionAddress;
	DWORD dwExceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;

	if (dwExceptionCode == EXCEPTION_BREAKPOINT)
	{
		if ((pExceptionAddr == Orig_PatchCoins) && (bPatchCoinsApplied == FALSE))
		{
			// patch coins
			if (GameList[dwCurrentGameID].CoinOffset)
			{
				if ((ExceptionInfo->ContextRecord->Rax == GameList[dwCurrentGameID].CoinOffset) && (((ExceptionInfo->ContextRecord->Rbx >> 8) & 0xf0) >= 0xd0))
				{
					PatchCoins((PVOID)(ExceptionInfo->ContextRecord->Rax + ExceptionInfo->ContextRecord->Rdx));
					MessageBoxA(NULL, "Patch correctly applied", "patch coin", MB_OK);
					bPatchCoinsApplied = TRUE;
				}
			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_PatchCoins, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == Orig_CreateLobby)
		{		
			// SSF2X 
			if ((strcmpi(ExceptionInfo->ContextRecord->R8, "game_1") == 0) && (strcmpi(ExceptionInfo->ContextRecord->R9, "1") == 0))
				*(PVOID*)((LPBYTE)ExceptionInfo->ContextRecord->R9) = SSF2X_PATCHED_ONLINE_ID;
		
			// patch others games ID
			if ((dwCurrentGameID != -1) && (GameList[dwCurrentGameID].OnlineInfo.Slot))
			{
				if ((strcmpi(ExceptionInfo->ContextRecord->R8, GameList[dwCurrentGameID].OnlineInfo.Slot) == 0) && (strcmpi(ExceptionInfo->ContextRecord->R9, "1") == 0))
					*(PVOID*)((LPBYTE)ExceptionInfo->ContextRecord->R9) = GameList[dwCurrentGameID].OnlineInfo.Id;
			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_CreateLobby, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == Orig_FindLobby)
		{
			// SSF2X
			if (strcmpi(ExceptionInfo->ContextRecord->Rdx, "game_1") == 0)
				ExceptionInfo->ContextRecord->R8 = SSF2X_PATCHED_ONLINE_ID - '0';

			// patch others games ID
			if ((dwCurrentGameID != -1) && (GameList[dwCurrentGameID].OnlineInfo.Slot))
			{
				if (strcmpi(ExceptionInfo->ContextRecord->Rdx, GameList[dwCurrentGameID].OnlineInfo.Slot) == 0)
					ExceptionInfo->ContextRecord->R8 = GameList[dwCurrentGameID].OnlineInfo.Id - '0';
			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_FindLobby, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);

		}
		else if ((pExceptionAddr == Orig_SwitchGames) && (dwCurrentGameID != -1))
		{
			if (ExceptionInfo->ContextRecord->Rax == SF2HF_ID) 
			{
				if (strcmp(GameList[dwCurrentGameID].Name, "Street Fighter 2 Champion Edition") == 0)
					ExceptionInfo->ContextRecord->Rax = SF2CE_ID;
			}
			else if (ExceptionInfo->ContextRecord->Rax == SFA3_ID) 
			{
				if (strcmp(GameList[dwCurrentGameID].Name, "Street Fighter Alpha 2") == 0)
					ExceptionInfo->ContextRecord->Rax = SFA2_ID;
				if (strcmp(GameList[dwCurrentGameID].Name, "Hyper Street Fighter II : The Anniversary Edition") == 0)
					ExceptionInfo->ContextRecord->Rax = SSF2_ID; // avec sfa2 c'est mieux
			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_SwitchGames, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == Orig_GetSize)
		{
			if ((dwCurrentGameID != -1) && (GameList[dwCurrentGameID].System == CPS1))
			{
				// save state (for multi)
				if (bIsMulti)
				{
					if (ExceptionInfo->ContextRecord->Rax == SF2HF_SAVESTATE_SIZE)
					{
						ExceptionInfo->ContextRecord->Rax = SF2HF_SAVESTATE_SIZE;
						dwDataSize = SF2HF_SAVESTATE_SIZE;
					}
				}
				// logo
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_LOGO_SIZE)
				{
					if (GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwLogoSize)
					{
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwLogoSize;
						dwDataSize = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwLogoSize;
					}
				}
				// 68k
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_68K_SIZE)
				{
					if (GameList[dwCurrentGameID].CoinOffset)
					{
						bPatchCoinsApplied = FALSE;
						InstallHook(Orig_PatchCoins, &OrigByte_PatchCoins);
						MessageBoxA(NULL, "Applying patch, may take a while...", "patching", MB_OK);
					}
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dw68kSize;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dw68kSize;
				}

				// vrom
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_VROM_SIZE)
				{
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwVromSize;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwVromSize;
				}

				// oki
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_OKI_SIZE)
				{
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwOkiSize;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwOkiSize;
				}
				// z80
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_Z80_SIZE)
				{
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwZ80Size;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwZ80Size;
				}
			}
			else if ((dwCurrentGameID != -1) && (GameList[dwCurrentGameID].System == CPS2))
			{
				// Z80
				if (ExceptionInfo->ContextRecord->Rax == SSF2_Z80_SIZE)
				{
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwZ80Size;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwZ80Size;
				}
				// QS
				if (ExceptionInfo->ContextRecord->Rax == SSF2_QS_SIZE)
				{
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwQsSize;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwQsSize;
				}
				// 68k
				if (ExceptionInfo->ContextRecord->Rax == SSF2_68K_SIZE)
				{
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dw68kSize;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dw68kSize;
				}
				// 68y
				if (ExceptionInfo->ContextRecord->Rax == SSF2_68Y_SIZE)
				{
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dw68ySize;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dw68ySize;
				}
				// vrom
				if (ExceptionInfo->ContextRecord->Rax == SSF2_VROM_SIZE)
				{
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwVromSize;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwVromSize;
				}
				// nv
				if (ExceptionInfo->ContextRecord->Rax == NV_SIZE)
				{
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwNvSize;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwNvSize;
				}
				// save state (for multi)
				if (bIsMulti)
				{
					if (ExceptionInfo->ContextRecord->Rax == SFA3_SAVESTATE_SIZE)
					{
						ExceptionInfo->ContextRecord->Rax = SFA3_SAVESTATE_SIZE;
						dwDataSize = SFA3_SAVESTATE_SIZE;
					}
				}
				// logo
				if (ExceptionInfo->ContextRecord->Rax == SFA3_LOGO_SIZE)
				{
					if (GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwLogoSize)
					{
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwLogoSize;
						dwDataSize = GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwLogoSize;
					}
				}
			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_GetSize, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == Orig_GetData)
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_GetData, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		ExceptionInfo->ContextRecord->EFlags |= 0x100;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else if (dwExceptionCode == STATUS_SINGLE_STEP)
	{
		if ((pExceptionAddr == (PVOID)((LPBYTE)Orig_PatchCoins + 4)) && (bPatchCoinsApplied == FALSE))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_PatchCoins, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_CreateLobby + 3))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_CreateLobby, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_FindLobby + 5))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_FindLobby, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_SwitchGames + 3))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_SwitchGames, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_GetSize + 3))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_GetSize, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_GetData + 3))
		{
			if ((dwCurrentGameID != -1) && (GameList[dwCurrentGameID].System == CPS1))
			{
				// save state (for multi)
				if (bIsMulti)
				{
					if (dwDataSize == SF2HF_SAVESTATE_SIZE)
					{
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_SAVESTATE_FILE);
						dwDataSize = 0;
					}
				}

				// logo
				if ((dwDataSize == GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwLogoSize) && (GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwLogoSize))
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_LOGO_FILE);
					dwDataSize = 0;
				}
				// oki
				if (dwDataSize == GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwOkiSize)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_OKI_FILE);
					dwDataSize = 0;
				}
				// z80
				if (dwDataSize == GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwZ80Size)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_Z80_FILE);
					dwDataSize = 0;
				}
				// 68k
				if (dwDataSize == GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dw68kSize)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_68K_FILE);
					dwDataSize = 0;
				}
				// vrom
				if (dwDataSize == GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS1.dwVromSize)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_VROM_FILE);
					dwDataSize = 0;
				}
			}
			else if ((dwCurrentGameID != -1) && (GameList[dwCurrentGameID].System == CPS2))
			{

				// z80
				if (dwDataSize == GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwZ80Size)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_Z80_FILE);
					dwDataSize = 0;
				}
				// QS
				if (dwDataSize == GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwQsSize)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_QS_FILE);
					dwDataSize = 0;
				}
				// 68k / 68y
				if (dwDataSize == GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dw68kSize)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, currentPath);
					dwDataSize = 0;
					currentPath = (currentPath == PATH_68K_FILE) ? PATH_68Y_FILE : PATH_68K_FILE;
				}
				// vrom
				if (dwDataSize == GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwVromSize)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_VROM_FILE);
					dwDataSize = 0;
				}
				// nv
				if (dwDataSize == GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwNvSize)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_NV_FILE);
					dwDataSize = 0;
				}
				// save state (for multi)
				if (bIsMulti)
				{
					if (dwDataSize == SFA3_SAVESTATE_SIZE)
					{
						// other check to ensure that current save state != 2x 
						PCHAR GameData = (PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize);
						if (GameData[8] != '\x44')
							PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_SAVESTATE_FILE);
						dwDataSize = 0;
					}
				}
				// logo
				if ((dwDataSize == GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwLogoSize) && (GameList[dwCurrentGameID].RomsInfo.RomsInfoCPS2.dwLogoSize))
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_LOGO_FILE);
					dwDataSize = 0;
				}

			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_GetData, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		ExceptionInfo->ContextRecord->EFlags &= ~(0x100);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

INT GetGameInfo(PBYTE hash)
{
	DWORD i, j;
	BYTE dbHash[SHA1_HASH_SIZE];

	if (hash != NULL)
	{
		for (i = 0; i < NUMBER_OF_GAMES; i++)
		{
			for (j = 0; j < SHA1_HASH_SIZE; j++)
				sscanf_s(GameList[i].Hash + 2 * j, "%02x", &dbHash[j]);

			if (memcmp(hash, dbHash, SHA1_HASH_SIZE) == 0)
				return i;
		}
	}
	return -1;
}

BOOL IsMulti()
{
	HANDLE hFile;

	if (GameList[dwCurrentGameID].bIsMulti == FALSE)
		return FALSE;

	hFile = CreateFileA(PATH_SAVESTATE_FILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(NULL, "Missing .sav file in roms folder", "Error", MB_ICONERROR);
		return FALSE;
	}
	CloseHandle(hFile);
	return TRUE;

}

BOOL IsEuroVersion()
{
	CHAR version[MAX_PATH];
	GetPrivateProfileStringA("MooHijack", "version", "", version, MAX_PATH, ".\\config.ini");
	if (strcmp(version, "european") == 0)
		return TRUE;
	return FALSE;
}

INT CheckROM()
{
	HANDLE hFile;
	DWORD dwBytesRead, dwFileSize;
	PBYTE buf = NULL;
	BYTE hash[SHA1_HASH_SIZE];

	if ((hFile = CreateFileA(PATH_68K_FILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		return -1;
	if ((dwFileSize = GetFileSize(hFile, NULL)) == 0)
		return -1;

	if ((buf = (PBYTE)malloc(dwFileSize * sizeof(BYTE))) == NULL)
		exit(EXIT_FAILURE);
	ReadFile(hFile, buf, dwFileSize, &dwBytesRead, NULL);
	CloseHandle(hFile);

	HashSHA1(buf, dwFileSize, hash);
	dwCurrentGameID = GetGameInfo(hash);
	free(buf);

	return dwCurrentGameID;
}

DWORD WINAPI Payload(LPVOID lpParameter)
{
	DWORD dwOldProtect;
	PVOID GameBaseAddr = GetModuleBaseAddress(L"SF30");

	DB_Init();

	// fix offsets for international version
	if (IsEuroVersion() == FALSE)
	{
		OFFSET_LOAD_STATE = 0x1A43E0;
		OFFSET_CPS1 = 0x2BF0B0;
		OFFSET_GETSIZE = 0x1487f8;
		OFFSET_GETDATA = 0x14883A;
		OFFSET_CPS1_CODE_TO_PATCH = 0x1AEE05;
		OFFSET_SWITCH_GAMES = 0x1D274;
		OFFSET_SPECTATOR_MODE = 0x4EE1C;
		OFFSET_TRAINING_ALPHA2 = 0x1B6B50;
		OFFSET_TRAINING_SF2CE = 0x1AAF45;
		OFFSET_CREATE_LOBBY = 0x227F4;
		OFFSET_GAME_VERSION = 0x248C10;
		OFFSET_SSF2_END_MATCH_CALLBACK = 0x1BC15A;
		
		patchSpectator = "\xb0\x01\x90\x90\x90\x90";
	}

	dwCurrentGameID = CheckROM();

	Sleep(2000);

	// patch to automatically set the spectator mode
	PatchInMemory(GameBaseAddr, OFFSET_SPECTATOR_MODE, patchSpectator);

	// basic training mode for alpha 2 
	PatchInMemory(GameBaseAddr, OFFSET_TRAINING_ALPHA2, "\x90\x90");
	PatchInMemory(GameBaseAddr, OFFSET_TRAINING_ALPHA2 + 0x25, "\x90\x90");
	PatchInMemory(GameBaseAddr, OFFSET_TRAINING_ALPHA2 + 0x50, "\x90\x90\x90\x90\x90\x90\x41\xB8\x90\x00\x00\x00");

	// basic training mode for sf2ce
	PatchInMemory(GameBaseAddr, OFFSET_TRAINING_SF2CE, "\x90\x90");
	PatchInMemory(GameBaseAddr, OFFSET_TRAINING_SF2CE + 0x25, "\x90\x90");
	PatchInMemory(GameBaseAddr, OFFSET_TRAINING_SF2CE + 0x30, "\x90\x90\x90\x90\x90\x90\x41\xB8\x90\x00\x00\x00");

	// game version (for online)
	PatchInMemory(GameBaseAddr, OFFSET_GAME_VERSION, "\x70\x61\x74\x63\x68\x65\x64\x00");

	// additional game to be load
	if (dwCurrentGameID != -1)
	{
		if (GameList[dwCurrentGameID].System == CPS1)
		{
			PatchCPS1GameSettings(GameBaseAddr);
			
			// patch code (additional gfx map used by Moo)
			PatchInMemory(GameBaseAddr, OFFSET_CPS1_CODE_TO_PATCH, "\xb0\x01\x90\x90");

			// patch load state
			if (GameList[dwCurrentGameID].CoinOffset)
				PatchInMemory(GameBaseAddr, OFFSET_LOAD_STATE, "\xc3");
		}
		else if (GameList[dwCurrentGameID].System == CPS2)
		{
			if (GameList[dwCurrentGameID].OffsetEndMatch != NULL)
				PatchInMemory(GameBaseAddr, OFFSET_SSF2_END_MATCH_CALLBACK, GameList[dwCurrentGameID].OffsetEndMatch);
		}
		bIsMulti = IsMulti();
	}

	currentPath = PATH_68K_FILE;

	// install VEH handler
	if ((VEHhandler = AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler)) == NULL)
		return 0;

	Orig_GetSize = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_GETSIZE);
	Orig_GetData = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_GETDATA);
	Orig_PatchCoins = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_PATCH_COINS);
	Orig_SwitchGames = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_SWITCH_GAMES);
	Orig_CreateLobby = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_CREATE_LOBBY);
	
	PVOID SteamClientBase = GetModuleBaseAddress(L"steamclient64");
	Orig_FindLobby = (PVOID)((LPBYTE)SteamClientBase + OFFSET_FIND_LOBBY);

	InstallHook(Orig_CreateLobby, &OrigByte_CreateLobby);
	InstallHook(Orig_FindLobby, &OrigByte_FindLobby);

	// additional game to be load
	if (dwCurrentGameID != -1)
	{
		InstallHook(Orig_GetSize, &OrigByte_GetSize);
		InstallHook(Orig_GetData, &OrigByte_GetData);
		InstallHook(Orig_SwitchGames, &OrigByte_SwitchGames);
	}
	return 1;
}
BOOL APIENTRY DllMain(
	_In_ HMODULE hInstDll,
	_In_ DWORD fdwReason,
	_In_ LPVOID lpReserved)
{
	DWORD dwThreadId = 0;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Payload, lpReserved, 0, &dwThreadId);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}