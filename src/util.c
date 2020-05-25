#include "util.h"

#define MAX_MODULES				500

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

DWORD GetBorderSize(PCHAR pPathFolder, PCHAR pPathFile)
{
	HANDLE hFile;
	DWORD dwSize = -1;
	CHAR gamePath[MAX_PATH];
	sprintf(gamePath, ".\\db\\%s\\%s", pPathFolder, pPathFile);
	hFile = CreateFileA(gamePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
		dwSize = GetFileSize(hFile, NULL);
	CloseHandle(hFile);
	return dwSize;
}

VOID PatchGameData(PVOID GameData, DWORD dwDataSize, PCHAR pPathFile)
{
	HANDLE hFile;
	DWORD dwOldProtect;
	DWORD dwBytesRead;

	hFile = CreateFileA(pPathFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		VirtualProtect(GameData, dwDataSize, PAGE_READWRITE, &dwOldProtect);
		ReadFile(hFile, GameData, dwDataSize, &dwBytesRead, NULL);
		VirtualProtect(GameData, dwDataSize, dwOldProtect, &dwOldProtect);
		CloseHandle(hFile);
	}
}

VOID PatchInMemory(PVOID Addr, INT Offset, PCHAR Data, DWORD Length)
{
	PVOID AddrToPatch = NULL;
	DWORD dwOldProtect;
	AddrToPatch = (PVOID)((LPBYTE)Addr + Offset);
	VirtualProtect(AddrToPatch, Length, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	memcpy(AddrToPatch, Data, Length);
	VirtualProtect(AddrToPatch, Length, dwOldProtect, &dwOldProtect);
}

VOID HashSHA1(PBYTE pData, DWORD dwDataLen, PBYTE pHash)
{
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	DWORD dwSize = SHA1_HASH_SIZE;

	if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == 0)
		goto clean;

	if (CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash) == 0)
		goto clean;

	if (CryptHashData(hHash, pData, dwDataLen, 0) == 0)
		goto clean;
	if (CryptGetHashParam(hHash, HP_HASHVAL, pHash, &dwSize, 0) == 0)
		goto clean;

clean:
	if (hHash)
		CryptDestroyHash(hHash);
	if (hProv)
		CryptReleaseContext(hProv, 0);
}