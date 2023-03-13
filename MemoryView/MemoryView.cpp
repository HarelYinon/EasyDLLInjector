//#include "pch.h"
#define DLL_EXPORT
#include "stdio.h"

#include "MemoryView.h"
#include "MainWindow.h"
#include "psapi.h"

HANDLE _hModule;

DECLDIR DWORD WINAPI InjectionFunctionInterface(LPVOID lpParam)
{
	CreateMemoryResearcherWindow(_hModule);
	// The exit code would be readable by the hanlde of the thread that run that function.
	FreeLibraryAndExitThread(GetModuleHandle(L"MemoryView.dll"), true);
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