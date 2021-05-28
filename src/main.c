#include "tabla_ops.h"
#include "operadores.h"
#include "interprete/interpretar.h"

int main (int argc, char** argv) {
	TablaOps tabla = tabla_ops_crear();
	
	cargar_operador(&tabla, "+", 2, suma, 0);
	cargar_operador(&tabla, "-", 2, resta, 0);
	cargar_operador(&tabla, "--", 1, opuesto, 1);
	cargar_operador(&tabla, "*", 2, producto, 2);
	cargar_operador(&tabla, "/", 2, division, 3);
	cargar_operador(&tabla, "%", 2, modulo, 2);
	cargar_operador(&tabla, "^", 2, potencia, 4);

	interpretar(&tabla);

	tabla_ops_limpiar(&tabla);
}
