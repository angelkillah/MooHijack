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
		else if (pExceptionAddr == Orig_StartMatch)
		{
			OutputDebugStringA("Match started");

			if (bMatchFinished == TRUE)
				bMatchFinished = FALSE;
			bBackToLobby = FALSE;

			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_StartMatch, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == Orig_FtSettings)
		{
			OutputDebugStringA("Getting FT settings...");

			if (strcmpi(ExceptionInfo->ContextRecord->Rax, "1") == 0)
				dwFirstTo = 1;
			else if (strcmpi(ExceptionInfo->ContextRecord->Rax, "2") == 0)
				dwFirstTo = 2;
			else if (strcmpi(ExceptionInfo->ContextRecord->Rax, "3") == 0)
				dwFirstTo = 3;
			else if (strcmpi(ExceptionInfo->ContextRecord->Rax, "0") == 0)
				dwFirstTo = 5;

			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_FtSettings, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == Orig_2xEndMatch)
		{
			if (bMatchFinished == FALSE)
			{
				OutputDebugStringA("2X match finished");
				bMatchFinished = TRUE;

				// P1 won the match
				if ((ExceptionInfo->ContextRecord->Rax & 0xFF) == 0)
				{
					OutputDebugStringA("P1 won");
					dwVictoryCountP1++;
				}
				// P2 won the match
				else
				{
					OutputDebugStringA("P2 won");
					dwVictoryCountP2++;
				}
				// P1 won the FT
				if (dwVictoryCountP1 == dwFirstTo)
				{
					OutputDebugStringA("P1 won the set");
					dwVictoryCountP1 = 0;
					dwVictoryCountP2 = 0;
					bBackToLobby = TRUE;
				}
				// P2 won the FT
				else if (dwVictoryCountP2 == dwFirstTo)
				{
					OutputDebugStringA("P2 won the set");
					dwVictoryCountP1 = 0;
					dwVictoryCountP2 = 0;
					bBackToLobby = TRUE;
				}
				else
					bBackToLobby = FALSE;
			}
			
			// there are still matches to play
			if(bBackToLobby == FALSE)
			{	// don't go back to lobby !
				char plop[100];
				sprintf(plop, "FT%d => score : %d - %d", dwFirstTo, dwVictoryCountP1, dwVictoryCountP2);
				OutputDebugStringA(plop);
				ExceptionInfo->ContextRecord->Rcx = 0;
			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &Orig_2xEndMatch, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == Orig_ssf2EndMatch)
		{
			if (strcmp(GameList[dwCurrentGameID[2]].Name, "vsav") == 0)
				ExceptionInfo->ContextRecord->Rsi = 0x8BF0;
			else if (strcmp(GameList[dwCurrentGameID[2]].Name, "hsf2") == 0)
				ExceptionInfo->ContextRecord->Rsi = 0x831f;
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_ssf2EndMatch, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == Orig_SwitchGames)
		{
			OutputDebugStringA("SwitchGames");
			
			// game_0
			if (ExceptionInfo->ContextRecord->Rax == SF2HF_ID) 
			{
				OutputDebugStringA("SF2HF chosen");
				if (dwCurrentGameID[0] != -1)
				{
					if (strcmp(GameList[dwCurrentGameID[0]].Name, "sf2ceua") == 0)
						ExceptionInfo->ContextRecord->Rax = SF2CE_ID;
				}
			}
			// game_1
			else if (ExceptionInfo->ContextRecord->Rax == SSF2X_ID)
			{
				OutputDebugStringA("2X chosen");
				bDoLoadCPS2 = FALSE;
			}
			// game_2
			else if (ExceptionInfo->ContextRecord->Rax == SFA3_ID)
			{
				OutputDebugStringA("SFA3 chosen");
				if (dwCurrentGameID[2] != -1)
				{
					bDoLoadCPS2 = TRUE;
					if (strcmp(GameList[dwCurrentGameID[2]].Name, "sfa2") == 0)
						ExceptionInfo->ContextRecord->Rax = SFA2_ID;
					else if (strcmp(GameList[dwCurrentGameID[2]].Name, "ssf2t") == 0)
						ExceptionInfo->ContextRecord->Rax = SSF2X_ID;
					else
						ExceptionInfo->ContextRecord->Rax = SSF2_ID;
				}
			}
			// game_3
			else if (ExceptionInfo->ContextRecord->Rax == SF33_ID)
			{
				if (dwCurrentGameID[3] != -1)
					ExceptionInfo->ContextRecord->Rax = SF32_ID;
			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_SwitchGames, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == Orig_CPS1)
		{
			OutputDebugStringA("CPS1");
			dwCurrentSystem = CPS1;

			for (int i = 0; i < 4; i++)
				bDataSF2HFound[i] = FALSE;

			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_CPS1, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == Orig_CPS2)
		{
			OutputDebugStringA("CPS2");
			dwCurrentSystem = CPS2;
			
			for (int i = 0; i < 5; i++)
			{
				bDataSSF2Found[i] = FALSE;
				bDataSSF2XFound[i] = FALSE;
			}
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_CPS2, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == Orig_CPS3)
		{
			OutputDebugStringA("CPS3");
			dwCurrentSystem = CPS3;

			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(pExceptionAddr, &OrigByte_CPS3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == Orig_GetSize)
		{
			if (dwCurrentGameID[0] != -1)
			{
				// logo
				if ((ExceptionInfo->ContextRecord->Rax == SF2HF_LOGO_SIZE) || (ExceptionInfo->ContextRecord->Rax == SF2HF_JAP_LOGO_SIZE))
				{
					if (GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwLogoSize)
					{
						OutputDebugStringA("SF2HF logo found");
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwLogoSize;
						dwDataSize = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwLogoSize;
					}
				}
			}
			if (dwCurrentGameID[2] != -1)
			{
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
				else if (ExceptionInfo->ContextRecord->Rax == SSF2_BORDER_TOP_SIZE)
				{
					if (GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwBorderTop)
					{
						dwBorderSize = GetBorderSize(GameList[dwCurrentGameID[2]].Name, PATH_SSF2_TOP_BORDER);
						if (dwBorderSize > 0)
						{
							OutputDebugStringA("SSF2 border top found");
							ExceptionInfo->ContextRecord->Rax = dwBorderSize;
							dwDataSize = dwBorderSize;
						}
					}
				}
				else if (ExceptionInfo->ContextRecord->Rax == SSF2_BORDER_BOTTOM_SIZE)
				{
					if (GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwBorderBottom)
					{
						dwBorderSize = GetBorderSize(GameList[dwCurrentGameID[2]].Name, PATH_SSF2_BOTTOM_BORDER);
						if(dwBorderSize > 0)
						{
							OutputDebugStringA("SSF2 border bottom found");
							ExceptionInfo->ContextRecord->Rax = dwBorderSize;
							dwDataSize = dwBorderSize;
						}
					}
				}
				else if (ExceptionInfo->ContextRecord->Rax == SSF2_BORDER_RIGHT_SIZE)
				{
					if (GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwBorderRight)
					{
						dwBorderSize = GetBorderSize(GameList[dwCurrentGameID[2]].Name, PATH_SSF2_RIGHT_BORDER);
						if(dwBorderSize > 0)
						{
							OutputDebugStringA("SSF2 border right found");
							ExceptionInfo->ContextRecord->Rax = dwBorderSize;
							dwDataSize = dwBorderSize;
						}
					}
				}
				else if (ExceptionInfo->ContextRecord->Rax == SSF2_BORDER_LEFT_SIZE)
				{
					if (GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwBorderLeft)
					{
						dwBorderSize = GetBorderSize(GameList[dwCurrentGameID[2]].Name, PATH_SSF2_LEFT_BORDER);
						if(dwBorderSize > 0)
						{
							OutputDebugStringA("SSF2 border left found");
							ExceptionInfo->ContextRecord->Rax = dwBorderSize;
							dwDataSize = dwBorderSize;
						}
					}
				}
			}
			if (dwCurrentGameID[3] != -1)
			{
				// logo
				if (ExceptionInfo->ContextRecord->Rax == SF33_LOGO_SIZE)
				{
					if (GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwLogoSize)
					{
						OutputDebugStringA("SF33 logo found");
						ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwLogoSize;
						dwDataSize = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwLogoSize;
					}
				}
				else if (ExceptionInfo->ContextRecord->Rax == SF3_BORDER_TOP_SIZE)
				{
					if (GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwBorderTop)
					{
						dwBorderSize = GetBorderSize(GameList[dwCurrentGameID[3]].Name, PATH_SF3_TOP_BORDER);
						if(dwBorderSize > 0)
						{
							OutputDebugStringA("SF3 border top found");
							ExceptionInfo->ContextRecord->Rax = dwBorderSize;
							dwDataSize = dwBorderSize;
						}
					}
				}
				else if (ExceptionInfo->ContextRecord->Rax == SF3_BORDER_BOTTOM_SIZE)
				{
					if (GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwBorderBottom)
					{
						dwBorderSize = GetBorderSize(GameList[dwCurrentGameID[3]].Name, PATH_SF3_BOTTOM_BORDER);
						{
							OutputDebugStringA("SF3 border bottom found");
							ExceptionInfo->ContextRecord->Rax = dwBorderSize;
							dwDataSize = dwBorderSize;
						}
					}
				}
				else if (ExceptionInfo->ContextRecord->Rax == SF3_BORDER_RIGHT_SIZE)
				{
					if (GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwBorderRight)
					{
						dwBorderSize = GetBorderSize(GameList[dwCurrentGameID[3]].Name, PATH_SF3_RIGHT_BORDER);
						if(dwBorderSize > 0)
						{
							OutputDebugStringA("SF3 border right found");
							ExceptionInfo->ContextRecord->Rax = dwBorderSize;
							dwDataSize = dwBorderSize;
						}
					}
				}
				else if (ExceptionInfo->ContextRecord->Rax == SF3_BORDER_LEFT_SIZE)
				{
					if (GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwBorderLeft)
					{
						dwBorderSize = GetBorderSize(GameList[dwCurrentGameID[3]].Name, PATH_SF3_LEFT_BORDER);
						if(dwBorderSize > 0)
						{
							OutputDebugStringA("SF3 border left found");
							ExceptionInfo->ContextRecord->Rax = dwBorderSize;
							dwDataSize = dwBorderSize;
						}
					}
				}
			}
			if ((dwCurrentSystem == CPS1) && (dwCurrentGameID[0] != -1))
			{
				if (ExceptionInfo->ContextRecord->Rax == SF2HF_SAVESTATE_SIZE)
				{
					OutputDebugStringA("SF2HF savestate found");
					ExceptionInfo->ContextRecord->Rax = SF2HF_SAVESTATE_SIZE;
					dwDataSize = SF2HF_SAVESTATE_SIZE;
				}
				// 68k
				else if ((ExceptionInfo->ContextRecord->Rax == SF2HF_68K_SIZE) && (bDataSF2HFound[0] == FALSE))
				{
					OutputDebugStringA("SF2HF 68k found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dw68kSize;
					dwDataSize = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dw68kSize;
				}
				// vrom
				else if ((ExceptionInfo->ContextRecord->Rax == SF2HF_VROM_SIZE) && (bDataSF2HFound[1] == FALSE))
				{
					OutputDebugStringA("SF2HF VROM found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwVromSize;
					dwDataSize = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwVromSize;
				}
				// oki
				else if ((ExceptionInfo->ContextRecord->Rax == SF2HF_OKI_SIZE) && (bDataSF2HFound[2] == FALSE))
				{
					OutputDebugStringA("SF2HF OKI found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwOkiSize;
					dwDataSize = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwOkiSize;
				}
				// z80
				else if ((ExceptionInfo->ContextRecord->Rax == SF2HF_Z80_SIZE) && (bDataSF2HFound[3] == FALSE))
				{
					OutputDebugStringA("SF2HF Z80 found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwZ80Size;
					dwDataSize = GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwZ80Size;
				}
			}
			else if ((dwCurrentSystem == CPS2) && (dwCurrentGameID[2] != -1) && (bDoLoadCPS2 == TRUE))
			{
				// save state 
				if (ExceptionInfo->ContextRecord->Rax == SSF2_SAVESTATE_SIZE)
				{
					OutputDebugStringA("SSF2 savestate found");
					ExceptionInfo->ContextRecord->Rax = SSF2_SAVESTATE_SIZE;
					dwDataSize = SSF2_SAVESTATE_SIZE;
				}
				// Z80
				else if ((ExceptionInfo->ContextRecord->Rax == SSF2_Z80_SIZE) && (bDataSSF2Found[0] == FALSE))
				{
					OutputDebugStringA("SSF2 Z80 found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwZ80Size;
					dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwZ80Size;
				}
				// 2X Z80
				else if ((ExceptionInfo->ContextRecord->Rax == SSF2X_Z80_SIZE) && (bDataSSF2XFound[0] == FALSE))
				{
					OutputDebugStringA("SSF2X Z80 found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwZ80Size;
					dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwZ80Size;
					bDataSSF2XFound[0] = TRUE;
				}
				// QS
				else if ((ExceptionInfo->ContextRecord->Rax == SSF2_QS_SIZE) && (bDataSSF2Found[1] == FALSE))
				{
					OutputDebugStringA("SSF2 QS found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwQsSize;
					dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwQsSize;
				}
				// 68k
				else if ((ExceptionInfo->ContextRecord->Rax == SSF2_68K_SIZE) && (bDataSSF2Found[2] == FALSE))
				{
					OutputDebugStringA("SSF2 68K found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68kSize;
					dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68kSize;
					bDataSSF2Found[2] = TRUE;
				}
				// 2X 68k
				else if ((ExceptionInfo->ContextRecord->Rax == SSF2X_68K_SIZE) && (bDataSSF2XFound[2] == FALSE))
				{
					OutputDebugStringA("SSF2X 68K found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68kSize;
					dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68kSize;
					bDataSSF2XFound[2] = TRUE;
				}
				// 68y
				else if ((ExceptionInfo->ContextRecord->Rax == SSF2_68Y_SIZE) && (bDataSSF2Found[3] == FALSE))
				{
					OutputDebugStringA("SSF2 68Y found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68ySize;
					dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68ySize;
					bDataSSF2Found[3] = TRUE;
				}
				// 2x 68y
				else if ((ExceptionInfo->ContextRecord->Rax == SSF2X_68Y_SIZE) && (bDataSSF2XFound[3] == FALSE))
				{
					OutputDebugStringA("SSF2X 68Y found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68ySize;
					dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dw68ySize;
					bDataSSF2XFound[3] = TRUE;
				}
				// vrom
				else if ((ExceptionInfo->ContextRecord->Rax == SSF2_VROM_SIZE) && (bDataSSF2Found[4] == FALSE))
				{
					OutputDebugStringA("SSF2 VROM found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwVromSize;
					dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwVromSize;
				}
				// 2x vrom
				else if ((ExceptionInfo->ContextRecord->Rax == SSF2X_VROM_SIZE) && (bDataSSF2XFound[4] == FALSE))
				{
					OutputDebugStringA("SSF2X VROM found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwVromSize;
					dwDataSize = GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwVromSize;
					bDataSSF2XFound[4] = TRUE;
				}
			}
			else if ((dwCurrentSystem == CPS3) && (dwCurrentGameID[3] != -1))
			{
				// save state 
				if (ExceptionInfo->ContextRecord->Rax == SF32_SAVESTATE_SIZE)
				{
					OutputDebugStringA("SF32 savestate found");
					ExceptionInfo->ContextRecord->Rax = SF32_SAVESTATE_SIZE;
					dwDataSize = SF32_SAVESTATE_SIZE;
				}
				
				if (ExceptionInfo->ContextRecord->Rax == SF32_BIOS_SIZE)
				{
					OutputDebugStringA("SF32 Bios found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwBiosSize;
					dwDataSize = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwBiosSize;
				}
				else if (ExceptionInfo->ContextRecord->Rax == SF32_S1_SIZE)
				{
					OutputDebugStringA("SF32 S1 found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS1Size;
					dwDataSize = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS1Size;
				}
				else if (ExceptionInfo->ContextRecord->Rax == SF32_S2_SIZE)
				{
					OutputDebugStringA("SF32 S2 found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS2Size;
					dwDataSize = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS2Size;
				}
				else if (ExceptionInfo->ContextRecord->Rax == SF32_S3_SIZE)
				{
					OutputDebugStringA("SF32 S3 found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS3Size;
					dwDataSize = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS3Size;
				}
				else if (ExceptionInfo->ContextRecord->Rax == SF32_S4_SIZE)
				{
					OutputDebugStringA("SF32 S4 found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS4Size;
					dwDataSize = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS4Size;
				}
				else if (ExceptionInfo->ContextRecord->Rax == SF32_S5_SIZE)
				{
					OutputDebugStringA("SF32 S5 found");
					ExceptionInfo->ContextRecord->Rax = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS5Size;
					dwDataSize = GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS5Size;
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
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_2xEndMatch + 2))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_2xEndMatch, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_ssf2EndMatch + 2))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_ssf2EndMatch, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_FtSettings + 3))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_FtSettings, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_StartMatch + 7))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_StartMatch, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_SwitchGames + 3))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_SwitchGames, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_CPS1 + 3))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_CPS1, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_CPS2 + 5))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_CPS2, &int3, 1);
			VirtualProtect(pExceptionAddr, 1, dwOldProtect, &dwOldProtect);
		}
		else if (pExceptionAddr == (PVOID)((LPBYTE)Orig_CPS3 + 3))
		{
			VirtualProtect(pExceptionAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect);
			memcpy(Orig_CPS3, &int3, 1);
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
			if (dwCurrentGameID[0] != -1)
			{
				// logo
				if ((dwDataSize == GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwLogoSize) && (GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwLogoSize))
				{
					OutputDebugStringA("cps1 logo patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[0]].Name, PATH_LOGO_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
			}
			if (dwCurrentGameID[2] != -1)
			{
				// logo
				if ((dwDataSize == GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwLogoSize) && (GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwLogoSize))
				{
					OutputDebugStringA("cps2 logo patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_LOGO_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// top border
				else if ((dwDataSize == GetBorderSize(GameList[dwCurrentGameID[2]].Name, PATH_SSF2_TOP_BORDER)) && (GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwBorderTop))
				{
					OutputDebugStringA("cps2 top border patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_SSF2_TOP_BORDER);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// bottom border
				else if ((dwDataSize == GetBorderSize(GameList[dwCurrentGameID[2]].Name, PATH_SSF2_BOTTOM_BORDER)) && (GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwBorderBottom))
				{
					OutputDebugStringA("cps2 bottom border patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_SSF2_BOTTOM_BORDER);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// left border
				else if ((dwDataSize == GetBorderSize(GameList[dwCurrentGameID[2]].Name, PATH_SSF2_LEFT_BORDER)) && (GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwBorderLeft))
				{
					OutputDebugStringA("cps2 left border patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_SSF2_LEFT_BORDER);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// right border
				else if ((dwDataSize == GetBorderSize(GameList[dwCurrentGameID[2]].Name, PATH_SSF2_RIGHT_BORDER)) && (GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwBorderRight))
				{
					OutputDebugStringA("cps2 right border patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_SSF2_RIGHT_BORDER);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
			}
			if (dwCurrentGameID[3] != -1)
			{
				// logo
				if ((dwDataSize == GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwLogoSize) && (GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwLogoSize))
				{
					OutputDebugStringA("cps3 logo patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[3]].Name, PATH_LOGO_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// top border
				else if ((dwDataSize == GetBorderSize(GameList[dwCurrentGameID[3]].Name, PATH_SF3_TOP_BORDER)) && (GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwBorderTop))
				{
					OutputDebugStringA("cps3 top border patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[3]].Name, PATH_SF3_TOP_BORDER);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// bottom border
				else if ((dwDataSize == GetBorderSize(GameList[dwCurrentGameID[3]].Name, PATH_SF3_BOTTOM_BORDER)) && (GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwBorderBottom))
				{
					OutputDebugStringA("cps3 bottom border patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[3]].Name, PATH_SF3_BOTTOM_BORDER);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// left border
				else if ((dwDataSize == GetBorderSize(GameList[dwCurrentGameID[3]].Name, PATH_SF3_LEFT_BORDER)) && (GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwBorderLeft))
				{
					OutputDebugStringA("cps3 left border patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[3]].Name, PATH_SF3_LEFT_BORDER);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// right border
				else if ((dwDataSize == GetBorderSize(GameList[dwCurrentGameID[3]].Name, PATH_SF3_RIGHT_BORDER)) && (GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwBorderRight))
				{
					OutputDebugStringA("cps3 right border patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[3]].Name, PATH_SF3_RIGHT_BORDER);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
			}
			if ((dwCurrentSystem == CPS1) && (dwCurrentGameID[0] != -1))
			{
				// save state
				if (dwDataSize == SF2HF_SAVESTATE_SIZE)
				{
					OutputDebugStringA("SF2HF savestate patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[0]].Name, PATH_SAVESTATE_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// oki
				else if (dwDataSize == GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwOkiSize)
				{
					HashSHA1((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), 0x100, hash);
					if (memcmp(hash, "\x3B\x9C\x49\xEA\x6A\xFB\x82\xEF\xD5\x12\x1D\x69\x02\x1C\x5F\x05\x32\xF7\xD0\x4E", SHA1_HASH_SIZE) != 0)
					{
						OutputDebugStringA("[-] SF2H oki hash mismatch !");
						bDataSF2HFound[2] = FALSE;
					}
					else
					{
						OutputDebugStringA("cps1 oki patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[0]].Name, PATH_OKI_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						bDataSF2HFound[2] = TRUE;
					}
					dwDataSize = 0;
				}
				// z80
				else if (dwDataSize == GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwZ80Size)
				{
					HashSHA1((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), 0x100, hash);
					if (memcmp(hash, "\x94\xDD\x94\x4F\x44\x7B\xE2\xF4\x4E\xD3\xFA\x53\x3A\x2E\xDD\x29\xFC\x74\x9C\x9A", SHA1_HASH_SIZE) != 0)
					{
						OutputDebugStringA("[-] SF2H z80 hash mismatch !");
						bDataSF2HFound[3] = FALSE;
					}
					else
					{
						OutputDebugStringA("cps1 z80 patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[0]].Name, PATH_Z80_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						bDataSF2HFound[3] = TRUE;
					}
					dwDataSize = 0;
				}
				// 68k
				else if (dwDataSize == GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dw68kSize)
				{
					HashSHA1((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), 0x100, hash);
					if ((memcmp(hash, "\x90\x39\x23\x40\x40\xFF\x1B\x09\x03\x10\x48\x71\x17\x45\xAF\x13\x5E\x48\x1C\x29", SHA1_HASH_SIZE) != 0) && (strcmp(GameList[dwCurrentGameID[0]].Name, "sf2ceua") != 0))
					{
						OutputDebugStringA("[-] SF2H 68k hash mismatch !");
						bDataSF2HFound[0] = FALSE;
					}
					else
					{
						OutputDebugStringA("cps1 68k patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[0]].Name, PATH_68K_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						bDataSF2HFound[0] = TRUE;
					}
					dwDataSize = 0;
				}
				// vrom
				else if (dwDataSize == GameList[dwCurrentGameID[0]].RomsInfo.RomsInfoCPS1.dwVromSize)
				{
					HashSHA1((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), 0x100, hash);
					if (memcmp(hash, "\xAA\x48\xE8\xDF\xC4\xFB\xFD\xCF\x34\x62\x28\x46\xC8\x90\xD9\x54\xB9\x6B\xC3\x07", SHA1_HASH_SIZE) != 0)
					{
						OutputDebugStringA("[-] SF2H vrom hash mismatch !");
						bDataSF2HFound[1] = FALSE;
					}
					else
					{
						OutputDebugStringA("cps1 vrom patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[0]].Name, PATH_VROM_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						bDataSF2HFound[1] = TRUE;
					}
					dwDataSize = 0;
				}
			}
			else if ((dwCurrentSystem == CPS2) && (dwCurrentGameID[2] != -1) && (bDoLoadCPS2 == TRUE))
			{
				// save state
				if (dwDataSize == SSF2_SAVESTATE_SIZE)
				{
					// other check to ensure that current save state != 2x modified state (:
					PCHAR GameData = (PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize);
					if (GameData[8] != '\x44')
					{
						OutputDebugStringA("cps2 savestate patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_SAVESTATE_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					}
					dwDataSize = 0;
				}
				// z80
				else if ((dwDataSize == GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwZ80Size) && (bDataSSF2Found[0] == FALSE))
				{
					OutputDebugStringA("cps2 z80 patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_Z80_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
					bDataSSF2Found[0] = TRUE;
				}
				// QS
				else if ((dwDataSize == GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwQsSize) && (bDataSSF2Found[1] == FALSE))
				{
					OutputDebugStringA("cps2 qs patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_QS_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
					bDataSSF2Found[1] = TRUE;
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
				else if ((dwDataSize == GameList[dwCurrentGameID[2]].RomsInfo.RomsInfoCPS2.dwVromSize) && (bDataSSF2Found[4] == FALSE))
				{
					OutputDebugStringA("cps2 vrom patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[2]].Name, PATH_VROM_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
					bDataSSF2Found[4] = TRUE;
				}
			}
			else if ((dwCurrentSystem == CPS3) && (dwCurrentGameID[3] != -1))
			{
				// save state
				if (dwDataSize == SF32_SAVESTATE_SIZE)
				{
					PCHAR GameData = (PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize);
					OutputDebugStringA("cps3 savestate patched");
					sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[3]].Name, PATH_SAVESTATE_FILE);
					PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
					dwDataSize = 0;
				}
				// Bios
				if (dwDataSize == GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwBiosSize)
				{
					HashSHA1((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), 0x100, hash);
					if (memcmp(hash, "\xFA\x3D\xA6\x20\x74\xB4\x43\xB7\x1F\x54\x3A\x51\x8A\x7B\x99\x2D\xEF\x81\xF8\xB9", SHA1_HASH_SIZE) == 0)
					{
						OutputDebugStringA("cps3 bios patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[3]].Name, PATH_BIOS_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						dwDataSize = 0;
					}
				}
				// S1
				if (dwDataSize == GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS1Size)
				{
					HashSHA1((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), 0x100, hash);
					if (memcmp(hash, "\x33\x5F\xA7\x49\x58\x6A\x81\xD9\xE8\x1F\x46\x9A\xC3\x70\x2C\x6F\x67\x6A\x95\x66", SHA1_HASH_SIZE) == 0)
					{
						OutputDebugStringA("cps3 s1 patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[3]].Name, PATH_S1_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						dwDataSize = 0;
					}
				}
				// S2
				if (dwDataSize == GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS2Size)
				{
					HashSHA1((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), 0x100, hash);
					if (memcmp(hash, "\x56\xDC\xA2\x2B\xA3\x82\xF5\x8D\x0A\xE9\xFD\x9C\xB9\x16\x81\x6D\x7D\x97\xAD\x66", SHA1_HASH_SIZE) == 0)
					{
						OutputDebugStringA("cps3 s2 patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[3]].Name, PATH_S2_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						dwDataSize = 0;
					}
				}
				// S3
				if (dwDataSize == GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS3Size)
				{
					HashSHA1((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), 0x100, hash);
					if (memcmp(hash, "\x32\x39\x0E\x41\x03\x96\x86\xAC\x02\x91\xA6\x2E\x3B\xC3\x35\xC5\xF3\x11\xFE\x6C", SHA1_HASH_SIZE) == 0)
					{
						OutputDebugStringA("cps3 s3 patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[3]].Name, PATH_S3_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						dwDataSize = 0;
					}
				}
				// S4
				if (dwDataSize == GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS4Size)
				{
					HashSHA1((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), 0x100, hash);
					if (memcmp(hash, "\x9C\xAC\xE3\xC6\x07\xFA\x39\x35\xBB\x4D\xE8\x6A\x55\xC2\xED\x5E\x1F\x4A\xE2\xE1", SHA1_HASH_SIZE) == 0)
					{
						OutputDebugStringA("cps3 s4 patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[3]].Name, PATH_S4_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						dwDataSize = 0;
					}
				}
				// S5
				if (dwDataSize == GameList[dwCurrentGameID[3]].RomsInfo.RomsInfoCPS3.dwS5Size)
				{
					HashSHA1((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), 0x100, hash);
					if (memcmp(hash, "\xA8\x47\xC6\x88\x45\x82\x07\xF0\x51\xA8\x64\x68\xF0\x41\x60\xF2\xBC\xF2\x4D\xD4", SHA1_HASH_SIZE) == 0)
					{
						OutputDebugStringA("cps3 s5 patched");
						sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[3]].Name, PATH_S5_FILE);
						PatchGameData((PVOID)(LPBYTE)(ExceptionInfo->ContextRecord->R10 - dwDataSize), dwDataSize, gamePath);
						dwDataSize = 0;
					}
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

BOOL IsTrainingMode()
{
	CHAR training[MAX_PATH];
	GetPrivateProfileStringA("MooHijack", "training", "", training, MAX_PATH, ".\\config.ini");
	if (strcmp(training, "enable") == 0)
	{
		OutputDebugStringA("Training enabled");
		return TRUE;
	}
	OutputDebugStringA("Training disabled");
	return FALSE;
}

BOOL CheckROM()
{
	HANDLE hFile;
	DWORD dwBytesRead, dwFileSize;
	PBYTE buf = NULL;
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
			// CPS3
			if(i == 3)
				sprintf(gamePath, ".\\db\\%s\\%s", GameList[dwCurrentGameID[i]].Name, PATH_S1_FILE);
			else
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
		OFFSET_SSF2_ENDMATCH_CALLBACK = 0x1BC150;
		OFFSET_FIND_LOBBY = 0x4B5A5;
		OFFSET_SSF2_NV = 0x2C3B10;
		OFFSET_SSF2_VROM = 0x2C3B18;
		OFFSET_SSF2X_NV = 0x2C46E0;
		OFFSET_SSF2X_VROM = 0x2C46E8;
		OFFSET_CPS1_SETUP = 0x1A53A0;
		OFFSET_CPS2_SETUP = 0x1B1710;
		OFFSET_CPS3_SETUP = 0x1C8FF0;
		OFFSET_SF32_KEYS = 0x2C7550;
		OFFSET_SF32_ENDMATCH_CALLBACK = 0x1CFE09;
		OFFSET_SF32_ENDMATCH_CALLBACK_2 = 0x1CEDF0;
		OFFSET_SSFT2_ENDMATCH_CALLBACK = 0x1C1516;
		OFFSET_FT_SETTINGS = 0x21F3C;
		OFFSET_SSF2T_STARTMATCH_CALLBACK = 0xB0A0;
		OFFSET_SSF2_ENDMATCH_CALLBACK_1 = 0x1BB2CC;  
		OFFSET_SSF2_ENDMATCH_CALLBACK_2 = 0x1BB2C0; 
	}

	if (CheckROM() == FALSE)
	{
		MessageBoxA(NULL, "Chosen games are not found in db folder", "Not found", MB_ICONERROR);
		return -1;
	}

	Sleep(2000);

	if (IsTrainingMode())
	{
		OutputDebugStringA("training mode on");

		// game version (for online)
		PatchInMemory(GameBaseAddr, OFFSET_GAME_VERSION, "\x74\x72\x61\x69\x6e\x69\x6e\x00", 8);

		// CPS2
		if (dwCurrentGameID[2] != -1)
		{
			if (strcmp(GameList[dwCurrentGameID[2]].Name, "vsav") == 0)
				IsEuroVersion() ? PatchVampireSaviorEU(GameBaseAddr) : PatchVampireSaviorJP(GameBaseAddr);
			else if (strcmp(GameList[dwCurrentGameID[2]].Name, "sfa2") == 0)
				IsEuroVersion() ? PatchSFA2EU(GameBaseAddr) : PatchSFA2JP(GameBaseAddr);
		}
		// SFA3
		else
			IsEuroVersion() ? PatchSFA3EU(GameBaseAddr) : PatchSFA3JP(GameBaseAddr);
		
		// CPS3
		if (dwCurrentGameID[3] != -1)
		{
			if(strcmp(GameList[dwCurrentGameID[3]].Name, "sf32") == 0)
				IsEuroVersion() ? PatchSF32EU(GameBaseAddr) : PatchSF32JP(GameBaseAddr);
			else if(strcmp(GameList[dwCurrentGameID[3]].Name, "jojoban") == 0)
				IsEuroVersion() ? PatchJojoEU(GameBaseAddr) : PatchJojoJP(GameBaseAddr);
		}
	}
	// patch cheat callbacks to set coins for arcade mode
	else
	{
		// CPS2
		if (dwCurrentGameID[2] != -1)
		{
			OutputDebugStringA("Patching CPS2 credits...");
			if (strcmp(GameList[dwCurrentGameID[2]].Name, "hsf2") == 0)
				IsEuroVersion() ? PatchCoinsHSF2EU(GameBaseAddr) : PatchCoinsHSF2JP(GameBaseAddr);
			else if (strcmp(GameList[dwCurrentGameID[2]].Name, "ssf2t") == 0)
				IsEuroVersion() ? PatchCoinsNLEU(GameBaseAddr) : PatchCoinsNLJP(GameBaseAddr);
			else if (strcmp(GameList[dwCurrentGameID[2]].Name, "vsav") == 0)
				IsEuroVersion() ? PatchCoinsVsavEU(GameBaseAddr) : PatchCoinsVsavJP(GameBaseAddr);
		}
		// CPS3
		if (dwCurrentGameID[3] != -1)
		{
			OutputDebugStringA("Patching CPS3 credits...");
			if (strcmp(GameList[dwCurrentGameID[3]].Name, "jojoban") == 0)
				IsEuroVersion() ? PatchCoinsJojoEU(GameBaseAddr) : PatchCoinsJojoJP(GameBaseAddr);
		}
	}

	// patch to automatically set the spectator mode
	PatchInMemory(GameBaseAddr, OFFSET_SPECTATOR_MODE, patchSpectator, strlen(patchSpectator));
	
	// additional CPS1 game chosen
	if ((dwCurrentGameID[0] != -1) && (dwCurrentGameID[0] != 4))
	{
		PatchCPS1GameSettings(GameBaseAddr);
		PatchInMemory(GameBaseAddr, OFFSET_CPS1_CALLBACKS, "\xb0\x01\x90\x90", 4);
	}

	// patch CPS2 emulator callbacks 
	if(dwCurrentGameID[2] != -1)
	{
		if (GameList[dwCurrentGameID[2]].CallbacksInfo.OffsetEndMatch != 0)
		{
			PatchInMemory(GameBaseAddr, OFFSET_SSF2_ENDMATCH_CALLBACK, GameList[dwCurrentGameID[2]].CallbacksInfo.OffsetEndMatch, strlen(GameList[dwCurrentGameID[2]].CallbacksInfo.OffsetEndMatch));
			PatchInMemory(GameBaseAddr, OFFSET_SSF2_ENDMATCH_CALLBACK_2, GameList[dwCurrentGameID[2]].CallbacksInfo.OffsetP2WinCount, strlen(GameList[dwCurrentGameID[2]].CallbacksInfo.OffsetP2WinCount));
		}
	}

	if (dwCurrentGameID[3] != -1)
	{
		// patch CPS3 emulator callbacks
		if (strcmp(GameList[dwCurrentGameID[3]].Name, "jojoban") == 0)
		{
			if (GameList[dwCurrentGameID[3]].CallbacksInfo.OffsetEndMatch != 0)
			{
				PatchInMemory(GameBaseAddr, OFFSET_SF32_ENDMATCH_CALLBACK, GameList[dwCurrentGameID[3]].CallbacksInfo.OffsetEndMatch, 4);
				PatchInMemory(GameBaseAddr, OFFSET_SF32_ENDMATCH_CALLBACK_2, GameList[dwCurrentGameID[3]].CallbacksInfo.OffsetP2WinCount, 4);
			}
		}
		// patch SF32 cps3 keys
		if(strcmp(GameList[dwCurrentGameID[3]].Name, "jojoban") == 0)
			PatchInMemory(GameBaseAddr, OFFSET_SF32_KEYS, "\x32\x23\xe3\x3e\x02\x03\x72\x19\x00\x00", 10);
	}

	currentPath = PATH_68K_FILE;

	// install VEH handler
	if ((VEHhandler = AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler)) == NULL)
		return 0;

	Orig_GetSize = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_GETSIZE);
	Orig_GetData = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_GETDATA);
	Orig_SwitchGames = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_SWITCH_GAMES);
	Orig_CreateLobby = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_CREATE_LOBBY);
	Orig_FindLobby = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_FIND_LOBBY);
	Orig_CPS1 = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_CPS1_SETUP);
	Orig_CPS2 = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_CPS2_SETUP);
	Orig_CPS3 = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_CPS3_SETUP);
	Orig_2xEndMatch = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_SSFT2_ENDMATCH_CALLBACK);
	Orig_FtSettings = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_FT_SETTINGS);
	Orig_StartMatch = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_SSF2T_STARTMATCH_CALLBACK);
	Orig_ssf2EndMatch = (PVOID)((LPBYTE)GameBaseAddr + OFFSET_SSF2_ENDMATCH_CALLBACK_1);

	InstallHook(Orig_CreateLobby, &OrigByte_CreateLobby);
	InstallHook(Orig_FindLobby, &OrigByte_FindLobby);
	InstallHook(Orig_2xEndMatch, &OrigByte_2xEndMatch);
	InstallHook(Orig_FtSettings, &OrigByte_FtSettings);
	InstallHook(Orig_StartMatch, &OrigByte_StartMatch);

	// additional game to be load
	for (int i = 0; i < 4; i++)
	{
		if (dwCurrentGameID[i] != -1)
		{
			InstallHook(Orig_GetSize, &OrigByte_GetSize);
			InstallHook(Orig_GetData, &OrigByte_GetData);
			InstallHook(Orig_SwitchGames, &OrigByte_SwitchGames);
			InstallHook(Orig_CPS1, &OrigByte_CPS1);
			InstallHook(Orig_CPS2, &OrigByte_CPS2);
			InstallHook(Orig_CPS3, &OrigByte_CPS3);
			InstallHook(Orig_ssf2EndMatch, &OrigByte_ssf2EndMatch);
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