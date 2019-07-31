#include <Windows.h>
#include <psapi.h>
#include <stdio.h>

#include "database.h"
#include "util.h"

#define MAX_MODULES				500

// CPS1
#define PATH_OKI_FILE			"C:\\roms\\rom.oki"
#define PATH_Z80_FILE			"C:\\roms\\rom.z80"
#define PATH_68K_FILE			"C:\\roms\\rom.68k"
#define PATH_VROM_FILE			"C:\\roms\\rom.vrom"
#define PATH_LOGO_FILE			"C:\\roms\\logo.png"
#define PATH_SAVESTATE_FILE		"C:\\roms\\multi.sav"

#define SF2HF_68K_SIZE		    0x180000  
#define SF2HF_VROM_SIZE			0x600000
#define SF2HF_OKI_SIZE			0x40000
#define SF2HF_Z80_SIZE			0x10000
#define SF2HF_LOGO_SIZE			0x35D84   
#define SF2HF_SAVESTATE_SIZE	0x5487A

// fix ssf2x online speed
#define SSF2T_SAVESTATE_SIZE		0x5B57F
#define PATH_SSF2T_SAVESTATE_FILE	"C:\\roms\\ssf2t.sav"

#define OFFSET_LOAD_STATE			0x1CFE30   
#define OFFSET_PATCH_GHOULS			0x220CF6   // patch number of coins in memory (for ghouls) 

#define OFFSET_CPS1					0x2BD520
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

#define OFFSET_GETSIZE				0x148238
#define OFFSET_GETDATA				0x14827A
#define OFFSET_CPS1_CODE_TO_PATCH	0x1A9E55

PVOID VEHhandler;

PVOID Orig_GetSize, Orig_GetData, Orig_PatchGhouls;
BYTE OrigByte_GetSize, OrigByte_GetData, OrigByte_PatchGhouls;
BYTE int3[] = "\xcc";

DWORD dwDataSize = 0;
BOOL bSSF2T_FixSpeed = FALSE;
BOOL bIsMulti = FALSE;
BOOL bGhoulsPatchApplied = FALSE;

VOID __declspec(dllexport) _()
{
}

PVOID GetGameBaseAddress()
{
	HMODULE hModules[MAX_MODULES];
	MODULEINFO ModuleInfo = { 0 };
	DWORD dwBytesNeeded, i;

	HANDLE hCurrentProcess = GetCurrentProcess();

	while(1)
	{ 
		if (EnumProcessModules(hCurrentProcess, hModules, sizeof(hModules), &dwBytesNeeded))
		{
			for (i = 0; i < (dwBytesNeeded / sizeof(HMODULE)); i++)
			{
				TCHAR szModName[MAX_PATH];
				if (GetModuleFileNameEx(hCurrentProcess, hModules[i], szModName,
					sizeof(szModName) / sizeof(TCHAR)))
				{
					if (wcsstr(szModName, L"SF30") != NULL)
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

VOID PatchGhouls(PVOID CoinAddr)
{
	DWORD dwOldProtect;
	VirtualProtect(CoinAddr, sizeof(WORD), PAGE_READWRITE, &dwOldProtect);
	memcpy(CoinAddr, "\x20\x20", sizeof(WORD));
	VirtualProtect(CoinAddr, sizeof(WORD), dwOldProtect, &dwOldProtect);
}

VOID PatchCPS1GameSettings(PVOID GameBaseAddress)
{
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_CLOCK), (PCHAR)&GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwCpuClockRate, sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_CPSB), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwCPSB), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_CTRL), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwCtrl), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PALCTRL), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwPalctrl), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PRIORITY_MASK), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwPriorityMask[0]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PRIORITY_MASK + 8), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwPriorityMask[1]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PRIORITY_MASK + 16), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwPriorityMask[2]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PRIORITY_MASK + 24), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwPriorityMask[3]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_MULTIPLY_PROTECTION), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwMultiplyProtection[0]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_MULTIPLY_PROTECTION + 8), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwMultiplyProtection[1]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_MULTIPLY_PROTECTION + 16), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwMultiplyProtection[2]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_MULTIPLY_PROTECTION + 24), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwMultiplyProtection[3]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_IN2), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwIn2), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_ID), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwID), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_LAYER_MASK), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.CpsbInfo.dwLayerEnableMask), sizeof(DWORD)*5);
	
	PVOID GFXMapper = *(PVOID*)((LPBYTE)GameBaseAddress + OFFSET_GFX_MAPPER);	
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SPRITES), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.MapperGfx.GfxSpritesInfo), sizeof(GFX_SPRITES) * 3);
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SPRITES_END), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.MapperGfx.dwGfxSpritesEndMarker), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SCROLL1), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.MapperGfx.GfxScrollInfo[0]), sizeof(GFX_SCROLL));
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SCROLL2), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.MapperGfx.GfxScrollInfo[1]), sizeof(GFX_SCROLL));
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SCROLL3), (PCHAR)&(GameList[dwCurrentGameID].GameInfo.MapperGfx.GfxScrollInfo[2]), sizeof(GFX_SCROLL));
}

VOID InstallHook(PVOID addr, BYTE *bOrig)
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
		if ((pExceptionAddr == Orig_PatchGhouls) && (bGhoulsPatchApplied == FALSE))
		{
			// patch specific to Daimakaimura
			if (strcmp(GameList[dwCurrentGameID].Name, "Daimakaimura") == 0)
			{
				if ((ExceptionInfo->ContextRecord->Rax == 0x646) && ((ExceptionInfo->ContextRecord->Rcx & 0xFFFF) == 0))
				{
					PatchGhouls((PVOID)(ExceptionInfo->ContextRecord->Rax + ExceptionInfo->ContextRecord->Rdx));
					MessageBoxA(NULL, "Patch correctly applied", "ghouls", MB_OK);
					bGhoulsPatchApplied = TRUE;
				}
			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_PatchGhouls, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}

		else if (pExceptionAddr == Orig_GetSize)
		{
			// SSF2X fix speed
			if (bSSF2T_FixSpeed)
			{
				if (ExceptionInfo->ContextRecord->Rax == SSF2T_SAVESTATE_SIZE)
				{
					ExceptionInfo->ContextRecord->Rax = SSF2T_SAVESTATE_SIZE;
					dwDataSize = SSF2T_SAVESTATE_SIZE;
				}
			}

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
					if (GameList[dwCurrentGameID].RomsInfo.dwLogoSize)
					{
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.dwLogoSize;
						dwDataSize = GameList[dwCurrentGameID].RomsInfo.dwLogoSize;
					}
				}
				// 68k
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_68K_SIZE)
				{
					if (strcmp(GameList[dwCurrentGameID].Name, "Daimakaimura") == 0)
					{
						bGhoulsPatchApplied = FALSE;
						InstallHook(Orig_PatchGhouls, &OrigByte_PatchGhouls);
						MessageBoxA(NULL, "Applying patch, may take a while...", "patching", MB_OK);
					}
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.dw68kSize;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.dw68kSize;
				}

				// vrom
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_VROM_SIZE)
				{
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.dwVromSize;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.dwVromSize;
				}

				// oki
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_OKI_SIZE)
				{
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.dwOkiSize;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.dwOkiSize;
				}
				// z80
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_Z80_SIZE)
				{
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID].RomsInfo.dwZ80Size;
					dwDataSize = GameList[dwCurrentGameID].RomsInfo.dwZ80Size;
				}
			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_GetSize, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if(pExceptionAddr == Orig_GetData)
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
		if ((pExceptionAddr == (PVOID)((LPBYTE)Orig_PatchGhouls + 4)) && (bGhoulsPatchApplied == FALSE))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_PatchGhouls, &int3, 1);
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
			// SSF2X fix speed
			if (bSSF2T_FixSpeed)
			{
				if (dwDataSize == SSF2T_SAVESTATE_SIZE)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_SSF2T_SAVESTATE_FILE);
					dwDataSize = 0;
				}
			}
			
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
				if ((dwDataSize == GameList[dwCurrentGameID].RomsInfo.dwLogoSize) && (GameList[dwCurrentGameID].RomsInfo.dwLogoSize))
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_LOGO_FILE);
					dwDataSize = 0;
				}
				// oki
				if (dwDataSize == GameList[dwCurrentGameID].RomsInfo.dwOkiSize)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_OKI_FILE);
					dwDataSize = 0;
				}
				// z80
				if (dwDataSize == GameList[dwCurrentGameID].RomsInfo.dwZ80Size)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_Z80_FILE);
					dwDataSize = 0;
				}
				// 68k
				if (dwDataSize == GameList[dwCurrentGameID].RomsInfo.dw68kSize)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_68K_FILE);
					dwDataSize = 0;
				}
				// vrom
				if (dwDataSize == GameList[dwCurrentGameID].RomsInfo.dwVromSize)
				{
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, PATH_VROM_FILE);
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

BOOL IsSSF2T_Hack()
{
	HANDLE hFile;
	hFile = CreateFileA(PATH_SSF2T_SAVESTATE_FILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;
	CloseHandle(hFile);
	return TRUE;
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
	PVOID GameBaseAddr = GetGameBaseAddress();
	PVOID AddrToPatch = NULL;
	
	bSSF2T_FixSpeed = IsSSF2T_Hack();
	DB_Init();
	
	if ((CheckROM() == -1) && (bSSF2T_FixSpeed == FALSE))
		return 1;
	
	Sleep(2000);

	if (dwCurrentGameID != -1)
	{
		MessageBoxA(NULL, GameList[dwCurrentGameID].Name, "game found", MB_OK);
		if (GameList[dwCurrentGameID].System == CPS1)
			PatchCPS1GameSettings(GameBaseAddr);
		bIsMulti = IsMulti();
	}

	// install VEH handler
	if ((VEHhandler = AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler)) == NULL)
		return 0;
	
	// patch code (additional gfx map used by moo)
	AddrToPatch = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_CPS1_CODE_TO_PATCH);
	VirtualProtect(AddrToPatch, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	memcpy(AddrToPatch, "\xb0\x01\x90\x90", 4);
	VirtualProtect(AddrToPatch, 4, dwOldProtect, &dwOldProtect);

	// patch load state
	if (dwCurrentGameID != -1)
	{
		if (GameList[dwCurrentGameID].bLoadState == FALSE)
		{
			AddrToPatch = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_LOAD_STATE);
			VirtualProtect(AddrToPatch, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(AddrToPatch, "\xc3", 1);
			VirtualProtect(AddrToPatch, 1, dwOldProtect, &dwOldProtect);
		}
	}

	Orig_GetSize = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_GETSIZE);
	Orig_GetData = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_GETDATA);
	Orig_PatchGhouls = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_PATCH_GHOULS);

	InstallHook(Orig_GetSize, &OrigByte_GetSize);
	InstallHook(Orig_GetData, &OrigByte_GetData);

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