#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "database.h"
#include "io.h"
#include "storage.h"
#include "string.h"

#define BUF_SIZE 128

void buscar(Database* database) {}

void agregar(Database* database) {
	char buf0[BUF_SIZE];
	char buf1[BUF_SIZE];
	char buf2[BUF_SIZE];
	char buf3[BUF_SIZE];

	puts("Ingrese nombre");
	assert(get_line(buf0, BUF_SIZE, stdin));

	puts("Ingrese apellido");
	assert(get_line(buf1, BUF_SIZE, stdin));

	puts("Ingrese edad");
	unsigned age;
	assert(get_line_as_uint(buf2, BUF_SIZE, &age, stdin));

	puts("Ingrese telefono");
	assert(get_line(buf3, BUF_SIZE, stdin));

	// TODO: validate string
	// TODO: trim strings
	char* name = string_dup(buf0);
	char* surname = string_dup(buf1);
	char* phone_number = string_dup(buf3);

	database_insert(database, name, surname, age, phone_number);
}

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
		char buf[BUF_SIZE];
		int selected;

		printf(">");
		while (!get_line_as_int(buf, BUF_SIZE, &selected, stdin) || selected <= 0 || selected > max_value) {
			assert(!feof(stdin));
			printf("Accion invalida, seleccione una accion valida:\n");
			printf(">");
		}

		return selected;
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

		if (selected == MENU_LEN) {
			break;
		}

		MenuEntry entry = entries[selected - 1];
		entry.action(&database);
	}

	database_release(&database);
	storage_release(&storage);

	return 0;
}
