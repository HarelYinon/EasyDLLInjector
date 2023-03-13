#include "RemoteThreading.h"
#include "string.h"
#include "stdio.h"

LPCSTR MY_DLL = "MemoryView.dll"; //FUTURE TODO: let the user select his dll to load, and function to run. and those constants should move to the main cpp
LPCSTR MY_DLL_FUNC = "?InjectionFunctionInterface@@YGKPAX@Z"; //FUTURE TODO: let the user select which function to use.
LPCSTR KERNEL32_DLL = "kernel32.dll";


// Allocate buffer on the remote process memory.
// Returns the address of the allocated memory.
PVOID AllocMeomoryInOtherProcess(HANDLE hProcess, DWORD size, LPCVOID buffer) {
	PVOID allocated_memory_address = (PVOID)VirtualAllocEx(hProcess, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (NULL == allocated_memory_address) {
		int err = GetLastError();
		return NULL;
	}
	BOOL check = WriteProcessMemory(hProcess, (LPVOID)allocated_memory_address, buffer, size, NULL);
	if (0 == check) {
		printf("check error = %d\n", GetLastError());
		return NULL;
	}

	return allocated_memory_address;
};

// Find the address of the function on the remote process.
// NOTE: Currently return only 32 bit address, see "TODO".

PVOID GetFunctionAddressOnRemoteProcess(HANDLE hProcess, LPCSTR dll, LPCSTR function) {
	PVOID remote_function_address;
	// This condition is necessary to avoid an infinite loop when the the function is need to use a function from kernel32.dll
	if (strcmp(dll, KERNEL32_DLL) == 0) {
		// Kernel32 loaded in all proccesses to the same address.
		remote_function_address = (PVOID)GetProcAddress(GetModuleHandleA(dll), function);
	}
	else {

		// The location of function on other process is calculate this way:
		// The function address is relative to its dll's address, and the dll's address is known only on the other process.
		// So we need to know :
		//  1. what is the dll's address on the remote process.
		//  2. find the realtive address of the function in the dll.
		//   2.1. the method used here, is to load the dll(not the optimal solution, cuase sometime we won't want that dllmain of that dll would run on this app).
		//   2.2. the relative address is the subtraction of the address of the dll from the address of the function.
		// 
		// Then adding the address of the dll on the remote process and the relative address of the function on that dll would be the address of the function on the remote process.
		//

		//TODO: make this line return an 64 bit address, suggested method: use IPC.
		DWORD dll_address_on_remote_process = RunFuncFromDLLOnRemoteProcess(hProcess, KERNEL32_DLL, "GetModuleHandleA", (LPVOID)dll, strlen(dll));
		//
		HMODULE hModule = LoadLibraryA(dll);
		FARPROC function_addr_on_this_process = GetProcAddress(hModule, function);
		DWORD relative_function_address = (DWORD)function_addr_on_this_process - (DWORD)hModule;

		remote_function_address = (PVOID)(dll_address_on_remote_process + relative_function_address);

		FreeLibrary(hModule);
	}
	return remote_function_address;
}

DWORD RunFuncFromDLLOnRemoteProcess(HANDLE hProcess, LPCSTR dll, LPCSTR function, LPVOID param,DWORD param_size) {
	// Alloc the param on the other process virtual memory.
	LPVOID param_address_on_remote_process = NULL;
	if (param != NULL) {
		param_address_on_remote_process = AllocMeomoryInOtherProcess(hProcess, param_size, param);
	}

	//find the address of the function on the remote process.
	PVOID remote_function_addr = GetFunctionAddressOnRemoteProcess(hProcess, dll, function);
	if (remote_function_addr == NULL) {
		printf("failed to find %s in %s\n",function,dll);
		return 0;
	}

	//start the remote thread or report failure.

	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)remote_function_addr, (LPVOID)param_address_on_remote_process, NULL, NULL);
	if (NULL == hRemoteThread) {
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

	WaitForSingleObject(hRemoteThread, INFINITE);

	// check the return value of the function to check if succeeded.

	DWORD remote_function_return_value;
	GetExitCodeThread(hRemoteThread, &remote_function_return_value);
	CloseHandle(hRemoteThread);

	if (param_address_on_remote_process != NULL)
		VirtualFreeEx(hProcess, param_address_on_remote_process, NULL, MEM_RELEASE);

	return remote_function_return_value;
}

//dll injection done by load the wanted dll to wanted process, and run the wanted function in it. 
void InjectProcess(HANDLE hRemoteProcess, DWORD id) {

	CHAR mydllFullPath[MAX_PATH];
	DWORD pathLen = GetFullPathNameA(MY_DLL, MAX_PATH, mydllFullPath, NULL); //mydllFullPath get the full path of MY_DLL, and pathLen get the amount of letters

	CONST DWORD DLL_LOAD_FAILED = NULL;
	if (RunFuncFromDLLOnRemoteProcess(hRemoteProcess, KERNEL32_DLL, "LoadLibraryA", mydllFullPath, pathLen)  != DLL_LOAD_FAILED) {
		//printf("dll load successfuly for id=%d\n", id);
		RunFuncFromDLLOnRemoteProcess(hRemoteProcess, MY_DLL, MY_DLL_FUNC, NULL, NULL);
	}
	else {
		char error_msg[50];
		sprintf_s(error_msg, "General error with pid %d", id);
		MessageBoxA(NULL, error_msg, "General Error", MB_OK | MB_ICONERROR);
	}
}