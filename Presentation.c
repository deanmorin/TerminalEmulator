#include "Presentation.h"

VOID Read(HWND hWnd, CHAR szReadBuf, DWORD dwBytesRead) {
    
    CHAR a[2] = {0};
    HDC hdc;
    static int x = 0;

    a[0] = szReadBuf;

    hdc = GetDC(hWnd);
    TextOut(hdc, x, 0, (LPCWSTR) a, 1);
    x+=10;
}


VOID ProcessRead(HWND hWnd, CHAR szReadBuf[], DWORD dwBytesRead) {

    CHAR*       szEscBuf        = NULL;
    static BOOL bIncomplete     = FALSE;
    
    if (bIncomplete) {
        // the last escape sequence was incomplete
        bIncomplete = ProcessEsc(hWnd, szReadBuf, dwBytesRead);
        return;
    }
    bIncomplete = ProcessBuffer(hWnd, szReadBuf, dwBytesRead);
}

//returns true if incomplete
BOOL ProcessBuffer(HWND hWnd, CHAR szBuffer[], DWORD dwLength) {
    
    DWORD   i   = 0;

    // check szReadBuf, character by character
    for (i = 0; i < dwLength; i++) {
        switch (szBuffer[i]) {
            case VK_ESCAPE:
                return ProcessEsc(hWnd, szBuffer + i, dwLength - i);
            default:
                // text out
                Read(hWnd, szBuffer[i], 1);
        }
    }
    return FALSE;
}

//returns true if incomplete
BOOL ProcessEsc(HWND hWnd, CHAR* szBuffer, DWORD length) {
    
    static CHAR     szIncomplete[4096]  = {0};
    static BOOL     bIncomplete         = FALSE;
    DWORD           i                   = 0;

    if (bIncomplete) {
        strcat(szIncomplete, szBuffer);
        szBuffer = szIncomplete; //overflow?
        length = strlen(szBuffer);
        bIncomplete = FALSE;
    }
    
    
    for (i = 1; i < length; i++) {
        switch (szBuffer[i]) {
        // invalid sequence
            case VK_ESCAPE:
                return ProcessBuffer(hWnd, szBuffer + i, length - i);
            // valid sequence
            case 'm':
                DISPLAY_ERROR("ESC WORKED?");
                if (i == length) {
                    return FALSE;
                }
                return ProcessBuffer(hWnd, szBuffer + i + 1, length - i - 1);
        }
    }
    strcpy(szIncomplete, szBuffer);
    return TRUE;
}

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
    CHAR        szWriteBuf[4]   = {0};
    OVERLAPPED  overlap         = {0};
    DWORD       dwBytesRead     = 0;
    UINT        bufLength       = 1;

    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

    if (bNonCharKey) {

        szWriteBuf[0] = VK_ESCAPE;
        if (wParam <= VK_DOWN) { 
            szWriteBuf[1] = '[';
        } else {
            szWriteBuf[1] = 'O';
        }
        bufLength = 3;

        switch (wParam) {
            case VK_UP:     szWriteBuf[2] = 'A';    break;
            case VK_DOWN:   szWriteBuf[2] = 'B';    break;
            case VK_RIGHT:  szWriteBuf[2] = 'C';    break;
            case VK_LEFT:   szWriteBuf[2] = 'D';    break;
            case VK_HOME:   szWriteBuf[2] = 'H';    break;
            case VK_END:    szWriteBuf[2] = 'K';    break;
            case VK_F1:     szWriteBuf[2] = 'P';    break;
            case VK_F2:     szWriteBuf[2] = 'Q';    break;
            case VK_F3:     szWriteBuf[2] = 'R';    break;
            case VK_F4:     szWriteBuf[2] = 'S';    break;
        }

    } else {
        szWriteBuf[0] = wParam;
    }

    if (!WriteFile(pwd->hPort, szWriteBuf, bufLength, &dwBytesRead, &overlap)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return FALSE;
        }
    }
    return TRUE;
}
