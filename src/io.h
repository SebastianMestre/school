#include <stdio.h>

// Lee una linea (incluyendo el caracter de salto de linea) y la guarda en buf.
// Devuelve false sii no alcanza el espacio.
// La linea se consume entera irrespectivamente de si entra en el buffer.
bool
get_line(char* buf, size_t n, FILE* f);

// Lee una linea usando get_line.
// Si la linea consta de espacio en blanco, seguido de un entero, seguido de mas
// espacio en blanco, guarda ese entero en out.
// Siempre modifica el buffer que se le pasa.
// Si falla, devuelve false, y no modifica out.
bool
get_line_as_int(char* buf, size_t n, int* out, FILE* f);

// lo mismo que get_line_as_int pero unsigned
bool
get_line_as_uint(char* buf, size_t n, unsigned int* out, FILE* f);

// llama intenta leer un renglon e interpretarlo como unsigned hasta que lo logra.
// si hay un error leyendo la linea, imprime line_error_msg
// si hay un error parseando el numero, imprime value_error_msg
void
get_line_as_uint_retry(char* buf, size_t n, unsigned int* out, char const* line_error_msg, char const* value_error_msg, FILE* f);

// imprime un string en modo titulo
void
print_title_case(char const* str, FILE* f);
