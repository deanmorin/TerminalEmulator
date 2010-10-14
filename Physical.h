#ifndef PHYSICAL_H
#define PHYSICAL_H

#include <Windows.h>
#include "TerminalEmulator.h"

DWORD WINAPI    ReadThreadProc(HWND hWnd);
VOID            ProcessCommError(HANDLE hPort);

#endif
