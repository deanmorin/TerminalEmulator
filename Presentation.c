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


BOOL ProcessRead(HWND hWnd, CHAR szReadBuf[], DWORD dwBytesRead) {

    CHAR        szEscBuffer[BUFSIZE]    = {0};
    BOOL        bEscape                 = FALSE;
    static BOOL bIncomplete             = FALSE;
    size_t      i                       = 0;

    if (bIncomplete) {
        bIncomplete = ProcessEsc(hWnd, szReadBuf);
        return bIncomplete;
    }
    for (i = 0; i < dwBytesRead && !bEscape; i++) {
        switch (szReadBuf[i]) {
            case '\033':
                bEscape = TRUE;
                break;
            default:
                // text out
                Read(hWnd, szReadBuf[i], 1);
        }
    }
    if (bEscape) {
        LTrimCopy(szEscBuffer, szReadBuf, i, dwBytesRead);
        bIncomplete = ProcessEsc(hWnd, szEscBuffer);
    }
    return bIncomplete;
}


VOID LTrimCopy(CHAR szDest[], const CHAR szSrc[], 
               size_t pos, DWORD dwBytesRead) {
	size_t j;
	for (j = 0; szSrc[pos] != '\0'; j++) {
		szDest[j] = szSrc[j + pos];
	}
	szDest[j] = '\0';
}


BOOL ProcessEsc(HWND hWnd, CHAR szBuffer[]) {
    int i = 0;
    char rest[BUFSIZE];
    static char incomplete[BUFSIZE];
    
    if (strcmp(incomplete,"\0") != 0) {
        strcat(incomplete, szBuffer);
        strcpy(szBuffer, incomplete);
        incomplete[i] = '\0';
    }
    
    for (i = 0; szBuffer[i] != '\0'; i++) {
        switch (szBuffer[i]) {
            /*invalid sequence*/
            case '\033':
                LTrimCopy(rest, szBuffer, i + 1);
                return ProcessRead(hWnd, rest, strlen(rest));
            /*valid sequence*/
            case 'm':
                strncpy(rest, szBuffer, i + 1);
                rest[i+1] = '\0';
                DISPLAY_ERROR("ESC WORKED");
                LTrimCopy(rest, szBuffer, i + 1);
                return ProcessRead(hWnd, rest, strlen(rest));
        }
    }
    strcpy(incomplete, szBuffer);
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
