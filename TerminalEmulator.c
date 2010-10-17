/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:     WinMain.c - Contains the WinMain function for a program that
--                              reads and writes characters over a serial 
--                              connection.
--
-- PROGRAM: Hyper Omega Terminal
--
-- FUNCTIONS:
--              int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
--                                 PSTR szCmdLine, int iCmdShow)
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
--              Contains WinMain - the conventional name used for the 
--              application entry point.
----------------------------------------------------------------------------------------------------------------------*/

#include "TerminalEmulator.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    WndProc
--
-- DATE:        Oct 03, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Microsoft / Dean Morin
--
-- PROGRAMMER:  Microsoft / Dean Morin
--
-- INTERFACE:   int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
--
-- RETURNS:     If the function succeeds, terminating when it receives a WM_QUIT 
--              message, it should return the exit value contained in that 
--              message's wParam parameter. If the function terminates before 
--              entering the message loop, it should return zero.
--
-- NOTES:
--              WinMain is the conventional name used for the application entry
--              point. The standard message loop has been modified to poll the
--              an open serial port if there are no messages on the queue.o
----------------------------------------------------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
                
    static TCHAR    szAppName[] = TEXT("TerminalEmulator");
    HWND            hWnd        = NULL;
    MSG             msg;
    WNDCLASS        wndclass;
    PWNDDATA        pwd         = NULL;

    wndclass.style          = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc    = WndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = sizeof(PWNDDATA);
    wndclass.hInstance      = hInstance;
    wndclass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground  = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName   = TEXT("MYMENU");
    wndclass.lpszClassName  = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT("Upgrade your OS! Seriously!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hWnd = CreateWindow(szAppName,
                        TEXT("Advanced Terminal Emulator Pro (Trial Expired)"), 
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);


    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }
    return msg.wParam;
}

/*------------------------------------------------------------------------------
-- SOURCE FILE:     TerminalEmulator.c - The main functionality for an application
--                                   that reads and writes characters over a 
--                                   serial connection.
--
-- PROGRAM:     Hyper Omega Terminal
--
-- FUNCTIONS:
--              LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
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
-- The program creates a serial connection. Once connected, all WM_CHAR 
-- messages will be sent over the active serial port. The program also
-- polls for incoming characters during idle time (when there are no
-- messages on the message queue). 
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
-- FUNCTION:    WndProc
--
-- DATE:        Oct 03, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Dean Morin
--
-- PROGRAMMER:  Dean Morin
--
-- INTERFACE:   LRESULT CALLBACK WndProc(HWND, UNIT, WPARAM LPARAM)
--
-- RETURNS:     The return value is the result of the message processing and 
--              depends on the message sent.
--
-- NOTES:
--              The standard WndProc function. For the messages WW_CREATE,
--              WM_SIZE, and WM_PAINT, the logic is contained in this function.
--              For other messages, the majority of the logic is in
--              Application.c. Currently, WM_PAINT will update the screen with
--              black text only.
------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, 
                         WPARAM wParam, LPARAM lParam) {
                             
    PWNDDATA        pwd         = {0};
    HDC             hdc         = {0};
    COMMCONFIG      cc          = {0};
    PAINTSTRUCT     ps          = {0};
    TEXTMETRIC      tm          = {0};
    static UINT     cxClient    = 0;
    static UINT     cyClient    = 0;
    UINT            i           = 0;
    UINT            j           = 0;
    LINE            line        = {0};
    CHAR            a[2]        = {0};
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);


    switch (message) {

        case WM_CREATE:
                
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

            pwd->displayBuf.hFont = CreateFont (0, 0, 0, 0, 0, 0, 0, 0,
                                                DEFAULT_CHARSET, 0, 0, 0, 
                                                FIXED_PITCH, NULL);
            hdc = GetDC(hWnd);
            SelectObject(hdc, pwd->displayBuf.hFont);
            /////SelectObject(hdc, GetStockObject(OEM_FIXED_FONT));
            GetTextMetrics(hdc, &tm);
            pwd->displayBuf.cxChar = tm.tmAveCharWidth;
            pwd->displayBuf.cyChar = tm.tmHeight;
            ReleaseDC(hWnd, hdc);
            DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
            X = 0;
            Y = 0;
            CreateCaret(hWnd, NULL, PADDING, PADDING);
            ShowCaret(hWnd);

            for (i = 0; i < LINES_PER_SCRN; i++) {
                pwd->displayBuf.rows[i] = malloc(sizeof(LINE));
                for (j = 0; j < CHARS_PER_LINE; j++) {
                    CHARACTER(j, i).character   = ' ';
                    CHARACTER(j, i).color       = 0;
                    CHARACTER(j, i).style       = 0;
                }
            }
            return 0;


        case WM_SIZE:

            hdc = GetDC(hWnd);
            GetTextMetrics(hdc, &tm);

            cxClient        = LOWORD(lParam);
            cyClient        = HIWORD(lParam);

            ReleaseDC(hWnd, hdc);
            
            
        case WM_PAINT:
            
            hdc = BeginPaint(hWnd, &ps) ;
            SelectObject(hdc, GetStockObject(OEM_FIXED_FONT));
                              
            for (i = 0; i < LINES_PER_SCRN; i++) {
                for (j = 0; j < CHARS_PER_LINE; j++) {
                    a[0] = CHARACTER(j, i).character;
                    TextOut(hdc, 
                            CHAR_WIDTH * j + PADDING, CHAR_HEIGHT * i + PADDING,
                            (LPCWSTR) a, 1);
                }
            }
            EndPaint(hWnd, &ps);
            return 0;


        case WM_SETFOCUS:
            CreateCaret(hWnd, NULL, CHAR_WIDTH, CHAR_HEIGHT);
            SetCaretPos(X_POS, Y_POS);
            ShowCaret(hWnd);
            return 0;

        case WM_KILLFOCUS:
            HideCaret(hWnd);
            DestroyCaret();
            return 0;

        case WM_KEYDOWN:
            if (pwd->bConnected) {
                // check if it's a special virtual-key that we need to handle
                if ((wParam >= VK_END  &&  wParam <= VK_DOWN)  ||
                    (wParam >= VK_F1   &&  wParam <= VK_F4)) {
                    
                    if (!ProcessWrite(hWnd, wParam, TRUE)) {
                        DISPLAY_ERROR("Error writing to serial port");
                    }
                }
            }
            return 0;


        case WM_CHAR:
            if (pwd->bConnected) {
                if (!ProcessWrite(hWnd, wParam, FALSE)) {
                    DISPLAY_ERROR("Error writing to serial port");
                }
            }
            return 0;


        case WM_COMMAND:
            PerformMenuAction(hWnd, message, wParam);
            return 0;


        case WM_DESTROY:
            Disconnect(hWnd);
            PostQuitMessage(0);
            return 0;


		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
