#ifndef STRICT
#define STRICT
#endif

#ifndef ASSIGN01_H
#define ASSIGN01_H

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include "Application.h"
#include "EscapeSequence.h"
#include "Menu.h"
#include "Physical.h"
#include "Presentation.h"
#include "Session.h"

#pragma warning(disable:4002)
#pragma warning(disable:4996)

/*--------------------------------Macros--------------------------------------*/
#define ASCII_DIGIT_OFFSET  48      // the ascii value for '0'
#define PADDING             10      // the distance between the edge of the
                                    // client area, and any text
#define NO_OF_PORTS         9       // the number of ports available from the
                                    // "Select Ports" dropdown
#define CHARS_PER_LINE      80      // characters per line
#define LINES_PER_SCRN      24      // lines per screen
#define X_FUDGE_FACTOR      1.01    // a hack used to size the window  
#define Y_FUDGE_FACTOR      1.19    // a hack used to size the window

#define DISPLAY_ERROR(x)    MessageBox(NULL, TEXT(x), TEXT(""), MB_OK)
#define X                   pwd->displayBuf.cxCursor
#define Y                   pwd->displayBuf.cyCursor
#define X_POS               pwd->displayBuf.cxCursor * pwd->displayBuf.cxChar + PADDING
#define Y_POS               pwd->displayBuf.cyCursor * pwd->displayBuf.cyChar + PADDING
#define CHAR_WIDTH          pwd->displayBuf.cxChar
#define CHAR_HEIGHT         pwd->displayBuf.cyChar
#define CHARACTER(x, y)     pwd->displayBuf.rows[y]->columns[x]
#define SET_BUFFER(c, x, y) pwd->displayBuf.rows[y]->columns[x].character = c;
#define ROW(y)              pwd->displayBuf.rows[y]
#define ESC_VAL(x)          pwd->dwEscSeqValues[x]
#define CUR_FG_COLOR        pwd->displayBuf.fgColor
#define CUR_BG_COLOR        pwd->displayBuf.bgColor
#define CUR_STYLE           pwd->displayBuf.style
#define BRIGHTNESS			pwd->displayBuf.brightness

/*-------------------------------Structures-----------------------------------*/
typedef struct charInfo {
    CHAR    character;
    BYTE    fgColor;
    BYTE    bgColor;
    BYTE    style;
} CHARINFO;

typedef struct line line;
typedef struct line {
    CHARINFO    columns[CHARS_PER_LINE];
} LINE, *PLINE;

typedef struct displayBuf {
    PLINE   rows[LINES_PER_SCRN];
    UINT    cxChar;
    UINT    cyChar;
    UINT    cxCursor;
    UINT    cyCursor;
    HFONT	hFont;
	BYTE    fgColor;
    BYTE    bgColor;
    BYTE    style;
	BYTE	brightness;
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
    DWORD           dwEscSeqValues[32];            
} WNDDATA, *PWNDDATA;

/*---------------------------Function Prototyes-------------------------------*/
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif
