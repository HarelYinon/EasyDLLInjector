#pragma once
#include "windows.h"

extern LPCSTR KERNEL32_DLL;

//Get name of wanted proccess and return its id number
//Mechanism: Going through a list of all processes and compare the disred process name to every name of every process 
PVOID allocMeomoryOtherProcess(HANDLE hProcess, DWORD size, LPCVOID buffer);

//Create remote thread on remote process and start for the disired function with parameter from the disired dll on the remote process.
// param and param size can be NULL.
//retrun the value of the remote function.
DWORD remoteFuncProc(HANDLE hProcess, LPCSTR dll, LPCSTR function, LPVOID param,DWORD paramSize);

//Handle the procedure of dll injection easily.
void injectProcess(HANDLE hProcess, DWORD id=0);