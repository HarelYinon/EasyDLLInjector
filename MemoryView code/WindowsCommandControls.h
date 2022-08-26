#pragma once
#include "windows.h"

extern const int APPLY_AND_REFRESH;
//Create the control
void AddContents(HWND);

//Apply the memory cahnge, and refresh the memory view.
void ApplyAndRefresh();