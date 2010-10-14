#ifndef PRESENTATION_H
#define PRESENTATION_H

#include "TerminalEmulator.h"

VOID    ProcessRead(HWND hWnd, CHAR szReadBuf[], DWORD dwBytesRead);
BOOL    ProcessWrite(HWND hWnd, WPARAM wParam, BOOL bNonCharKey);

#endif
