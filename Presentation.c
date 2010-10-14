#include "Presentation.h"

VOID ProcessRead(HWND hWnd, CHAR szReadBuf[], DWORD dwBytesRead) {
    
    HDC hdc;
    static int x = 0;

    hdc = GetDC(hWnd);
    TextOut(hdc, x, 0, (LPCWSTR) szReadBuf, dwBytesRead);
    x+=10;
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
