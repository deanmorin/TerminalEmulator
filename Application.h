#ifndef APPLICATION_H
#define APPLICATION_H

#include "TerminalEmulator.h"

VOID    InitTerminal(HWND hWnd);
VOID    Paint(HWND hWnd);
VOID    PerformMenuAction(HWND hWnd, UINT message, WPARAM wParam);
VOID    SetColorAndStyle(BYTE fgColor, BYTE bgColor, BYTE style);

#endif
