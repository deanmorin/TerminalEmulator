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
	*psDigits = *psBuffer;
	psDigits[length] = '\0';

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

    if (isdigit(psBuffer[--(*i)])) {
        if (*i == length - 1)
			return FALSE;

	    digit = GetDigit(psBuffer, length, i);
        pwd->dwEscSeqValues[0]++;
        pwd->dwEscSeqValues[1] = digit;

	    if ((*i)++ == length - 1)
			return FALSE;

	    if (digit >= 0) {
	        switch (psBuffer[(*i)++]) {
			    case 'A':
					DISPLAY_ERROR("num A");
			        break;
			    case 'B':
					DISPLAY_ERROR("num B");
			        break;
			    case 'C':
					DISPLAY_ERROR("num C");
			        break;
			    case 'D':
					DISPLAY_ERROR("num D");
			        break;
			    case 'g':
					DISPLAY_ERROR("num g");
			        break;
			    case 'K':
					DISPLAY_ERROR("num K");
			        break;
			    case 'J':                                       // Esc[0J
					if (pwd->dwEscSeqValues[1] == 0) {
                        ClearScreen(hWnd, X, Y, CLR_DOWN);
                    } else if (pwd->dwEscSeqValues[1] == 1) {
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

    if (isdigit(psBuffer[*i])) {
        if (*i == length - 1)
			return FALSE;

	    digit = GetDigit(psBuffer, length, i);
        pwd->dwEscSeqValues[0]++;
        pwd->dwEscSeqValues[1] = digit;

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

    if (isdigit(psBuffer[*i])) {
        if (*i == length - 1)
			return FALSE;

	    digit = GetDigit(psBuffer, length, i);
        pwd->dwEscSeqValues[0]++;
        pwd->dwEscSeqValues[2] = digit;
	    
		if (*i == length - 1)
			return FALSE;

	    if (digit >= 0) {
	        switch (psBuffer[(*i)++]) {
				case 'r':                                       
					DISPLAY_ERROR("num semi num r");            
					break;
				case 'H':                                       // Esc0;0H
					MoveCursor(hWnd, pwd->dwEscSeqValues[1],        
                              pwd->dwEscSeqValues[2]);
					break;
				case 'f':
					DISPLAY_ERROR("num semi num f");
					break;
				case 'm':
					DISPLAY_ERROR("num semi num m");
					break;
				case ';':
					do {
						if (*i == length)
							return FALSE;
						if (isdigit(psBuffer[*i])) {
							digit = GetDigit(psBuffer, length, i);
                            pwd->dwEscSeqValues[0]++;
                            pwd->dwEscSeqValues[3 + count++] = digit;
							//validate digit

							if ((*i)++ == length)
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
			}
		} else {
			(*i)--;
		}
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
			i--;
			break;
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
		case 'H':
            MoveCursor(hWnd, 1, 1);
		    break;
		case 'f':
			DISPLAY_ERROR("f");
		    break;
		case 'g':
			DISPLAY_ERROR("g");
		    break;
		case 'K':
			DISPLAY_ERROR("K");
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
        case ';':
            if (psBuffer[*i] == 'H') {                          // Esc[;H
                MoveCursor(hWnd, 1, 1);                             
            } else {
                (*i)--;
            }
            break;
		default:
			if (!CheckDigits(hWnd, psBuffer, length, i)) {
				return FALSE;
			}
			(*i)--;
			break;
	}
	return TRUE;
}
