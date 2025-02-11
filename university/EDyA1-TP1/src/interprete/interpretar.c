#include "interpretar.h"

#include "expresion.h"
#include "parser.h"
#include "error.h"

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

// Almacena los datos de un alias definido por el usuario.
// Tiene una referencia al siguiente alias en la tabla.
typedef struct EntradaTablaAlias EntradaTablaAlias;
struct EntradaTablaAlias {
	EntradaTablaAlias* sig;
	char* input;
	char const* alias;
	int alias_n;
	Expresion* expresion;
};

// Almacena la lista de alias definidos por el usuario.
typedef struct {
	EntradaTablaAlias* entradas;
} TablaAlias;

// Busca un alias en la tabla de alias. De no encontrarlo devuelve NULL.
static EntradaTablaAlias* ta_encontrar(TablaAlias* tabla, char const* alias, 
	int alias_n) {
	for (EntradaTablaAlias* it = tabla->entradas; it; it = it->sig)
		if (it->alias_n == alias_n && memcmp(it->alias, alias, alias_n) == 0)
			return it;
	return NULL;
}

// Inserta un alias nuevo en la tabla de alias.
static EntradaTablaAlias* ta_insertar(
	TablaAlias* tabla, 
	char* input, 
	char const* alias, 
	int alias_n, 
	Expresion* expresion) {
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

// Busca el alias en la tabla de alias. En caso de encontrarlo, lo reemplaza y
// limpia el input y la expresion anteriores.
// En caso de no existir aun, llama a 'ta_insertar'.
static EntradaTablaAlias* ta_insertar_o_reemplazar(
	TablaAlias* tabla, 
	char* input, 
	char const* alias, 
	int alias_n, 
	Expresion* expresion) {
	EntradaTablaAlias* encontrado = ta_encontrar(tabla, alias, alias_n);

	// Si no lo encontramos simplemente insertamos.
	if (encontrado == NULL)
		return ta_insertar(tabla, input, alias, alias_n, expresion);
	// Si ya existe, borramos los datos anteriores y lo reemplazamos.
	free(encontrado->input);
	encontrado->input = input;
	encontrado->alias = alias;

	expresion_limpiar(encontrado->expresion);
	encontrado->expresion = expresion;

	return encontrado;
}

// Libera el espacio de memoria utilizado por la tabla de alias.
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


// Estructura que representa el estado de la sesion con el usuario.
// Guarda una tabla con los alias definidos y el buffer del input.   
typedef struct {
	TablaAlias aliases;
	char* bufferInput;
	int tamanoBufferInput;
} Entorno;

// Devuelve un entorno vacio.
static Entorno entorno_crear() {
	return (Entorno){};
}

// Lee por stdin y almacena en el buffer.
static void leer_input(Entorno* entorno) {
	if (entorno->bufferInput == NULL) {
		entorno->bufferInput = malloc(BUFFER);
		assert(entorno->bufferInput);
		entorno->tamanoBufferInput = BUFFER;
	}
	char c;
	size_t size = BUFFER - 1, i = 0;
	while ((c = getchar()) != '\n') {
		if (size == i) {
			entorno->bufferInput = realloc(entorno->bufferInput, size + BUFFER);
			assert(entorno->bufferInput);
			size += BUFFER;
		}
		entorno->bufferInput[i++] = c;
	}
	entorno->bufferInput[i] = '\0';
}

// Libera el buffer.
static void descartar_input(Entorno* entorno) {
	assert(entorno != NULL);
	free(entorno->bufferInput);
	entorno->bufferInput = NULL;
	entorno->tamanoBufferInput = 0;
}

// Se roba el buffer del entorno y lo deja nulo.
static char* robar_input(Entorno* entorno) {
	char* buffer = entorno->bufferInput;
	entorno->bufferInput = NULL;
	entorno->tamanoBufferInput = 0;
	return buffer;
}

// Libera el espacio de memoria ocupado por el entorno.
static void entorno_limpiar_datos(Entorno* entorno) {
	if (entorno->bufferInput != NULL)
		descartar_input(entorno);
	ta_limpiar(&entorno->aliases);
	return;
}


// Maneja e imprime en pantalla el error dado.
// En caso de precisarlo, toma valores (y su respectivos largos) para imprimir 
// el mensaje de error. 
static void manejar_error(ErrorTag error, const char** val, int* val_n) {
	printf("ERROR: ");
	switch (error) {
		case E_PARSER_ALIAS: 
			puts("debe especificarse un alias valido.");
			break;
		case E_PARSER_CARGA: 
			puts("error en la sintaxis de carga.");
			break;
		case E_PARSER_EXPRESION:
			puts("expresion invalida.");
			break;
		case E_PARSER_OPERACION:
			puts("no se reconocio niguna operacion valida.\n" 
				"Ingrese \'salir\' para terminar el programa.");
			break;
		case E_PARSER_VACIA:
			puts("no se permite una expresion vacia.");
			break;
		case E_PARSER_OPERADOR:
			printf("\'%s\' es un operador y no puede utilizarse como alias.\n", val[0]);
			break;
		case E_INTERPRETE_ALIAS:
			printf("El alias \'%.*s\' no esta definido.\n", val_n[0], val[0]);
			break;
		default:
			fflush(stdout); assert(0);
	}
}


// Chequea que la expresion no tenga alias no definidos. 
static int chequear_expresion(Expresion* expresion, Entorno* entorno);

// Chequea que el alias exista, y que su expresion asociada no tenga alias no
// definidos. En caso de no ser valido, el alias no podra evaluarse.
static int chequear_alias(Entorno* entorno, char const* alias, int alias_n) {
	// Buscamos el alias.
	EntradaTablaAlias* entradaAlias = 
		ta_encontrar(&entorno->aliases, alias, alias_n);
	if (entradaAlias)
		return chequear_expresion(entradaAlias->expresion, entorno);
	// No lo encontramos:
	else {
		// Manejamos el error correspondiente.
		manejar_error(E_INTERPRETE_ALIAS, &alias, &alias_n);
		return 0;
	}
}

// 'chequear_expresion' y 'chequear_alias' son mutuamente dependientes.
static int chequear_expresion(Expresion* expresion, Entorno* entorno) {
	int esValida;
	switch (expresion->tag) {
	case X_OPERACION:
		// Si la operacion es unaria solo chequeamos un argumento.
		if (expresion->op->aridad == 1)
			esValida = chequear_expresion(expresion->sub[0], entorno);
		else
			esValida = chequear_expresion(expresion->sub[1], entorno) &&
				chequear_expresion(expresion->sub[0], entorno);
		break;
	case X_NUMERO:
		esValida =  1;
    break;
	case X_ALIAS:
		// Llamamos a 'chequear_alias'
		esValida = chequear_alias(entorno, expresion->alias, expresion->valor);
		break;
	}
	return esValida;
}


// Evalua un arbol de expresion.
static int evaluar_arbol(Expresion* expresion, Entorno* entorno);

// Encuentra la expresion asociada al alias y llama a evaluar_arbol.
static int evaluar_alias(Entorno* entorno, char const* alias, int alias_n) {
	// Buscamos el alias.
	EntradaTablaAlias* entradaAlias = 
		ta_encontrar(&entorno->aliases, alias, alias_n);
	Expresion* expresion = entradaAlias->expresion;
	// Evaluamos la expresion asociada.
	return evaluar_arbol(expresion, entorno);
}

// 'evaluar_arbol' y 'evaluar_alias' son mutuamente dependientes.								
static int evaluar_arbol(Expresion* expresion, Entorno* entorno) {
	// Si no hay expresion, devolvemos 0. Este valor NO DEBE SER UTILIZADO.
	if (!expresion) return 0;
  switch (expresion->tag) {
	case X_OPERACION: {
		// Evaluamos los subarboles recursivamente y los usamos como argumentos.
		int args[2] =
			{evaluar_arbol(expresion->sub[0], entorno),
			evaluar_arbol(expresion->sub[1], entorno)};
		return expresion->op->eval(args);
	} break;
	case X_NUMERO:
		return expresion->valor;
		break;
	case X_ALIAS:
		// Llamamos a 'evaluar_alias'.
		return evaluar_alias(entorno, expresion->alias, expresion->valor);
		break;
	}
	// No deberiamos llegar hasta aca.
	assert(0);
} 

// Imprime una expresion en pantalla de forma infija.
// En caso de la expresion contener un alias no definido, imprime el nombre del 
// alias.
// LLamamos a la funcion con: 
// la precedencia de la expresion padre, para determinar si necesitamos usar 
// parentesis;
// un valor que determine si nos encontramos a la izquierda de la operacion. 
static void imprimir_expresion(Expresion* expresion, int precedencia, 
	int izquierda, Entorno* entorno) {
	switch (expresion->tag) {
	case X_OPERACION: {
		int precedenciaOp = expresion->op->precedencia;
		// Manejamos operaciones unarias.
		if (expresion->op->aridad == 1) {
			// Si no estamos a la izquierda de un termino usamos parentesis.
			if (!izquierda) printf("(");
			printf("%s", expresion->op->simbolo);
			imprimir_expresion(expresion->sub[0], precedenciaOp, 0, entorno);
			if (!izquierda) printf(")");
		}
		else {
			// Si tenemos menor precedencia usamos parentesis.
			if (precedenciaOp < precedencia) {
				printf("(");
				// Comenzamos un termino nuevo, por lo tanto estamos a la izquierda.
				izquierda = 1;
			}
			imprimir_expresion(expresion->sub[1], precedenciaOp, izquierda, entorno);
			printf(" %s ", expresion->op->simbolo);
			imprimir_expresion(expresion->sub[0], precedenciaOp, 0, entorno);
			if (expresion->op->precedencia < precedencia) printf(")");
		}
	}	break;
	case X_NUMERO:
		// Imprimimos el numero.
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
		// Si no lo reconocemos, imprimimos el nombre del alias.
		else {
			if (!izquierda) printf(" ");
			printf("%.*s", expresion->valor, expresion->alias);
		}
	}	break;
	}
}

// Imprime en pantalla la expresion asociada al alias.
// Busca la expresion asociada y llama a 'imprimir_expresion'.
// En caso de que esta no exista, imprime el nombre del alias.
static void imprimir(Entorno* entorno, char const* alias, int alias_n) {
	EntradaTablaAlias* entradaAlias = 
		ta_encontrar(&entorno->aliases, alias, alias_n);
	if (entradaAlias) {
		Expresion* expresion = entradaAlias->expresion; 
		int precedencia = 0;
		if (expresion->tag == X_OPERACION) precedencia = expresion->op->precedencia;
		imprimir_expresion(expresion, precedencia, 1, entorno);		
		puts("");
	}
	// Si el alias no esta definido, elevamos error.
	else manejar_error(E_INTERPRETE_ALIAS, &alias, &alias_n);
}

// Carga el alias en la tabla de alias. Si ya esta definido, lo reemplaza.
static void cargar(Entorno* entorno, char* input, char const* alias, int alias_n, 
	Expresion* expresion) {
	ta_insertar_o_reemplazar(&entorno->aliases, input, alias, alias_n, expresion);
}

// Parsea el input y procede de acuerdo al tipo de sentencia ingresada.
void interpretar(TablaOps* tablaOps) {
	Entorno entorno = entorno_crear(); // creamos el entorno de la sesion.
	// Solo nos detenemos cuando el usuario ingrese la palabra clave 'salir'.
	while (1) {
		printf("> "); // inicio de linea
		leer_input(&entorno); // leemos el input
		Parseado parseado = parsear(entorno.bufferInput, tablaOps); // parseamos
		Sentencia sentencia = parseado.sentencia; // obtenemos la sentencia

		// Determinamos el tipo de sentencia.
		switch (sentencia.tag) {
		case S_CARGA:
			// Cargamos el alias.
			cargar(&entorno, robar_input(&entorno),
				sentencia.alias, sentencia.alias_n, sentencia.expresion);
			break;
		case S_IMPRIMIR:
			// Imprimimos el alias.
			imprimir(&entorno, sentencia.alias, sentencia.alias_n);
			break;
		case S_EVALUAR:
			// Si es valido, evaluamos el alias e imprimimos el resultado.
			if (chequear_alias(&entorno, sentencia.alias, sentencia.alias_n)) {
				int resultado = 
					evaluar_alias(&entorno, sentencia.alias, sentencia.alias_n);
				printf("%d\n", resultado);
			}
			break;
		case S_INVALIDO:
			// Manejamos el error.
			manejar_error(parseado.error, &parseado.resto, NULL);
			break;
		case S_SALIR:
			// Limpiamos el entorno y terminamos el programa.
			entorno_limpiar_datos(&entorno);
			return;
		}
	}
}