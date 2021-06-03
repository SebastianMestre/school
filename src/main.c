#include "tabla_ops.h"
#include "operadores.h"
#include "interprete/interpretar.h"

int main () {
	// Creamos una tabla de operadores.
	TablaOps tabla = tabla_ops_crear();
	
	// Cargamos los operadores en la tabla.
	cargar_operador(&tabla, "+", 2, suma, 0);
	cargar_operador(&tabla, "-", 2, resta, 1);
	cargar_operador(&tabla, "--", 1, opuesto, 2);
	cargar_operador(&tabla, "*", 2, producto, 3);
	cargar_operador(&tabla, "%", 2, modulo, 4);
	cargar_operador(&tabla, "/", 2, division, 5);
	cargar_operador(&tabla, "^", 2, potencia, 6);

	// Iniciamos la sesion interactiva.
	interpretar(&tabla);

	// Limpiamos la tabla de operaciones.
	tabla_ops_limpiar(&tabla);
}
