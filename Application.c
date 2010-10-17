/*------------------------------------------------------------------------------
-- SOURCE FILE:     Application.c - Contains all the OSI "application layer"
--                                  functions for the Dumb Terminal Emulator.
--
-- PROGRAM:     Hyper Omega Terminal
--
-- FUNCTIONS:
--              BOOL    Connect(HWND);
--              VOID    Disconnect(HWND);
--              VOID    EchoBuffer(HWND, TCHAR[], COLORREF);
--              LRESULT PerformMenuAction(HWND, UINT, WPARAM);
--              BOOL    ReadPort(HWND);
--              VOID    SelectPort(HWND, INT);
--              BOOL    StoreTextForRepaint(HWND, TCHAR[], COLORREF);
--              BOOL    WriteToPort(HWND, WPARAM);
--
--
-- DATE:        Oct 03, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Dean Morin
--
-- PROGRAMMER:  Dean Morin
--
-- NOTES:
-- Contains application level functions for the Dumb Terminal Emulator program.
------------------------------------------------------------------------------*/

#include "Application.h"


VOID InitTerminal(HWND hWnd) {

    PWNDDATA    pwd = {0};
    HDC         hdc = {0};
    COMMCONFIG  cc  = {0};
    TEXTMETRIC  tm  = {0};
    PAINTSTRUCT ps  = {0};
    UINT        i   = 0;
    UINT        j   = 0;


    if ((pwd = (PWNDDATA) malloc(sizeof(WNDDATA))) == 0) {
        DISPLAY_ERROR("Error allocating memory for WNDDATA structure");
    }
    pwd->lpszCommName   = TEXT("COM3");
    pwd->hPort          = NULL;
    SetWindowLongPtr(hWnd, 0, (LONG_PTR) pwd);

    cc.dwSize = sizeof(COMMCONFIG);
    GetCommConfig(pwd->hPort, &cc, &cc.dwSize);
    FillMemory(&cc.dcb, sizeof(DCB), 0);
    cc.dcb.DCBlength = sizeof(DCB);
    BuildCommDCB((LPCWSTR)"96,N,8,1", &cc.dcb);

    pwd->bConnected         = FALSE;
    pwd->psIncompleteEsc    = NULL;

    hdc = GetDC(hWnd);
    SelectObject(hdc, GetStockObject(OEM_FIXED_FONT));
    GetTextMetrics(hdc, &tm);
    pwd->displayBuf.cxChar = tm.tmAveCharWidth;
    pwd->displayBuf.cyChar = tm.tmHeight;
    ReleaseDC(hWnd, hdc);

    X = 0;
    Y = 0;
    CreateCaret(hWnd, NULL, PADDING, PADDING);
    ShowCaret(hWnd);

    for (i = 0; i < LINES_PER_SCRN; i++) {
        pwd->displayBuf.rows[i] = (PLINE) malloc(sizeof(LINE));
        for (j = 0; j < CHARS_PER_LINE; j++) {
            CHARACTER(j, i).character   = ' ';
            CHARACTER(j, i).color       = 0;
            CHARACTER(j, i).style       = 0;
        }
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION:    PerformMenuAction
--
-- DATE:        Oct 3, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Dean Morin
--
-- PROGRAMMER:  Dean Morin
--
-- INTERFACE:   LRESULT PerformMenuAction(HWND, UINT, WPARAM)
--
-- RETURNS:     The result that will be passed onto the winproc. It will always
--              be 0 since this function does not check for any errors.
--
-- NOTES:
--              Responds to a user's selection of a menu item.
------------------------------------------------------------------------------*/
VOID PerformMenuAction(HWND hWnd, UINT message, WPARAM wParam) {
    
    PWNDDATA   pwd = NULL;
    COMMCONFIG  cc;

    switch (LOWORD(wParam)) {
                
        case IDM_CONNECT:       
            if (!Connect(hWnd)) {
                Disconnect(hWnd);
            }
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
            pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
            CommConfigDialog(pwd->lpszCommName, hWnd, &cc);
            SetCommState(pwd->hPort, &cc.dcb);
		    return;
        
        default:
            return;
    }
}

VOID MoveCursor(HWND hWnd, DWORD cxCoord, DWORD cyCoord) {
    PWNDDATA pwd = NULL;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
    X = cxCoord - 1;
    Y = cyCoord - 1;
    SetCaretPos(X_POS, Y_POS);
}
