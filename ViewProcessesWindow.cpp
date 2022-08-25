#include "RemoteThreading.h"
#include "ListViewControl.h"

#include "tchar.h"
#include <psapi.h>
#include "stdio.h"

#pragma comment(lib, "comctl32.lib")
#include <CommCtrl.h>



BOOL validProcessHandle(HANDLE& handle) {
	if (!handle) {
		if (GetLastError() == ERROR_INVALID_PARAMETER)
			MessageBoxA(NULL, "Process not found\n", "Error", MB_OK | MB_ICONERROR);
		else if (GetLastError() == ERROR_ACCESS_DENIED)
			MessageBoxA(NULL, "Permission Error occured while openning process\n", "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

DWORD WINAPI InjectProcessThread(LPVOID pid) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, *(DWORD*)pid);//getProcessIdByName(REMOTE_PROCESS_NAME));
	if (validProcessHandle(hProcess)) {
		injectProcess(hProcess, *((DWORD*)pid));
		CloseHandle(hProcess);
	}
	return NULL;
}



LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

//Put the Processes name and pid in ListView
void FillProcListView(HWND hListView) {
	DWORD pidProcessList[1024], procListSize;
	if (!EnumProcesses(pidProcessList, sizeof(pidProcessList), &procListSize))
		return;

	TCHAR processName[MAX_PATH] = TEXT("<unknown>");

	procListSize /= sizeof(DWORD);
	for (DWORD i = 0; i < procListSize; ++i) {
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, pidProcessList[i]);
		if (hProcess != NULL)
		{
			GetModuleBaseName(hProcess, NULL, processName, MAX_PATH);
			TCHAR wPID[10] = { 0 };
			_itow_s(pidProcessList[i], wPID, 10);
			Create2ColItem(hListView, wPID, processName);
			CloseHandle(hProcess);
		}
	}
}

void CaseItemDoubleClick(HWND hListView, LPARAM lp){
	LVHITTESTINFO hti;
	hti.pt = ((NMLISTVIEW*)lp)->ptAction;
	LV_DISPINFO* pI = (LV_DISPINFO*)lp;
	TCHAR buff[10] = { 0 };

	switch ((*pI).hdr.code) {
	case NM_DBLCLK:
		ListView_SubItemHitTest(hListView, &hti);
		ListView_GetItemText(hListView, hti.iItem, 0, (LPTSTR)buff, 10);

		DWORD pid = (DWORD)_wtoi(buff);
	
		CreateThread(NULL, NULL, InjectProcessThread,&pid, NULL, NULL);
		Sleep(10);//Needed to let the thread have enough time to use the value of pid.

		break;
	}
}

HWND hProcessesListView, hBtnRefresh;
const int REFRESH_BUTTON_CLICKED = 1;


LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_NOTIFY:		
		CaseItemDoubleClick(hProcessesListView, lp);;
		break;
	case WM_COMMAND:
		switch (wp) {
		case REFRESH_BUTTON_CLICKED:
			ListView_DeleteAllItems(hProcessesListView);
			FillProcListView(hProcessesListView);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}


//Create the Processes ListView Window
int main(int argc, char* argv[]) {

	WNDCLASS wc = { 0 };
	MSG msg = { 0 };

	wc.lpszClassName = L"Simple Window";
	wc.hInstance = NULL;
	wc.lpfnWndProc = WindowProcedure;

	RegisterClass(&wc);

	HWND hWnd = CreateWindowW(wc.lpszClassName, L"Choose Process", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 50, 50, 500, 600, NULL, NULL, NULL, NULL);

	hProcessesListView = CreateListView(hWnd, NULL, 20, 5, 450, 450);

	TCHAR tmp[20];
	CreateColumn(hProcessesListView, 0, (LPWSTR)L"PID", 200);
	CreateColumn(hProcessesListView, 1, (LPWSTR)L"Process Name", 250);

	FillProcListView(hProcessesListView);

	hBtnRefresh = CreateWindow(WC_BUTTON, L"Refresh", WS_VISIBLE | WS_CHILD, 20, 475, 80, 50, hWnd, (HMENU)REFRESH_BUTTON_CLICKED, NULL, NULL);

	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}