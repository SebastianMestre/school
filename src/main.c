#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "database.h"
#include "io.h"
#include "storage.h"
#include "string.h"

#define BUF_SIZE 128

void buscar(Database* database) {
	char buf0[BUF_SIZE];
	char buf1[BUF_SIZE];

	puts("Ingrese nombre");
	printf(">");
	assert(get_line(buf0, BUF_SIZE, stdin));

	puts("Ingrese apellido");
	printf(">");
	assert(get_line(buf1, BUF_SIZE, stdin));

	// TODO: validate strings
	// TODO: trim strings
	char* name = string_dup(buf0);
	char* surname = string_dup(buf1);

	OptionalContactId contact = database_find(database, name, surname);
	if (!contact.active) {
		puts("No existe un contacto con ese nombre y apellido");
	} else {
		printf("Id de contacto: %lu (TODO: implement print)\n", contact.id);
	}
}

void agregar(Database* database) {
	char buf0[BUF_SIZE];
	char buf1[BUF_SIZE];
	char buf2[BUF_SIZE];
	char buf3[BUF_SIZE];

	puts("Ingrese nombre");
	printf(">");
	assert(get_line(buf0, BUF_SIZE, stdin));

	puts("Ingrese apellido");
	printf(">");
	assert(get_line(buf1, BUF_SIZE, stdin));

	puts("Ingrese edad");
	printf(">");
	unsigned age;
	assert(get_line_as_uint(buf2, BUF_SIZE, &age, stdin));

	puts("Ingrese telefono");
	printf(">");
	assert(get_line(buf3, BUF_SIZE, stdin));

	// TODO: validate strings
	// TODO: trim strings
	char* name = string_dup(buf0);
	char* surname = string_dup(buf1);
	char* phone_number = string_dup(buf3);

	bool success = database_insert(database, name, surname, age, phone_number);
	if (!success) {
		puts("Ya existe un contacto con ese nombre y apellido");
	}
}

void eliminar(Database* database) {
	char buf0[BUF_SIZE];
	char buf1[BUF_SIZE];

	puts("Ingrese nombre");
	assert(get_line(buf0, BUF_SIZE, stdin));

	puts("Ingrese apellido");
	assert(get_line(buf1, BUF_SIZE, stdin));

	// TODO: validate strings
	// TODO: trim strings
	char* name = string_dup(buf0);
	char* surname = string_dup(buf1);

	bool success = database_delete(database, name, surname);
	if (!success) {
		puts("No existe un contacto con ese nombre y apellido");
	}
}

void editar(Database* database) {}

void cargar(Database* database) {}
void guardar(Database* database) {}

void deshacer(Database* database) {
	bool success = database_rewind(database);
	if (success) {
		puts("Se deshizo la ultima operacion");
	} else {
		puts("No hay operaciones para deshacer");
	}
}

void rehacer(Database* database) {
	bool success = database_advance(database);
	if (success) {
		puts("Se rehizo la ultima operacion");
	} else {
		puts("No hay operaciones para rehacer");
	}
}

void conjuncion(Database* database) {
}

void disjuncion(Database* database) {
}

void guardar_ordenado(Database* database) {
}

void buscar_por_suma_de_edades(Database* database) {
}

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

	printf("Menu de acciones:\n");
	for (int i = 0; i < MENU_LEN; ++i) {
		printf("%d. %s\n", i+1, entries[i].name);
	}

	while (1) {
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
