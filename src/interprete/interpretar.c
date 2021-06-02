#include "interpretar.h"

#include "expresion.h"
#include "parser.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER 1024

// Explicacion:
// para simplificar el uso de memoria, en vez de guardar los aliases, cada uno
// en su propia region de memoria, referenciamos su posicion original en la
// linea que ingreso el usuario, mediante un puntero. (char const* alias)
// Esta linea se guarda en su entrada correspondiente en la tabla de aliases
// especificamente, el puntero al buffer de entrada va en el campo 'input' de
// EntradaTablaAlias

typedef struct EntradaTablaAlias EntradaTablaAlias;
struct EntradaTablaAlias {
	EntradaTablaAlias* sig;

	char* input;
	char const* alias;
	int alias_n;
	Expresion* expresion;
};

typedef struct {
	EntradaTablaAlias* entradas;
} TablaAlias;

static EntradaTablaAlias* ta_encontrar(TablaAlias* tabla, char const* alias, int alias_n) {
	for (EntradaTablaAlias* it = tabla->entradas; it; it = it->sig)
		if (it->alias_n == alias_n && memcmp(it->alias, alias, alias_n) == 0)
			return it;
	return NULL;
}

// limpia 'input' y 'expresion'
static EntradaTablaAlias* ta_insertar(TablaAlias* tabla, char* input, char const* alias, int alias_n, Expresion* expresion) {
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
static EntradaTablaAlias* ta_insertar_o_reemplazar(TablaAlias* tabla, char* input, char const* alias, int alias_n, Expresion* expresion) {
	EntradaTablaAlias* encontrado = ta_encontrar(tabla, alias, alias_n);

	if (encontrado == NULL)
		return ta_insertar(tabla, input, alias, alias_n, expresion);

	free(encontrado->input);
	encontrado->input = input;
	encontrado->alias = alias;

	expresion_limpiar(encontrado->expresion);
	encontrado->expresion = expresion;

	return encontrado;
}

static void ta_limpiar(TablaAlias* tabla) {
	EntradaTablaAlias* it = tabla->entradas;
	while (it) {
		EntradaTablaAlias* sig = it->sig;
		expresion_limpiar(it->expresion);
		free(it->input);
		free(it);
		it = sig;
	}
}



typedef struct {
	TablaAlias aliases;
	char* buffer_input;
	int tamano_buffer_input;
} Entorno;

static Entorno entorno_crear() {
	return (Entorno){};
}

static void leer_input(Entorno* entorno) {
	// NICETOHAVE que se banque renglones arbitrariamente grandes, usando un
	// buffer que crece dinamicamente si hace falta
	if (entorno->buffer_input == NULL) {
		entorno->buffer_input = malloc(BUFFER);
		entorno->tamano_buffer_input = BUFFER;
	}
	// NICETOHAVE: esto no se porta muy bien si se pasa de input: deja todo lo q
	// sobra en el buffer de entrada
	fgets(entorno->buffer_input, BUFFER - 1, stdin);
}

static void descartar_input(Entorno* entorno) {
	assert(entorno != NULL);
	free(entorno->buffer_input);
	entorno->buffer_input = NULL;
	entorno->tamano_buffer_input = 0;
}

static char* robar_input(Entorno* entorno) {
	char* buffer = entorno->buffer_input;
	entorno->buffer_input = NULL;
	entorno->tamano_buffer_input = 0;
	return buffer;
}

static void entorno_limpiar_datos(Entorno* entorno) {
	if (entorno->buffer_input != NULL)
		descartar_input(entorno);
	ta_limpiar(&entorno->aliases);
	return;
}

static int chequear_expresion(Expresion* expresion, Entorno* entorno);

static int chequear_alias(Entorno* entorno, char const* alias, int alias_n) {
	EntradaTablaAlias* entradaAlias = ta_encontrar(&entorno->aliases, alias, alias_n);
	if (entradaAlias)
		return chequear_expresion(entradaAlias->expresion, entorno);
	else {
		// avisamos al usuario y devolvemos false.
		printf("El alias \'%.*s\' no esta definido.\n", alias_n, alias);
		return 0;
	}
}

static int chequear_expresion(Expresion* expresion, Entorno* entorno) {
	int es_valida;
	switch (expresion->tag) {
	case X_OPERACION:
		if (expresion->op->aridad == 1)
			es_valida = chequear_expresion(expresion->sub[0], entorno);
		else
			es_valida = chequear_expresion(expresion->sub[1], entorno) &&
				chequear_expresion(expresion->sub[0], entorno);
		break;
	case X_NUMERO:
		es_valida =  1;
    break;
	case X_ALIAS:
		es_valida = chequear_alias(entorno, expresion->alias, expresion->valor);
		break;
	}
	return es_valida;
}

// TODO: testear evaluar_alias (y evaluar_arbol).
static int evaluar_arbol(Expresion* expresion, Entorno* entorno);

// encuentra la expresion correspondiente y llama a evaluar_arbol
static int evaluar_alias(Entorno* entorno, char const* alias, int alias_n) {
	EntradaTablaAlias* entradaAlias = ta_encontrar(&entorno->aliases, alias, alias_n);
	Expresion* expresion = entradaAlias->expresion;
	return evaluar_arbol(expresion, entorno);
}

// funcion auxliar a evaluar_alias; evalua el arbol de expresiones
// necesitamos el entorno en caso de encontrar un alias que evaluar									
static int evaluar_arbol(Expresion* expresion, Entorno* entorno) {
	if (!expresion) return 0;
  switch (expresion->tag) {
	case X_OPERACION: {
		// evaluamos los subarboles recursivamente y los usamos como args
		int args[2] =
			{evaluar_arbol(expresion->sub[0], entorno),
			evaluar_arbol(expresion->sub[1], entorno)};
		return expresion->op->eval(args);
	} break;
	case X_NUMERO:
		return expresion->valor;
		break;
	case X_ALIAS:
		return evaluar_alias(entorno, expresion->alias, expresion->valor);
		break;
	}
} 

static void imprimir_expresion(Expresion* expresion, int precedencia, int izquierda, Entorno* entorno) {
	switch (expresion->tag) {
	case X_OPERACION: 
		if (expresion->op->aridad == 1) {
			if (!izquierda) printf("(");
			printf("%s", expresion->op->simbolo);
			imprimir_expresion(expresion->sub[0], expresion->op->precedencia, 0, entorno);
			if (!izquierda) printf(")");
		}
		else {
			if (expresion->op->precedencia < precedencia) {
				printf("(");
				izquierda = 1;
			}
			imprimir_expresion(expresion->sub[1], expresion->op->precedencia, izquierda, entorno);
			printf(" %s ", expresion->op->simbolo);
			imprimir_expresion(expresion->sub[0], expresion->op->precedencia, 0, entorno);
			if (expresion->op->precedencia < precedencia) printf(")");
		}
		break;
	case X_NUMERO:
		printf("%d", expresion->valor);
		break;
	case X_ALIAS: {
		EntradaTablaAlias* entradaAlias = 
			ta_encontrar(&entorno->aliases, expresion->alias, expresion->valor);
		if (entradaAlias) {
			expresion = entradaAlias->expresion;
			// imprimo la expresion asociada al alias
			imprimir_expresion(expresion, precedencia, izquierda, entorno);		
		}
		else {
			if (!izquierda) printf(" ");
			printf("%.*s", expresion->valor, expresion->alias);
		}
	}	break;
	}
}

static void imprimir(Entorno* entorno, char const* alias, int alias_n) {
	EntradaTablaAlias* entradaAlias = ta_encontrar(&entorno->aliases, alias, alias_n);
	if (entradaAlias) {
		Expresion* expresion = entradaAlias->expresion; 
		int precedencia = 0;
		if (expresion->tag == X_OPERACION) precedencia = expresion->op->precedencia;
		imprimir_expresion(expresion, precedencia, 1, entorno);		
	}
	// si el alias no esta definido, imprimimos su nombre.
	else printf("%.*s ", alias_n, alias);
	puts("");
}

// limpia 'input' y 'expresion'
static void cargar(Entorno* entorno, char* input, char const* alias, int alias_n, Expresion* expresion) {
	ta_insertar_o_reemplazar(&entorno->aliases, input, alias, alias_n, expresion);
}

static void manejar_error(ErrorTag error) {
	printf("ERROR: ");
	switch (error) {
		case E_ALIAS: 
			puts("debe especificarse un alias valido.");
			break;
		case E_CARGA: 
			puts("error en la sintaxis de carga.");
			break;
		case E_EXPRESION:
			puts("expresion invalida.");
			break;
		case E_OPERACION:
			puts("no se reconocio niguna operacion valida. " 
				"Ingrese \'salir\' para terminar el programa.");
			break;
		case E_VACIA:
			puts("no se permite una expresion vacia.");
			break;
		default:
			fflush(stdout); assert(0);
	}
}

void interpretar(TablaOps* tabla_ops) {
	Entorno entorno = entorno_crear();
	while (1) {
		printf("> ");
		leer_input(&entorno);
		Parseado parseado = parsear(entorno.buffer_input, tabla_ops);
		Sentencia sentencia = parseado.sentencia;

		switch (sentencia.tag) {
		case S_CARGA:
			cargar(&entorno, robar_input(&entorno), sentencia.alias, sentencia.alias_n, sentencia.expresion);
			break;
		case S_IMPRIMIR:
			imprimir(&entorno, sentencia.alias, sentencia.alias_n);
			break;
		case S_EVALUAR:
			if (chequear_alias(&entorno, sentencia.alias, sentencia.alias_n)) {
				int resultado = evaluar_alias(&entorno, sentencia.alias, sentencia.alias_n);
				printf("%d\n", resultado);
			}
			break;
		case S_INVALIDO:
			manejar_error(sentencia.error);
			break;
		case S_SALIR:
			entorno_limpiar_datos(&entorno);
			return;
		}
	}
}