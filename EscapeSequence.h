#ifndef ESCAPESEQUENCE_H
#define ESCAPESEQUENCE_H

#include <ctype.h>
#include <string.h>
#include "TerminalEmulator.h"

VOID    ProcessEsc(HWND hWnd, CHAR* psBuffer, DWORD dwBytesRead);
BOOL	ProcessSquare(HWND hWnd, CHAR* psBuffer, DWORD length, DWORD *i);
BOOL	ProcessParen(CHAR* psBuffer, DWORD length, DWORD *i);
DWORD	GetDigit(CHAR* psBuffer, DWORD length, DWORD *i);
BOOL	CheckDigits(HWND hWnd, CHAR* psBuffer, DWORD length, DWORD *i);
BOOL	CheckDigitsQ(HWND hWnd, CHAR* psBuffer, DWORD length, DWORD *i);
BOOL	CheckDigitsSemi(HWND hWnd, CHAR* psBuffer, DWORD length, DWORD *i);
VOID    ProcessFont(HWND hWnd);

#endif
