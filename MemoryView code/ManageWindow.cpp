#include "ManageWindow.h"
#include "WindowsCommandControls.h"

const int APPLY_AND_REFRESH = 0;


VOID CreateManagmentWindow(LPVOID hModule)
{
	HINSTANCE hInstance = (HINSTANCE)hModule;

	const wchar_t CLASS_NAME[] = L"Simple Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Memory Reseracher",    // Window text
		WS_VISIBLE|WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);
	
	// Run the message loop.

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND: {
		switch (wParam) {
		case APPLY_AND_REFRESH:
			ApplyAndRefresh();
			break;
		}
		break;
	}
	case WM_CREATE:
		AddContents(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}