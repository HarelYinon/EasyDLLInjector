#pragma once
#include "windows.h"

//Create remote thread on remote process and start for the disired function with parameter from the disired dll on the remote process.
// param and param size can be NULL.
//retrun value is the return value of the remote function.
DWORD RunFuncFromDLLOnRemoteProcess(HANDLE hRemoteProcess, LPCSTR dll, LPCSTR function, LPVOID param,DWORD paramSize);

//Handle the procedure of dll injection easily.
void InjectProcess(HANDLE hProcess, DWORD id=0);