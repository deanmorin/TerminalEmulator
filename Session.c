#include "Session.h"

/*------------------------------------------------------------------------------
-- FUNCTION:    Connect
--
-- DATE:        Oct 16, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Dean Morin
--
-- PROGRAMMER:  Dean Morin
--
-- INTERFACE:   Connect(HWND)
--
-- RETURNS:     True if the serial connection was succefully made.
--
-- NOTES:
--              Opens a serial port connection, displaying appropriate dialogue
--              messages for failed connections. If successful, it sets comm 
--              settings and creates a read thread. It then enables/disables
--              the appropriate menu choices.
------------------------------------------------------------------------------*/
BOOL Connect(HWND hWnd) {
    
    PWNDDATA        pwd         = {0};
    COMMTIMEOUTS    timeOut     = {0};
    DWORD           dwThreadid  = 0;
    DWORD           i           = 0;

    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

    // open serial port
    pwd->hPort = CreateFile(pwd->lpszCommName,
                          GENERIC_READ | GENERIC_WRITE, 0,
                          NULL, OPEN_EXISTING,
                          FILE_FLAG_OVERLAPPED, NULL);

    if (pwd->hPort == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            DISPLAY_ERROR("Serial port does not exist");
        } else {
            DISPLAY_ERROR("Error opening port");
        }
        return FALSE;
    }
    pwd->bConnected = TRUE;


    if (!EscapeCommFunction(pwd->hPort, SETRTS)) {
        DISPLAY_ERROR("Error sending RTS signal");
    }
    if (!EscapeCommFunction(pwd->hPort, SETDTR)) {
        DISPLAY_ERROR("Error sending DTR signal");
    }

    // set timeouts for the port
    if (!GetCommTimeouts(pwd->hPort, &pwd->defaultTimeOuts)) {
        DISPLAY_ERROR("Error retrieving comm timeouts");
        return FALSE;   
    }
    timeOut.ReadIntervalTimeout         = 10;
    timeOut.WriteTotalTimeoutConstant   = 5000;

    if (!SetCommTimeouts(pwd->hPort, &timeOut)) {
        DISPLAY_ERROR("Could not set comm timeouts");
        return FALSE;
    }
    
    // create thread for reading
    pwd->hThread = CreateThread(NULL, 0,
                                (LPTHREAD_START_ROUTINE) ReadThreadProc,
                                hWnd, 0, &dwThreadid);

    if (pwd->hThread == INVALID_HANDLE_VALUE) {
        DISPLAY_ERROR("Error creating read thread");
        return FALSE;
    }
                                
    // enable/disable appropriate menu choices
    EnableMenuItem(GetMenu(hWnd), IDM_DISCONNECT, MF_ENABLED);
    EnableMenuItem(GetMenu(hWnd), IDM_CONNECT,    MF_GRAYED);
    EnableMenuItem(GetMenu(hWnd), IDM_COMMSET,    MF_GRAYED);
    for (i = 0; i < NO_OF_PORTS; i++) {
        EnableMenuItem(GetMenu(hWnd), IDM_COM1 + i, MF_GRAYED);
    }

    return TRUE;
}

/*------------------------------------------------------------------------------
-- FUNCTION:    Disconnect
--
-- DATE:        Oct 16, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Dean Morin
--
-- PROGRAMMER:  Dean Morin
--
-- INTERFACE:   Disconnect(HWND)
--
-- RETURNS:     VOID.
--
-- NOTES:
--              Closes the active serial port and read thread, then
--              enables/disables the appropriate menu choices.
------------------------------------------------------------------------------*/
VOID Disconnect(HWND hWnd) {

    PWNDDATA        pwd         = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
    COMMTIMEOUTS    timeOut     = {0};
    DWORD           dwThreadid  = 0;
    DWORD           i           = 0;
    
    if (pwd->hPort == NULL) {
        return;
    }

    // this will end the outer while loop in the read thread
    pwd->bConnected = FALSE;
   
    if (!SetCommTimeouts(pwd->hPort, &pwd->defaultTimeOuts)) {
        DISPLAY_ERROR("Could not reset comm timeouts to defaults");
    }

    // let the read thread finish up
    do {
        GetExitCodeThread(pwd->hThread, &dwThreadid);
    } while (dwThreadid == STILL_ACTIVE);

    CloseHandle(pwd->hThread);
    CloseHandle(pwd->hPort);
    pwd->hPort = NULL;

    // enable/disable appropriate menu choices    
    EnableMenuItem(GetMenu(hWnd), IDM_DISCONNECT, MF_GRAYED);
    EnableMenuItem(GetMenu(hWnd), IDM_CONNECT,    MF_ENABLED);
    EnableMenuItem(GetMenu(hWnd), IDM_COMMSET,    MF_ENABLED);
    for (i = 0; i < NO_OF_PORTS; i++) {
        EnableMenuItem(GetMenu(hWnd), IDM_COM1 + i, MF_ENABLED);
    }
}
