#include "WindowsCommandControls.h"
#include <CommCtrl.h>
#include "ProcessMemoryManagment.h"
#include "stdio.h"
#include "windowsx.h"
#include "string.h"

HWND hApplyAndRefresh, hOffset, hChangeMemory, hCurrentMemory;

void applyMemoryChange(DWORD offset) {
	const int MAXIMUM_INPUT = 1000;

	int ok = MessageBoxA(NULL, "Memory change can crush the process!\nAre you sure yo to apply changes?", "Alert!", MB_ICONWARNING | MB_OKCANCEL);
	if (ok == IDOK) {//currently cuse application crush
		
		int new_code_length = GetWindowTextLengthA(hChangeMemory);
		if (new_code_length > (MAXIMUM_INPUT*2)) {
			MessageBoxA(NULL, "Input is bigger than 1000B", "Error", MB_OK);
		}
		else if (new_code_length != 0)
		{

			char new_code1[MAXIMUM_INPUT*2] = { 0 };
			char new_code[MAXIMUM_INPUT] = { 0 };
			char tmp[3]={0};
			GetWindowTextA(hChangeMemory, new_code, new_code_length +1);
			for (int i = 0; i < new_code_length/2; ++i) {
				memcpy(tmp, new_code+(i*2), 2);
				DWORD hex_val = strtol(tmp,NULL,16);
				new_code1[i] = hex_val;
			}

			ChangeProcessMemory(offset, new_code1, new_code_length/2);
		}
	}
}

void showCurrentMemory(DWORD offset) {
	
	char pure_memory_buff[1001];
	//pure_memory_buff[1000] = 0;
	char memory_as_hex[3001];
	//memory_as_hex[3000] = 0;

	GetProcessMemory(offset, pure_memory_buff, 1000);

	for (int i = 0; i < 1000; ++i) {
		char tmp[3] = { 0 };
		sprintf(tmp, "%02X", (unsigned char)pure_memory_buff[i]);

		sprintf((memory_as_hex + i * 3), "%s ", tmp);

	}
	SetWindowTextA(hCurrentMemory, (LPCSTR)memory_as_hex);
}

void ApplyAndRefresh() {
	char offset_buff[33];
	GetWindowTextA(hOffset, offset_buff, 32);
	DWORD offset = strtol(offset_buff, NULL, 16);
	
	applyMemoryChange(offset);
	showCurrentMemory(offset);
}

//TODO: make the x,y,width,height more generic.
void AddContents(HWND hwnd) {
	hApplyAndRefresh = CreateWindow(WC_BUTTON, L"Apply&Refresh", WS_VISIBLE | WS_CHILD, 15, 45, 140, 38, hwnd, (HMENU)APPLY_AND_REFRESH, NULL, NULL);

	CreateWindowW(WC_STATIC, L"Offset(HEX):", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTERIMAGE | SS_CENTER, 215, 45, 115, 38, hwnd, NULL, NULL, NULL);
	DWORD base_address = (DWORD)GetModuleHandle(NULL);
	char text_buff[33];
	text_buff[32] = 0;
	_itoa(base_address, text_buff, 16);
	hOffset = CreateWindowA("Edit", text_buff, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOVSCROLL | SS_CENTERIMAGE | SS_CENTER, 330, 45, 250, 38, hwnd, NULL, NULL, NULL);

	CreateWindowW(WC_STATIC, L"Size Of Block: 1000B", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTERIMAGE | SS_CENTER, 215, 115, 200, 38, hwnd, NULL, NULL, NULL);

	//CreateWindow(WC_STATIC, L"Change Memory:\r\nMaximum input is 1000B\r\nNOTE: Any not hex character count as 0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER, 215, 200, 200, 400, hwnd, NULL, NULL, NULL);
	CreateWindow(WC_STATIC, L"Change Memory:", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTERIMAGE | SS_CENTER, 215, 200, 200, 400, hwnd, NULL, NULL, NULL);
	hChangeMemory = CreateWindowW(WC_EDIT, L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL, 415, 200, 250, 400, hwnd, NULL, NULL, NULL);

	CreateWindowW(WC_STATIC, L"Current Memory:", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTERIMAGE | SS_CENTER, 215 + 600, 200, 200, 400, hwnd, NULL, NULL, NULL);
	hCurrentMemory = CreateWindowW(WC_EDIT, L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL, 415 + 600, 200, 250, 400, hwnd, NULL, NULL, NULL);
}