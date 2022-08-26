#pragma once
#include "windows.h"


	VOID CreateManagmentWindow(LPVOID hModule);
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
