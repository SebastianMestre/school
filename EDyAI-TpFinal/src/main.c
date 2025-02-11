#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"
#include "io.h"
#include "quicksort.h"
#include "search_by_sum.h"
#include "serialization.h"
#include "storage.h"
#include "string.h"

#define BUF_SIZE 256

static void
read_a_line_with_retry_message(char* buf, size_t n) {
	get_line_retry(buf, n, "Linea demasiado larga. Vuelva a intentar:\n>", stdin);
}

static uint32_t
read_u32_with_retry_message(char* buf, size_t n) {
	uint32_t value;
	get_line_as_u32_retry(buf, n, &value, "Linea demasiado larga, vuelva a intentar\n>", "Ingrese un numero valido\n>", stdin);
	return value;
}

static IncompleteContact
read_query_parameters() {
	char buf0[BUF_SIZE];
	char buf1[BUF_SIZE];
	char buf2[BUF_SIZE];
	char buf3[BUF_SIZE];

	printf("Ingrese nombre:\n>");
	read_a_line_with_retry_message(buf0, BUF_SIZE);

	printf("Ingrese apellido:\n>");
	read_a_line_with_retry_message(buf1, BUF_SIZE);

	printf("Ingrese edad:\n>");
	bool has_age = false;
	uint32_t age = 0;
	while (1) {
		read_a_line_with_retry_message(buf2, BUF_SIZE);
		string_trim(buf2);
		if (strlen(buf2) == 0) {
			break;
		}
		bool ok = parse_u32(buf2, &age);
		if (!ok) {
			printf("Edad invalida. Ingrese una edad valida:\n>");
			continue;
		}
		has_age = true;
		break;
	}

	printf("Ingrese telefono:\n>");
	read_a_line_with_retry_message(buf3, BUF_SIZE);

	string_trim(buf0);
	string_trim(buf1);
	string_trim(buf3);

	char* name = strlen(buf0) == 0 ? nullptr : string_dup(buf0);
	char* surname = strlen(buf1) == 0 ? nullptr : string_dup(buf1);
	char* phone_number = strlen(buf3) == 0 ? nullptr : string_dup(buf3);

	return (IncompleteContact) {
		.name = name,
		.surname = surname,
		.has_age = has_age,
		.age = age,
		.phone_number = phone_number,
	};
}

static int
by_name_cmp(void const* arg0, void const* arg1, void* metadata) {
	ContactId lhs = *(ContactId const*)arg0;
	ContactId rhs = *(ContactId const*)arg1;
	Storage* storage = metadata;
	return strcmp(
		storage_get_name(storage, lhs),
		storage_get_name(storage, rhs));
}

static int
by_surname_cmp(void const* arg0, void const* arg1, void* metadata) {
	ContactId lhs = *(ContactId const*)arg0;
	ContactId rhs = *(ContactId const*)arg1;
	Storage* storage = metadata;
	return strcmp(
		storage_get_surname(storage, lhs),
		storage_get_surname(storage, rhs));
}

static int
by_age_cmp(void const* arg0, void const* arg1, void* metadata) {
	ContactId lhs = *(ContactId const*)arg0;
	ContactId rhs = *(ContactId const*)arg1;
	Storage* storage = metadata;

	uint32_t lhs_age = storage_get_age(storage, lhs);
	uint32_t rhs_age = storage_get_age(storage, rhs);

	if (lhs_age < rhs_age)
		return -1;
	return rhs_age < lhs_age;
}

static int
by_phone_number_cmp(void const* arg0, void const* arg1, void* metadata) {
	ContactId lhs = *(ContactId const*)arg0;
	ContactId rhs = *(ContactId const*)arg1;
	Storage* storage = metadata;
	return strcmp(
		storage_get_phone_number(storage, lhs),
		storage_get_phone_number(storage, rhs));
}


static void
find_contact(Database* database) {
	char buf0[BUF_SIZE];
	char buf1[BUF_SIZE];

	printf("Ingrese nombre:\n>");
	read_a_line_with_retry_message(buf0, BUF_SIZE);

	printf("Ingrese apellido:\n>");
	read_a_line_with_retry_message(buf1, BUF_SIZE);

	string_trim(buf0);
	char* name = string_dup(buf0);

	string_trim(buf1);
	char* surname = string_dup(buf1);

	OptionalContactId contact = database_find(database, name, surname);
	if (!contact.active) {
		puts("No existe un contacto con ese nombre y apellido");
	} else {
		write_contact(storage_at(database->storage, contact.id), true, stdout);
		printf("\n");
	}
}

static void
create_contact(Database* database) {
	char buf0[BUF_SIZE];
	char buf1[BUF_SIZE];
	char buf2[BUF_SIZE];
	char buf3[BUF_SIZE];

	printf("Ingrese nombre:\n>");
	read_a_line_with_retry_message(buf0, BUF_SIZE);

	printf("Ingrese apellido:\n>");
	read_a_line_with_retry_message(buf1, BUF_SIZE);

	printf("Ingrese edad:\n>");
	uint32_t age = read_u32_with_retry_message(buf2, BUF_SIZE);

	printf("Ingrese telefono:\n>");
	read_a_line_with_retry_message(buf3, BUF_SIZE);

	string_trim(buf0);
	char* name = string_dup(buf0);

	string_trim(buf1);
	char* surname = string_dup(buf1);

	// TODO: validate phone number?
	string_trim(buf3);
	char* phone_number = string_dup(buf3);

	bool success = database_insert(database, name, surname, age, phone_number);
	if (!success) {
		puts("Ya existe un contacto con ese nombre y apellido");
	}
}

static void
delete_contact(Database* database) {
	char buf0[BUF_SIZE];
	char buf1[BUF_SIZE];

	printf("Ingrese nombre:\n>");
	read_a_line_with_retry_message(buf0, BUF_SIZE);

	printf("Ingrese apellido:\n>");
	read_a_line_with_retry_message(buf1, BUF_SIZE);

	string_trim(buf0);
	char* name = string_dup(buf0);

	string_trim(buf1);
	char* surname = string_dup(buf1);

	bool success = database_delete(database, name, surname);
	if (!success) {
		puts("No existe un contacto con ese nombre y apellido");
	}
}

static void
update_contact(Database* database) {
	char buf0[BUF_SIZE];
	char buf1[BUF_SIZE];

	printf("Ingrese nombre:\n>");
	read_a_line_with_retry_message(buf0, BUF_SIZE);

	printf("Ingrese apellido:\n>");
	read_a_line_with_retry_message(buf1, BUF_SIZE);

	string_trim(buf0);

	string_trim(buf1);

	{
		char* name = string_dup(buf0);
		char* surname = string_dup(buf1);

		if (!database_has(database, name, surname)) {
			puts("No existe un contacto con ese nombre y apellido");
			return;
		}
	}

	char buf2[BUF_SIZE];
	char buf3[BUF_SIZE];

	printf("Ingrese edad:\n>");
	uint32_t age = read_u32_with_retry_message(buf2, BUF_SIZE);

	printf("Ingrese telefono:\n>");
	read_a_line_with_retry_message(buf3, BUF_SIZE);

	char* name = string_dup(buf0);
	char* surname = string_dup(buf1);
	char* phone_number = string_dup(buf3);

	bool success = database_update(database, name, surname, age, phone_number);
	assert(success);
}

static void
load_from_fs(Database* database) {
	char buf0[BUF_SIZE];

	printf("Advertencia: se sobreescribiran los datos actuales\n");
	printf("Ingrese ruta de entrada:\n>");
	read_a_line_with_retry_message(buf0, BUF_SIZE);
	string_trim(buf0);

	FILE* f = fopen(buf0, "r");

	if (!f) {
		printf("No se pudo abrir el archivo, no se puede cargar.\n");
		goto cleanup0;
	}

	#define LINE_BUF_SIZE 1024
	char line_buf[LINE_BUF_SIZE];

	bool success;

	success = get_line(line_buf, LINE_BUF_SIZE, f);

	if (!success) {
		printf("Cabecera demasiado larga, no se puede cargar.\n");
		goto cleanup1;
	}

	string_trim(line_buf);
	if (strcmp(line_buf, "nombre,apellido,edad,telefono") != 0) {
		printf("Cabecera invalida, no se puede cargar.\n");
		goto cleanup1;
	}

	Database new_database = database_create(database->storage);
	for (size_t line_idx = 1; 1; ++line_idx) {

		IncompleteContact query = {};
		ReadContactStatus status =
			read_contact(line_buf, LINE_BUF_SIZE, &query, f);

		if (status == RCS_OK) {
			assert(query.has_age);
			database_insert(
				&new_database,
				string_dup(query.name),
				string_dup(query.surname),
				query.age,
				string_dup(query.phone_number));

			continue;
		}

		if (status == RCS_E_BAD_LINE && feof(f))
			break;

		switch(status) {
		case RCS_E_BAD_LINE:
			printf("Linea demasiado larga (linea %lu), no se puede cargar.\n", line_idx);
			break;
		case RCS_E_FIELD_COUNT:
			printf("Cantidad incorrecta de valores (linea %lu), no se puede cargar.\n", line_idx);
			break;
		case RCS_E_INVALID_AGE:
			printf("La edad no es un natural valido (linea %lu), no se puede cargar.\n", line_idx);
			break;
		case RCS_OK:
			assert(0);
		}
		goto cleanup2;
	}

	fclose(f);

	// pisamos la database que nos pasaron con la nueva
	database_release(database);
	*database = new_database;
	return;

cleanup2:
	database_release(&new_database);
cleanup1:
	fclose(f);
cleanup0:
	printf("Los datos anteriores quedaron intactos.\n");

#undef LINE_BUF_SIZE
}

static void
store_to_fs(Database* database) {
	char buf0[BUF_SIZE];

	printf("Ingrese ruta de salida:\n>");
	read_a_line_with_retry_message(buf0, BUF_SIZE);
	string_trim(buf0);

	FILE* f = fopen(buf0, "w");
	fprintf(f, "nombre,apellido,edad,telefono\n");
	write_contacts(
		storage_begin(database->storage),
		storage_end(database->storage),
		false, f);

	fclose(f);
}

static void
store_to_fs_sorted(Database* database) {
	char buf0[BUF_SIZE];
	char buf1[BUF_SIZE];

	printf("Ingrese ruta de salida:\n>");
	read_a_line_with_retry_message(buf0, BUF_SIZE);
	string_trim(buf0);

	printf("Ingrese nombre de atributo:\n>");
	read_a_line_with_retry_message(buf1, BUF_SIZE);
	string_trim(buf1);
	string_tolower(buf1);

	Comparator cmp;
	if (strcmp(buf1, "nombre") == 0) {
		cmp = (Comparator){ by_name_cmp, database->storage };
	} else if (strcmp(buf1, "apellido") == 0) {
		cmp = (Comparator){ by_surname_cmp, database->storage };
	} else if (strcmp(buf1, "edad") == 0) {
		cmp = (Comparator){ by_age_cmp, database->storage };
	} else if (strcmp(buf1, "telefono") == 0) {
		cmp = (Comparator){ by_phone_number_cmp, database->storage };
	} else {
		printf("No existe ese atributo.\n");
		return;
	}

	Vector contact_ids = database_contacts(database);
	quicksort(
		vector_begin(&contact_ids),
		vector_end(&contact_ids),
		vector_element_width(&contact_ids),
		cmp);

	FILE* f = fopen(buf0, "w");
	fprintf(f, "nombre,apellido,edad,telefono\n");
	write_contacts_by_id(
		vector_begin(&contact_ids),
		vector_end(&contact_ids),
		database->storage, false, f);

	fclose(f);
	vector_release(&contact_ids);
}

static void
undo(Database* database) {
	bool success = database_rewind(database);
	if (success) {
		puts("Se deshizo la ultima operacion");
	} else {
		puts("No hay operaciones para deshacer");
	}
}

static void
redo(Database* database) {
	bool success = database_advance(database);
	if (success) {
		puts("Se rehizo la ultima operacion");
	} else {
		puts("No hay operaciones para rehacer");
	}
}

static void
conjunction_query(Database* database) {
	IncompleteContact query_data = read_query_parameters();

	if (
		query_data.name == nullptr &&
		query_data.surname == nullptr &&
		!query_data.has_age &&
		query_data.phone_number == nullptr
	) {
		printf("No se puede realizar una consulta OR vacia\n");
		return;
	}

	Vector result = database_query_and(database, query_data);

	write_contacts_by_ptr(
		vector_begin(&result),
		vector_end(&result),
		true, stdout);

	vector_release(&result);
	free(query_data.name);
	free(query_data.surname);
	free(query_data.phone_number);
}

static void
disjunction_query(Database* database) {
	IncompleteContact query_data = read_query_parameters();

	if (
		query_data.name == nullptr &&
		query_data.surname == nullptr &&
		!query_data.has_age &&
		query_data.phone_number == nullptr
	) {
		printf("No se puede realizar una consulta OR vacia\n");
		return;
	}

	Vector result = database_query_or(database, query_data);

	write_contacts_by_ptr(
		vector_begin(&result),
		vector_end(&result),
		true, stdout);

	vector_release(&result);
	free(query_data.name);
	free(query_data.surname);
	free(query_data.phone_number);
}

static void
age_sum_query(Database* database) {
	char buf0[BUF_SIZE];

	printf("Ingrese un natural:\n>");
	uint32_t sum;
	while (1) {
		get_line_as_u32_retry(
			buf0, BUF_SIZE,
			&sum,
			"Linea muy larga. Vuelva a intentar:\n>",
			"Ingrese un natural valido:\n>", stdin);
		if (sum == 0) {
			printf("Ingrese un numero mayor a 0:\n>");
			continue;
		}
		break;
	}

	Vector ages = vector_create(sizeof(uint32_t));

	Contact const* begin = storage_begin(database->storage);
	Contact const* end = storage_end(database->storage);

	for (Contact const* it = begin; it < end; ++it) {
		uint32_t age = it->age;
		vector_push(&ages, SPANOF(age));
	}

	Vector result = search_by_sum(vector_begin(&ages), ages.size, sum);

	if (result.size == 0) {
		printf("No existe un conjunto de contactos que sumen %u\n", sum);
	} else {
		for (size_t i = 0; i < result.size; ++i) {
			size_t j; span_write(&j, vector_at(&result, i));
			write_contact(begin + j, true, stdout);
			printf("\n");
		}
	}

	vector_release(&result);
	vector_release(&ages);
}

typedef void (*MenuAction)(Database*);

struct _MenuEntry {
	char const* name;
	MenuAction action;
};
typedef struct _MenuEntry MenuEntry;

static int
action_id_prompt(int max_value) {
		char buf[BUF_SIZE];
		int selected;

		printf(">");
		while (1) {
			get_line_as_int_retry(buf, BUF_SIZE, &selected, "Linea demasiado larga. Vuelva a intentar:\n>", "Inserte un numero valido:\n>", stdin);
			if (selected < 1 || max_value < selected) {
				printf("Seleccion fuera de rango. Inserte una opcion valida:\n>");
				continue;
			}
			return selected;
		}
}

int
main() {

	Storage storage = storage_create();
	Database database = database_create(&storage);

#define MENU_LEN 13
	MenuEntry entries[MENU_LEN] = {
		{"Buscar", find_contact},
		{"Agregar", create_contact},
		{"Eliminar", delete_contact},
		{"Editar", update_contact},
		{"Cargar", load_from_fs},
		{"Guardar", store_to_fs},
		{"Deshacer", undo},
		{"Rehacer", redo},
		{"AND", conjunction_query},
		{"OR", disjunction_query},
		{"Guardar ordenado", store_to_fs_sorted},
		{"Buscar por suma de edades", age_sum_query},
		{"Salir", nullptr},
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
