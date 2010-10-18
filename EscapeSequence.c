#include "Presentation.h"

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
			DISPLAY_ERROR(">");
			if (i == length)
				return;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;
		case '=':
			DISPLAY_ERROR("=");
			if (i == length)
				return;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;
		case 'M':
			DISPLAY_ERROR("M");
			if (i == length)
				return;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;
		case 'H':
			DISPLAY_ERROR("H");
			if (i == length)
				return;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;
		case 'E':
			DISPLAY_ERROR("E");
			if (i == length)
				return;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;
		case 'D':
			DISPLAY_ERROR("D");
			if (i == length)
				return;
			ProcessRead(hWnd, psBuffer + i, length - i);
			return;
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
	*psDigits = *(psBuffer + *i);
	psDigits[length - *i] = '\0';

	j = strspn(psDigits + *i, "0123456789");
	strncpy(psDigits, psBuffer + *i, j);
	digit = atoi(psDigits);
	*i += j;
	free(psDigits);

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
					MoveCursor(hWnd, X + 1, Y - ESC_VAL(1) + 1);
			        break;
			    case 'B':                                       // Esc[0B
					MoveCursor(hWnd, X + 1, Y + ESC_VAL(1) + 1);
			        break;
			    case 'C':                                       // Esc[0C
					MoveCursor(hWnd, X + ESC_VAL(1) + 1, Y + 1);
			        break;
			    case 'D':                                       // Esc[0D
					MoveCursor(hWnd, X - ESC_VAL(1) + 1, Y + 1);
			        break;
			    case 'g':                                      
					DISPLAY_ERROR("num g");
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
					DISPLAY_ERROR("num m");
			        break;
			    case ';':
			        if (!CheckDigitsSemi(hWnd, psBuffer, length, i))
			            return FALSE;
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
				case 'r':                                       
					DISPLAY_ERROR("num semi num r");            
					break;
				case 'H':                                       // Esc0;0H
                case 'f':                                       // Esc0;0f
					MoveCursor(hWnd, ESC_VAL(1), ESC_VAL(2));
					break;
				case 'm':
					DISPLAY_ERROR("num semi num m");
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
						DISPLAY_ERROR("num semi num etc m");
					} else {
						(*i)--;
					}
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
					DISPLAY_ERROR("q num h");
					break;
			    case 'l':
					DISPLAY_ERROR("q num l");
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
        case 'H':                                               // Esc[H
        case 'f':                                               // Esc[f
            MoveCursor(hWnd, 1, 1);
		    break;
		case 'g':
			DISPLAY_ERROR("g");
		    break;
		case 'K':                                               // Esc[K
			ClearLine(hWnd, X, Y, CLR_RIGHT);
		    break;
		case 'J':                                               // Esc[J
			ClearScreen(hWnd, X, Y, CLR_DOWN);                      
		    break;
		case 'm':
			DISPLAY_ERROR("m");
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
                MoveCursor(hWnd, 1, 1);                             
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
			DISPLAY_ERROR("paran A");
		    break;
		case 'B':
			DISPLAY_ERROR("paran B");
		    break;
		case '0':
			DISPLAY_ERROR("paran 0");
		    break;
		case '1':
			DISPLAY_ERROR("paran 1");
		    break;
		case '2':
			DISPLAY_ERROR("paran 2");
		    break;
		default:
			(*i)--;
			break;
	}
	return TRUE;
}
