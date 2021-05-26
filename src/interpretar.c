#include "interpretar.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef enum {
	T_NOMBRE,   // una cadena alfanumrica
	T_OPERADOR, // un operador
	T_IMPRIMIR, // 'imprimir'
	T_EVALUAR,  // 'evaluar'
	T_CARGAR,   // 'cargar'
	T_SALIR,    // 'salir'
	T_IGUAL,    // '='
	T_FIN,      // el final del string
	T_INVALIDO, // un error
} TokenTag;

typedef struct {

	TokenTag tag;
	char const* resto;

	char const* inicio;
	int valor;
} Tokenizado;

#define CANT_STRINGS_FIJOS 4
int const largo_strings_fijos[CANT_STRINGS_FIJOS] = { 5, 6, 7, 8 };
char const* const strings_fijos[CANT_STRINGS_FIJOS] = { "salir", "cargar", "evaluar", "imprimir" };
TokenTag const token_strings_fijos[CANT_STRINGS_FIJOS] = { T_SALIR, T_CARGAR, T_EVALUAR, T_IMPRIMIR };

// TODO: recibir tabla de operadores para reconocerlos
Tokenizado tokenizar(char const* str) {
	while (isspace(*str))
		str += 1;

	if (*str == '\0')
		return (Tokenizado){T_FIN, str, NULL, 0};

	// TODO? chequear si hay un operador que arranca con =
	if (*str == '=')
		return (Tokenizado){T_IGUAL, str+1, NULL, 0};

	if (isalpha(str[0])) {
		// reconozco un nombre
		int largo = 1;
		while (isalnum(str[largo]))
			largo += 1;

		for (int i = 0; i < CANT_STRINGS_FIJOS; ++i)
			if (largo_strings_fijos[i] == largo &&
			    memcmp(str, strings_fijos[i], largo) == 0)
				return (Tokenizado){token_strings_fijos[i], str+largo, NULL, 0};

		return (Tokenizado){T_NOMBRE, str+largo, str, largo};
	}
}



typedef enum {
	E_CARGA,    // T_NOMBRE '=' 'cargar' expresion
	E_IMPRIMIR, // 'imprimir' T_NOMBRE
	E_EVALUAR,  // 'evaluar' T_NOMBRE
	E_SALIR,    // 'salir'
	E_INVALIDO, // un error
} ExprTag;

typedef struct {
	ExprTag tag;
	char const* resto;

	char const* alias;
	void* expresion; // TODO
} Parseado;

Parseado parsear(char const* str) {
	return (Parseado){E_INVALIDO, str, NULL, NULL};
}



typedef struct {
	char* buffer_input;
	int tamano_buffer_input;
} Entorno;

Entorno entorno_nuevo() {
	return (Entorno){NULL, 0};
}

char const* leer_input(Entorno* entorno) {
	entorno->buffer_input = NULL;
	entorno->tamano_buffer_input = 0;
	return entorno->buffer_input;
}

void entorno_limpiar(Entorno* entorno) {
	return;
}

void interpretar(TablaOps* tabla) {
	Entorno entorno = entorno_nuevo();
	while (1) {
		char const* input = leer_input(&entorno);
		Parseado parseado = parsear(input);

		switch (parseado.tag) {
		case E_CARGA:
			break;
		case E_IMPRIMIR:
			break;
		case E_EVALUAR:
			break;
		case E_INVALIDO:
			// TODO mejor error
			puts("Error, saliendo.");
		// fallthrough
		case E_SALIR:
			entorno_limpiar(&entorno);
			return;
		}

	}
}
