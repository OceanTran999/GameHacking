#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <thread>
#include <string>

HANDLE handPro = GetCurrentProcess();

DWORD ReadfromMem(DWORD* targetRead)
{
	DWORD recvPtr;
	if (ReadProcessMemory(handPro, targetRead, &recvPtr, sizeof(int), NULL) == 0)
	{
		MessageBoxA(NULL, (LPCSTR)GetLastError(), "Can't read memory!", MB_OK);
	}
	return recvPtr;
}

void WritetoMem(DWORD* targetWri, DWORD wtrPtr)
{
	if (WriteProcessMemory(handPro, (LPVOID)targetWri, (LPCVOID) & wtrPtr, sizeof(int), NULL) == 0)
	{
		MessageBoxA(NULL, (LPCSTR)GetLastError(), "Can't write memory!", MB_OK);
	}
}

namespace offset {
	DWORD start_base = 0x10C740;							// Change this
	DWORD coin_base = 0x326E14;								// Change this
	int round[6] = {0x3C, 0x28, 0x15C, 0x2AC, 0x118, 0x4C};	// Change this
	DWORD sun = 0x5578;
	DWORD coin = 0x178;
}

//void debugInfo(DWORD* addr) {
//	std::string strSun = std::to_string((unsigned long)addr);
//	char const* pchar = strSun.c_str();
//	MessageBoxA(NULL, (LPCSTR)pchar, "Info", NULL);
//}

void hackTime(LPVOID param)
{
	while (true)
	{
		// F5 for sun's value
		if (GetAsyncKeyState(VK_F5) & 80000) {
			// Get base address
			DWORD* baseAddress = (DWORD*)((DWORD)GetModuleHandle(NULL) + offset::start_base);

			// Calculate address of round started
			for (int i = 0; i < 6; i++) {
				baseAddress = (DWORD*)(*baseAddress + offset::round[i]);
				/*debugInfo(baseAddress);*/
			}
			
			// Calculate address of sun's value
			DWORD* sunaddr = (DWORD*)((*baseAddress + offset::sun));

			// Read the current value
			DWORD currentVal = ReadfromMem(sunaddr);
			currentVal += 5000;

			// Modify the current value
			WritetoMem(sunaddr, currentVal);
		}
		
		// F6 for coin's value
		else if (GetAsyncKeyState(VK_F6) & 80000)
		{
			DWORD* baseAddr = (DWORD*)((DWORD)GetModuleHandle(NULL) + offset::coin_base);
			DWORD* coinaddr = (DWORD*)(*baseAddr + offset::coin);
			
			// Get coin
			DWORD currentVal = ReadfromMem(coinaddr);
			currentVal += 100000;

			// Update coin
			WritetoMem(coinaddr, currentVal);
		}
		Sleep(100);
	}
}

bool WINAPI DllMain(HINSTANCE hIns, DWORD reason, LPVOID reserved) {
	if (reason == 1)		// DLL_PROCESS_ATTACH
	{
		MessageBoxA(NULL, "Injected Successfully!", "Info!", MB_OK);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)hackTime, hIns, NULL, 0);
	}
	return 1;
}