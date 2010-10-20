#ifndef APPLICATION_H
#define APPLICATION_H

#include "TerminalEmulator.h"

static const COLORREF TXT_COLOURS[16] = {RGB(0,0,0),		RGB(128,0,0),
								         RGB(0,128,0),		RGB(128,128,0),
								         RGB(0,0,128),		RGB(128,0,128),
								         RGB(0,128,128),	RGB(192,192,192),
								         RGB(128,128,128),  RGB(255,0,0),
								         RGB(0,255,0),		RGB(255,255,0),
								         RGB(0,0,255),		RGB(255,0,255),
								         RGB(0,255,255),	RGB(255,255,255)};

VOID    InitTerminal(HWND hWnd);
VOID    Paint(HWND hWnd);
VOID    PerformMenuAction(HWND hWnd, WPARAM wParam);
VOID    SetBell(HWND hWnd, INT iSelected);

#endif
