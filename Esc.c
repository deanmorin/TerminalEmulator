#include <stdio.h>
#include <string.h>
#define BUFSIZE 256
/*
const char va1[] = {'[', '(', ')', '>', '=', 'M', 'H', 'E', 'D'};
const char vasquare[] = {'H', 'f', 'g', 'K', 'J', 'm' '?'};
const char vaparan[] = {'A', 'B', '0', '1', '2'};
const char vadigit1[] = {'A', 'B', 'C', 'D', 'g', 'K', 'J', 'm', ';'};
const char vadigit2[] = {'r', 'H', 'f', 'm', ';'};
const char vadigit3plus[] = {'m', ';'};
const char vadigitq[] = {'h', 'l'};
*/

int procEsc(char*);
void procRead(char*);
void ltrim_copy(char[], const char[], size_t pos);

int main() {
    char line[BUFSIZE];
    
    printf("Read:\n");
    while (fgets(line, BUFSIZE, stdin) != NULL) {
        procRead(line);
    }
    
    return 0;
}

void ltrim_copy(char dest[], const char src[], size_t pos) {
	size_t j;
	for (j = 0; src[pos] != '\0'; pos++, j++) {
		dest[j] = src[pos];
	}
	dest[j] = '\0';
}

void procRead(char buffer[]) {
    size_t i = 0;
    char escBuffer[BUFSIZE];
    int escape = 0;
    static int incomplete = 0;
    if (incomplete) {
        incomplete = procEsc(buffer);
        return;
    }
    for (i = 0; buffer[i] != '\0' && !escape; i++) {
        switch (buffer[i]) {
            case '\033':
                escape = 1;
                break;
            default:
                printf("%c", buffer[i]);
        }
    }
    if (escape) {
        ltrim_copy(escBuffer, buffer, i);
        incomplete = procEsc(escBuffer);
    }
}

int procEsc(char buffer[]) {
    int i = 0;
    char rest[BUFSIZE];
    static char incomplete[BUFSIZE];
    
    if (strcmp(incomplete,"\0") != 0) {
        strcat(incomplete, buffer);
        strcpy(buffer, incomplete);
        incomplete[i] = '\0';
    }
    
    for (i = 0; buffer[i] != '\0'; i++) {
        switch (buffer[i]) {
            /*invalid sequence*/
            case '\033':
                ltrim_copy(rest, buffer, i + 1);
                procRead(rest);
                return 0;
            /*valid sequence*/
            case 'm':
                strncpy(rest, buffer, i + 1);
                rest[i+1] = '\0';
                printf("Escape(%s)\n", rest);
                ltrim_copy(rest, buffer, i + 1);
                procRead(rest);
                return 0;
        }
    }
    strcpy(incomplete, buffer);
    return 1;
}
