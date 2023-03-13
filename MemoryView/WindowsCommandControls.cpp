#include "WindowsCommandControls.h"
#include <CommCtrl.h>
#include "ProcessMemoryManagment.h"
#include "stdio.h"
#include "windowsx.h"
#include "string.h"

HWND hApplyAndRefresh, hOffset, hNewMemory, hCurrentMemory;

void ApplyEditedMemory(DWORD offset) {
	const int MAXIMUM_INPUT = 1000;

	int ok = MessageBoxA(NULL, "Edit memory can crush the process!\nAre you sure yo to apply changes?", "Alert!", MB_ICONWARNING | MB_OKCANCEL);
	if (ok == IDOK) {
		
		int new_memory_length = GetWindowTextLengthA(hNewMemory);
		if (new_memory_length > (MAXIMUM_INPUT*2)) {
			MessageBoxA(NULL, "Input is too big, more than 1000B", "Error", MB_OK);
		}
		else if (new_memory_length > 0) // Read the user memory input, and apply changes
		{
			char new_memory[MAXIMUM_INPUT] = { 0 };
			char user_input_new_memory[MAXIMUM_INPUT * 2] = { 0 };
			char single_byte_as_hex_str[3]={0};
			GetWindowTextA(hNewMemory, user_input_new_memory, new_memory_length +1);

			for (int i = 0; i < new_memory_length/2; ++i) {
				memcpy(single_byte_as_hex_str, user_input_new_memory+(i*2), 2);
				DWORD hex_val = strtol(single_byte_as_hex_str,NULL,16);
				new_memory[i] = hex_val;
			}
			
			EditProcessMemory(offset, new_memory, new_memory_length/2);
		}
	}
}

//print the memory to the window in hex values
void ShowCurrentMemory(DWORD offset) {
	
	char pure_memory_buff[1001];
	char memory_as_hex[3001];

	GetProcessMemory(offset, pure_memory_buff, 1000);

	for (int i = 0; i < 1000; ++i) {
		char tmp[3] = { 0 };
		sprintf(tmp, "%02X", (unsigned char)pure_memory_buff[i]);

		sprintf((memory_as_hex + i * 3), "%s ", tmp);

	}
	SetWindowTextA(hCurrentMemory, (LPCSTR)memory_as_hex);
}

//Aplly the changes in the memory, and refresh the view.
void ApplyAndRefresh() {
	char offset_buff[33];
	GetWindowTextA(hOffset, offset_buff, 32);
	DWORD offset = strtol(offset_buff, NULL, 16);
	
	ApplyEditedMemory(offset);
	ShowCurrentMemory(offset);
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
	hNewMemory = CreateWindowW(WC_EDIT, L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL, 415, 200, 250, 400, hwnd, NULL, NULL, NULL);

	CreateWindowW(WC_STATIC, L"Current Memory:", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTERIMAGE | SS_CENTER, 215 + 600, 200, 200, 400, hwnd, NULL, NULL, NULL);
	hCurrentMemory = CreateWindowW(WC_EDIT, L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL, 415 + 600, 200, 250, 400, hwnd, NULL, NULL, NULL);
}