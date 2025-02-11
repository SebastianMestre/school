// Programa de prueba para implementaciones de kv_store

#include "kv_store.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* s(char const* str) {
	return strcpy(malloc(strlen(str) + 1), str);
}

#define S(x) s(x), strlen(x)

int main() {
	int err;

	struct kv_store* store = kv_store_init();

	err = kv_store_put(store, S("hola"), S("mundo"));
	if (err != 0) {
		fprintf(stderr, "PUT error code = %d\n", err);
		exit(EXIT_FAILURE);
	}

	err = kv_store_put(store, S("lio"), S("messi"));
	if (err != 0) {
		fprintf(stderr, "PUT error code = %d\n", err);
		exit(EXIT_FAILURE);
	}

	err = kv_store_put(store, S("cris"), S("ronaldo"));
	if (err != 0) {
		fprintf(stderr, "PUT error code = %d\n", err);
		exit(EXIT_FAILURE);
	}

	err = kv_store_del(store, S("lio"));
	if (err != 0) {
		fprintf(stderr, "DEL error code = %d\n", err);
		exit(EXIT_FAILURE);
	}

	char* val = NULL;
	size_t val_len = 0;
	err = kv_store_get(store, S("hola"), &val, &val_len);
	if (err != 0) {
		fprintf(stderr, "GET error code = %d\n", err);
		exit(EXIT_FAILURE);
	}

	fwrite(val, 1, val_len, stdout);
	putchar('\n');
}