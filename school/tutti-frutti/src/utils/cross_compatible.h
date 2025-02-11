#pragma once

#include <stdio.h>
#include<stdbool.h>

void clear_screen ();

char ntilde ();

char* file_read_line (FILE* stream);
char* read_line();


bool strcmp_wrapper (void* a, void* b);
