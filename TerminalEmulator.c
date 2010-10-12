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
                             
    PWNDDATA        pwd             = NULL;
    HDC             hdc             = {0};
    COMMCONFIG      cc              = {0};
    PAINTSTRUCT     ps              = {0};
    TEXTMETRIC      tm              = {0};
    int             cyPos           = CELL_PADDING;
    static int      cxClient        = 0;
    static int      cyClient        = 0;
    UINT            i               = 0;
    PTCHARNODE      ptCharNode      = NULL;
    PCOLORNODE      pColorNode      = NULL;
    static TCHAR    *pszRepaintBuf;
    SIZE            size;


    switch (message) {

        case WM_CREATE:
                
            if ((pwd = (PWNDDATA) malloc(sizeof(WNDDATA))) == 0) {
                DISPLAY_ERROR("Error allocating memory for WNDDATA structure");
            }
            pwd->lpszCommName = TEXT("COM3");
            pwd->hPort = NULL;
            SetWindowLongPtr(hWnd, 0, (LONG_PTR) pwd);

            cc.dwSize = sizeof(COMMCONFIG);
            GetCommConfig(pwd->hPort, &cc, &cc.dwSize);
            FillMemory(&cc.dcb, sizeof(DCB), 0);
            cc.dcb.DCBlength = sizeof(DCB);
            BuildCommDCB((LPCWSTR)"96,N,8,1", &cc.dcb);


            //get rid of
            pwd->textInfo.ptCharHead    = NULL;
            pwd->textInfo.ptCharTail    = NULL;
            pwd->textInfo.pColorHead    = NULL;
            pwd->textInfo.pColorTail    = NULL;
            pwd->textInfo.cxPos         = CELL_PADDING;
            pwd->textInfo.cyPos         = CELL_PADDING;

            return 0;


        case WM_SIZE:

            pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
            hdc = GetDC(hWnd);
            GetTextMetrics(hdc, &tm);

            cxClient        = LOWORD(lParam);
            cyClient        = HIWORD(lParam);

            // Unneccesarry for monospace fonts
            pszRepaintBuf   = (TCHAR*) malloc(sizeof(TCHAR)
                                            * cxClient / tm.tmAveCharWidth * 4);  
            // Unneccesarry for monospace fonts
            pwd->textInfo.uMinStrLength = (cxClient - CELL_PADDING)
                                          / tm.tmMaxCharWidth;
            ReleaseDC(hWnd, hdc);
            

        case WM_PAINT:
            /*
            pwd         = (PWNDDATA) GetWindowLongPtr(hWnd, 0);      
            ptCharNode  = pwd->textInfo.ptCharHead;
            pColorNode  = pwd->textInfo.pColorHead;
            hdc         = BeginPaint(hWnd, &ps); 
            
            while (ptCharNode != NULL) {
                
                GetTextMetrics(hdc, &tm);
                i = 0;
                
                /* 
                 * Adds the number of characters to pszRepaintBuf that are 
                 * guaranteed to fit on a line.
                 */ 
                /*while (i < pwd->textInfo.uMinStrLength  &&  ptCharNode != NULL) {
                    pszRepaintBuf[i++] = ptCharNode->data;
                    ptCharNode = ptCharNode->next;
                }

                /*
                 * Measures the line so far, and adds the remaining characters 
                 * that can fit on the line, one-by-one.
                 */
               /* if (ptCharNode != NULL) {
                    
                    GetTextExtentPoint32(hdc, pszRepaintBuf, i, &size);  

                    while (size.cx + CELL_PADDING < cxClient - tm.tmMaxCharWidth
                           &&  ptCharNode != NULL) {

                        pszRepaintBuf[i++] = ptCharNode->data;
                        ptCharNode = ptCharNode->next;     
                        GetTextExtentPoint32(hdc, pszRepaintBuf, i, &size);
                    }                
                }
                TextOut(hdc, CELL_PADDING, cyPos, pszRepaintBuf, i);

                GetTextExtentPoint32(hdc, pszRepaintBuf, i, &size);
                pwd->textInfo.cxPos = size.cx + CELL_PADDING;
                pwd->textInfo.cyPos = cyPos;
                cyPos += tm.tmHeight + tm.tmExternalLeading;
            }
        
            EndPaint(hWnd, &ps);*/
            return 0;


        case WM_CHAR:
            return ProcessWrite(hWnd, wParam, FALSE); //check for ctrl down
            /*
            pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

            if (wParam == VK_ESCAPE) {
                Disconnect(hWnd);
            } else if (wParam >= VK_SPACE  &&  wParam <= TILDE) {
                WriteToPort(hWnd, wParam);
            }
            return 0;
            */

        case WM_COMMAND:
            return PerformMenuAction(hWnd, message, wParam);


        case WM_DESTROY:
            Disconnect(hWnd);
            PostQuitMessage(0);
            return 0;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
