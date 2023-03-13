#pragma once
#include "windows.h"


	VOID CreateMemoryResearcherWindow(LPVOID hModule);
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
