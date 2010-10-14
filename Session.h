#ifndef SESSION_H
#define SESSION_H

#include <Windows.h>
#include "TerminalEmulator.h"

BOOL    Connect(HWND hWnd);
VOID    Disconnect(HWND hWnd);

#endif
