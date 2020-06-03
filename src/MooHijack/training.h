#pragma once

#include <Windows.h>
#include "util.h"

VOID PatchVampireSaviorEU(PVOID GameBaseAddr);
VOID PatchVampireSaviorJP(PVOID GameBaseAddr);
VOID PatchSFA3EU(PVOID GameBaseAddr);
VOID PatchSFA3JP(PVOID GameBaseAddr);
VOID PatchSFA2EU(PVOID GameBaseAddr);
VOID PatchSFA2JP(PVOID GameBaseAddr);
VOID PatchSF32EU(PVOID GameBaseAddr);
VOID PatchSF32JP(PVOID GameBaseAddr);
VOID PatchJojoEU(PVOID GameBaseAddr);
VOID PatchJojoJP(PVOID GameBaseAddr);

VOID PatchCoinsHSF2EU(PVOID GameBaseAddr);
VOID PatchCoinsHSF2JP(PVOID GameBaseAddr);
VOID PatchCoinsNLEU(PVOID GameBaseAddr);
VOID PatchCoinsNLJP(PVOID GameBaseAddr);
VOID PatchCoinsVsavEU(PVOID GameBaseAddr);
VOID PatchCoinsVsavJP(PVOID GameBaseAddr);
VOID PatchCoinsJojoEU(PVOID GameBaseAddr);
VOID PatchCoinsJojoJP(PVOID GameBaseAddr);


