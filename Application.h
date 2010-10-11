#ifndef APPLICATION_H
#define APPLICATION_H

#include <Windows.h>
#include "TerminalEmulator.h"

VOID    EchoBuffer(HWND hWnd, TCHAR szBuffer[], COLORREF textColor);
LRESULT PerformMenuAction(HWND hWnd, UINT message, WPARAM wParam);
BOOL    ReadPort(HWND hWnd);
VOID    SelectPort(HWND hWnd, INT selected);
BOOL    StoreTextForRepaint(HWND hWnd, TCHAR szBuffer[], COLORREF textColor);
BOOL    WriteToPort(HWND hWnd, WPARAM wParam);

#endif
