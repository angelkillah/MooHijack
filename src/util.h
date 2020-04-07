#pragma once

#include <Windows.h>

#define SHA1_HASH_SIZE	20

VOID HashSHA1(PBYTE pData, DWORD dwDataLen, PBYTE pHash);
VOID PatchGameData(PVOID GameData, DWORD dwDataSize, PCHAR pPathFile);
VOID PatchInMemory(PVOID GameBaseAddr, INT Offset, PCHAR Data);