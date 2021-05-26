#include "tabla_ops.h"

#include <stddef.h>
#include <stdlib.h>

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


void cargar_operador(TablaOps* tabla, char const* simbolo, int aridad, FuncionEvaluacion eval) {
	// NICETOHAVE: verificar que no exista el mismo simbolo
	// NICETOHAVE: validar aridad
	// NICETOHAVE: chequear nulls

	EntradaTablaOps* nueva_entrada = malloc(sizeof(*nueva_entrada));
	*nueva_entrada = (EntradaTablaOps){
		.sig = tabla->entradas,
		.eval = eval,
		.simbolo = simbolo,
		.aridad = aridad,
	};

	tabla->entradas = nueva_entrada;

	return;
}
