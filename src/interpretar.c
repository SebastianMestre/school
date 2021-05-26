#include "interpretar.h"

#include "expresion.h"
#include "parser.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



// Explicacion:
// para simplificar el uso de memoria, en vez de guardar los aliases, cada uno
// en su propia region de memoria, referenciamos su posicion original en la
// linea que ingreso el usuario.
// Esta linea se guarda en su entrada correspondiente en la tabla de aliases
// especificamente, el puntero al buffer de entrada va en el campo 'input' de
// EntradaTablaAlias

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
		case S_CARGA:
			cargar(&entorno, robar_input(&entorno), parseado.alias, parseado.alias_n, parseado.expresion);
			break;
		case S_IMPRIMIR:
			imprimir(&entorno, parseado.alias, parseado.alias_n);
			break;
		case S_EVALUAR: {
			int resultado = evaluar(&entorno, parseado.alias, parseado.alias_n);
			printf("%d\n", resultado);
			} break;
		case S_INVALIDO:
			puts("Error, saliendo."); // NICETOHAVE mejor error
		// fallthrough
		case S_SALIR:
			entorno_limpiar(&entorno);
			return;
		}
	}
}
