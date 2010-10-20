#ifndef SESSION_H
#define SESSION_H

#include "TerminalEmulator.h"

BOOL    Connect(HWND hWnd);
VOID    Disconnect(HWND hWnd);
VOID    SelectPort(HWND hWnd, INT iSelected);

#endif
