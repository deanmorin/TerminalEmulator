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
LRESULT PerformMenuAction(HWND hWnd, UINT message, WPARAM wParam) {
    
    PWNDDATA   pwd = NULL;
    COMMCONFIG  cc;

    switch (LOWORD(wParam)) {
                
        case IDM_CONNECT:       
            if (!Connect(hWnd)) {
                Disconnect(hWnd);
            }
            return 0;

        case IDM_DISCONNECT:
            Disconnect(hWnd);
            return 0;

        case IDM_EXIT:
            PostMessage(hWnd, WM_DESTROY, 0, 0);
            return 0;

        case IDM_COM1:  SelectPort(hWnd, IDM_COM1);  return 0;
        case IDM_COM2:  SelectPort(hWnd, IDM_COM2);  return 0;
        case IDM_COM3:  SelectPort(hWnd, IDM_COM3);  return 0;
        case IDM_COM4:  SelectPort(hWnd, IDM_COM4);  return 0;
        case IDM_COM5:  SelectPort(hWnd, IDM_COM5);  return 0;
        case IDM_COM6:  SelectPort(hWnd, IDM_COM6);  return 0;
        case IDM_COM7:  SelectPort(hWnd, IDM_COM7);  return 0;
        case IDM_COM8:  SelectPort(hWnd, IDM_COM8);  return 0;
        case IDM_COM9:  SelectPort(hWnd, IDM_COM9);  return 0;

        case IDM_COMMSET:
            pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
            CommConfigDialog(pwd->lpszCommName, hWnd, &cc);
            SetCommState(pwd->hPort, &cc.dcb);
		    return 0;
        
        default:
            return 0;
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION:    EchoBuffer
--
-- DATE:        Oct 03, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Dean Morin
--
-- PROGRAMMER:  Dean Morin
--
-- INTERFACE:   VOID EchoBuffer(HWND, TCHAR, COLORREF
--
-- RETURNS:     VOID.
--
-- NOTES:
--              Takes the TCHAR array that is passed, and prints it to the
--              screen, using the COLORREF as the text color. szBuffer and
--              textColor are used as arguments in StoreTextForRepaint.
------------------------------------------------------------------------------*/
VOID EchoBuffer(HWND hWnd, TCHAR szBuffer[], COLORREF textColor) {

    PWNDDATA   pwd = NULL;
    HDC         hdc = NULL;
    TEXTMETRIC  tm;
    SIZE        size;
    RECT        rect;


    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
    hdc = GetDC(hWnd);

    if (!StoreTextForRepaint(hWnd, szBuffer, textColor)) {
        DISPLAY_ERROR("Failed to allocate memory");
    }

    SetTextColor(hdc, textColor);

    GetTextMetrics(hdc, &tm);
    GetTextExtentPoint32(hdc, szBuffer, _tcslen(szBuffer), &size);
    
    TextOut(hdc, pwd->textInfo.cxPos, pwd->textInfo.cyPos, (LPCWSTR) szBuffer,
            _tcslen(szBuffer));
    
    pwd->textInfo.cxPos += size.cx;
    GetClientRect(hWnd, &rect);
    
    if (pwd->textInfo.cxPos >= rect.right - tm.tmMaxCharWidth) {
        pwd->textInfo.cxPos = CELL_PADDING;
        pwd->textInfo.cyPos += tm.tmHeight + tm.tmExternalLeading;
    }       
    ReleaseDC(hWnd, hdc);
}

/*------------------------------------------------------------------------------
-- FUNCTION:    ReadPort
--
-- DATE:        Oct 03, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Dean Morin
--
-- PROGRAMMER:  Dean Morin
--
-- INTERFACE:   BOOL ReadPort(HWND)
--
-- RETURNS:     True if the port read was successful.
--
-- NOTES:
--              After reading a byte off the serial port, the function calls    
--              EchoBuffer() to display it on the screen.
------------------------------------------------------------------------------*/
/*BOOL ReadPort(HWND hWnd) {
    
    PWNDDATA   pwd             = NULL;
    TCHAR       szReadBuf[2]    = {0};
    DWORD       dwBytesRead     = 0;
    
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

    ReadFile(pwd->hPort, szReadBuf, 1, &dwBytesRead, NULL);

    if (dwBytesRead) {
        EchoBuffer(hWnd, szReadBuf, RGB(0,170,0));
        return TRUE;
    }
    return FALSE;
}
*/
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

/*------------------------------------------------------------------------------
-- FUNCTION:    StoreTextForRepaint
--
-- DATE:        Oct 03, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Dean Morin
--
-- PROGRAMMER:  Dean Morin
--
-- INTERFACE:   VOID StoreTextForRepaint(HWND, TCHAR[], COLORREF)
--
-- RETURNS:     FALSE if malloc failed to allocate any nodes.
--
-- NOTES:
--              Adds the character in szBuffer as a node at the end of a linked
--              list. Adds color info to a seperate linked list. The list
--              increments a counter if the text color was the same as the last
--              character. If it's different, a new node is added to the end.
--              These lists are accessable through the wndExtra pointer.
------------------------------------------------------------------------------*/
BOOL StoreTextForRepaint(HWND hWnd, TCHAR szBuffer[], COLORREF textColor) {
    
    PWNDDATA   pwd             = NULL;
    PTCHARNODE  newtCharNode    = NULL;
    PCOLORNODE  newColorNode    = NULL;

    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
    if ((newtCharNode = (PTCHARNODE) malloc(sizeof(tCharNode))) == NULL) {
        return FALSE;
    }
    newtCharNode->data   = szBuffer[0];

    /* Store the character */
    if (pwd->textInfo.ptCharHead == NULL) {
        pwd->textInfo.ptCharHead        = newtCharNode;
        pwd->textInfo.ptCharTail        = pwd->textInfo.ptCharHead;
        pwd->textInfo.ptCharTail->next  = NULL;
    
    } else {
        pwd->textInfo.ptCharTail->next  = newtCharNode;
        pwd->textInfo.ptCharTail        = pwd->textInfo.ptCharTail->next;
        pwd->textInfo.ptCharTail->next  = NULL;
    }
    
    /* Store the color info */
    if (pwd->textInfo.pColorHead == NULL) {
        if ((newColorNode = (PCOLORNODE) malloc(sizeof(colorNode))) == NULL) {
            return FALSE;
        }
        newColorNode->color            = textColor;
        newColorNode->count            = 1;
        pwd->textInfo.pColorHead       = newColorNode;
        pwd->textInfo.pColorTail       = pwd->textInfo.pColorHead;
        pwd->textInfo.pColorTail->next = NULL;

    } else if (pwd->textInfo.pColorTail->color == textColor) {
        pwd->textInfo.pColorTail->count++;

    } else {
        if ((newColorNode = (PCOLORNODE) malloc(sizeof(colorNode))) == NULL) {
            return FALSE;
        }
        newColorNode->color            = textColor;
        newColorNode->count            = 1;
        pwd->textInfo.pColorTail->next = newColorNode;
        pwd->textInfo.pColorTail       = pwd->textInfo.pColorTail->next;
        pwd->textInfo.pColorTail->next = NULL;
    }
    return TRUE;
}

/*------------------------------------------------------------------------------
-- FUNCTION:    WriteToPort
--
-- DATE:        Oct 03, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Dean Morin
--
-- PROGRAMMER:  Dean Morin
--
-- INTERFACE:   BOOL WriteToPort(HWND, WPARAM)
--
-- RETURNS:     True if the port write was successful.
--
-- NOTES:
--              After displaying the character in wParam on screen, it writes it
--              to the serial port.
------------------------------------------------------------------------------*/
BOOL WriteToPort(HWND hWnd, WPARAM wParam) {
    
    PWNDDATA    pwd             = NULL;
    TCHAR       szWriteBuf[2]   = {0};
    DWORD       dwBytesRead     = 0;

    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
    _stprintf(szWriteBuf, TEXT("%C", wParam), wParam);           
    EchoBuffer(hWnd, szWriteBuf, RGB(0,0,0));

    if (!WriteFile(pwd->hPort, szWriteBuf, 1, &dwBytesRead, NULL)) {
        DISPLAY_ERROR("UHOH");
        return FALSE;
    }
    return TRUE;
}
