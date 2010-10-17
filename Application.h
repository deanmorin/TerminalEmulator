#ifndef APPLICATION_H
#define APPLICATION_H

#include <Windows.h>
#include "TerminalEmulator.h"

VOID    EchoBuffer(HWND hWnd, TCHAR szBuffer[], COLORREF textColor);
VOID    InitTerminal(HWND hWnd);
VOID    PerformMenuAction(HWND hWnd, UINT message, WPARAM wParam);
VOID    SelectPort(HWND hWnd, INT selected);
BOOL    StoreTextForRepaint(HWND hWnd, TCHAR szBuffer[], COLORREF textColor);

#endif
