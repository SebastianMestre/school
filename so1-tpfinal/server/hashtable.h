/**
 * Este modulo implementa una tabla hash que sigue las normas impuestas por kv_store.
 * Ver informe para mas informacion.
 */
#pragma once

#include <stddef.h>

// Crea la tabla.
struct hashtable* hashtable_create();

// Busca el valor asociado a la clave en la tabla.
int hashtable_lookup(
	struct hashtable* t,

	unsigned char* key,
	size_t key_size,

	unsigned char** out_value,
	size_t* out_value_size);

// Inserta el par (clave, valor) en la tabla.
// Si es que existe, pisa el valor viejo.
int hashtable_insert(
	struct hashtable* t,

	unsigned char* key,
	size_t key_size,

	unsigned char* value,
	size_t value_size);

// Si existe, borra de la tabla el valor asociado la clave dada.
int hashtable_delete(
	struct hashtable* tabla,
	unsigned char* key,
	size_t key_size);

// Si exsite, obtiene el valor asociado a la clave dada
// y lo remueve de la tabla.
int hashtable_take(
	struct hashtable* t,

	unsigned char* key,
	size_t key_size,

	unsigned char** out_value,
	size_t* out_value_size);

// Borra el valor menos usado de la tabla (ver informe).
int hashtable_evict(struct hashtable* t);

// Obtiene la cantidad de entradas de la tabla.
size_t hashtable_size(struct hashtable* t);
