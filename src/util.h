#pragma once

#include <Windows.h>
#include <psapi.h>

#define SHA1_HASH_SIZE	20

PVOID GetModuleBaseAddress(PWCHAR moduleName);
VOID HashSHA1(PBYTE pData, DWORD dwDataLen, PBYTE pHash);
VOID PatchGameData(PVOID GameData, DWORD dwDataSize, PCHAR pPathFile);
VOID PatchInMemory(PVOID Addr, INT Offset, PCHAR Data, DWORD Length);
DWORD GetBorderSize(PCHAR pPathFolder, PCHAR pPathFile);
