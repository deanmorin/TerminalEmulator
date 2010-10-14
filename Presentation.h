#ifndef PRESENTATION_H
#define PRESENTATION_H

#include "TerminalEmulator.h"
#include <string.h>
#define BUFSIZE 256

BOOL    ProcessRead(HWND hWnd, CHAR szReadBuf[], DWORD dwBytesRead);
BOOL    ProcessWrite(HWND hWnd, WPARAM wParam, BOOL bNonCharKey);
BOOL    ProcessEsc(HWND hWnd, CHAR szBuffer[], DWORD dwBytesRead);

VOID LTrimCopy(CHAR dest[], const CHAR src[], size_t pos, DWORD dwBytesRead);

VOID Read(HWND hWnd, CHAR szReadBuf, DWORD dwBytesRead);
VOID Read2(HWND hWnd, CHAR szReadBuf[], DWORD dwBytesRead);

#endif
