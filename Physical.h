#ifndef PHYSICAL_H
#define PHYSICAL_H

#include "TerminalEmulator.h"

#define READ_BUFSIZE    1024
#define WAIT_TIME       100

VOID            ProcessCommError(HANDLE hPort);
DWORD WINAPI    ReadThreadProc(HWND hWnd);

#endif
