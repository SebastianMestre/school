#include "parser.h"

#include "tabla_ops.h"

#include <ctype.h>
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
	EntradaTablaOps op;
} Tokenizado;

#define CANT_STRINGS_FIJOS 4
static int const largo_strings_fijos[CANT_STRINGS_FIJOS] = { 5, 6, 7, 8 };
static char const* const strings_fijos[CANT_STRINGS_FIJOS] = { "salir", "cargar", "evaluar", "imprimir" };
static TokenTag const token_strings_fijos[CANT_STRINGS_FIJOS] = { T_SALIR, T_CARGAR, T_EVALUAR, T_IMPRIMIR };

static Tokenizado tokenizar(char const* str, TablaOps* tabla_ops) {
	// TODO: reconocer operadores

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

		return (Tokenizado){T_NUMERO, str+largo, NULL, valor};
	}
}


Parseado parsear(char const* str, TablaOps* tabla_ops) {
	Tokenizado tokenizado = tokenizar(str, tabla_ops);

	switch (tokenizado.tag) {
	case T_SALIR:
		return (Parseado){S_SALIR};
		break;

	case T_EVALUAR:
		break;
		str = tokenizado.resto;
		tokenizado = tokenizar(str, tabla_ops);
		if (tokenizado.tag != T_NOMBRE)
			return (Parseado){S_INVALIDO, str};
		return (Parseado){S_EVALUAR, tokenizado.resto, tokenizado.inicio, tokenizado.valor};
		break;

	case T_IMPRIMIR:
		str = tokenizado.resto;
		tokenizado = tokenizar(str, tabla_ops);
		if (tokenizado.tag != T_NOMBRE)
			return (Parseado){S_INVALIDO, str};
		return (Parseado){S_IMPRIMIR, tokenizado.resto, tokenizado.inicio, tokenizado.valor};
		break;

	case T_NOMBRE: {
		char const* alias = tokenizado.inicio;
		int alias_n = tokenizado.valor;

		str = tokenizado.resto;
		tokenizado = tokenizar(str, tabla_ops);
		if (tokenizado.tag != T_IGUAL)
			return (Parseado){S_INVALIDO, str};

		str = tokenizado.resto;
		tokenizado = tokenizar(str, tabla_ops);
		if (tokenizado.tag != T_CARGAR)
			return (Parseado){S_INVALIDO, str};

		void* expresion = NULL; // TODO parsear expresiones, cambiar por Expresion*
		// return (Parseado){S_CARGA, tokenizado.resto, alias, alias_n, expresion};

		return (Parseado){S_INVALIDO, str};
		} break;

	default:
		return (Parseado){S_INVALIDO, str};
	}
}
