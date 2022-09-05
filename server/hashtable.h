#pragma once

#include <stddef.h>

/*

Este modulo implementa una tabla hash que sigue las normas impuestas por kv_store

*/

struct hashtable* hashtable_create();

int hashtable_lookup(
	struct hashtable* t,

	unsigned char* key,
	size_t key_size,

	unsigned char** out_value,
	size_t* out_value_size);


int hashtable_insert(
	struct hashtable* t,

	unsigned char* key,
	size_t key_size,

	unsigned char* value,
	size_t value_size);


int hashtable_delete(
	struct hashtable* tabla,
	unsigned char* key,
	size_t key_size);


int hashtable_take(
	struct hashtable* t,

	unsigned char* key,
	size_t key_size,

	unsigned char** out_value,
	size_t* out_value_size);

int hashtable_evict(struct hashtable* t);

size_t hashtable_size(struct hashtable* t);
