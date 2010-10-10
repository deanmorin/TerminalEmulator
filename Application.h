#ifndef APPLICATION_H
#define APPLICATION_H

#include <Windows.h>
#include "TerminalEmulator.h"

VOID    EchoBuffer(HWND hWnd, TCHAR szBuffer[], COLORREF textColor);
LRESULT PerformMenuAction(HWND hWnd, UINT message, WPARAM wParam);
VOID    ProcessCommError(HANDLE hPort);
BOOL    ReadPort(HWND hWnd);
DWORD WINAPI ReadThreadProc(LPVOID lpParameter);
VOID    SelectPort(HWND hWnd, INT selected);
BOOL    StoreTextForRepaint(HWND hWnd, TCHAR szBuffer[], COLORREF textColor);
BOOL    WriteToPort(HWND hWnd, WPARAM wParam);

#endif
