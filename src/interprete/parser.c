#include "parser.h"

#include "../tabla_ops.h"
#include "expresion.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// En el contexto de un Token, sirve para interpretar la informacion de este
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

// Representa una secuencia de caracteres.
// Consolidarla en una sola entidad permite expresar algunas partes del codigo
// mas claramente.
typedef struct Token {
	TokenTag tag;
	char const* inicio;  // apunta al texto, en caso de ser un nombre
	int valor;           // la longitud del nombre, o el valor de un numero
	EntradaTablaOps* op; // la entrada en la tabla de operadores, de ser un operador
} Token;

typedef struct Tokenizado {
	char const* resto; // resto del input
	Token token;       // token interpretado
} Tokenizado;

// Contantes asociadas a las keywords del programa.
#define CANT_STRINGS_FIJOS 4
static int const largoStringsFijos[CANT_STRINGS_FIJOS] = 
	{ 5, 6, 7, 8 };
static char const* const stringsFijos[CANT_STRINGS_FIJOS] = 
	{ "salir", "cargar", "evaluar", "imprimir" };
static TokenTag const tokenStringsFijos[CANT_STRINGS_FIJOS] = 
	{ T_SALIR, T_CARGAR, T_EVALUAR, T_IMPRIMIR };

// Funciones axuliriares para construir una estructura 'Tokenizado'.
static Tokenizado tokenizado_fin(const char* str) {
	// Rellenamos con 0 los campos no usados.
	return (Tokenizado) {str, (Token){.tag = T_FIN}};
}
static Tokenizado tokenizado_igual(const char* str) {
	return (Tokenizado) {str, (Token) {.tag = T_IGUAL}};
}
static Tokenizado tokenizado_str_fijo(const char* str, size_t i) {
	assert(i <= CANT_STRINGS_FIJOS);
	return (Tokenizado) {str, (Token){.tag = tokenStringsFijos[i]}};
}
static Tokenizado tokenizado_nombre(const char* str, char const *token_str, int largo) {
	return (Tokenizado) {str, (Token) {T_NOMBRE, token_str, largo, 0}};
}
static Tokenizado tokenizado_numero(const char* str, int valor) {
	return (Tokenizado) {str, (Token) {T_NUMERO, 0, valor, 0}};
}

// Analiza el pricipio del string, y extrae una pieza, dandole sentido.
// Luego, devuelve una representacion de esa pieza, y un puntero a donde esa
// pieza termina, y empieza el resto del string.
//
// # uso de memoria:
//  -argumentos: No limpia nada.
//  -resultado: Nada se debe limpiar.
static Tokenizado tokenizar(char const* str, TablaOps* tablaOps) {

	// Descartamos espacio en blanco.
	while (isspace(*str))
		str += 1;

	// LLegamos al fin de la linea.
	if (*str == '\0')
		return tokenizado_fin(str);
	
	// Reconocemos un operador (el mas largo que matchee).
	EntradaTablaOps* opQueMatchea = NULL;
	int largoOpQueMatchea = 0;
	// Buscamos el operador en la tabla de operaciones.
	for (EntradaTablaOps* it = tablaOps->entradas; it; it = it->sig) {
		int largoOp = strlen(it->simbolo);

		if (largoOp < largoOpQueMatchea)
			continue;

		int matchea = 1;
		for (int i = 0; i < largoOp; ++i) {
			if (str[i] != it->simbolo[i]) {
				matchea = 0;
				break;
			}
		}

		if (matchea) {
			opQueMatchea = it;
			largoOpQueMatchea = largoOp;
		}
	}

	if (opQueMatchea != NULL && !isalpha(str[largoOpQueMatchea]))
		return (Tokenizado){
			str + largoOpQueMatchea,
			(Token){T_OPERADOR, NULL, 0, opQueMatchea}};

	// Identificamos un simbolo '='. 
	if (*str == '=')
		return tokenizado_igual(str + 1);

	// Reconocemos un nombre.
	if (isalpha(str[0]) || (str[0] == '_')) {
		int largo = 1;
		while (isalnum(str[largo]) || (str[largo] == '_'))
			largo += 1;
		
		// Chequeamos si la palabra es una keyword (cargar, salir, etc.). 
		for (size_t i = 0; i < CANT_STRINGS_FIJOS; ++i)
			if (largoStringsFijos[i] == largo &&
			    memcmp(str, stringsFijos[i], largo) == 0) {
				return tokenizado_str_fijo(str + largo, i);}
		
		// Si no lo es, debe ser un alias. 
		return tokenizado_nombre(str + largo, str, largo);
	}

	// Reconocemos un numero.
	if (isdigit(str[0])) {
		int valor = str[0] - '0';
		int largo = 1;
		while (isdigit(str[largo])) {
			valor = (valor * 10) + (str[largo] - '0');
			largo += 1;
		}

		return tokenizado_numero(str + largo, valor);
	}

	// No hay coincidencias: el token es invalido.
	return (Tokenizado){str, (Token){T_INVALIDO, 0, 0, 0}};
}


// Usamos esta pila para armar un arbol de expresiones a partir de la expresion
// postifja.
typedef struct EntradaPilaDeExpresiones EntradaPilaDeExpresiones;
struct EntradaPilaDeExpresiones {
	EntradaPilaDeExpresiones* sig;
	Expresion* expresion;
};

typedef struct PilaDeExpresiones {
	EntradaPilaDeExpresiones* entradas;
} PilaDeExpresiones;

// Inserta una expresion en el tope de la pila.
static void pila_de_expresiones_push(PilaDeExpresiones* pila, 
	Expresion* expresion) {
	EntradaPilaDeExpresiones* entrada = malloc(sizeof(*entrada));
	*entrada = (EntradaPilaDeExpresiones){
		.sig = pila->entradas,
		.expresion = expresion
	};
	pila->entradas = entrada;
}

// Remueve un elemento del tope de la pila y lo devuelve.
static Expresion* pila_de_expresiones_pop(PilaDeExpresiones* pila) {
	if (pila->entradas == NULL) return NULL;
	Expresion* result = pila->entradas->expresion;
	EntradaPilaDeExpresiones* entrada = pila->entradas;
	pila->entradas = entrada->sig;
	free(entrada);
	return result;
}

// Devuelve el elemento tope de la pila.
static Expresion* pila_de_expresiones_top(PilaDeExpresiones* pila) {
	if (pila->entradas == NULL)
		return NULL;
	return pila->entradas->expresion;
}

// Libera el espacio de memoria ocupado por la pila.
static void pila_de_expresiones_limpiar_datos(PilaDeExpresiones* pila) {
	EntradaPilaDeExpresiones* it = pila->entradas;
	while (it) {
		EntradaPilaDeExpresiones* sig = it->sig;
		expresion_limpiar(it->expresion);
		free(it);
		it = sig;
	}
}

// Funciones auxiliares para construir una estructura 'Parseado'.
static Parseado parseado_invalido(char const* str, ErrorTag error) {
	return (Parseado){str, (Sentencia){.tag = S_INVALIDO}, error};
}
static Parseado parseado_salir(const char* str) {
	return (Parseado){str, (Sentencia){.tag = S_SALIR}, 0};
}
static Parseado parseado_evaluar(const char* str, const char* alias,
	int alias_n) {
	return (Parseado) {str, (Sentencia) {S_EVALUAR, alias, alias_n, 0}, 0};
	}
static Parseado parseado_imprimir(const char* str, const char* alias, 
	int alias_n) {
	return (Parseado) {str, (Sentencia) {S_IMPRIMIR, alias, alias_n, 0}, 0}; 
}
static Parseado parseado_cargar(
	const char* str,
	const char* alias,
	int alias_n,
	Expresion* expresion) {
	return (Parseado){str, (Sentencia){S_CARGA, alias, alias_n, expresion}, 0};
	}


Parseado parsear(char const* str, TablaOps* tablaOps) {
	// Obtenemos el primer token del input.
	Tokenizado tokenizado = tokenizar(str, tablaOps);
	str = tokenizado.resto;

	// Procedemos acorde al tipo de token:
	switch (tokenizado.token.tag) {
	case T_SALIR:
		return parseado_salir(str);
		break;
	
	// evaluar
	case T_EVALUAR:
		tokenizado = tokenizar(str, tablaOps);
		str = tokenizado.resto;
		// Si no se ingreso un alias, el input es invalido.
		if (tokenizado.token.tag != T_NOMBRE)
			return parseado_invalido(str, E_PARSER_ALIAS);
		return
			parseado_evaluar(str, tokenizado.token.inicio, tokenizado.token.valor);
		break;

	// imprimir
	case T_IMPRIMIR:
		tokenizado = tokenizar(str, tablaOps);
		str = tokenizado.resto;
		// Si no se ingreso un alias, el input es invalido.
		if (tokenizado.token.tag != T_NOMBRE)
			return parseado_invalido(str, E_PARSER_ALIAS);
		return 
			parseado_imprimir(str, tokenizado.token.inicio, tokenizado.token.valor);
		break;

	// alias
	case T_NOMBRE: {
		char const* alias = tokenizado.token.inicio;
		int alias_n = tokenizado.token.valor;

		tokenizado = tokenizar(str, tablaOps);
		str = tokenizado.resto;
		
		// Si no se ingreso un '=', el input es invalido.
		if (tokenizado.token.tag != T_IGUAL)
			return parseado_invalido(str, E_PARSER_OPERACION);

		// Chequeamos que se haya ingresado la keyword 'cargar'.
		tokenizado = tokenizar(str, tablaOps);
		str = tokenizado.resto;
		if (tokenizado.token.tag != T_CARGAR)
			return parseado_invalido(str, E_PARSER_CARGA);

		// Convierto expresion postfija a infija, usando una pila:
		// Los valores sueltos, como numeros y aliases, los inserto en la pila.
		// Al encontrar un operador, extraigo tantas expresiones de la pila como
		// sea la aridad del operador, y creo la expresion que representa la
		// aplicacion del operador a sus operandos. Finalmente, inserto esa
		// expresion en la pila.

		// {} inicializa con 0s, lo cual es el estado inicial correcto
		PilaDeExpresiones p = {};
		// parseo y, mientras, voy validando
		while (1) {
			tokenizado = tokenizar(str, tablaOps);
			str = tokenizado.resto;
			Token token = tokenizado.token;

			if (token.tag == T_FIN)
				break;

			switch (token.tag) {
			case T_NUMERO: {
				pila_de_expresiones_push(&p, expresion_numero(token.valor));
				} break;
			case T_NOMBRE: {
				pila_de_expresiones_push(&p, expresion_alias(token.inicio, token.valor));
				} break;
			case T_OPERADOR: {
				Expresion* arg1 = pila_de_expresiones_pop(&p);
				if (arg1 == NULL)
					goto fail_arg1;

				Expresion* arg2 = NULL;
				if (token.op->aridad == 2) {
					arg2 = pila_de_expresiones_pop(&p);
					if (arg2 == NULL)
						goto fail_arg2;
				}

				pila_de_expresiones_push(&p, expresion_operacion(token.op, arg1, arg2));
				break;

				// Si el arg2 es invalido, debemos limpiar el arg1 primero.
				fail_arg2:
				expresion_limpiar(arg1);

				fail_arg1:
				pila_de_expresiones_limpiar_datos(&p);
				return parseado_invalido(str, E_PARSER_EXPRESION);

				} break;

			// No reconocimos numero, operacion o alias.
			default:
				pila_de_expresiones_limpiar_datos(&p);
				return parseado_invalido(str, E_PARSER_EXPRESION);
			}
		}

		// Obtenemos la expresion final de la pila.
		Expresion* expresion = pila_de_expresiones_pop(&p);

		// Si no se ingreso ninguna expresion, informamos el error.
		if (expresion == NULL) return parseado_invalido(str, E_PARSER_VACIA);
		// Si hay elementos de mas en la pila, la expresion es invalida.
		if (pila_de_expresiones_top(&p) != NULL) {
			expresion_limpiar(expresion);
			pila_de_expresiones_limpiar_datos(&p);
			return parseado_invalido(str, E_PARSER_EXPRESION);
		}
		// En caso de estar todo ok, devolvemos la sentencia apropiada.
		return parseado_cargar(str, alias, alias_n, expresion);
		} break;
	
	case T_OPERADOR:
		// Una sentencia valida no comienza con un operador.
		// Elevamos error y pasamos el simbolo del operador para alertar al usuario.
		return parseado_invalido(tokenizado.token.op->simbolo, E_PARSER_OPERADOR);
	// Si ninuna operacion (cargar, salir, etc.) matchea, el input es invalido.
	default:
		return parseado_invalido(str, E_PARSER_OPERACION);
	}
}
