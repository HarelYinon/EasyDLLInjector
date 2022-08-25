#include "ListViewControl.h"
#pragma comment(lib, "comctl32.lib")
#include <CommCtrl.h>

//Manage ListView creation
HWND CreateListView(HWND hwndParent, HINSTANCE hInstance, DWORD x, DWORD y, DWORD width, DWORD height) {
	INITCOMMONCONTROLSEX icex;
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	RECT rcClient; //Parnet window's client area

	GetClientRect(hwndParent, &rcClient);

	HWND hwndList = CreateWindowW(WC_LISTVIEW, L"",
		WS_VISIBLE | WS_BORDER | WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
		rcClient.left + x, rcClient.top + y, width, height,
		hwndParent, (HMENU)IDOK, hInstance, 0);
	return(hwndList);

}

int CreateItem(HWND hwndList, wchar_t* text)
{
	LVITEMW lvi = { 0 };
	lvi.mask = LVIF_TEXT;
	lvi.pszText = text;
	return ListView_InsertItem(hwndList, &lvi);
}

int Create2ColItem(HWND hwndList, wchar_t* Text1, wchar_t* Text2)
{
	LVITEMW lvi = { 0 };
	int Ret;
	// Initialize LVITEMW members that are common to all items.
	lvi.mask = LVIF_TEXT;
	lvi.pszText = Text1;
	Ret = ListView_InsertItem(hwndList, &lvi);
	if (Ret >= 0)
	{
		ListView_SetItemText(hwndList, Ret, 1, Text2);
	}
	return Ret;
}

int CreateColumn(HWND hwndLV, int iCol, LPWSTR text, int iWidth)
{
	LVCOLUMN lvc;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = iWidth;
	lvc.pszText = text;
	lvc.iSubItem = iCol;
	return ListView_InsertColumn(hwndLV, iCol, &lvc);
}
