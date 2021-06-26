#include <stdio.h>

// Lee una linea (incluyendo el caracter de salto de linea) y la guarda en buf.
// Devuelve false si necesita mas espacio.
bool get_line(char* buf, size_t n, FILE* f);

// Lee una linea (incluyendo el caracter de salto de linea).
// Si la linea consta de espacio en blanco, seguido de un entero, seguido de mas
// espacio en blanco, guarda ese entero en out.
// Siempre modifica el buffer que se le pasa.
// Si falla, devuelve false, y no modifica out.
bool get_line_as_int(char* buf, size_t n, int* out, FILE* f);

// lo mismo que get_line_as_int pero unsigned
bool get_line_as_uint(char* buf, size_t n, unsigned int* out, FILE* f);

// devuelve el proximo caracter en un archivo, sin consumirlo
char file_peek(FILE* f);
