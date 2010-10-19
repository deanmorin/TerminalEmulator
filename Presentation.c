#include "Presentation.h"

/*------------------------------------------------------------------------------
-- FUNCTION:    ProcessWrite
--
-- DATE:        Oct 13, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Dean Morin
--
-- PROGRAMMER:  Dean Morin
--
-- INTERFACE:   BOOL ProcessWrite(HWND, WPARAM, BOOL)
--
-- RETURNS:     True if the port write was successful.
--
-- NOTES:
--              Writes wParam (the key pressed) to the port, if. If the key is a
--              special non-character key such as 'home' or 'arrow up', it is 
--              first translated to its corresponding command sequence.
------------------------------------------------------------------------------*/
BOOL ProcessWrite(HWND hWnd, WPARAM wParam, BOOL bNonCharKey) {
 
    PWNDDATA    pwd             = {0};
    CHAR        psWriteBuf[4]   = {0};
    OVERLAPPED  overlap         = {0};
    DWORD       dwBytesRead     = 0;
    UINT        bufLength       = 1;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

    if (bNonCharKey) {

        psWriteBuf[0] = VK_ESCAPE;
		if (wParam <= VK_DOWN && pwd->cursorMode) { 
            psWriteBuf[1] = '[';
        } else {
            psWriteBuf[1] = 'O';
        }
        bufLength = 3;

        switch (wParam) {
            case VK_UP:     psWriteBuf[2] = 'A';    break;
            case VK_DOWN:   psWriteBuf[2] = 'B';    break;
            case VK_RIGHT:  psWriteBuf[2] = 'C';    break;
            case VK_LEFT:   psWriteBuf[2] = 'D';    break;
            case VK_HOME:   psWriteBuf[2] = 'H';    break;
            case VK_END:    psWriteBuf[2] = 'K';    break;
            case VK_F1:     psWriteBuf[2] = 'P';    break;
            case VK_F2:     psWriteBuf[2] = 'Q';    break;
            case VK_F3:     psWriteBuf[2] = 'R';    break;
            case VK_F4:     psWriteBuf[2] = 'S';    break;
        }

    } else {
        psWriteBuf[0] = wParam;
    }

    if (!WriteFile(pwd->hPort, psWriteBuf, bufLength, &dwBytesRead, &overlap)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return FALSE;
        }
    }
    return TRUE;
}


VOID ProcessRead(HWND hWnd, CHAR* psReadBuf, DWORD dwBytesRead) {

    PWNDDATA    pwd                 = NULL;
    CHAR*       psCombined          = NULL;
    DWORD       dwCombinedLength    = 0;
    DWORD       i                   = 0;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
    
    if (pwd->psIncompleteEsc) {
        // the last escape sequence was incomplete, but still could be valid
        dwCombinedLength    = pwd->dwIncompleteLength + dwBytesRead;
        psCombined          = (CHAR*) malloc(sizeof(CHAR) * dwCombinedLength);
        strncpy(psCombined, pwd->psIncompleteEsc, pwd->dwIncompleteLength);
        strncpy((psCombined + pwd->dwIncompleteLength), psReadBuf, dwBytesRead);
        pwd->psIncompleteEsc = NULL;
        ProcessEsc(hWnd, psCombined, dwCombinedLength);
        return;
    }
    for (i = 0; i < dwBytesRead; i++) {
        if (psReadBuf[i] == VK_ESCAPE) {
            ProcessEsc(hWnd, psReadBuf + i, dwBytesRead - i);
            return;
        }
        if (psReadBuf[i] >= VK_SPACE  &&  psReadBuf[i] <= '~') {
            UpdateDisplayBuf(hWnd, psReadBuf[i]);
        } else {
            ProcessSpecialChar(hWnd, psReadBuf[i]);
        }
    }
}


VOID ProcessSpecialChar(HWND hWnd, CHAR cSpChar) {
    
    switch (cSpChar) {
        case 0x07:  Bell(hWnd);             break;
        case 0x08:  BackSpace(hWnd);        break;
        case 0x09:  HorizontalTab(hWnd);    break;
        case 0x0A:  LineFeed(hWnd);         break;
        case 0x0B:  VerticalTab(hWnd);      break;
        case 0x0C:  FormFeed(hWnd);         break;
        case 0x0D:  CarraigeReturn(hWnd);   break;
        default:                            break;  // ignore character
    }
}


VOID UpdateDisplayBuf(HWND hWnd, CHAR cCharacter) {
    
    PWNDDATA    pwd     = NULL;
    CHAR        a[2]    = {0};    
    HDC         hdc     = {0};
    CHARINFO    ci      = {0};
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

    a[0] = cCharacter;
    /*
    hdc = GetDC(hWnd);  
    SelectObject(hdc, GetStockObject(OEM_FIXED_FONT));
    TextOut(hdc, 
            X_POS + PADDING, 
            Y_POS + PADDING, 
            (LPCWSTR) a, 1);
    */
           
    CHARACTER(X, Y).character   = cCharacter;
    CHARACTER(X, Y).fgColor     = CUR_FG_COLOR;
	CHARACTER(X, Y).bgColor     = CUR_BG_COLOR;
	CHARACTER(X, Y).style	    = CUR_STYLE;
    
    if (X >= CHARS_PER_LINE - 1) { 
        X = 0;
        if (Y < LINES_PER_SCRN - 1) { 
            Y++;
        } else {
            ScrollDown(hWnd);
        }
    } else {
        X++;
    }
    SetCaretPos(X_POS + PADDING, Y_POS + PADDING); 
    //ReleaseDC(hWnd, hdc);
}


VOID Bell(HWND hWnd) {
    // this function does nothing on grounds of good taste
}


VOID BackSpace(HWND hWnd) {
    PWNDDATA pwd = NULL;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
    X--;
}


VOID HorizontalTab(HWND hWnd) {
    PWNDDATA pwd = NULL;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);   
    if (X < CHARS_PER_LINE - TAB_LENGTH) {
        X += TAB_LENGTH - X % TAB_LENGTH;
    } else {
        X = CHARS_PER_LINE - 1;
    }
}


VOID LineFeed(HWND hWnd) {   
    PWNDDATA pwd = NULL;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0); 

    X = 0;
    if (Y < LINES_PER_SCRN - 1) {
        Y++;
    } else {
        ScrollDown(hWnd);
    }
    SetCaretPos(X, Y);
}


VOID VerticalTab(HWND hWnd) { 
    PWNDDATA    pwd         = NULL;
    PLINE       pNewLine    = NULL;
    UINT        i           = 0;
    UINT        j           = 0;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);    
    pNewLine = (PLINE) malloc(sizeof(LINE));    
    
    if (Y < LINES_PER_SCRN - 1) { 
        Y++;
    } else {
        ScrollDown(hWnd);
    }
    SetCaretPos(X, Y);
}


VOID FormFeed(HWND hWnd) { 
    PWNDDATA    pwd = NULL;
    UINT        i   = 0;
    UINT        j   = 0;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

    for (i = 0; i < LINES_PER_SCRN; i++) {
        for (j = 0; j < CHARS_PER_LINE; j++) {
            CHARACTER(j, i).character   = ' ';
            CHARACTER(j, i).bgColor     = CUR_BG_COLOR;
            CHARACTER(j, i).style       = 0;
         }
    }
    SetCursorPos(0, 0);
}


VOID CarraigeReturn(HWND hWnd) {
    PWNDDATA pwd = NULL;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);    
    X = 0;
    SetCaretPos(X, Y);
}

VOID MoveCursor(HWND hWnd, INT cxCoord, INT cyCoord) {
    PWNDDATA pwd = NULL;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
    
    if (cxCoord < 1) {
        X = 0;
    } else if (cxCoord > CHARS_PER_LINE) {
        X = CHARS_PER_LINE - 1;
    } else {
        X = --cxCoord;   
    }
    if (cyCoord < 1) {
        Y = 0;
    } else if (cyCoord > LINES_PER_SCRN) {
        Y = LINES_PER_SCRN - 1;
    } else {
        Y = --cyCoord;
    }
    SetCaretPos(X_POS, Y_POS);
}

VOID ClearLine(HWND hWnd, UINT cxCoord, UINT cyCoord, INT iDirection) {
    PWNDDATA    pwd = NULL;
    UINT        i   = 0;
    UINT        j   = 0;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
    
    i = cyCoord;
    j = cxCoord;
    while (j < CHARS_PER_LINE  &&  j >= 0) {
        CHARACTER(j, i).character   = ' ';
        CHARACTER(j, i).bgColor     = CUR_BG_COLOR;
        CHARACTER(j, i).style       = 0;
        j += iDirection;
    }
}

VOID ClearScreen(HWND hWnd, UINT cxCoord, UINT cyCoord, INT iDirection) {
    PWNDDATA    pwd = NULL;
    UINT        i   = 0;
    UINT        j   = 0;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

    ClearLine(hWnd, cxCoord, cyCoord, iDirection);

    i = cyCoord + iDirection;
    while (i < LINES_PER_SCRN  &&  i >= 0) {
        for (j = 0; j < CHARS_PER_LINE; j++) {
            CHARACTER(j, i).character   = ' ';
            CHARACTER(j, i).bgColor     = CUR_BG_COLOR;
            CHARACTER(j, i).style       = 0;
         }
         i += iDirection;
    }
}

VOID ScrollDown(HWND hWnd) {
    PWNDDATA    pwd         = NULL;
    PLINE       pNewLine    = NULL;
    UINT        i           = 0;
    UINT        j           = 0;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0); 
    pNewLine = (PLINE) malloc(sizeof(LINE));

    for (i = 0; i < LINES_PER_SCRN - 1; i++) {
        ROW(i) = ROW(i + 1);
    }
    ROW(i) = pNewLine;
    for (j = 0; j < CHARS_PER_LINE; j++) {
        CHARACTER(j, i).character   = ' ';
        CHARACTER(j, i).bgColor     = CUR_BG_COLOR;
        CHARACTER(j, i).style       = 0;
    }
}
