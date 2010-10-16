#ifndef PRESENTATION_H
#define PRESENTATION_H

#include "TerminalEmulator.h"
#include <string.h>
#define BUFSIZE 256

VOID    ProcessRead(HWND hWnd, CHAR szReadBuf[], DWORD dwBytesRead);
BOOL    ProcessWrite(HWND hWnd, WPARAM wParam, BOOL bNonCharKey);
BOOL ProcessBuffer(HWND hWnd, CHAR szBuffer[], DWORD dwLength);
BOOL    ProcessEsc(HWND hWnd, CHAR* szBuffer, DWORD dwBytesRead);

VOID Read(HWND hWnd, CHAR szReadBuf, DWORD dwBytesRead);
VOID Read2(HWND hWnd, CHAR szReadBuf[], DWORD dwBytesRead);

#endif
