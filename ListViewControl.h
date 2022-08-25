#pragma once
#include "windows.h"

HWND CreateListView(HWND hwndParent, HINSTANCE hInstance, DWORD x, DWORD y, DWORD width, DWORD height);

int CreateItem(HWND hwndList, wchar_t* text);

int Create2ColItem(HWND hwndList, wchar_t* Text1, wchar_t* Text2);

int CreateColumn(HWND hwndLV, int iCol, LPWSTR text, int iWidth);
