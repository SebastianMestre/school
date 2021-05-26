#ifndef TABLA_OPS_H
#define TABLA_OPS_H

#include "funcion_evaluacion.h"

typedef struct {} TablaOps;

void cargar_operador(TablaOps* tabla, char* simbolo, int aridad, FuncionEvaluacion eval);

#endif // TABLA_OPS_H
