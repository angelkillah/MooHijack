#include "training.h"

VOID PatchVampireSaviorEU(PVOID GameBaseAddr)
{
	// infinite time : FF8109 -> 63
	PatchInMemory(GameBaseAddr, 0x1B0F25, "\x90\x90", 2);      // nop; nop
	PatchInMemory(GameBaseAddr, 0x1B0F2E, "\xBA\x09\x81", 3);  // mov edx, FF8109
	PatchInMemory(GameBaseAddr, 0x1B0F33, "\x41\xb8\x63", 3);  // mov r8d, 63

	// Infinite Energy PL1 FF8450 -> 1200120
	PatchInMemory(GameBaseAddr, 0x1B0F3E, "\xBA\x50\x84\xFF\x00", 5);			// mov edx, FF8450
	PatchInMemory(GameBaseAddr, 0x1B0F43, "\x48\x8D\x8B\x98\xAE\x0C\x00", 7);	// lea rcx, qword ptr ds:[rbx+cae98]
	PatchInMemory(GameBaseAddr, 0x1B0F4A, "\x41\xB8\x20\x01\x20\x01", 6);		// mov r8d, 1200120
	PatchInMemory(GameBaseAddr, 0x1B0F50, "\xE8\x1B\x9A\xE5\xFF", 5);			// call PatchDwordInMemory

	// Infinite Energy PL2 FF8850 -> 1200120
	PatchInMemory(GameBaseAddr, 0x1B0F55, "\xBA\x50\x88\xFF\x00", 5);			// mov edx, FF8850
	PatchInMemory(GameBaseAddr, 0x1B0F5A, "\x48\x8D\x8B\x98\xAE\x0C\x00", 7);	// lea rcx, qword ptr ds:[rbx+cae98]
	PatchInMemory(GameBaseAddr, 0x1B0F61, "\x41\xB8\x20\x01\x20\x01", 6);		// mov r8d, 1200120
	PatchInMemory(GameBaseAddr, 0x1B0F67, "\xE8\x04\x9A\xE5\xFF", 5);			// call PatchDwordInMemory
	PatchInMemory(GameBaseAddr, 0x1B0F6C, "\xEB\x3D", 2);						// jmp to gadget add rsp,20; pop rbx; ret
}

VOID PatchVampireSaviorJP(PVOID GameBaseAddr)
{
	// infinite time : FF8109 -> 63
	PatchInMemory(GameBaseAddr, 0x1BC335, "\x90\x90", 2);      // nop; nop
	PatchInMemory(GameBaseAddr, 0x1BC33E, "\xBA\x09\x81", 3);  // mov edx, FF8109
	PatchInMemory(GameBaseAddr, 0x1BC343, "\x41\xb8\x63", 3);  // mov r8d, 63

	// Infinite Energy PL1 FF8450 -> 1200120
	PatchInMemory(GameBaseAddr, 0x1BC34E, "\xBA\x50\x84\xFF\x00", 5);			// mov edx, FF8450
	PatchInMemory(GameBaseAddr, 0x1BC353, "\x48\x8D\x8B\x98\xAE\x0C\x00", 7);	// lea rcx, qword ptr ds:[rbx+cae98]
	PatchInMemory(GameBaseAddr, 0x1BC35A, "\x41\xB8\x20\x01\x20\x01", 6);		// mov r8d, 1200120
	PatchInMemory(GameBaseAddr, 0x1BC360, "\xE8\x1B\xE6\xE4\xFF", 5);			// call PatchDwordInMemory

	// Infinite Energy PL2 FF8850 -> 1200120
	PatchInMemory(GameBaseAddr, 0x1BC365, "\xBA\x50\x88\xFF\x00", 5);			// mov edx, FF8850
	PatchInMemory(GameBaseAddr, 0x1BC36A, "\x48\x8D\x8B\x98\xAE\x0C\x00", 7);	// lea rcx, qword ptr ds:[rbx+cae98]
	PatchInMemory(GameBaseAddr, 0x1BC371, "\x41\xB8\x20\x01\x20\x01", 6);		// mov r8d, 1200120
	PatchInMemory(GameBaseAddr, 0x1BC377, "\xE8\x04\xE6\xE4\xFF", 5);			// call PatchDwordInMemory
	PatchInMemory(GameBaseAddr, 0x1BC37C, "\xEB\x3D", 2);						// jmp to gadget add rsp,20; pop rbx; ret
}

VOID PatchSFA3EU(PVOID GameBaseAddr)
{
	// force Infinite Time
	PatchInMemory(GameBaseAddr, 0x1AFBC0, "\x90\x90", 2);

	// force Infinite Energy PL1
	PatchInMemory(GameBaseAddr, 0x1AFBE0, "\x90\x90", 2);

	// Infinite Energy PL2 FF8850 -> 90
	PatchInMemory(GameBaseAddr, 0x1AFBF9, "\x90\x41\xB8\x90\x00\x00\x00", 7);	// nop; mov r8d, 90
	PatchInMemory(GameBaseAddr, 0x1AFC00, "\x90\x90", 2);						// nop; nop
	PatchInMemory(GameBaseAddr, 0x1AFC09, "\x90\x90\x90", 3);					// nop; nop; nop
}

VOID PatchSFA3JP(PVOID GameBaseAddr)
{
	// force Infinite Time
	PatchInMemory(GameBaseAddr, 0x1BAFD0, "\x90\x90", 2);

	// force Infinite Energy PL1
	PatchInMemory(GameBaseAddr, 0x1BAFF0, "\x90\x90", 2);

	// Infinite Energy PL2 FF8850 -> 90
	PatchInMemory(GameBaseAddr, 0x1BB009, "\x90\x41\xB8\x90\x00\x00\x00", 7);	// nop; mov r8d, 90
	PatchInMemory(GameBaseAddr, 0x1BB010, "\x90\x90", 2);						// nop; nop
	PatchInMemory(GameBaseAddr, 0x1BB019, "\x90\x90\x90", 3);					// nop; nop; nop
}

VOID PatchSFA2EU(PVOID GameBaseAddr)
{
	// force Infinite Time
	PatchInMemory(GameBaseAddr, 0x1AB740, "\x90\x90", 2);

	// force Infinite Energy PL1
	PatchInMemory(GameBaseAddr, 0x1AB765, "\x90\x90", 2);

	// Infinite Energy PL2 FF8851 -> 90, FF8853 -> 90
	PatchInMemory(GameBaseAddr, 0x1AB795, "\x41\xB8\x90\x00\x00\x00", 6);			// mov r8d, 90
	PatchInMemory(GameBaseAddr, 0x1AB79B, "\x48\x8D\x8F\x98\xAE\x0C\x00", 7);		// lea rcx, qword ptr ds:[rdi+cae98]
	PatchInMemory(GameBaseAddr, 0x1AB7A2, "\xBA\x51\x88\xFF\x00", 5);				// mov edx, ff8851
	PatchInMemory(GameBaseAddr, 0x1AB7A7, "\xE8\xA4\xF5\xE5\xFF", 5);				// call PatchByteInMemory
	PatchInMemory(GameBaseAddr, 0x1AB7AC, "\x41\xB8\x90\x00\x00\x00", 6);			// mov r8d, 90
	PatchInMemory(GameBaseAddr, 0x1AB7B2, "\x48\x8D\x8F\x98\xAE\x0C\x00", 7);		// lea rcx, qword ptr ds:[rdi+cae98]
	PatchInMemory(GameBaseAddr, 0x1AB7B9, "\xBA\x53\x88\xFF\x00\x90\x90\x90", 8);	// mov edx, FF8853; nop; nop; nop
}

VOID PatchSFA2JP(PVOID GameBaseAddr)
{
	// force Infinite Time
	PatchInMemory(GameBaseAddr, 0x1B6B50, "\x90\x90", 2);

	// force Infinite Energy PL1
	PatchInMemory(GameBaseAddr, 0x1B6B75, "\x90\x90", 2);

	// Infinite Energy PL2 FF8851 -> 90, FF8853 -> 90
	PatchInMemory(GameBaseAddr, 0x1B6BA5, "\x41\xB8\x90\x00\x00\x00", 6);			// mov r8d, 90
	PatchInMemory(GameBaseAddr, 0x1B6BAB, "\x48\x8D\x8F\x98\xAE\x0C\x00", 7);		// lea rcx, qword ptr ds:[rdi+cae98]
	PatchInMemory(GameBaseAddr, 0x1B6BB2, "\xBA\x51\x88\xFF\x00", 5);				// mov edx, ff8851
	PatchInMemory(GameBaseAddr, 0x1B6BB7, "\xE8\x74\x45\xE5\xFF", 5);				// call PatchByteInMemory
	PatchInMemory(GameBaseAddr, 0x1B6BBC, "\x41\xB8\x90\x00\x00\x00", 6);			// mov r8d, 90
	PatchInMemory(GameBaseAddr, 0x1B6BC2, "\x48\x8D\x8F\x98\xAE\x0C\x00", 7);		// lea rcx, qword ptr ds:[rdi+cae98]
	PatchInMemory(GameBaseAddr, 0x1B6BC9, "\xBA\x53\x88\xFF\x00\x90\x90\x90", 8);	// mov edx, FF8853; nop; nop; nop
}

VOID PatchSF32EU(PVOID GameBaseAddr)
{
	// force Infinite Time
	PatchInMemory(GameBaseAddr, 0x1BABD0, "\x90\x90", 2);		// nop; nop

	// force Infinite Energy PL1
	PatchInMemory(GameBaseAddr, 0x1BAC2A, "\x90\x90", 2);		// nop; nop

	// Infinite Energy PL2
	PatchInMemory(GameBaseAddr, 0x1BAC84, "\x90\x90", 2);		// nop; nop
	PatchInMemory(GameBaseAddr, 0x1BACC6, "\xB2\xA0", 2);		// mov dl, A0
	PatchInMemory(GameBaseAddr, 0x1BACBA, "\xA0", 1);
}

VOID PatchSF32JP(PVOID GameBaseAddr)
{
	// force Infinite Time
	PatchInMemory(GameBaseAddr, 0x1CF9C0, "\x90\x90", 2);		// nop; nop
	
	// force Infinite Energy PL1
	PatchInMemory(GameBaseAddr, 0x1CFA1A, "\x90\x90", 2);		// nop; nop

	// Infinite Energy PL2
	PatchInMemory(GameBaseAddr, 0x1CFA74, "\x90\x90", 2);		// nop; nop
	PatchInMemory(GameBaseAddr, 0x1CFAB6, "\xB2\xA0", 2);		// mov dl, A0
	PatchInMemory(GameBaseAddr, 0x1CFAAA, "\xA0", 1);	
}

VOID PatchJojoEU(PVOID GameBaseAddr)
{
	// Infinite Time
	PatchInMemory(GameBaseAddr, 0x1BABD0, "\x90\x90", 2);				// nop; nop
	PatchInMemory(GameBaseAddr, 0x1BAC10, "\xB9\xB4\x14\x03\x02", 5);	// mov ecx, 20314B4 
	PatchInMemory(GameBaseAddr, 0x1BAC01, "\xB4\x14\x03\x02", 4);		// 20314B4 

	// Infinite Energy PL1
	PatchInMemory(GameBaseAddr, 0x1BAC2A, "\x90\x90", 2);						// nop; nop
	PatchInMemory(GameBaseAddr, 0x1BAC63, "\xB9\xCD\x49\x03\x02\xBA\x90", 7);	// mov ecx, 20349CD; mov edx, 90 
	PatchInMemory(GameBaseAddr, 0x1BAC5A, "\xC6\x81\xCD\x49\x03\x02\x90", 7);   // mov byte ptr [rcx+20349CD], 90

	// Infinite Energy PL2
	PatchInMemory(GameBaseAddr, 0x1BAC84, "\x90\x90", 2);						// nop; nop
	PatchInMemory(GameBaseAddr, 0x1BACC1, "\xB9\xED\x4D\x03\x02\xB2\x90", 7);	// mov ecx, 2034DED; mov dl, 90 
	PatchInMemory(GameBaseAddr, 0x1BACB4, "\xC6\x81\xED\x4D\x03\x02\x90", 7);   // mov byte ptr [rcx+2034DED], 90
}

VOID PatchJojoJP(PVOID GameBaseAddr)
{
	// Infinite Time
	PatchInMemory(GameBaseAddr, 0x1CF9C0, "\x90\x90", 2);				// nop; nop
	PatchInMemory(GameBaseAddr, 0x1CFA00, "\xB9\xB4\x14\x03\x02", 5);	// mov ecx, 20314B4 
	PatchInMemory(GameBaseAddr, 0x1CF9F1, "\xB4\x14\x03\x02", 4);		// 20314B4 

	// Infinite Energy PL1
	PatchInMemory(GameBaseAddr, 0x1CFA1A, "\x90\x90", 2);						// nop; nop
	PatchInMemory(GameBaseAddr, 0x1CFA53, "\xB9\xCD\x49\x03\x02\xB2\x90", 7);	// mov ecx, 20349CD; mov dl, 90 
	PatchInMemory(GameBaseAddr, 0x1CFA4A, "\xC6\x81\xCD\x49\x03\x02\x90", 7);   // mov byte ptr [rcx+2039CD], 90


	// Infinite Energy PL2
	PatchInMemory(GameBaseAddr, 0x1CFA74, "\x90\x90", 2);						// nop; nop
	PatchInMemory(GameBaseAddr, 0x1CFAB1, "\xB9\xCD\x49\x03\x02\xBA\x90", 7);	// mov ecx, 2034DED; mov dl, 90 
	PatchInMemory(GameBaseAddr, 0x1CFAA4, "\xC6\x81\xED\x4D\x03\x02\x90", 7);   // mov byte ptr [rcx+2034DED], 90
}

VOID PatchCoinsHSF2EU(PVOID GameBaseAddr)
{
	// Infinite Credits FF830A -> 09
	PatchInMemory(GameBaseAddr, 0x1B0F45, "\x90\x90", 2);				// nop; nop
	PatchInMemory(GameBaseAddr, 0x1B0F4E, "\x41\xB0\x09", 3);			// mov r8b, 09
	PatchInMemory(GameBaseAddr, 0x1B0F51, "\xBA\x0A\x83\xFF\x00", 5);	// mov edx, FF830A
}

VOID PatchCoinsHSF2JP(PVOID GameBaseAddr)
{
	// Infinite Credits FF830A -> 09
	PatchInMemory(GameBaseAddr, 0x1BC355, "\x90\x90", 2);				// nop; nop
	PatchInMemory(GameBaseAddr, 0x1BC35E, "\x41\xB0\x09", 3);			// mov r8b, 09
	PatchInMemory(GameBaseAddr, 0x1BC361, "\xBA\x0A\x83\xFF\x00", 5);	// mov edx, FF830A
}

VOID PatchCoinsNLEU(PVOID GameBaseAddr)
{
	// Infinite Credits FF82E9 -> 0909
	PatchInMemory(GameBaseAddr, 0x1B68B8, "\x90\x90", 2);								// nop; nop
	PatchInMemory(GameBaseAddr, 0x1B68C1, "\xBA\xE9\x82\xFF\x00\x41\xB8\x09\x09", 9);	// mov edx, FF82E9; mov r8d, 0909
}

VOID PatchCoinsNLJP(PVOID GameBaseAddr)
{
	// Infinite Credits FF82E9 -> 0909
	PatchInMemory(GameBaseAddr, 0x1C1CC8, "\x90\x90", 2);								// nop; nop
	PatchInMemory(GameBaseAddr, 0x1C1CD1, "\xBA\xE9\x82\xFF\x00\x41\xB8\x09\x09", 9);	// mov edx, FF82E9; mov r8d, 0909
}

VOID PatchCoinsVsavEU(PVOID GameBaseAddr)
{
	// Infinite Credits FF8076 -> 09
	PatchInMemory(GameBaseAddr, 0x1B0F45, "\x90\x90", 2);				// nop; nop
	PatchInMemory(GameBaseAddr, 0x1B0F4E, "\x41\xB0\x09", 3);			// mov r8b, 09
	PatchInMemory(GameBaseAddr, 0x1B0F51, "\xBA\x76\x80\xFF\x00", 5);	// mov edx, FF8076
}

VOID PatchCoinsVsavJP(PVOID GameBaseAddr)
{
	// Infinite Credits FF8076 -> 09
	PatchInMemory(GameBaseAddr, 0x1BC355, "\x90\x90", 2);				// nop; nop
	PatchInMemory(GameBaseAddr, 0x1BC35E, "\x41\xB0\x09", 3);			// mov r8b, 09
	PatchInMemory(GameBaseAddr, 0x1BC361, "\xBA\x76\x80\xFF\x00", 5);	// mov edx, FF8076
}

VOID PatchCoinsJojoEU(PVOID GameBaseAddr)
{
	// Infinite Credits 20713A8 -> 09
	PatchInMemory(GameBaseAddr, 0x1BAC84, "\x90\x90", 2);						// nop; nop
	PatchInMemory(GameBaseAddr, 0x1BACC1, "\xB9\xA8\x13\x07\x02", 5);			// mov ecx, 20713A8
	PatchInMemory(GameBaseAddr, 0x1BACC6, "\xB2\x09", 2);						// mov dl, 09
	PatchInMemory(GameBaseAddr, 0x1BACB4, "\xC6\x81\xA8\x13\x07\x02\x09", 7);	// mov byte ptr [rcx+20713A8], 09
}

VOID PatchCoinsJojoJP(PVOID GameBaseAddr)
{
	// Infinite Credits 20713A8 -> 09
	PatchInMemory(GameBaseAddr, 0x1CFA74, "\x90\x90", 2);						// nop; nop
	PatchInMemory(GameBaseAddr, 0x1CFAB1, "\xB9\xA8\x13\x07\x02", 5);			// mov ecx, 20713A8
	PatchInMemory(GameBaseAddr, 0x1CFAB6, "\xB2\x09", 2);						// mov dl, 09
	PatchInMemory(GameBaseAddr, 0x1CFAA4, "\xC6\x81\xA8\x13\x07\x02\x09", 7);	// mov byte ptr [rcx+20713A8], 09
}