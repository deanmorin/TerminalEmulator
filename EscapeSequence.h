#ifndef ESCAPESEQUENCE_H
#define ESCAPESEQUENCE_H

#include <ctype.h>
#include <string.h>
#include "TerminalEmulator.h"

BOOL	CheckDigits(HWND hWnd, CHAR* psBuffer, DWORD length, DWORD *i);
BOOL	CheckDigitsQ(HWND hWnd, CHAR* psBuffer, DWORD length, DWORD *i);
BOOL	CheckDigitsSemi(HWND hWnd, CHAR* psBuffer, DWORD length, DWORD *i);
DWORD	GetDigit(CHAR* psBuffer, DWORD length, DWORD *i);
VOID    ProcessEsc(HWND hWnd, CHAR* psBuffer, DWORD dwBytesRead);
VOID    ProcessFont(HWND hWnd);
BOOL	ProcessParen(CHAR* psBuffer, DWORD length, DWORD *i);
BOOL	ProcessSquare(HWND hWnd, CHAR* psBuffer, DWORD length, DWORD *i);

#endif
