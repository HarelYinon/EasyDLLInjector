#pragma once
#include "windows.h"

//Return the memory as hex in buff
VOID GetProcessMemory(DWORD offset, LPSTR buff, DWORD byteToRead);

VOID ChangeProcessMemory(DWORD dest_address, LPSTR new_memory, DWORD bytes_to_copy);