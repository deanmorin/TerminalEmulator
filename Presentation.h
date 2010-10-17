#ifndef PRESENTATION_H
#define PRESENTATION_H

#include "TerminalEmulator.h"

#define TAB_LENGTH  8

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
VOID    MoveCursor(HWND hWnd, DWORD cxCoord, DWORD cyCoord);

#endif
