#include "tabla_ops.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

TablaOps tabla_ops_crear() {
	return (TablaOps){ NULL };
}

void tabla_ops_limpiar(TablaOps* tabla) {
	EntradaTablaOps* it = tabla->entradas;
	while (it) {
		EntradaTablaOps* sig = it->sig;
		free(it);
		it = sig;
	}
}

static int existe_simbolo(TablaOps* tabla, char const* simbolo) {
	for (EntradaTablaOps* it = tabla->entradas; it; it = it->sig)
		if (strcmp(simbolo, it->simbolo) == 0) return 1;
	return 0;
}

void cargar_operador(TablaOps* tabla, char const* simbolo, int aridad, 
	FuncionEvaluacion eval, int precedencia) {
	// NICETOHAVE: chequear nulls

	// Chequeamos que la nueva operacion sea valida.
	if (aridad != 1 && aridad != 2) {
		puts("ERROR: las operaciones deben tener aridad 1 o 2.");
		printf("En operacion \'%s\'; ", simbolo);
		fflush(stdout); assert(0);
	}
	if (existe_simbolo(tabla, simbolo)) {
		printf("ERROR: la operacion \'%s\' ya esta definida.\n", simbolo);
		fflush(stdout); assert(0);
	}

	EntradaTablaOps* nuevaEntrada = malloc(sizeof(*nuevaEntrada));
	*nuevaEntrada = (EntradaTablaOps){
		.sig = tabla->entradas,
		.eval = eval,
		.simbolo = simbolo,
		.aridad = aridad,
		.precedencia = precedencia,
	};

	tabla->entradas = nuevaEntrada;

	return;
}
