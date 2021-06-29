#pragma once

bool
is_whitespace(char c);

// toma un string, y crea una copia en el heap
char*
string_dup(char const* str);

// modifica el string dado, eliminando los espacios que rodean el string
void
string_trim(char* str);

// modifica el string dado para que este en minusculas
void
string_tolower(char* str);

// parsea un unsigned y lo guarda en out.
// si el input no es valido, devuelve false.
bool
parse_u32(char* buf, unsigned int* out);

// parsea un unsigned y lo guarda en out.
// si el input no es valido, devuelve false.
bool
parse_int(char* buf, int* out);
