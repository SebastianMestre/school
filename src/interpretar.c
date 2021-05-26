#include "interpretar.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef enum {
	T_NOMBRE,   // una cadena alfanumrica
	T_OPERADOR, // un operador
	T_NUMERO,   // un numero (secuencia de digitos)
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
	FuncionEvaluacion eval;
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
		return (Tokenizado){T_FIN, str};

	// NICETOHAVE chequear si hay un operador que arranca con =
	if (*str == '=')
		return (Tokenizado){T_IGUAL, str+1};

	if (isalpha(str[0])) {
		// reconozco un nombre
		int largo = 1;
		while (isalnum(str[largo]))
			largo += 1;

		for (int i = 0; i < CANT_STRINGS_FIJOS; ++i)
			if (largo_strings_fijos[i] == largo &&
			    memcmp(str, strings_fijos[i], largo) == 0)
				return (Tokenizado){token_strings_fijos[i], str+largo};

		return (Tokenizado){T_NOMBRE, str+largo, str, largo};
	}

	if (isdigit(str[0])) {
		// reconozco un numero
		int valor = str[0] - '0';
		int largo = 1;
		while (isdigit(str[largo])) {
			valor = (valor * 10) + (str[largo] - '0');
			largo += 1;
		}

		return (Tokenizado){T_NOMBRE, str+largo, NULL, valor};
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
	int alias_n;
	void* expresion; // TODO reemplazar por Expresion*
} Parseado;

// TODO: recibir tabla de operadores para reconocerlos
Parseado parsear(char const* str) {
	Tokenizado tokenizado = tokenizar(str);

	switch (tokenizado.tag) {
	default:
		return (Parseado){E_INVALIDO, str};
	}
}


typedef struct EntradaTablaAlias EntradaTablaAlias;
struct EntradaTablaAlias {
	EntradaTablaAlias* sig;

	char const* alias;
	int alias_n;
	void* expresion; // TODO reemplazar por Expresion*
};

typedef struct {
	EntradaTablaAlias* entradas;
} TablaAlias;

typedef struct {
	// TODO: lista de alias y expresiones etc
	char* buffer_input;
	int tamano_buffer_input;
} Entorno;

Entorno entorno_crear() {
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

int evaluar(Entorno* entorno, char const* alias, int alias_n) {
}

void imprimir(Entorno* entorno, char const* alias, int alias_n) {
}

void cargar(Entorno* entorno, char const* alias, int alias_n, void* expresion) {
}

void interpretar(TablaOps* tabla) {
	Entorno entorno = entorno_crear();
	while (1) {
		char const* input = leer_input(&entorno);
		Parseado parseado = parsear(input);

		switch (parseado.tag) {
		case E_CARGA:
			cargar(&entorno, parseado.alias, parseado.alias_n, parseado.expresion);
			break;
		case E_IMPRIMIR:
			imprimir(&entorno, parseado.alias, parseado.alias_n);
			break;
		case E_EVALUAR: {
			int resultado = evaluar(&entorno, parseado.alias, parseado.alias_n);
			printf("%d\n", resultado);
			} break;
		case E_INVALIDO:
			puts("Error, saliendo."); // NICETOHAVE mejor error
		// fallthrough
		case E_SALIR:
			entorno_limpiar(&entorno);
			return;
		}
	}
}
