#include "Presentation.h"

VOID Read(HWND hWnd, CHAR psReadBuf, DWORD dwBytesRead) {
    
    CHAR a[2] = {0};
    HDC hdc;
    static int x = 0;

    a[0] = psReadBuf;

    hdc = GetDC(hWnd);
    TextOut(hdc, x, 0, (LPCWSTR) a, 1);
    x+=10;
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
        *psCombined         = *pwd->psIncompleteEsc;
        *(psCombined + pwd->dwIncompleteLength) = *psReadBuf;
        pwd->psIncompleteEsc = NULL;
        ProcessEsc(hWnd, psCombined, dwCombinedLength);
        return;
    }
    for (i = 0; i < dwBytesRead; i++) {
        switch (psReadBuf[i]) {
            case VK_ESCAPE:
                ProcessEsc(hWnd, psReadBuf + i, dwBytesRead - i);
                return;
            default:
                // text out
                Read(hWnd, psReadBuf[i], 1);
        }
    }
}


VOID ProcessEsc(HWND hWnd, CHAR* psBuffer, DWORD length) {
    
    PWNDDATA    pwd = NULL;
    DWORD       i   = 0;  
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
    
    for (i = 1; i < length; i++) {
        switch (psBuffer[i]) {
            // invalid sequence
            case VK_ESCAPE:
                ProcessEsc(hWnd, psBuffer + i, length - i);
                return;
            // valid sequence
            case 'm':
                DISPLAY_ERROR("ESC WORKED?");
                if (i == length - 1) {
                    return;
                }
                ProcessRead(hWnd, psBuffer + i + 1, length - i - 1);
                return;
        }
    }
    pwd->psIncompleteEsc    = psBuffer;
    pwd->dwIncompleteLength = length;
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
    CHAR        psWriteBuf[4]   = {0};
    OVERLAPPED  overlap         = {0};
    DWORD       dwBytesRead     = 0;
    UINT        bufLength       = 1;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

    if (bNonCharKey) {

        psWriteBuf[0] = VK_ESCAPE;
        if (wParam <= VK_DOWN) { 
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
