#ifndef PRESENTATION_H
#define PRESENTATION_H

#include "TerminalEmulator.h"
#include <string.h>

#define TAB_LENGTH  8
#define CLR_UP      -1
#define CLR_DOWN    1
#define CLR_LEFT    -1
#define CLR_RIGHT   1

VOID    ProcessEsc(HWND hWnd, CHAR* psBuffer, DWORD dwBytesRead);
VOID    ProcessRead(HWND hWnd, CHAR psReadBuf[], DWORD dwBytesRead);
VOID    ProcessSpecialChar(HWND hWnd, CHAR cSpChar);
BOOL    ProcessWrite(HWND hWnd, WPARAM wParam, BOOL bNonCharKey);
VOID    UpdateDisplayBuf(HWND hWnd, CHAR cCharacter);
VOID    Bell(HWND hWnd);
VOID    BackSpace(HWND hWnd);
VOID    HorizontalTab(HWND hWnd);
VOID    LineFeed(HWND hWnd);
VOID    VerticalTab(HWND hWnd);
VOID    FormFeed(HWND hWnd);
VOID    CarraigeReturn(HWND hWnd);
VOID    ClearLine(HWND hWnd, UINT cxCoord, UINT cyCoord, INT iDirection);
VOID    ClearScreen(HWND hWnd, UINT cxCoord, UINT cyCoord, INT iDirection);
VOID    MoveCursor(HWND hWnd, INT cxCoord, INT cyCoord);
VOID    ScrollDown(hWnd);

#endif
