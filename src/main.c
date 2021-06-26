#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "database.h"
#include "storage.h"

void buscar(Database* database) {}
void agregar(Database* database) {}
void eliminar(Database* database) {}
void editar(Database* database) {}
void cargar(Database* database) {}
void guardar(Database* database) {}
void deshacer(Database* database) {}
void rehacer(Database* database) {}
void conjuncion(Database* database) {}
void disjuncion(Database* database) {}
void guardar_ordenado(Database* database) {}
void buscar_por_suma_de_edades(Database* database) {}
void exit_program(Database* database) { exit(0); } // TODO: cleanup database and storage

typedef void (*ProgramAction)(Database*);

struct _MenuEntry {
	char const* name;
	ProgramAction action;
};
typedef struct _MenuEntry MenuEntry;

static int
action_id_prompt(int max_value) {
		int selected;
		int scanf_result;

		printf(">");
		scanf_result = scanf("%d", &selected);
		while (scanf_result != 1 || selected <= 0 || selected > max_value) {
			assert(scanf_result != EOF);
			if (scanf_result == 0) {
				while (fgetc(stdin) != '\n');
			}

			printf("Accion invalida, seleccione una accion valida:\n");
			printf(">");
			scanf_result = scanf("%d", &selected);
		}

		return selected;
}

void menu() {
}

int main() {

	Storage storage = storage_create();
	Database database = database_create(&storage);

#define MENU_LEN 13
	MenuEntry entries[MENU_LEN] = {
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
		{"Salir", exit_program},
	};

	while (1) {
		printf("Menu de acciones:\n");
		for (int i = 0; i < MENU_LEN; ++i) {
			printf("%d. %s\n", i+1, entries[i].name);
		}

		printf("\n");
		printf("Seleccione una accion:\n");

		int selected = action_id_prompt(MENU_LEN);
		MenuEntry entry = entries[selected - 1];
		entry.action(&database);
	}
}
