#include <stdio.h>
#include <stdlib.h>

void buscar(void) {}
void agregar(void) {}
void eliminar(void) {}
void editar(void) {}
void cargar(void) {}
void guardar(void) {}
void deshacer(void) {}
void rehacer(void) {}
void conjuncion(void) {}
void disjuncion(void) {}
void guardar_ordenado(void) {}
void buscar_por_suma_de_edades(void) {}

void salir(void) {
	exit(0);
}

typedef void (*AccionDelPrograma)(void);

struct _EntradaMenu {
	char const* nombre;
	AccionDelPrograma accion;
};
typedef struct _EntradaMenu EntradaMenu;

static int obtener_seleccion_del_usuario(int opcion_maxima) {
		int seleccionada;
		int resultado_scanf;

		printf(">");
		resultado_scanf = scanf("%d", &seleccionada);
		while (!resultado_scanf || seleccionada <= 0 || seleccionada > opcion_maxima) {
			printf("Accion invalida, seleccione una accion valida:\n");
			printf(">");
			resultado_scanf = scanf("%d", &seleccionada);
		}

		return seleccionada;
}

void menu() {
#define LONGITUD_MENU 13
	EntradaMenu entradas[LONGITUD_MENU] = {
		{"Buscar", buscar},
		{"Agregar", agregar},
		{"Eliminar", eliminar},
		{"Editar", editar},
		{"Cargar", cargar},
		{"Guardar", guardar},
		{"Deshacer", deshacer},
		{"Rehacer", rehacer},
		{"AND", conjuncion},
		{"OR", disjuncion},
		{"Guardar ordenado", guardar_ordenado},
		{"Buscar por suma de edades", buscar_por_suma_de_edades},
		{"Salir", salir},
	};

	while (1) {
		printf("Menu de acciones:\n");
		for (int i = 0; i < LONGITUD_MENU; ++i) {
			printf("%d. %s\n", i+1, entradas[i].nombre);
		}

		printf("\n");
		printf("Seleccione una accion:\n");

		int seleccionada = obtener_seleccion_del_usuario(LONGITUD_MENU);
		EntradaMenu entrada = entradas[seleccionada - 1];
		entrada.accion();
	}
}

int main() {
	menu();
}
