#ifndef APPLICATION_H
#define APPLICATION_H

#include "TerminalEmulator.h"

VOID    InitTerminal(HWND hWnd);
VOID    PerformMenuAction(HWND hWnd, UINT message, WPARAM wParam);
VOID    MoveCursor(HWND hWnd, DWORD cxCoord, DWORD cyCoord);

#endif
