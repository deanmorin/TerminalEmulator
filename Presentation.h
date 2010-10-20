#ifndef PRESENTATION_H
#define PRESENTATION_H

#include "TerminalEmulator.h"

#define TAB_LENGTH  8
#define CLR_UP      -1
#define CLR_DOWN    1
#define CLR_LEFT    -1
#define CLR_RIGHT   1

VOID    Bell(HWND hWnd);
VOID    ClearLine(HWND hWnd, UINT cxCoord, UINT cyCoord, INT iDirection);
VOID    ClearScreen(HWND hWnd, UINT cxCoord, UINT cyCoord, INT iDirection);
VOID    FormFeed(HWND hWnd);
VOID    HorizontalTab(HWND hWnd);
VOID    MoveCursor(HWND hWnd, INT cxCoord, INT cyCoord, BOOL bScroll);
VOID    ProcessRead(HWND hWnd, CHAR psReadBuf[], DWORD dwBytesRead);
VOID    ProcessSpecialChar(HWND hWnd, CHAR cSpChar);
BOOL    ProcessWrite(HWND hWnd, WPARAM wParam, BOOL bNonCharKey);
VOID	ScreenAlignment(HWND hWnd);
VOID    ScrollDown(HWND hWnd);
VOID    ScrollUp(HWND hWnd);
VOID    SetScrollRegion(HWND hWnd, INT cyTop, INT cyBottom); 
VOID    UpdateDisplayBuf(HWND hWnd, CHAR cCharacter);


#endif
