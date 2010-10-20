/*------------------------------------------------------------------------------
-- SOURCE FILE:     Application.c - Contains all the OSI "application layer"
--                                  functions for the Terminal Emulator.
--
-- PROGRAM:     Advanced Terminal Emulator Pro
--
-- FUNCTIONS:
--              VOID    InitTerminal(HWND);
--              LRESULT PerformMenuAction(HWND, UINT, WPARAM);
--              BOOL    ReadPort(HWND); 8*******************************************************
--              VOID    SelectPort(HWND, INT);
--              BOOL    StoreTextForRepaint(HWND, TCHAR[], COLORREF);
--              BOOL    WriteToPort(HWND, WPARAM);
--
--
-- DATE:        Oct 19, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Dean Morin
--
-- PROGRAMMER:  Dean Morin
--
-- NOTES:
-- Contains application level functions for the Terminal Emulator program.
------------------------------------------------------------------------------*/

#include "Application.h"


VOID InitTerminal(HWND hWnd) {

    PWNDDATA    pwd         = {0};
    HDC         hdc         = {0};
    COMMCONFIG  cc          = {0};
    TEXTMETRIC  tm          = {0};
    PAINTSTRUCT ps          = {0};
    RECT        windowRect  = {0};
    RECT        clientRect  = {0};
    UINT        i           = 0;
    UINT        j           = 0;
    LONG        lxDiff      = 0;
    LONG        lyDiff      = 0;

    if ((pwd = (PWNDDATA) calloc(1, sizeof(WNDDATA))) == 0) {
        DISPLAY_ERROR("Error allocating memory for WNDDATA structure");
    }
    pwd->lpszCommName   = TEXT("COM3");
    SetWindowLongPtr(hWnd, 0, (LONG_PTR) pwd);

    // set default comm settings
    cc.dwSize = sizeof(COMMCONFIG);
    GetCommConfig(pwd->hPort, &cc, &cc.dwSize);
    FillMemory(&cc.dcb, sizeof(DCB), 0);
    cc.dcb.DCBlength = sizeof(DCB);
    BuildCommDCB((LPCWSTR)"96,N,8,1", &cc.dcb);

    pwd->bConnected         = FALSE;
    pwd->psIncompleteEsc    = NULL;
    pwd->iBellSetting       = IDM_BELL_DIS;

    // get text attributes and store values into the window extra struct
    hdc = GetDC(hWnd);
	pwd->displayBuf.hFont = (HFONT) GetStockObject(OEM_FIXED_FONT);
    SelectObject(hdc, pwd->displayBuf.hFont);
    GetTextMetrics(hdc, &tm);
    ReleaseDC(hWnd, hdc);

    CHAR_WIDTH      = tm.tmAveCharWidth;
    CHAR_HEIGHT     = tm.tmHeight;
    CUR_FG_COLOR    = 7;
    WINDOW_BOTTOM   = LINES_PER_SCRN -1;

    CreateCaret(hWnd, NULL, PADDING, PADDING);
    ShowCaret(hWnd);

    // initialize a "blank" display buffer
    for (i = 0; i < LINES_PER_SCRN; i++) {
        pwd->displayBuf.rows[i] = (PLINE) calloc(1, sizeof(LINE));
        for (j = 0; j < CHARS_PER_LINE; j++) {
            CHARACTER(j, i).character   = ' ';
            CHARACTER(j, i).fgColor     = 7;
        }
    }
    GetWindowRect(hWnd, &windowRect);
    GetClientRect(hWnd, &clientRect);

    lxDiff  = (windowRect.right  - windowRect.left) 
            - (clientRect.right  - clientRect.left);
    lyDiff  = (windowRect.bottom - windowRect.top)
            - (clientRect.bottom - clientRect.top);

    MoveWindow(hWnd,
               windowRect.left, windowRect.top,
               CHAR_WIDTH  * CHARS_PER_LINE + PADDING * 2 + lxDiff,
               CHAR_HEIGHT * LINES_PER_SCRN + PADDING * 2 + lyDiff,
               TRUE);
}

/*------------------------------------------------------------------------------
-- FUNCTION:    PerformMenuAction
--
-- DATE:        Oct 19, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Dean Morin
--
-- PROGRAMMER:  Dean Morin
--
-- INTERFACE:   LRESULT PerformMenuAction(HWND, UINT, WPARAM)
--
-- RETURNS:     VOID.
--
-- NOTES:
--              Responds to a user's selection of a menu item.
------------------------------------------------------------------------------*/
VOID PerformMenuAction(HWND hWnd, UINT message, WPARAM wParam) {
    
    PWNDDATA   pwd = NULL;
    COMMCONFIG  cc;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

    switch (LOWORD(wParam)) {
                
        case IDM_CONNECT:       
            Connect(hWnd);
            return;

        case IDM_DISCONNECT:
            Disconnect(hWnd);
            return;

        case IDM_EXIT:
            PostMessage(hWnd, WM_DESTROY, 0, 0);
            return;

        case IDM_COM1:  SelectPort(hWnd, IDM_COM1);  return;
        case IDM_COM2:  SelectPort(hWnd, IDM_COM2);  return;
        case IDM_COM3:  SelectPort(hWnd, IDM_COM3);  return;
        case IDM_COM4:  SelectPort(hWnd, IDM_COM4);  return;
        case IDM_COM5:  SelectPort(hWnd, IDM_COM5);  return;
        case IDM_COM6:  SelectPort(hWnd, IDM_COM6);  return;
        case IDM_COM7:  SelectPort(hWnd, IDM_COM7);  return;
        case IDM_COM8:  SelectPort(hWnd, IDM_COM8);  return;
        case IDM_COM9:  SelectPort(hWnd, IDM_COM9);  return;

        case IDM_COMMSET:
            if (!CommConfigDialog(pwd->lpszCommName, hWnd, &cc)) {
                DISPLAY_ERROR("The comm settings dialogue failed");
            }
            if (!SetCommState(pwd->hPort, &cc.dcb)) {
                DISPLAY_ERROR("The comm settings did not set properly.\nPlease ensure that the port exists");
            }
		    return;

        case IDM_BELL_DIS:  SetBell(hWnd, IDM_BELL_DIS);    return;
        case IDM_BELL_VIS:  SetBell(hWnd, IDM_BELL_VIS);    return;
        case IDM_BELL_AUR:  SetBell(hWnd, IDM_BELL_AUR);    return;
        
        default:
            return;
    }
}


VOID Paint(HWND hWnd) {
    PLOGFONT	    plf         = NULL;
    PWNDDATA        pwd         = NULL;
    CHAR            a[2]        = {0};
    HDC             hdc         = {0};
    PAINTSTRUCT     ps          = {0};
    UINT            i           = 0;
    UINT            j           = 0;
    UINT            tempfgColor = 0;
    UINT            tempbgColor = 0;
	UINT            tempStyle	= 0;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
    plf = (PLOGFONT) calloc(1, sizeof(LOGFONT));

    HideCaret(hWnd);
    hdc = BeginPaint(hWnd, &ps) ;
    SelectObject(hdc, pwd->displayBuf.hFont);

    tempfgColor = CUR_FG_COLOR;
    tempbgColor = CUR_BG_COLOR;
	tempStyle	= CUR_STYLE;

    SetTextColor(hdc, TXT_COLOURS[CUR_FG_COLOR]);
    SetBkColor(hdc, TXT_COLOURS[CUR_BG_COLOR]);

                             
    for (i = 0; i < LINES_PER_SCRN; i++) {
        for (j = 0; j < CHARS_PER_LINE; j++) {
            
            if (CHARACTER(j, i).fgColor != tempfgColor) {
                SetTextColor(hdc, TXT_COLOURS[CHARACTER(j, i).fgColor]);
                tempfgColor = CHARACTER(j, i).fgColor;
            }
            if (CHARACTER(j, i).bgColor != tempbgColor) {
	            SetBkColor(hdc, TXT_COLOURS[CHARACTER(j, i).bgColor]);
                tempbgColor = CHARACTER(j, i).bgColor;
            }/*
            if (CHARACTER(j, i).style != CUR_STYLE) {
				GetObject(pwd->displayBuf.hFont, sizeof(LOGFONT), plf);
				plf->lfUnderline = CHARACTER(j, i).style;
				SelectObject(hdc, CreateFontIndirect(plf));
				tempStyle	= CUR_STYLE;
            }*/

            a[0] = CHARACTER(j, i).character;
            TextOut(hdc, CHAR_WIDTH * j + PADDING, CHAR_HEIGHT * i + PADDING,
                    (LPCWSTR) a, 1);
        }
    }
    EndPaint(hWnd, &ps);
    SetCaretPos(X_POS, Y_POS);
    ShowCaret(hWnd);
}


VOID ShowTheCursor(HWND hWnd, BYTE flag) {
    static INT count = 0;
    if (flag == CUR_HIDE) {
        while (count > 0) {
            HideCaret(hWnd);
            count--;
        }
    } else {
        while (count <= 0) {
            ShowCaret(hWnd);
            count++;
        }
    }
}


VOID SetBell(HWND hWnd, INT iSelected) {
    PWNDDATA pwd;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
    CheckMenuItem(GetMenu(hWnd), pwd->iBellSetting, MF_UNCHECKED);
    CheckMenuItem(GetMenu(hWnd), iSelected,        MF_CHECKED);
    pwd->iBellSetting = iSelected;
}
