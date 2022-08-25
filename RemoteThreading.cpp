#include "RemoteThreading.h"
#include "string.h"
#include "stdio.h"

LPCSTR MY_DLL = "MemoryView.dll"; //FUTURE TODO: let the user select his dll to load, and function to run. and those constants should move to the main cpp
LPCSTR MY_DLL_FUNC = "?CreateMyWindow@@YGKPAX@Z";
LPCSTR KERNEL32_DLL = "kernel32.dll";

//alocate buffer on the remote process memory.
//returns the address of the allocated memory.
PVOID allocMeomoryOtherProcess(HANDLE hProcess, DWORD size, LPCVOID buffer) {
	PVOID memAddr = (PVOID)VirtualAllocEx(hProcess, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (NULL == memAddr) {
		int err = GetLastError();
		return NULL;
	}
	BOOL check = WriteProcessMemory(hProcess, (LPVOID)memAddr, buffer, size, NULL);
	if (0 == check) {
		printf("check error = %d\n", GetLastError());
		return NULL;
	}

	return memAddr;
};

//find the address of the function on the remote process.
//NOTE: Currently return only 32 bit address, see "TODO".

PVOID remoteFuncAddress(HANDLE hProcess, LPCSTR dll, LPCSTR function) {
	PVOID remFunAddr;
	if (strcmp(dll, KERNEL32_DLL) == 0) {
		//Kernel32 loaded in all proccesses to the same address.
		remFunAddr = (PVOID)GetProcAddress(GetModuleHandleA(dll), function);
	}
	else {
		//TODO: make this line return an 64 bit address
		//suggested method, when loading dll create a known file(can send the path in param) which address store there, and load it from the file
		DWORD remProcDllAddr = remoteFuncProc(hProcess, KERNEL32_DLL, "GetModuleHandleA", (LPVOID)dll, strlen(dll));
		//
		HMODULE hModule = LoadLibraryA(dll);
		FARPROC funAddr = GetProcAddress(hModule, function);
		DWORD funOff = (DWORD)funAddr - (DWORD)hModule;
		remFunAddr = (PVOID)(funOff + remProcDllAddr);
		FreeLibrary(hModule);
	}
	return remFunAddr;
}

DWORD remoteFuncProc(HANDLE hProcess, LPCSTR dll, LPCSTR function, LPVOID param,DWORD param_size) {
	//alloc the param on the other process virtual memory.
	LPVOID remote_param = param;
	if (remote_param != NULL) {
		remote_param = allocMeomoryOtherProcess(hProcess, param_size, param);
	}

	//find the address of the function on the remote process.
	PVOID remFunAddr = remoteFuncAddress(hProcess, dll, function);
	if (remFunAddr == NULL) {
		printf("failed to find %s in %s\n",function,dll);
		return 0;
	}

	//start the remote thread and return the value of the function if succed.
	HANDLE hRemote = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)remFunAddr, (LPVOID)remote_param, NULL, NULL);
	if (NULL == hRemote) {
		DWORD error = GetLastError();
		if (error == ERROR_ACCESS_DENIED) {
			char error_msg[50];
			sprintf_s(error_msg, "Load %s denied!", dll);
			MessageBoxA(NULL, error_msg, "Load library error", MB_OK | MB_ICONERROR);
		}
		else {
			char error_msg[50];
			sprintf_s(error_msg, "%s Function in %s dll failed\n", function, dll);
			MessageBoxA(NULL, error_msg, "Load library error", MB_OK | MB_ICONERROR);
		}
		return 0;
	}

	WaitForSingleObject(hRemote, INFINITE);
	DWORD exitCode;
	GetExitCodeThread(hRemote, &exitCode);
	CloseHandle(hRemote);

	if (remote_param != NULL)
		VirtualFreeEx(hProcess, remote_param, NULL, MEM_RELEASE);

	return exitCode;
}

//dll injection done by load the wanted dll to wanted process, and run the wanted function in it. 
void injectProcess(HANDLE hProcess, DWORD id) {

	CHAR mydllFullPath[MAX_PATH];
	DWORD pathLen = GetFullPathNameA(MY_DLL, MAX_PATH, mydllFullPath, NULL);

	if (remoteFuncProc(hProcess, KERNEL32_DLL, "LoadLibraryA", mydllFullPath, pathLen)) {
		//printf("dll load successfuly for id=%d\n", id);
		remoteFuncProc(hProcess, MY_DLL, MY_DLL_FUNC, NULL, NULL);
	}
	else {
		char error_msg[50];
		sprintf_s(error_msg, "General error with pid %d", id);
		MessageBoxA(NULL, error_msg, "General Error", MB_OK | MB_ICONERROR);
	}
}