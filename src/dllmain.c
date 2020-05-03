#include "dllmain.h"

VOID __declspec(dllexport) _()
{
}

VOID PatchCPS1GameSettings(PVOID GameBaseAddress)
{
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_CLOCK), (PCHAR)&GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwCpuClockRate, sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_CPSB), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwCPSB), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_CTRL), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwCtrl), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PALCTRL), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwPalctrl), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PRIORITY_MASK), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwPriorityMask[0]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PRIORITY_MASK + 8), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwPriorityMask[1]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PRIORITY_MASK + 16), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwPriorityMask[2]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_PRIORITY_MASK + 24), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwPriorityMask[3]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_MULTIPLY_PROTECTION), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwMultiplyProtection[0]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_MULTIPLY_PROTECTION + 8), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwMultiplyProtection[1]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_MULTIPLY_PROTECTION + 16), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwMultiplyProtection[2]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_MULTIPLY_PROTECTION + 24), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwMultiplyProtection[3]), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_IN2), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwIn2), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_ID), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwID), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GameBaseAddress + OFFSET_LAYER_MASK), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.CpsbInfo.dwLayerEnableMask), sizeof(DWORD) * 5);

	PVOID GFXMapper = *(PVOID*)((LPBYTE)GameBaseAddress + OFFSET_GFX_MAPPER);
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SPRITES), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.MapperGfx.GfxSpritesInfo), sizeof(GFX_SPRITES) * 3);
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SPRITES_END), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.MapperGfx.dwGfxSpritesEndMarker), sizeof(DWORD));
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SCROLL1), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.MapperGfx.GfxScrollInfo[0]), sizeof(GFX_SCROLL));
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SCROLL2), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.MapperGfx.GfxScrollInfo[1]), sizeof(GFX_SCROLL));
	memcpy((PVOID)((LPBYTE)GFXMapper + OFFSET_GFX_SCROLL3), (PCHAR) & (GameList[dwCurrentGameID[0]].GameInfo.MapperGfx.GfxScrollInfo[2]), sizeof(GFX_SCROLL));
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
		if (pExceptionAddr == Orig_CreateLobby)
		{		
			OutputDebugStringA("CreateLobby");

			// SSF2X 
			if ((strcmpi(ExceptionInfo->ContextRecord->R8, "game_1") == 0) && (strcmpi(ExceptionInfo->ContextRecord->R9, "1") == 0))
			{
				OutputDebugStringA("SSF2X");
				*(PVOID*)((LPBYTE)ExceptionInfo->ContextRecord->R9) = SSF2X_PATCHED_ONLINE_ID;
			}
			// patch others games ID
			for (int i = 0; i < 4; i++)
			{
				if ((dwCurrentGameID[i] != -1) && (GameList[dwCurrentGameID[i]].OnlineInfo.Slot))
				{
					if ((strcmpi(ExceptionInfo->ContextRecord->R8, GameList[dwCurrentGameID[i]].OnlineInfo.Slot) == 0) && (strcmpi(ExceptionInfo->ContextRecord->R9, "1") == 0))
					{
						OutputDebugStringA("game_id patched");
						*(PVOID*)((LPBYTE)ExceptionInfo->ContextRecord->R9) = GameList[dwCurrentGameID[i]].OnlineInfo.Id;
					}
				}
			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_CreateLobby, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		// AddRequestLobbyListNumericalFilter
		else if (pExceptionAddr == Orig_FindLobby)
		{
			OutputDebugStringA("FindLobby");

			// SSF2X
			if (strcmpi(ExceptionInfo->ContextRecord->Rdx, "game_1") == 0)
			{
				OutputDebugStringA("SSF2X");
				ExceptionInfo->ContextRecord->R9 = (SSF2X_PATCHED_ONLINE_ID - '0') - 1;
			}
			// patch others games ID
			for (int i = 0; i < 4; i++)
			{
				if ((dwCurrentGameID[i] != -1) && (GameList[dwCurrentGameID[i]].OnlineInfo.Slot))
				{
					if (strcmpi(ExceptionInfo->ContextRecord->Rdx, GameList[dwCurrentGameID[i]].OnlineInfo.Slot) == 0)
					{
						OutputDebugStringA("game_id patched");
						ExceptionInfo->ContextRecord->R9 = (GameList[dwCurrentGameID[i]].OnlineInfo.Id - '0') - 1;
					}
				}
			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_FindLobby, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);

		}
		else if (pExceptionAddr == Orig_SwitchGames)
		{
			OutputDebugStringA("SwitchGames");
			
			// game_1
			if (ExceptionInfo->ContextRecord->Rax == SF2HF_ID) 
			{
				OutputDebugStringA("SF2HF chosen");
				if (dwCurrentGameID[0] != -1)
				{
					if (strcmp(GameList[dwCurrentGameID[0]].Name, "sf2ceua") == 0)
						ExceptionInfo->ContextRecord->Rax = SF2CE_ID;
				}
			}
			// game_3
			else if (ExceptionInfo->ContextRecord->Rax == SFA3_ID)
			{
				OutputDebugStringA("SFA3 chosen");
				if (dwCurrentGameID[2] != -1)
				{
					bDoLoadCPS2 = TRUE;
					if (strcmp(GameList[dwCurrentGameID[2]].Name, "sfa2") == 0)
						ExceptionInfo->ContextRecord->Rax = SFA2_ID;
					if (strcmp(GameList[dwCurrentGameID[2]].Name, "hsf2") == 0)
						ExceptionInfo->ContextRecord->Rax = SSF2_ID;
					if (strcmp(GameList[dwCurrentGameID[2]].Name, "ssf2t") == 0)
						ExceptionInfo->ContextRecord->Rax = SSF2X_ID;
				}
			}
			else if (ExceptionInfo->ContextRecord->Rax == SSF2X_ID)
			{
				OutputDebugStringA("2X chosen");
				bDoLoadCPS2 = FALSE;
			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_SwitchGames, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == Orig_GetSize)
		{
			// CPS1
			if (dwCurrentGameID[0] != -1)
			{
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_SAVESTATE_SIZE)
				{
					OutputDebugStringA("SF2HF savestate found");
					ExceptionInfo->ContextRecord->Rax = SF2HF_SAVESTATE_SIZE;
					dwDataSize = SF2HF_SAVESTATE_SIZE;
				}
				// logo
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_LOGO_SIZE)
				{
					if (GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwLogoSize)
					{
						OutputDebugStringA("SF2HF logo found");
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwLogoSize;
						dwDataSize = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwLogoSize;
					}
				}
				// 68k
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_68K_SIZE)
				{
					OutputDebugStringA("SF2HF 68k found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dw68kSize;
					dwDataSize = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dw68kSize;
				}

				// vrom
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_VROM_SIZE)
				{
					OutputDebugStringA("SF2HF VROM found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwVromSize;
					dwDataSize = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwVromSize;
				}

				// oki
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_OKI_SIZE)
				{
					OutputDebugStringA("SF2HF OKI found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwOkiSize;
					dwDataSize = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwOkiSize;
				}
				// z80
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_Z80_SIZE)
				{
					OutputDebugStringA("SF2HF Z80 found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwZ80Size;
					dwDataSize = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwZ80Size;
				}
			}
			// CPS2
			if (dwCurrentGameID[2] != -1)
			{
				if (bDoLoadCPS2 == TRUE)
				{
					// Z80
					if (ExceptionInfo->ContextRecord->Rax == SSF2_Z80_SIZE)
					{
						OutputDebugStringA("SSF2 Z80 found");
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwZ80Size;
						dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwZ80Size;
					}
					// 2X Z80
					else if (ExceptionInfo->ContextRecord->Rax == SSF2X_Z80_SIZE)
					{
						OutputDebugStringA("SSF2X Z80 found");
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwZ80Size;
						dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwZ80Size;
					}
					// QS
					else if (ExceptionInfo->ContextRecord->Rax == SSF2_QS_SIZE)
					{
						OutputDebugStringA("SSF2 QS found");
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwQsSize;
						dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwQsSize;
					}
					// 68k
					else if (ExceptionInfo->ContextRecord->Rax == SSF2_68K_SIZE)
					{
						OutputDebugStringA("SSF2 68K found");
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68kSize;
						dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68kSize;
					}
					// 2X 68k
					else if (ExceptionInfo->ContextRecord->Rax == SSF2X_68K_SIZE)
					{
						OutputDebugStringA("SSF2X 68K found");
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68kSize;
						dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68kSize;
					}
					// 68y
					else if (ExceptionInfo->ContextRecord->Rax == SSF2_68Y_SIZE)
					{
						OutputDebugStringA("SSF2 68Y found");
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68ySize;
						dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68ySize;
					}
					// 2x 68y
					else if (ExceptionInfo->ContextRecord->Rax == SSF2X_68Y_SIZE)
					{
						OutputDebugStringA("SSF2X 68Y found");
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68ySize;
						dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68ySize;
					}
					// vrom
					else if (ExceptionInfo->ContextRecord->Rax == SSF2_VROM_SIZE)
					{
						OutputDebugStringA("SSF2 VROM found");
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwVromSize;
						dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwVromSize;
					}
					// 2x vrom
					else if (ExceptionInfo->ContextRecord->Rax == SSF2X_VROM_SIZE)
					{
						OutputDebugStringA("SSF2X VROM found");
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwVromSize;
						dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwVromSize;
					}
					// save state 
					else if (ExceptionInfo->ContextRecord->Rax == SFA3_SAVESTATE_SIZE)
					{
						OutputDebugStringA("SFA3 savestate found");
						ExceptionInfo->ContextRecord->Rax = SFA3_SAVESTATE_SIZE;
						dwDataSize = SFA3_SAVESTATE_SIZE;
					}
				}
				// logo
				if (ExceptionInfo->ContextRecord->Rax == SFA3_LOGO_SIZE)
				{
					if (GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwLogoSize)
					{
						OutputDebugStringA("SFA3 logo found");
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwLogoSize;
						dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwLogoSize;
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
		if (pExceptionAddr == (PVOID)((LPBYTE)Orig_CreateLobby + 3))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_CreateLobby, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_FindLobby + 4))
		{
			ExceptionInfo->ContextRecord->R9 = 0;
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
			CHAR gamePath[MAX_PATH];
			
			// CPS1
			if (dwCurrentGameID[0] != -1)
			{
				// save state
				if (dwDataSize == SF2HF_SAVESTATE_SIZE)
				{
					OutputDebugStringA("SF2HF savestate patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[0]].Name, PATH_SAVESTATE_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// logo
				else if ((dwDataSize == GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwLogoSize) && (GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwLogoSize))
				{
					OutputDebugStringA("cps1 logo patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[0]].Name, PATH_LOGO_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// oki
				else if (dwDataSize == GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwOkiSize)
				{
					PCHAR GameData = (PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize);
					if (GameData[0] == '\x00')
					{
						OutputDebugStringA("cps1 oki patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[0]].Name, PATH_OKI_FILE);
						PatchGameData(GameData, dwDataSize, gamePath);
					}
					dwDataSize = 0;
				}
				// z80
				else if (dwDataSize == GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwZ80Size)
				{
					OutputDebugStringA("cps1 z80 patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[0]].Name, PATH_Z80_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// 68k
				else if (dwDataSize == GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dw68kSize)
				{
					OutputDebugStringA("cps1 68k patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[0]].Name, PATH_68K_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// vrom
				else if (dwDataSize == GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwVromSize)
				{
					OutputDebugStringA("cps1 vrom patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[0]].Name, PATH_VROM_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
			}
			// CPS2
			if (dwCurrentGameID[2] != -1)
			{
				if (bDoLoadCPS2 == TRUE)
				{
					// z80
					if (dwDataSize == GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwZ80Size)
					{
						OutputDebugStringA("cps2 z80 patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_Z80_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						dwDataSize = 0;
					}
					// QS
					else if (dwDataSize == GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwQsSize)
					{
						OutputDebugStringA("cps2 qs patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_QS_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						dwDataSize = 0;
					}
					// 68k / 68y
					else if (dwDataSize == GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68kSize)
					{
						OutputDebugStringA("cps2 68k/68y patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, currentPath);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						dwDataSize = 0;
						if (GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68kSize == GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68ySize)
							currentPath = (currentPath == PATH_68K_FILE) ? PATH_68Y_FILE : PATH_68K_FILE;
					}

					// 68y file (if size != 68k file)
					else if (dwDataSize == GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68ySize)
					{
						if (currentPath == PATH_68K_FILE)
						{
							OutputDebugStringA("cps2 68y patched");
							sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_68Y_FILE);
							PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
							dwDataSize = 0;
						}
					}
					// vrom
					else if (dwDataSize == GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwVromSize)
					{
						OutputDebugStringA("cps2 vrom patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_VROM_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						dwDataSize = 0;
					}
					// save state
					else if (dwDataSize == SFA3_SAVESTATE_SIZE)
					{
						// other check to ensure that current save state != 2x 
						PCHAR GameData = (PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize);
						if (GameData[8] != '\x44')
						{
							OutputDebugStringA("cps2 savestate patched");
							sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_SAVESTATE_FILE);
							PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						}
						dwDataSize = 0;
					}
				}
				// logo
				if ((dwDataSize == GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwLogoSize) && (GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwLogoSize))
				{
					OutputDebugStringA("cps2 logo patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_LOGO_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
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

BOOL IsEuroVersion()
{
	CHAR version[MAX_PATH];
	GetPrivateProfileStringA("MooHijack", "version", "", version, MAX_PATH, ".\\config.ini");
	if (strcmp(version, "european") == 0)
		return TRUE;
	return FALSE;
}

BOOL CheckROM()
{
	HANDLE hFile;
	DWORD dwBytesRead, dwFileSize;
	PBYTE buf = NULL;
	BYTE hash[SHA1_HASH_SIZE];
	CHAR gameID[MAX_PATH];
	CHAR gamePath[MAX_PATH];
	int i;

	GetPrivateProfileStringA("MooHijack", "game1", "", gameID, MAX_PATH, ".\\config.ini");
	dwCurrentGameID[0] = atoi(gameID);
	GetPrivateProfileStringA("MooHijack", "game2", "", gameID, MAX_PATH, ".\\config.ini");
	dwCurrentGameID[1] = atoi(gameID);
	GetPrivateProfileStringA("MooHijack", "game3", "", gameID, MAX_PATH, ".\\config.ini");
	dwCurrentGameID[2] = atoi(gameID);
	GetPrivateProfileStringA("MooHijack", "game4", "", gameID, MAX_PATH, ".\\config.ini");
	dwCurrentGameID[3] = atoi(gameID);

	for (i = 0; i < 4; i++)
	{
		if (dwCurrentGameID[i] != -1)
		{
			sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[i]].Name, PATH_68K_FILE);
			if ((hFile = CreateFileA(gamePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
				return -1;
			if ((dwFileSize = GetFileSize(hFile, NULL)) == 0)
				return -1;
			if ((buf = (PBYTE)malloc(dwFileSize * sizeof(BYTE))) == NULL)
				exit(EXIT_FAILURE);
			ReadFile(hFile, buf, dwFileSize, &dwBytesRead, NULL);
			CloseHandle(hFile);
			
			HashSHA1(buf, dwFileSize, hash);
			free(buf);

			if (GetGameInfo(hash) == -1)
				return FALSE;
		}
	}
	return TRUE;
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
		OFFSET_CPS1_CALLBACKS = 0x1AEE05;
		OFFSET_SWITCH_GAMES = 0x1D274;
		OFFSET_SPECTATOR_MODE = 0x4EE42;
		OFFSET_CREATE_LOBBY = 0x227F4;
		OFFSET_GAME_VERSION = 0x248C10;
		OFFSET_SSF2_END_MATCH_CALLBACK = 0x1BC15A;
		OFFSET_FIND_LOBBY = 0x4B5A5;
		OFFSET_SSF2_NV = 0x2C3B10;
		OFFSET_SSF2_VROM = 0x2C3B18;
		OFFSET_SSF2X_NV = 0x2C46E0;
		OFFSET_SSF2X_VROM = 0x2C46E8;
	}

	if (CheckROM() == FALSE)
	{
		MessageBoxA(NULL, "Chosen games are not found in db folder", "Not found", MB_ICONERROR);
		return -1;
	}

	Sleep(2000);

	// patch to automatically set the spectator mode
	PatchInMemory(GameBaseAddr, OFFSET_SPECTATOR_MODE, patchSpectator);

	// game version (for online)
	PatchInMemory(GameBaseAddr, OFFSET_GAME_VERSION, "\x70\x61\x74\x63\x68\x65\x64\x00");

	// additional CPS1 game chosen
	if (dwCurrentGameID[0] != -1)
	{
		PatchCPS1GameSettings(GameBaseAddr);

		// patch cps1 callbacks
		PatchInMemory(GameBaseAddr, OFFSET_CPS1_CALLBACKS, "\xb0\x01\x90\x90");
	}

	// additional CPS2 game chosen
	if(dwCurrentGameID[2] != -1)
	{
		if (GameList[dwCurrentGameID[2]].OffsetEndMatch != NULL)
			PatchInMemory(GameBaseAddr, OFFSET_SSF2_END_MATCH_CALLBACK, GameList[dwCurrentGameID[2]].OffsetEndMatch);
	}

	// remove nv and additional vrom
	PatchInMemory(GameBaseAddr, OFFSET_SSF2X_NV, "\x00\x00\x00\x00\x00\x00\x00\x00");
	PatchInMemory(GameBaseAddr, OFFSET_SSF2X_VROM, "\x00\x00\x00\x00\x00\x00\x00\x00");
	PatchInMemory(GameBaseAddr, OFFSET_SSF2_NV, "\x00\x00\x00\x00\x00\x00\x00\x00");
	PatchInMemory(GameBaseAddr, OFFSET_SSF2_VROM, "\x00\x00\x00\x00\x00\x00\x00\x00");

	currentPath = PATH_68K_FILE;

	// install VEH handler
	if ((VEHhandler = AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler)) == NULL)
		return 0;

	Orig_GetSize = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_GETSIZE);
	Orig_GetData = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_GETDATA);
	Orig_SwitchGames = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_SWITCH_GAMES);
	Orig_CreateLobby = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_CREATE_LOBBY);
	Orig_FindLobby = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_FIND_LOBBY);
	
	InstallHook(Orig_CreateLobby, &OrigByte_CreateLobby);
	InstallHook(Orig_FindLobby, &OrigByte_FindLobby);

	// additional game to be load
	for (int i = 0; i < 4; i++)
	{
		if (dwCurrentGameID[i] != -1)
		{
			InstallHook(Orig_GetSize, &OrigByte_GetSize);
			InstallHook(Orig_GetData, &OrigByte_GetData);
			InstallHook(Orig_SwitchGames, &OrigByte_SwitchGames);
			break;
		}
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