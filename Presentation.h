#ifndef PRESENTATION_H
#define PRESENTATION_H

#include "TerminalEmulator.h"
#include <string.h>
#define BUFSIZE 256

VOID    ProcessRead(HWND hWnd, CHAR psReadBuf[], DWORD dwBytesRead);
BOOL    ProcessWrite(HWND hWnd, WPARAM wParam, BOOL bNonCharKey);
BOOL    ProcessBuffer(HWND hWnd, CHAR psBuffer[], DWORD dwLength);
VOID    ProcessEsc(HWND hWnd, CHAR* psBuffer, DWORD dwBytesRead);

VOID Read(HWND hWnd, CHAR psReadBuf, DWORD dwBytesRead);
VOID Read2(HWND hWnd, CHAR psReadBuf[], DWORD dwBytesRead);

#endif
