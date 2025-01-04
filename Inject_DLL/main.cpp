#include <iostream>
#include <Windows.h>
#include <processthreadsapi.h>
#include <TlHelp32.h>
#include <libloaderapi.h>
#include <Psapi.h>
#include <tchar.h>

int findPID(const wchar_t* gameName)
{
	// Create snapshot to contain list of all processes
	HANDLE listPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (listPID == INVALID_HANDLE_VALUE)
	{
		CloseHandle(listPID);
		std::cout << "[-] Can't create snapshot..." << GetLastError() << std::endl;
		return 0;
	}

	DWORD target = 0;
	PROCESSENTRY32 pe32;

	// Set size
	pe32.dwSize = sizeof(PROCESSENTRY32);

	do {
		if (wcscmp(gameName, pe32.szExeFile) == 0)
		{
			target = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(listPID, &pe32));
	
	CloseHandle(listPID);
	return target;
}

int main() {
	const wchar_t* gameName = L"PlantsVsZombies.exe";
	// Find process ID and Base Address of the game
	int pID = findPID(gameName);

	if (pID == 0)
	{
		std::cout << "[-] Can't find process. Exiting..." << std::endl;
		return 1;
	}
	else
	{
		std::cout << "[+] Found game's PID: " << pID << std::endl;

		// Attach
		HANDLE openProc = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pID);
		if (openProc == NULL)
		{
			std::cout << "[-] Can't open process. Exiting..." << std::endl;
			return 1;
		}

		char dllPath[MAX_PATH] = "PvZHack.dll";
		std::cout << "[+] Opened process!" << std::endl;

		// Allocate game's memory
		HANDLE allocMem = VirtualAllocEx(openProc, NULL, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (allocMem == NULL)
		{
			CloseHandle(openProc);
			std::cout << "[-] Can't allocate memory: " << GetLastError() << std::endl;
			return 1;
		}
		std::cout << "[+] Allocated successfully!" << std::endl;

		// Write DLL's path to memory
		if (!WriteProcessMemory(openProc, allocMem, dllPath, strlen(dllPath) + 1, NULL))
		{
			CloseHandle(allocMem);
			CloseHandle(openProc);
			std::cout << "[-] Can't write to memory: " << GetLastError() << std::endl;
			return 1;
		}
		std::cout << "[+] Successfully wrote to memory!" << std::endl;

		// Create a new thread to run DLL
		HANDLE newThread = CreateRemoteThread(openProc, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, allocMem, 0, NULL);
		if (newThread == NULL)
		{
			CloseHandle(openProc);
			CloseHandle(allocMem);
			std::cout << "[-} Can't create thread: " << GetLastError() << std::endl;
			return 1;
		}
		std::cout << "[+] Created new thread!" << std::endl;

		WaitForSingleObject(openProc, INFINITE);
		CloseHandle(allocMem);
		VirtualFreeEx(openProc, dllPath, 0, MEM_RELEASE);
		CloseHandle(openProc);
		std::cout << "[+] Exiting..." << std::endl;
	}

	return 0;
}
