/*------------------------------------------------------------------------------
-- SOURCE FILE:     EscapeSequence.c - This contains all the functions necessary
--                                     to parse an escaped control sequence.
--
-- PROGRAM:     Advanced Terminal Emulator Pro
--
-- FUNCTIONS:
--              BOOL	CheckDigits(HWND hWnd, CHAR* psBuffer, 
--                                  DWORD length, DWORD *i);
--              BOOL	CheckDigitsQ(HWND hWnd, CHAR* psBuffer, 
--                                   DWORD length, DWORD *i);
--              BOOL	CheckDigitsSemi(HWND hWnd, CHAR* psBuffer, 
--                                      DWORD length, DWORD *i);
--              DWORD	GetDigit(CHAR* psBuffer, DWORD length, DWORD *i);
--              VOID    ProcessEsc(HWND hWnd, CHAR* psBuffer, 
--                                 DWORD dwBytesRead);
--              VOID    ProcessFont(HWND hWnd);
--              BOOL	ProcessParen(CHAR* psBuffer, DWORD length, DWORD *i);
--              BOOL	ProcessSquare(HWND hWnd, CHAR* psBuffer, 
--                                    DWORD length, DWORD *i);
--
--
-- DATE:        Oct 19, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Marcel Vangrootheest
--
-- PROGRAMMER:  Marcel Vangrootheest
--
-- NOTES:
-- Receives a buffer from a port read and processes it. Displayable characters
-- and special characters such as line feed are handled character by character.
-- If an escape character is received, the buffer is stepped through until one
-- of three things happens:
--
--      1. A valid sequence is completed:
--          The remainder of the buffer is then once again processed character
--          by character.
--
--      2. An sequence is found that cannot be valid:
--          In this case, the buffer starting at the character that made the 
--          sequence invalid is processed character by character. For example,
--          if the buffer contains " ESC[0;$H ", then $H would be added to the
--          display buffer and displayed on screen.
--
--      3. The buffer contains a partial sequence:
--          The incomplete sequence is stored in the window extra structure, and
--          the next buffer from a port read is appended to the end of it.
------------------------------------------------------------------------------*/

#include "EscapeSequence.h"

/*------------------------------------------------------------------------------
-- FUNCTION:    ProcessEsc
--
-- DATE:        Oct 17, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Marcel Vangrootheest
--
-- PROGRAMMER:  Marcel Vangrootheest
--
-- INTERFACE:   VOID ProcessEsc(HWND hWnd, CHAR* psBuffer, DWORD length)
--								hWnd		- Handle to the window
--								psBuffer	- Array of chars to be processed
--								length		- Number of chars to process
--
-- RETURNS:     VOID
--
-- NOTES:
--              Processes an Escape sequence. If a valid sequence is encountered
--				it will either end or send the rest of the array to ProcessRead.
--				If it is invalid it will send the invalid character plus the
--				rest of the array to ProcessRead. If it is incomplete it will
--				save the current sequence and another Read will be done to later
--				process the rest of the sequence.
------------------------------------------------------------------------------*/
VOID ProcessEsc(HWND hWnd, CHAR* psBuffer, DWORD length) {
    
    PWNDDATA    pwd = NULL;
    DWORD       i   = 0;  
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
    ESC_VAL(0) = 0;
    
    if (i++ == length - 1) {
		pwd->psIncompleteEsc	= (CHAR*)malloc(sizeof(CHAR)*length);
		strncpy(pwd->psIncompleteEsc, psBuffer, length);
		pwd->dwIncompleteLength = length;
		return;
	}
    switch (psBuffer[i++]) {
		case '[':
			if (i == length) {
				pwd->psIncompleteEsc	= (CHAR*)malloc(sizeof(CHAR)*length);
				strncpy(pwd->psIncompleteEsc, psBuffer, length);
				pwd->dwIncompleteLength = length;
				return;
			}

			if (!ProcessSquare(hWnd, psBuffer, length, &i))
				break;
			else {
				if (i == length)
					return;
				ProcessRead(hWnd, psBuffer + i, length - i);
				return;
			}
		case '(':
			if (i == length) {
				pwd->psIncompleteEsc	= (CHAR*)malloc(sizeof(CHAR)*length);
				strncpy(pwd->psIncompleteEsc, psBuffer, length);
				pwd->dwIncompleteLength = length;
				return;
			}

			if (!ProcessParen(psBuffer, length, &i))
				break;
			else {
				if (i == length)
					return;
				ProcessRead(hWnd, psBuffer + i, length - i);
				return;
			}
			break;
		case ')':
			if (i == length) {
				pwd->psIncompleteEsc	= (CHAR*)malloc(sizeof(CHAR)*length);
				strncpy(pwd->psIncompleteEsc, psBuffer, length);
				pwd->dwIncompleteLength = length;
				return;
			}

			if (!ProcessParen(psBuffer, length, &i))
				break;
			else {
				if (i == length)
					return;
				ProcessRead(hWnd, psBuffer + i, length - i);
				return;
			}
			break;
		case '>':
			//DISPLAY_ERROR(">");
			if (i == length)
				return;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;
		case '=':
			//DISPLAY_ERROR("=");
			if (i == length)
				return;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;
		case 'I':
		case 'M':   // move up one line, scrolling if necessary
			MoveCursor(hWnd, X + 1, --Y + 1, TRUE);
			if (i == length)
				return;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;
		case 'H':
			//DISPLAY_ERROR("H");
			if (i == length)
				return;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;
		case 'E':   // move down one line to 1st position, 
                    // scrolling if necessary
			MoveCursor(hWnd, 1, ++Y + 1, TRUE);          
			if (i == length)
				return;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;	
        case 'D':   // move down one line, scrolling if necessary
			MoveCursor(hWnd, X + 1, ++Y + 1, TRUE);          
			if (i == length)
				return;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;
		case '#':
			if (i >= length)
				break;
			if (isdigit(psBuffer[i])) {
				if (psBuffer[i] == '8') {
					ScreenAlignment(hWnd);
				}
				if (i >= length - 1)
					return;
			}
			i++;
			ProcessRead(hWnd, psBuffer + i, length - i);
		case 'c':
			FormFeed(hWnd);
			if (i == length)
				return;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;
		case 'F':
		case 'G':
		case '7':
		case '8':
			break;
		default :
			i--;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;
			
    }
    pwd->psIncompleteEsc	= (CHAR*)malloc(sizeof(CHAR)*length);
	strncpy(pwd->psIncompleteEsc, psBuffer, length);
    pwd->dwIncompleteLength = length;
}

/*------------------------------------------------------------------------------
-- FUNCTION:    GetDigit
--
-- DATE:        Oct 17, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Marcel Vangrootheest
--
-- PROGRAMMER:  Marcel Vangrootheest
--
-- INTERFACE:   DWORD GetDigit(CHAR* psBuffer, DWORD length, DWORD *i)
--								hWnd		- Handle to the window
--								psBuffer	- Array of chars to be processed
--								length		- Number of chars to process
--
-- RETURNS:     DWORD digit - The number currently being checked in the array of
--								chars.
--
-- NOTES:
--              Processes digits after a square bracket. It decrements i
--				if the sequence is invalid so it can send the invalid character.
------------------------------------------------------------------------------*/
DWORD GetDigit(CHAR* psBuffer, DWORD length, DWORD *i) {
	DWORD j = 0;
	CHAR *psDigits = NULL;
	DWORD digit;
	psDigits = (CHAR*)malloc(sizeof(CHAR)*(length + 1));
	strncpy(psDigits, psBuffer + *i, length - *i + 1);
	psDigits[length - *i] = '\0';

	j = strspn(psDigits, "0123456789");
	strncpy(psDigits, psBuffer + *i, j);
	digit = atoi(psDigits);
	*i += j - 1;

	return digit;
}

/*------------------------------------------------------------------------------
-- FUNCTION:    CheckDigits
--
-- DATE:        Oct 17, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Marcel Vangrootheest
--
-- PROGRAMMER:  Marcel Vangrootheest
--
-- INTERFACE:   BOOL CheckDigits(HWND hWnd, CHAR* psBuffer, DWORD length)
--								hWnd		- Handle to the window
--								psBuffer	- Array of chars to be processed
--								length		- Number of chars to process
--
-- RETURNS:     BOOL - TRUE if the sequence is valid or invalid
--					 - FALSE if the sequence is incomplete
--
-- NOTES:
--              Processes digits after a square bracket. It decrements i
--				if the sequence is invalid so it can send the invalid character.
------------------------------------------------------------------------------*/
BOOL CheckDigits(HWND hWnd, CHAR* psBuffer, DWORD length, DWORD *i) {
	PWNDDATA    pwd     = NULL;
    DWORD       digit   = 0;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

	if (*i >= length)
		return FALSE;

    if (isdigit(psBuffer[--(*i)])) {

	    digit = GetDigit(psBuffer, length, i);
        ESC_VAL(0)++;
        ESC_VAL(1) = digit;

	    if ((*i)++ == length - 1)
			return FALSE;

	    if (digit >= 0) {
	        switch (psBuffer[(*i)++]) {
			    case 'A':                                       // Esc[0A
					MoveCursor(hWnd, X + 1, Y - ESC_VAL(1) + 1, FALSE);
			        break;
			    case 'B':                                       // Esc[0B
					MoveCursor(hWnd, X + 1, Y + ESC_VAL(1) + 1, FALSE);
			        break;
			    case 'C':                                       // Esc[0C
					MoveCursor(hWnd, X + ESC_VAL(1) + 1, Y + 1, FALSE);
			        break;
			    case 'D':                                       // Esc[0D
					MoveCursor(hWnd, X - ESC_VAL(1) + 1, Y + 1, FALSE);
			        break;
			    case 'g':                                      
					//DISPLAY_ERROR("num g");
			        break;
			    case 'K':                                       // Esc[0K
				    if (ESC_VAL(1) == 0) {
                        ClearLine(hWnd, X, Y, CLR_RIGHT);
                    } else if (ESC_VAL(1) == 1) {
                        ClearLine(hWnd, X, Y, CLR_LEFT);
                    } else {
                        ClearLine(hWnd, X, Y, CLR_LEFT);
                        ClearLine(hWnd, X, Y, CLR_RIGHT);
                    }
			        break;
			    case 'J':                                       // Esc[0J
					if (ESC_VAL(1) == 0) {
                        ClearScreen(hWnd, X, Y, CLR_DOWN);
                    } else if (ESC_VAL(1) == 1) {
                        ClearScreen(hWnd, X, Y, CLR_UP);
                    } else {
                        ClearScreen(hWnd, X, Y, CLR_UP);
                        ClearScreen(hWnd, X, Y, CLR_DOWN);
                    }
			        break;
			    case 'm':
					ProcessFont(hWnd);
			        break;
			    case ';':
			        if (!CheckDigitsSemi(hWnd, psBuffer, length, i))
			            return FALSE;
			        break;
				case 'c':
					ProcessWrite(hWnd, '0', FALSE);
				case 'q':
					break;
				default:
					(*i)--;
		    }
		} else {
			(*i)--;
		}
    }
    return TRUE;
}

/*------------------------------------------------------------------------------
-- FUNCTION:    CheckDigitsSemi
--
-- DATE:        Oct 17, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Marcel Vangrootheest
--
-- PROGRAMMER:  Marcel Vangrootheest
--
-- INTERFACE:   BOOL CheckDigitsSemi(HWND hWnd, CHAR* psBuffer, DWORD length)
--								hWnd		- Handle to the window
--								psBuffer	- Array of chars to be processed
--								length		- Number of chars to process
--
-- RETURNS:     BOOL - TRUE if the sequence is valid or invalid
--					 - FALSE if the sequence is incomplete
--
-- NOTES:
--              Processes digits after a semi colon. Loops with digit and
--				semicolon for the font changes. If the sequence is invalid
--				i is decremented.
------------------------------------------------------------------------------*/
BOOL CheckDigitsSemi(HWND hWnd, CHAR* psBuffer, DWORD length, DWORD *i) {
    PWNDDATA    pwd     = NULL;
    DWORD       digit   = 0;
    DWORD       count   = 0;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

    if (*i >= length)
		return FALSE;

    if (isdigit(psBuffer[*i])) {

	    digit = GetDigit(psBuffer, length, i);
        ESC_VAL(0)++;
        ESC_VAL(2) = digit;
	    
		if ((*i)++ == length - 1)
			return FALSE;

	    if (digit >= 0) {
	        switch (psBuffer[(*i)++]) {
				case 'r':                                       // Esc0;0r         
					SetScrollRegion(hWnd, ESC_VAL(1), ESC_VAL(2));  
					break;
				case 'H':                                       // Esc0;0H
                case 'f':                                       // Esc0;0f
					MoveCursor(hWnd, ESC_VAL(2), ESC_VAL(1), FALSE);
					break;
				case 'm':
					ProcessFont(hWnd);
					break;
				case ';':
					do {
						if (*i >= length)
							return FALSE;
						if (isdigit(psBuffer[*i])) {
							digit = GetDigit(psBuffer, length, i);
                            ESC_VAL(0)++;
                            ESC_VAL(3 + count++) = digit;
							//validate digit

							if ((*i)++ == length-1)
								return FALSE;
						} else {
							(*i)--;
							return TRUE;
						}
					} while (psBuffer[(*i)++] == ';');
					if (psBuffer[(*i)-1] == 'm') {
						ProcessFont(hWnd);
					} else {
						(*i)--;
					}
					break;
				case 'q':
				case 'y':
				case 'R':
					break;
				default:
					(*i)--;
			}
		} else {
			(*i)--;
		}
    }
    return TRUE;
}

/*------------------------------------------------------------------------------
-- FUNCTION:    CheckDigitsQ
--
-- DATE:        Oct 17, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Marcel Vangrootheest
--
-- PROGRAMMER:  Marcel Vangrootheest
--
-- INTERFACE:   BOOL CheckDigitsQ(HWND hWnd, CHAR* psBuffer, DWORD length)
--								hWnd		- Handle to the window
--								psBuffer	- Array of chars to be processed
--								length		- Number of chars to process
--
-- RETURNS:     BOOL - TRUE if the sequence is valid or invalid
--					 - FALSE if the sequence is incomplete
--
-- NOTES:
--              Processes digits after a question mark. It decrements i
--				if the sequence is valid so it can send the invalid character.
------------------------------------------------------------------------------*/
BOOL CheckDigitsQ(HWND hWnd, CHAR* psBuffer, DWORD length, DWORD *i) {
    PWNDDATA    pwd     = NULL;
    DWORD       digit   = 0;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

    if (*i == length)
		return FALSE;

    if (isdigit(psBuffer[*i])) {

	    digit = GetDigit(psBuffer, length, i);
        ESC_VAL(0)++;
        ESC_VAL(1) = digit;

	    if ((*i)++ == length - 1)
			return FALSE;

	    if (digit >= 0) {
	        switch (psBuffer[(*i)++]) {
			    case 'h':
					if (digit == 1) {
						pwd->cursorMode = FALSE;
					} else if (digit == 25) {
                        // DISPLAY_ERROR("Show cursor")
                    } else if (digit == 7) {
						pwd->wordWrap = TRUE;
					} else if (digit == 6) {
						pwd->relOrigin = TRUE;
					}
					break;
			    case 'l':
					if (digit == 1) {
						pwd->cursorMode = TRUE;
					} else if (digit == 25) {
                        // DISPLAY_ERROR("Hide cursor")
                    } else if (digit == 7) {
						pwd->wordWrap = FALSE;
					} else if (digit == 6) {
						pwd->relOrigin = FALSE;
					}
				    break;
				case 'c':
					break;
				default:
					(*i)--;
					break;
			} 
		} else {
			(*i)--;
		}
    }
    return TRUE;
}

/*------------------------------------------------------------------------------
-- FUNCTION:    ProcessSquare
--
-- DATE:        Oct 17, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Marcel Vangrootheest
--
-- PROGRAMMER:  Marcel Vangrootheest
--
-- INTERFACE:   BOOL ProcessSquare(HWND hWnd, CHAR* psBuffer, DWORD length)
--								hWnd		- Handle to the window
--								psBuffer	- Array of chars to be processed
--								length		- Number of chars to process
--
-- RETURNS:     BOOL - TRUE if the sequence is valid or invalid
--					 - FALSE if the sequence is incomplete
--
-- NOTES:
--              Processes a square bracket from an escape sequence. It decrements i
--				if the sequence is valid so it can send the invalid character.
------------------------------------------------------------------------------*/
BOOL ProcessSquare(HWND hWnd, CHAR* psBuffer, DWORD length, DWORD *i) {
    PWNDDATA    pwd = NULL;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);
	
    switch (psBuffer[(*i)++]) {
		case 'A':                                       // Esc[A
			MoveCursor(hWnd, X + 1, --Y + 1, FALSE);
			break;
		case 'B':                                       // Esc[B
			MoveCursor(hWnd, X + 1, ++Y + 1, FALSE);
			break;
		case 'C':                                       // Esc[C
			MoveCursor(hWnd, ++X + 1, Y + 1, FALSE);
			break;
		case 'D':                                       // Esc[D
			MoveCursor(hWnd, --X + 1, Y + 1, FALSE);
			break;
        case 'H':                                       // Esc[H
        case 'f':                                       // Esc[f
            MoveCursor(hWnd, 1, 1, FALSE);
		    break;
		case 'g':
			//DISPLAY_ERROR("g");
		    break;
		case 'K':                                       // Esc[K
			ClearLine(hWnd, X, Y, CLR_RIGHT);
		    break;
		case 'J':                                       // Esc[J
			ClearScreen(hWnd, X, Y, CLR_DOWN);                      
		    break;
		case 'm':
			ESC_VAL(0)++;
			ESC_VAL(1) = 0;
			ProcessFont(hWnd);
		    break;
		case '?':
			if (!CheckDigitsQ(hWnd, psBuffer, length, i)) {
				return FALSE;
			}
		    break;
        case ';':                                               // Esc[;H
            if (*i >= length) {
                return FALSE;
            }
            if (psBuffer[*i] == 'H'  ||  psBuffer[*i] == 'f') {                      
                (*i)++;
                MoveCursor(hWnd, 1, 1, FALSE);                             
            } else {
                *i;
            }
            break;
		default:
			if (!CheckDigits(hWnd, psBuffer, length, i)) {
				return FALSE;
			}
			break;
	}
	return TRUE;
}

/*------------------------------------------------------------------------------
-- FUNCTION:    ProcessParen
--
-- DATE:        Oct 17, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Marcel Vangrootheest
--
-- PROGRAMMER:  Marcel Vangrootheest
--
-- INTERFACE:   BOOL ProcessParen(HWND hWnd, CHAR* psBuffer, DWORD length)
--								hWnd		- Handle to the window
--								psBuffer	- Array of chars to be processed
--								length		- Number of chars to process
--
-- RETURNS:     BOOL - TRUE if the sequence is valid or invalid
--
-- NOTES:
--              Processes a parantheses from an escape sequence. It decrements i
--				if the sequence is invalid so it can send the invalid character.
------------------------------------------------------------------------------*/
BOOL ProcessParen(CHAR* psBuffer, DWORD length, DWORD *i) {
	switch (psBuffer[(*i)++]) {
		case 'A':
			//DISPLAY_ERROR("paran A");
		    break;
		case 'B':
			//DISPLAY_ERROR("paran B");
		    break;
		case '0':
			//DISPLAY_ERROR("paran 0");
		    break;
		case '1':
			//DISPLAY_ERROR("paran 1");
		    break;
		case '2':
			//DISPLAY_ERROR("paran 2");
		    break;
		default:
			(*i)--;
			break;
	}
	return TRUE;
}

/*------------------------------------------------------------------------------
-- FUNCTION:    ProcessFont
--
-- DATE:        Oct 17, 2010
--
-- REVISIONS:   (Date and Description)
--
-- DESIGNER:    Marcel Vangrootheest
--
-- PROGRAMMER:  Marcel Vangrootheest
--
-- INTERFACE:   VOID ProcessFont(HWND hWnd)
--								hWnd		- Handle to the window
--
-- RETURNS:     VOID
--
-- NOTES:
--              Process a Font change from a sequence ending in m.
--				Changes color information based on color array defined in 
--				Application.h. Style is set to 1 for underline.
------------------------------------------------------------------------------*/
VOID ProcessFont(HWND hWnd) {
	UINT		i		= 0;
	UINT		temp	= 0;
	PWNDDATA    pwd		= NULL;
    pwd = (PWNDDATA) GetWindowLongPtr(hWnd, 0);

	for (i = 1; i <= ESC_VAL(0); i++) {
		switch (ESC_VAL(i)) {
			case 0: // default
				CUR_FG_COLOR	= 7;
				CUR_BG_COLOR	= 0;
				CUR_STYLE	    = 0;
				break;
			case 1: // bright
				if (CUR_FG_COLOR <= 7)
					CUR_FG_COLOR += 8;
				BRIGHTNESS = 8;
				break;
			case 2: // dim
				if (CUR_FG_COLOR > 7)
					CUR_FG_COLOR -= 8;
				BRIGHTNESS = 0;
				break;
			case 4: // underline
				CUR_STYLE = 1;
				break;
			case 5: // blink
				// not implemented
				break;
			case 7: // inverse
				temp            = CUR_FG_COLOR;
				CUR_FG_COLOR	= CUR_BG_COLOR;
				CUR_BG_COLOR    = temp;
				break;
			case 8: // hide
				CUR_FG_COLOR = CUR_BG_COLOR;
				break;
			default: // set colors by value
    			if (ESC_VAL(i) / 10 == 3)
					CUR_FG_COLOR	= (ESC_VAL(i) % 10) + BRIGHTNESS;
				if (ESC_VAL(i) / 10 == 4)
					CUR_BG_COLOR	= (ESC_VAL(i) % 10) + BRIGHTNESS;
		}
	}
}