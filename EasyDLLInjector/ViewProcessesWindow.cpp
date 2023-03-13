#include "RemoteThreading.h"
#include "ListViewControl.h"

#include "tchar.h"
#include <psapi.h>
#include "stdio.h"

#pragma comment(lib, "comctl32.lib")
#include <CommCtrl.h>



BOOL CheckProcessValidation(HANDLE& handle) {
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
	if (CheckProcessValidation(hProcess)) {
		InjectProcess(hProcess, *((DWORD*)pid));
		CloseHandle(hProcess);
	}
	return NULL;
}



LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

//Put the Processes name and pid in ListView
void FillWindowWithCurrentRunningProcessList(HWND hListView) {
	DWORD processes_pid_list[1024], process_list_size;
	if (!EnumProcesses(processes_pid_list, sizeof(processes_pid_list), &process_list_size))
		return;

	TCHAR processName[MAX_PATH] = TEXT("<unknown>");

	process_list_size /= sizeof(DWORD);
	for (DWORD i = 0; i < process_list_size; ++i) {
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes_pid_list[i]);
		if (hProcess != NULL)
		{
			GetModuleBaseName(hProcess, NULL, processName, MAX_PATH);
			TCHAR wPID[10] = { 0 };
			_itow_s(processes_pid_list[i], wPID, 10);
			Create2ColItem(hListView, wPID, processName);
			CloseHandle(hProcess);
		}
	}
}

void CaseItemDoubleClicked(HWND hListView, LPARAM lp){
	LVHITTESTINFO hti;
	hti.pt = ((NMLISTVIEW*)lp)->ptAction;
	LV_DISPINFO* pI = (LV_DISPINFO*)lp;
	TCHAR buff[10] = { 0 };

	switch ((*pI).hdr.code) {
	case NM_DBLCLK:
		ListView_SubItemHitTest(hListView, &hti);
		ListView_GetItemText(hListView, hti.iItem, 0, (LPTSTR)buff, 10); //read the process id(PID) from the table

		DWORD pid = (DWORD)_wtoi(buff);
	
		CreateThread(NULL, NULL, InjectProcessThread,&pid, NULL, NULL);
		Sleep(10);//Needed to let the thread have enough time to use the value of pid.

		break;
	}
}

HWND hListViewProcessesList, hBtnRefresh;
const int REFRESH_BUTTON_CLICKED = 1;


LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_NOTIFY:		
		CaseItemDoubleClicked(hListViewProcessesList, lp);;
		break;
	case WM_COMMAND:
		switch (wp) {
		case REFRESH_BUTTON_CLICKED:
			ListView_DeleteAllItems(hListViewProcessesList);
			FillWindowWithCurrentRunningProcessList(hListViewProcessesList);
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

	//Initiallize a window that will serve as an interface for the user to be able to select the diserd process to inject the dll

	WNDCLASS wc = { 0 };
	MSG msg = { 0 };

	wc.lpszClassName = L"Simple Window";
	wc.hInstance = NULL;
	wc.lpfnWndProc = WindowProcedure;

	RegisterClass(&wc);

	HWND hUserIntefaceWindow = CreateWindowW(wc.lpszClassName, L"Choose Process", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 50, 50, 500, 600, NULL, NULL, NULL, NULL);


	//Add listview to userInterfaceWindow that would will have the list of the processes to display 
	hListViewProcessesList = CreateListView(hUserIntefaceWindow, NULL, 20, 5, 450, 450);

	CreateColumn(hListViewProcessesList, 0, (LPWSTR)L"PID", 200);
	CreateColumn(hListViewProcessesList, 1, (LPWSTR)L"Process Name", 250);

	FillWindowWithCurrentRunningProcessList(hListViewProcessesList);

	//A button to detect changes in the list of the running processes
	hBtnRefresh = CreateWindow(WC_BUTTON, L"Refresh", WS_VISIBLE | WS_CHILD, 20, 475, 80, 50, hUserIntefaceWindow, (HMENU)REFRESH_BUTTON_CLICKED, NULL, NULL);

	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}