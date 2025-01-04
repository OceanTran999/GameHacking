#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

DWORD WINAPI ThreadMain(LPVOID param) {
	while (true)
	{
		if (GetAsyncKeyState(VK_F5) & 0x80000)
		{
			MessageBoxA(NULL, "F5 pressed!", "Warning", MB_OK);
		}
		Sleep(100);
	}
}

// Entry Point
bool WINAPI DllMain(
	HINSTANCE hinstDLL,		// handle to DLL module
	DWORD reason,			// reason for calling function
	LPVOID reserved			// reserved
)
{
	if (reason == DLL_PROCESS_ATTACH) {
		MessageBoxA(NULL, "DLL Injected", "Warning", MB_OK);
		CreateThread(NULL, 0, ThreadMain, hinstDLL, NULL, 0);;
	}
	return true;
}