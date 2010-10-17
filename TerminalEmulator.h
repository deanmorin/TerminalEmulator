#ifndef STRICT
#define STRICT
#endif

#ifndef ASSIGN01_H
#define ASSIGN01_H

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include "Application.h"
#include "Menu.h"
#include "Physical.h"
#include "Presentation.h"
#include "Session.h"

#pragma warning(disable:4002)

/*--------------------------------Macros--------------------------------------*/
#define ASCII_DIGIT_OFFSET  48      // the ascii value for '0'
#define PADDING             5      // the distance between the edge of the
                                    // client area, and any text
#define NO_OF_PORTS         9       // the number of ports available from the
                                    // "Select Ports" dropdown
#define CHARS_PER_LINE      80      // characters per line
#define LINES_PER_SCRN      24      // lines per screen

#define DISPLAY_ERROR(x)    MessageBox(NULL, TEXT(x), TEXT(""), MB_OK)
#define X                   pwd->displayBuf.cxCursor
#define Y                   pwd->displayBuf.cyCursor
#define X_POS               pwd->displayBuf.cxCursor * pwd->displayBuf.cxChar + PADDING
#define Y_POS               pwd->displayBuf.cyCursor * pwd->displayBuf.cyChar + PADDING
#define CHAR_WIDTH          pwd->displayBuf.cxChar
#define CHAR_HEIGHT         pwd->displayBuf.cyChar
#define CHARACTER(X, Y)     pwd->displayBuf.rows[Y]->columns[X]
#define SET_BUFFER(C, X, Y) pwd->displayBuf.rows[Y]->columns[X].character = C;

/*-------------------------------Structures-----------------------------------*/
typedef struct charInfo {
    CHAR    character;
    BYTE    color;
    BYTE    style;
} CHARINFO;

typedef struct line line;
typedef struct line {
    CHARINFO    columns[CHARS_PER_LINE];
} LINE, *PLINE;

typedef struct displayBuf {
    PLINE   rows[LINES_PER_SCRN];
    HFONT   hFont;
    UINT    cxChar;
    UINT    cyChar;
    UINT    cxCursor;
    UINT    cyCursor;
} DISPLAYBUF;

typedef struct wndData {
    HANDLE          hPort;
    LPTSTR          lpszCommName;
    BOOL            bConnected;
    HANDLE          hThread;
    DWORD           dwThreadid;
    COMMTIMEOUTS    defaultTimeOuts;
    CHAR*           psIncompleteEsc;
    DWORD           dwIncompleteLength;
    DISPLAYBUF      displayBuf;
} WNDDATA, *PWNDDATA;

/*---------------------------Function Prototyes-------------------------------*/
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif
