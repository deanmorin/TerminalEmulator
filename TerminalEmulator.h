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

/*--------------------Constants Used in Calculations--------------------------*/
#define ASCII_DIGIT_OFFSET  48      // The ascii value for '0'

#define DISPLAY_ERROR(x)    MessageBox(NULL, TEXT(x), TEXT(""), MB_OK)

/*-------------------------------Structures-----------------------------------*/
typedef struct tCharNode tCharNode;
typedef struct tCharNode {
    TCHAR       data;
    tCharNode   *next;
} *PTCHARNODE;


typedef struct colorNode colorNode;
typedef struct colorNode {
    COLORREF    color;
    UINT        count;
    colorNode   *next;
} *PCOLORNODE;

typedef struct textInfo {
    UINT        uMinStrLength;
    PTCHARNODE  ptCharHead;
    PTCHARNODE  ptCharTail;
    LONG        cxPos;
    LONG        cyPos;
    PCOLORNODE  pColorHead;
    PCOLORNODE  pColorTail;
} TEXTINFO;

typedef struct wndData {
    HANDLE          hPort;
    LPTSTR          lpszCommName;
    BOOL            bConnected;
    HANDLE          hThread;
    DWORD           dwThreadid;
    COMMTIMEOUTS    defaultTimeOuts;
    CHAR*           psIncompleteEsc;
    DWORD           dwIncompleteLength;
    TEXTINFO        textInfo;
} WNDDATA, *PWNDDATA;

/*---------------------------Function Prototyes-------------------------------*/
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif
