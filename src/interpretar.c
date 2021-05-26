#include "interpretar.h"

#include "expresion.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
int const largo_strings_fijos[CANT_STRINGS_FIJOS] = { 5, 6, 7, 8 };
char const* const strings_fijos[CANT_STRINGS_FIJOS] = { "salir", "cargar", "evaluar", "imprimir" };
TokenTag const token_strings_fijos[CANT_STRINGS_FIJOS] = { T_SALIR, T_CARGAR, T_EVALUAR, T_IMPRIMIR };

Tokenizado tokenizar(char const* str, TablaOps* tabla_ops) {
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

Parseado parsear(char const* str, TablaOps* tabla_ops) {
	Tokenizado tokenizado = tokenizar(str, tabla_ops);

	switch (tokenizado.tag) {
	case T_SALIR:
		return (Parseado){E_SALIR};
		break;

	case T_EVALUAR:
		break;
		str = tokenizado.resto;
		tokenizado = tokenizar(str, tabla_ops);
		if (tokenizado.tag != T_NOMBRE)
			return (Parseado){E_INVALIDO, str};
		return (Parseado){E_EVALUAR, tokenizado.resto, tokenizado.inicio, tokenizado.valor};
		break;

	case T_IMPRIMIR:
		str = tokenizado.resto;
		tokenizado = tokenizar(str, tabla_ops);
		if (tokenizado.tag != T_NOMBRE)
			return (Parseado){E_INVALIDO, str};
		return (Parseado){E_IMPRIMIR, tokenizado.resto, tokenizado.inicio, tokenizado.valor};
		break;

	case T_NOMBRE: {
		char const* alias = tokenizado.inicio;
		int alias_n = tokenizado.valor;

		str = tokenizado.resto;
		tokenizado = tokenizar(str, tabla_ops);
		if (tokenizado.tag != T_IGUAL)
			return (Parseado){E_INVALIDO, str};

		str = tokenizado.resto;
		tokenizado = tokenizar(str, tabla_ops);
		if (tokenizado.tag != T_CARGAR)
			return (Parseado){E_INVALIDO, str};

		void* expresion = NULL; // TODO parsear expresiones, cambiar por Expresion*
		// return (Parseado){E_CARGA, tokenizado.resto, alias, alias_n, expresion};

		return (Parseado){E_INVALIDO, str};
		} break;

	default:
		return (Parseado){E_INVALIDO, str};
	}
}


// Explicacion:
// para simplificar el uso de memoria, en vez de guardar los aliases, cada uno
// en su propia region de memoria, referenciamos su posicion original en la
// linea que ingreso el usuario.
// Esta linea se guarda en su entrada correspondiente en la tabla de aliases

typedef struct EntradaTablaAlias EntradaTablaAlias;
struct EntradaTablaAlias {
	EntradaTablaAlias* sig;

	char* input;
	char const* alias;
	int alias_n;
	void* expresion; // TODO reemplazar por Expresion*
};

typedef struct {
	EntradaTablaAlias* entradas;
} TablaAlias;

EntradaTablaAlias* ta_encontrar(TablaAlias* tabla, char const* alias, int alias_n) {
	for (EntradaTablaAlias* it = tabla->entradas; it; it = it->sig)
		if (it->alias_n == alias_n && memcmp(it->alias, alias, alias_n) == 0)
			return it;
	return NULL;
}

// limpia 'input' y 'expresion'
EntradaTablaAlias* ta_insertar(TablaAlias* tabla, char* input, char const* alias, int alias_n, void* expresion) {
	EntradaTablaAlias* nuevo = malloc(sizeof(*nuevo));
	*nuevo = (EntradaTablaAlias) {
		.sig = tabla->entradas,
		.alias = alias,
		.alias_n = alias_n,
		.input = input,
		.expresion = expresion
	};
	tabla->entradas = nuevo;
	return nuevo;
}

// limpia 'input' y 'expresion'
EntradaTablaAlias* ta_insertar_o_reemplazar(TablaAlias* tabla, char* input, char const* alias, int alias_n, void* expresion) {
	EntradaTablaAlias* encontrado = ta_encontrar(tabla, alias, alias_n);

	if (encontrado == NULL)
		return ta_insertar(tabla, input, alias, alias_n, expresion);

	free(encontrado->input);
	expresion_limpiar(encontrado->expresion);

	encontrado->input = input;
	encontrado->expresion = expresion;
	return encontrado;
}

void ta_limpiar(TablaAlias* tabla) {
	// TODO
}



typedef struct {
	TablaAlias aliases;
	char* buffer_input;
	int tamano_buffer_input;
} Entorno;

Entorno entorno_crear() {
	return (Entorno){};
}

void leer_input(Entorno* entorno) {
	// NICETOHAVE que se banque renglones arbitrariamente grandes, usando un
	// buffer que crece dinamicamente si hace falta
	if (entorno->buffer_input == NULL) {
		entorno->buffer_input = malloc(1024);
		entorno->tamano_buffer_input = 1024;
	}
	// NICETOHAVE: esto no se porta muy bien si se pasa de input: deja todo lo q
	// sobra en el buffer de entrada
	fgets(entorno->buffer_input, 1023, stdin);
}

void descartar_input(Entorno* entorno) {
	assert(entorno != NULL);
	free(entorno->buffer_input);
	entorno->buffer_input = NULL;
	entorno->tamano_buffer_input = 0;
}

char* robar_input(Entorno* entorno) {
	char* buffer = entorno->buffer_input;
	entorno->buffer_input = NULL;
	entorno->tamano_buffer_input = 0;
	return buffer;
}

void entorno_limpiar(Entorno* entorno) {
	if (entorno->buffer_input != NULL)
		descartar_input(entorno);
	ta_limpiar(&entorno->aliases);
	// TODO
	return;
}

int evaluar(Entorno* entorno, char const* alias, int alias_n) {
	// TODO
	return 0;
}

void imprimir(Entorno* entorno, char const* alias, int alias_n) {
	// TODO
}

// limpia 'input' y 'expresion'
void cargar(Entorno* entorno, char* input, char const* alias, int alias_n, void* expresion) {
	ta_insertar_o_reemplazar(&entorno->aliases, input, alias, alias_n, expresion);
}

void interpretar(TablaOps* tabla_ops) {
	Entorno entorno = entorno_crear();
	while (1) {
		printf("> ");
		leer_input(&entorno);
		Parseado parseado = parsear(entorno.buffer_input, tabla_ops);

		switch (parseado.tag) {
		case E_CARGA:
			cargar(&entorno, robar_input(&entorno), parseado.alias, parseado.alias_n, parseado.expresion);
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
