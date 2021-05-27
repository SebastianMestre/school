#include "parser.h"

#include "tabla_ops.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define CANT_STRINGS_FIJOS 4
static int const largo_strings_fijos[CANT_STRINGS_FIJOS] = { 5, 6, 7, 8 };
static char const* const strings_fijos[CANT_STRINGS_FIJOS] = { "salir", "cargar", "evaluar", "imprimir" };
static TokenTag const token_strings_fijos[CANT_STRINGS_FIJOS] = { T_SALIR, T_CARGAR, T_EVALUAR, T_IMPRIMIR };

Tokenizado tokenizar(char const* str, TablaOps* tabla_ops) {
	// TODO: reconocer operadores

	while (isspace(*str))
		str += 1;

	if (*str == '\0')
		return (Tokenizado){str, (Token){T_FIN}};

	// NICETOHAVE chequear si hay un operador que arranca con =
	if (*str == '=')
		return (Tokenizado){str+1, (Token){T_IGUAL}};

	if (isalpha(str[0])) {
		// reconozco un nombre
		int largo = 1;
		while (isalnum(str[largo]))
			largo += 1;

		for (int i = 0; i < CANT_STRINGS_FIJOS; ++i)
			if (largo_strings_fijos[i] == largo &&
			    memcmp(str, strings_fijos[i], largo) == 0)
				return (Tokenizado){str+largo, (Token){token_strings_fijos[i]}};

		return (Tokenizado){str+largo, (Token){T_NOMBRE, str, largo}};
	}

	if (isdigit(str[0])) {
		// reconozco un numero
		int valor = str[0] - '0';
		int largo = 1;
		while (isdigit(str[largo])) {
			valor = (valor * 10) + (str[largo] - '0');
			largo += 1;
		}

		return (Tokenizado){str+largo, (Token){T_NUMERO, NULL, valor}};
	}
}


Parseado parsear(char const* str, TablaOps* tabla_ops) {
	Tokenizado tokenizado = tokenizar(str, tabla_ops);
	str = tokenizado.resto;

	switch (tokenizado.token.tag) {
	case T_SALIR:
		return (Parseado){S_SALIR};
		break;

	case T_EVALUAR:
		break;
		tokenizado = tokenizar(str, tabla_ops);
		str = tokenizado.resto;
		if (tokenizado.token.tag != T_NOMBRE)
			return (Parseado){S_INVALIDO, str};
		return (Parseado){S_EVALUAR, str, tokenizado.token.inicio, tokenizado.token.valor};
		break;

	case T_IMPRIMIR:
		tokenizado = tokenizar(str, tabla_ops);
		str = tokenizado.resto;
		if (tokenizado.token.tag != T_NOMBRE)
			return (Parseado){S_INVALIDO, str};
		return (Parseado){S_IMPRIMIR, str, tokenizado.token.inicio, tokenizado.token.valor};
		break;

	case T_NOMBRE: {
		char const* alias = tokenizado.token.inicio;
		int alias_n = tokenizado.token.valor;

		tokenizado = tokenizar(str, tabla_ops);
		str = tokenizado.resto;
		if (tokenizado.token.tag != T_IGUAL)
			return (Parseado){S_INVALIDO, str};

		tokenizado = tokenizar(str, tabla_ops);
		str = tokenizado.resto;
		if (tokenizado.token.tag != T_CARGAR)
			return (Parseado){S_INVALIDO, str};

		// Ahora parseo la expresion infija dos veces:
		// La primera vez para saber cuantos tokens hay, y asi poder reservar
		// la memoria necesaria para guardarlos
		// Y la segunda para guardarlos en la memoria que reserve

		char const* str_al_principio_de_la_expresion = str;
		int tokens_en_la_expresion = 0;

		// parseo y, mientras, voy validando
		while (1) {
			tokenizado = tokenizar(str, tabla_ops);
			str = tokenizado.resto;
			Token token = tokenizado.token;

			if (token.tag == T_FIN)
				break;

			if (!(token.tag == T_NUMERO || token.tag == T_NOMBRE || token.tag == T_OPERADOR))
				return (Parseado){S_INVALIDO, str};

			tokens_en_la_expresion += 1;
		}

		ExpresionPostfija expresion = {
			tokens_en_la_expresion,
			malloc(tokens_en_la_expresion * sizeof(Token)),
		};

		// no necesito validar porque ya valide en el loop anterior
		for (int i = 0; i < tokens_en_la_expresion; ++i) {
			tokenizado = tokenizar(str, tabla_ops);
			str = tokenizado.resto;
			Token token = tokenizado.token;

			expresion.tokens[i] = token;
		}

		return (Parseado){S_CARGA, tokenizado.resto, alias, alias_n, expresion};
		} break;

	default:
		return (Parseado){S_INVALIDO, str};
	}
}
