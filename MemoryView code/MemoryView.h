#pragma once
#include "windows.h"
#ifdef DLL_EXPORT
#define DECLDIR __declspec(dllexport)
#else
#define DECLDIR __declspec(dllimport)
#endif

	DECLDIR DWORD WINAPI CreateMyWindow(LPVOID lpParam);
