#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"
#include "io.h"
#include "storage.h"
#include "string.h"
#include "search_by_sum.h"
#include "quicksort.h"

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

static QueryData
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

	return (QueryData) {
		.name = name,
		.surname = surname,
		.has_age = has_age,
		.age = age,
		.phone_number = phone_number,
	};
}

static void
print_contact(Contact* contact, bool braces, FILE* f) {
	if (braces) printf("{");
	print_title_case(contact->name, f);
	fprintf(f, ",");
	print_title_case(contact->surname, f);
	fprintf(f, ",%u,%s", contact->age, contact->phone_number);
	if (braces) printf("}");
}

static void
print_vector_of_contacts(Database* database, Vector const* contacts, bool braces, FILE* f) {
	for (size_t i = 0; i < contacts->size; ++i) {
		ContactId id; span_write(&id, vector_at(contacts, i));
		Contact* contact = storage_at(database->storage, id);
		print_contact(contact, braces, f);
		fprintf(f, "\n");
	}
}


void
buscar(Database* database) {
	char buf0[BUF_SIZE];
	char buf1[BUF_SIZE];

	printf("Ingrese nombre:\n>");
	read_a_line_with_retry_message(buf0, BUF_SIZE);

	printf("Ingrese apellido:\n>");
	read_a_line_with_retry_message(buf1, BUF_SIZE);

	string_tolower(buf0);
	string_trim(buf0);
	char* name = string_dup(buf0);

	string_tolower(buf1);
	string_trim(buf1);
	char* surname = string_dup(buf1);

	OptionalContactId contact = database_find(database, name, surname);
	if (!contact.active) {
		puts("No existe un contacto con ese nombre y apellido");
	} else {
		print_contact(storage_at(database->storage, contact.id), true, stdout);
		printf("\n");
	}
}

void
agregar(Database* database) {
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

	string_tolower(buf0);
	string_trim(buf0);
	char* name = string_dup(buf0);

	string_tolower(buf1);
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

void
eliminar(Database* database) {
	char buf0[BUF_SIZE];
	char buf1[BUF_SIZE];

	printf("Ingrese nombre:\n>");
	read_a_line_with_retry_message(buf0, BUF_SIZE);

	printf("Ingrese apellido:\n>");
	read_a_line_with_retry_message(buf1, BUF_SIZE);

	string_tolower(buf0);
	string_trim(buf0);
	char* name = string_dup(buf0);

	string_tolower(buf1);
	string_trim(buf1);
	char* surname = string_dup(buf1);

	bool success = database_delete(database, name, surname);
	if (!success) {
		puts("No existe un contacto con ese nombre y apellido");
	}
}

void
editar(Database* database) {
	char buf0[BUF_SIZE];
	char buf1[BUF_SIZE];

	printf("Ingrese nombre:\n>");
	read_a_line_with_retry_message(buf0, BUF_SIZE);

	printf("Ingrese apellido:\n>");
	read_a_line_with_retry_message(buf1, BUF_SIZE);

	string_tolower(buf0);
	string_trim(buf0);

	string_tolower(buf1);
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
release_db_and_storage(Database* database) {
	printf("Los datos anteriores quedaron intactos.\n");
	database_release(database);
	storage_release(database->storage);
}

void
cargar(Database* database) {
	char buf0[BUF_SIZE];

	printf("Advertencia: se sobreescribiran los datos actuales\n");
	printf("Ingrese ruta de entrada:\n>");
	read_a_line_with_retry_message(buf0, BUF_SIZE);
	string_trim(buf0);

	FILE* f = fopen(buf0, "r");

	Storage new_storage = storage_create();
	Database new_database = database_create(&new_storage);

	#define LINE_BUF_SIZE 1024
	char line_buf[LINE_BUF_SIZE];

	bool success;

	success = get_line(line_buf, LINE_BUF_SIZE, f);

	if (!success) {
		printf("Cabecera demasiado larga, no se puede cargar.\n");
		return release_db_and_storage(&new_database);
	}

	for (size_t line_idx = 1; !feof(f); ++line_idx) {
		get_line(line_buf, LINE_BUF_SIZE, f);

		success = get_line(line_buf, LINE_BUF_SIZE, f);

		if (!success) {
			printf("Linea demasiado larga (linea %lu), no se puede cargar.\n", line_idx);
			return release_db_and_storage(&new_database);
		}

		char* name = buf0;

		char* surname = name;
		for (; *surname != ','; ++surname) {
			if (*surname == '\0') {
				printf("Final de linea inesperado (linea %lu), no se puede cargar.\n", line_idx);
				return release_db_and_storage(&new_database);
			}
		}
		*surname++ = '\0'; // name terminator

		char* age_str = surname;
		for (; *age_str != ','; ++age_str) {
			if (*age_str == '\0') {
				printf("Final de linea inesperado (linea %lu), no se puede cargar.\n", line_idx);
				return release_db_and_storage(&new_database);
			}
		}
		*age_str++ = '\0'; // surname terminator

		char* phone_number = age_str;
		for (; *phone_number != ','; ++phone_number) {
			if (*phone_number == '\0') {
				printf("Final de linea inesperado (linea %lu), no se puede cargar.\n", line_idx);
				return release_db_and_storage(&new_database);
			}
		}
		*phone_number++ = '\0'; // age terminator

		string_trim(name);
		string_tolower(name);

		string_trim(surname);
		string_tolower(surname);

		string_trim(age_str);
		string_tolower(age_str);

		string_trim(phone_number);
		string_tolower(phone_number);

		uint32_t age;
		success = parse_u32(age_str, &age);

		if (!success) {
			printf("La edad no es un natural valido (linea %lu), no se puede cargar.\n", line_idx);
			return release_db_and_storage(&new_database);
		}

		database_insert(&new_database, string_dup(name), string_dup(surname), age, string_dup(phone_number));
	}

	database_release(database);
	storage_release(database->storage);

	*database->storage = new_storage;

	new_database.storage = database->storage;
	*database = new_database;
}

void
guardar(Database* database) {
	char buf0[BUF_SIZE];

	printf("Ingrese ruta de salida:\n>");
	read_a_line_with_retry_message(buf0, BUF_SIZE);
	string_trim(buf0);

	Vector contacts = database_contacts(database);

	FILE* f = fopen(buf0, "w");

	fprintf(f, "nombre,apellido,edad,telefono\n");
	print_vector_of_contacts(database, &contacts, false, f);

	fclose(f);
	vector_release(&contacts);
}

void
deshacer(Database* database) {
	bool success = database_rewind(database);
	if (success) {
		puts("Se deshizo la ultima operacion");
	} else {
		puts("No hay operaciones para deshacer");
	}
}

void
rehacer(Database* database) {
	bool success = database_advance(database);
	if (success) {
		puts("Se rehizo la ultima operacion");
	} else {
		puts("No hay operaciones para rehacer");
	}
}

void
conjuncion(Database* database) {
	QueryData query_data = read_query_parameters();

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

	print_vector_of_contacts(database, &result, true, stdout);

	vector_release(&result);
	free(query_data.name);
	free(query_data.surname);
	free(query_data.phone_number);
}

void
disjuncion(Database* database) {
	QueryData query_data = read_query_parameters();

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

	print_vector_of_contacts(database, &result, true, stdout);

	vector_release(&result);
	free(query_data.name);
	free(query_data.surname);
	free(query_data.phone_number);
}

static int
by_name_cmp(void const* arg0, void const* arg1, void* metadata) {
	ContactId const* lhs = arg0;
	ContactId const* rhs = arg1;
	Database* database = metadata;
	Storage* storage = database->storage;
	return strcmp(storage_at(storage, *lhs)->name, storage_at(storage, *rhs)->name);
}

static int
by_surname_cmp(void const* arg0, void const* arg1, void* metadata) {
	ContactId const* lhs = arg0;
	ContactId const* rhs = arg1;
	Database* database = metadata;
	Storage* storage = database->storage;
	return strcmp(storage_at(storage, *lhs)->surname, storage_at(storage, *rhs)->surname);
}

static int
by_age_cmp(void const* arg0, void const* arg1, void* metadata) {
	ContactId const* lhs = arg0;
	ContactId const* rhs = arg1;
	Database* database = metadata;
	Storage* storage = database->storage;

	uint32_t lhs_age = storage_at(storage, *lhs)->age;
	uint32_t rhs_age = storage_at(storage, *rhs)->age;

	if (lhs_age < rhs_age)
		return -1;
	return rhs_age < lhs_age;
}

static int
by_phone_number_cmp(void const* arg0, void const* arg1, void* metadata) {
	ContactId const* lhs = arg0;
	ContactId const* rhs = arg1;
	Database* database = metadata;
	Storage* storage = database->storage;
	return strcmp(storage_at(storage, *lhs)->phone_number, storage_at(storage, *rhs)->phone_number);
}

void
guardar_ordenado(Database* database) {
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
		cmp = (Comparator){ by_name_cmp, database };
	} else if (strcmp(buf1, "apellido") == 0) {
		cmp = (Comparator){ by_surname_cmp, database };
	} else if (strcmp(buf1, "edad") == 0) {
		cmp = (Comparator){ by_age_cmp, database };
	} else if (strcmp(buf1, "telefono") == 0) {
		cmp = (Comparator){ by_phone_number_cmp, database };
	} else {
		printf("No existe ese atributo.\n");
		return;
	}

	Vector contacts = database_contacts(database);
	quicksort(vector_full_segment(&contacts), cmp);

	FILE* f = fopen(buf0, "w");
	fprintf(f, "nombre,apellido,edad,telefono\n");
	print_vector_of_contacts(database, &contacts, false, f);

	fclose(f);
	vector_release(&contacts);
}

void
buscar_por_suma_de_edades(Database* database) {
	char buf0[BUF_SIZE];

	printf("Ingrese un natural:\n>");
	uint32_t sum;
	while (1) {
		get_line_as_u32_retry(buf0, BUF_SIZE, &sum, "Linea muy larga. Vuelva a intentar:\n>", "Ingrese un natural valido:\n>", stdin);
		if (sum == 0) {
			printf("Ingrese un numero mayor a 0:\n>");
			continue;
		}
		break;
	}

	Vector contact_ids = database_contacts(database);

	Vector ages = vector_create(sizeof(uint32_t));
	for (size_t i = 0; i < contact_ids.size; ++i) {
		ContactId id; span_write(&id, vector_at(&contact_ids, i));
		uint32_t age = storage_at(database->storage, id)->age;
		vector_push(&ages, SPANOF(age));
	}

	Vector result = search_by_sum(ages.buffer.data.begin, ages.size, sum);

	if (result.size == 0) {
		printf("No existe un conjunto de contactos que sumen %u\n", sum);
	} else {
		Vector to_print = vector_create(sizeof(ContactId));
		for (size_t i = 0; i < result.size; ++i) {
			size_t j; span_write(&j, vector_at(&result, i));
			vector_push(&to_print, vector_push(&contact_ids, SPANOF(j)));
		}
		print_vector_of_contacts(database, &to_print, true, stdout);
		vector_release(&to_print);
	}

	vector_release(&result);
	vector_release(&ages);
	vector_release(&contact_ids);
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
