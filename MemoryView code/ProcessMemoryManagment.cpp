#include "ProcessMemoryManagment.h"
#include "string"

VOID GetProcessMemory(DWORD offset, LPSTR buff, DWORD byteToRead) {
	DWORD addressOfMemory = offset;
	HANDLE hThisProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	ReadProcessMemory(hThisProc, (LPCVOID)addressOfMemory, buff, byteToRead, NULL);
	CloseHandle(hThisProc);
}

VOID ChangeProcessMemory(DWORD dest_address, LPSTR new_memory, DWORD bytes_to_copy) {
	DWORD dwOld = NULL;
	VirtualProtect((LPVOID)dest_address, sizeof(DWORD), PAGE_READWRITE, &dwOld);
	memcpy((LPVOID)dest_address, new_memory, bytes_to_copy);
	VirtualProtect((LPVOID)dest_address, sizeof(DWORD), dwOld, &dwOld);
}
/*
For future use.
DWORD GetTextAddress() {
	DWORD baseAddress = (DWORD)GetModuleHandle(NULL);
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)baseAddress;
	if ((*dosHeader).e_magic != IMAGE_DOS_SIGNATURE)
		return 1;
	PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(baseAddress + (*dosHeader).e_lfanew);
	if ((*ntHeader).Signature != IMAGE_NT_SIGNATURE)
		return 2;
	PIMAGE_OPTIONAL_HEADER opHeader = &(*ntHeader).OptionalHeader;
	if ((*opHeader).Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
		return 3;


	DWORD addressOfText = (DWORD)baseAddress + (*opHeader).AddressOfEntryPoint;
	return addressOfText;
}


/*
//For future use.
DWORD GetProcessCode(DWORD offset) {
	int msgboxID = 0;
	SetLastError(0);
	DWORD baseAddress = (DWORD)GetModuleHandle(NULL);
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)baseAddress;
	if ((*dosHeader).e_magic != IMAGE_DOS_SIGNATURE)
		return 1;
	PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(baseAddress + (*dosHeader).e_lfanew);
	if ((*ntHeader).Signature != IMAGE_NT_SIGNATURE)
		return 2;
	PIMAGE_OPTIONAL_HEADER opHeader = &(*ntHeader).OptionalHeader;
	if ((*opHeader).Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
		return 3;

	msgboxID = MessageBoxA(NULL, std::to_string(GetLastError()).c_str(),
		(LPCSTR)"Check headers",
		MB_DEFBUTTON2);
	SetLastError(0);

	DWORD addressOfText = (DWORD)baseAddress + (*opHeader).AddressOfEntryPoint + offset;

	msgboxID = MessageBoxA(NULL, std::to_string(GetLastError()).c_str(),
		(LPCSTR)"Memory change",
		MB_DEFBUTTON2);
	SetLastError(0);

	CHAR buff[0x1001];
	HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	msgboxID = MessageBoxA(NULL, std::to_string(GetLastError()).c_str(),
		(LPCSTR)"HANDLE to this process",
		MB_DEFBUTTON2);
	SetLastError(0);

	BOOL b = ReadProcessMemory(h, (LPCVOID)addressOfText, buff, 0x1000, NULL);
	printf("%d\n", GetLastError());
	msgboxID = MessageBoxA(NULL, std::to_string(GetLastError()).c_str(),
		(LPCSTR)"Read Memory",
		MB_DEFBUTTON2);
	SetLastError(0);

	CloseHandle(h);

	HANDLE hfile = CreateFileA("c:\\os\\code.txt", FILE_ALL_ACCESS, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile != INVALID_HANDLE_VALUE) {
		WriteFile(hfile, buff, 0x1000, NULL, NULL);
		CloseHandle(hfile);
	}
	else {
		msgboxID = MessageBoxA(NULL, std::to_string(GetLastError()).c_str(),
			(LPCSTR)"Failed to open code.txt",
			MB_DEFBUTTON2);
		SetLastError(0);
	}
	return NULL;
}
	/*
		//for future use.
		DWORD PrintPE(LPCSTR path) {
			int msgboxID = 0;

			FILE* fPEtxt = NULL;
			fopen_s(&fPEtxt, path, "w");

			if (!fPEtxt) {
				msgboxID = MessageBoxA(NULL, "failed to create pe_header.txt",
					(LPCSTR)"Create PE.txt",
					MB_DEFBUTTON2);
				SetLastError(0);
				return NULL;
			}

			DWORD baseAddress = (DWORD)GetModuleHandle(NULL);
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)baseAddress;

			fprintf(fPEtxt, "BASE_ADDRESS: %08X\n", baseAddress);

			fprintf(fPEtxt, "PE_HEADER\n\tDOS_HEADER:\n");


			fprintf(fPEtxt, "\t\te_magic:\t\%04X\n", (*dosHeader).e_magic);
			fprintf(fPEtxt, "\t\te_cblp:\t\%04X\n", (*dosHeader).e_cblp);
			fprintf(fPEtxt, "\t\te_cp:\t\%04X\n", (*dosHeader).e_cp);
			fprintf(fPEtxt, "\t\te_crlc:\t\%04X\n", (*dosHeader).e_crlc);
			fprintf(fPEtxt, "\t\te_cparhdr:\t\%04X\n", (*dosHeader).e_cparhdr);
			fprintf(fPEtxt, "\t\te_minalloc:\t\%04X\n", (*dosHeader).e_minalloc);
			fprintf(fPEtxt, "\t\te_maxalloc:\t\%04X\n", (*dosHeader).e_maxalloc);
			fprintf(fPEtxt, "\t\te_ss:\t\%04X\n", (*dosHeader).e_ss);
			fprintf(fPEtxt, "\t\te_sp:\t\%04X\n", (*dosHeader).e_sp);
			fprintf(fPEtxt, "\t\te_csum:\t\%04X\n", (*dosHeader).e_csum);
			fprintf(fPEtxt, "\t\te_ip:\t\%04X\n", (*dosHeader).e_ip);
			fprintf(fPEtxt, "\t\te_lfarlc:\t\%04X\n", (*dosHeader).e_lfarlc);
			fprintf(fPEtxt, "\t\te_ovno:\t\%04X\n", (*dosHeader).e_ovno);
			fprintf(fPEtxt, "\t\te_res:\t\%04X\n", (*dosHeader).e_res);

			fprintf(fPEtxt, "\t\tRESERVED:\t\n");

			fprintf(fPEtxt, "\t\te_oemid:\t\%04X\n", (*dosHeader).e_oemid);
			fprintf(fPEtxt, "\t\te_oeminfo:\t\%04X\n", (*dosHeader).e_oeminfo);
			fprintf(fPEtxt, "\t\te_res:\t\%04X\n", (*dosHeader).e_res2);

			fprintf(fPEtxt, "\t\tRESERVED:\t\n");

			fprintf(fPEtxt, "\t\te_lfanew:\t\%08X\n", (*dosHeader).e_lfanew);
			fclose(fPEtxt);
			return NULL;
		}
		*/
