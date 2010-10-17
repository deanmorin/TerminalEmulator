#ifndef PHYSICAL_H
#define PHYSICAL_H

#include "TerminalEmulator.h"

#define READ_BUFSIZE    128
#define WAIT_TIME       100

DWORD WINAPI    ReadThreadProc(HWND hWnd);
VOID            ProcessCommError(HANDLE hPort);

#endif
