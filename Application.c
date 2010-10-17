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

/*------------------------------------------------------------------------------
-- FUNCTION:    SelectPort
--
-- DATE:        Oct 03, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Dean Morin
--
-- PROGRAMMER:  Dean Morin
--
-- INTERFACE:   VOID SelectPort(HWND, INT)
--
-- RETURNS:     VOID.
--
-- NOTES:
--              Selects a new port. It changes the menu checkmark to the new
--              port, and it changes the actual port by renaming lpszCommName.
                The INT argument is the menu item that was selected.
------------------------------------------------------------------------------*/
VOID SelectPort(HWND hWnd, INT selected) {
    
    PWNDDATA pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

    INT prevPortNo = pwd->lpszCommName[3];
    CheckMenuItem(GetMenu(hWnd),
                 (IDM_COM1 - 1) + (prevPortNo - ASCII_DIGIT_OFFSET),
                 MF_UNCHECKED);
    CheckMenuItem(GetMenu(hWnd), selected, MF_CHECKED);

    switch (selected) {

        case IDM_COM1:  pwd->lpszCommName = TEXT("COM1");   return;
        case IDM_COM2:  pwd->lpszCommName = TEXT("COM2");   return;       
        case IDM_COM3:  pwd->lpszCommName = TEXT("COM3");   return;       
        case IDM_COM4:  pwd->lpszCommName = TEXT("COM4");   return;        
        case IDM_COM5:  pwd->lpszCommName = TEXT("COM5");   return;        
        case IDM_COM6:  pwd->lpszCommName = TEXT("COM6");   return;        
        case IDM_COM7:  pwd->lpszCommName = TEXT("COM7");   return;        
        case IDM_COM8:  pwd->lpszCommName = TEXT("COM8");   return;        
        case IDM_COM9:  pwd->lpszCommName = TEXT("COM9");   return;
        
        default:    return;
    }
}
