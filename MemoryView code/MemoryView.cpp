//#include "pch.h"
#define DLL_EXPORT
#include "stdio.h"

#include "MemoryView.h"
#include "ManageWindow.h"
#include "psapi.h"

HANDLE _hModule;

DECLDIR DWORD WINAPI CreateMyWindow(LPVOID lpParam)
{
	CreateManagmentWindow(_hModule);
	//The exit code can be any value it would return it to the hanlde of the thread called the function.
	FreeLibraryAndExitThread(GetModuleHandle(L"MemoryView.dll"), 12);
	return FALSE;
}
	
BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD ul_reason_for_call,
	LPVOID lpReserved)
{
	_hModule = hModule;

	int msgboxID = 0;
	CHAR message[100] = { 0 };

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

