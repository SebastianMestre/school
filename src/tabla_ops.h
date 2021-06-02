#ifndef TABLA_OPS_H
#define TABLA_OPS_H

#include "funcion_evaluacion.h"

typedef struct EntradaTablaOps EntradaTablaOps;
struct EntradaTablaOps {
	EntradaTablaOps* sig;
	FuncionEvaluacion eval;
	char const* simbolo;
	int aridad;
	int precedencia;
};

typedef struct TablaOps {
	EntradaTablaOps* entradas;
} TablaOps;

/**
 * Devuelve una tabla de operaciones vacia.
 */ 
TablaOps tabla_ops_crear();

/**
 * Libera el espacio de memoria ocupado por la tabla.
 */
void tabla_ops_limpiar(TablaOps* tabla);

/**
 * Inserta un operador nuevo en la tabla.
 */
void cargar_operador(TablaOps* tabla, char const* simbolo, int aridad, 
	FuncionEvaluacion eval, int precedencia);

#endif // TABLA_OPS_H
