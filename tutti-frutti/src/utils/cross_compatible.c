#include "./cross_compatible.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void clear_screen () {
    // return;
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

char ntilde () {
    #ifdef _WIN32
        return '\241';
    #else
        return 'n';
    #endif
}

char* file_read_line (FILE* stream) {
    char BUFFER[1024];

    int v = fscanf(stream, "%[^\n]%*c", BUFFER);

    if(v == 0 || v == EOF)
        return NULL;
    else
        return strcpy(malloc(strlen(BUFFER)+1), BUFFER);
}

char* read_line () {
    return file_read_line(stdin);
}


bool strcmp_wrapper (void* a, void* b) {
	return strcmp (a, b) < 0;
}
