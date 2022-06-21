#pragma once

struct kv_store;

// pide que se libere espacio de memoria
//
// RESULT
// return: devuelve un numero negativo si se produce un error y 0 en caso contrario
//
// ERRORES
// no hay elementos en el almacenamiento
int kv_store_evict(struct kv_store* store);

// pide que se inserte un par (clave, valor)
// puede liberar otras entradas del almacenamiento, incluso en el caso de error
//
// PARAMS
// key: la clave a insertar
// key_length: longitud en bytes de la clave
// value: el valor a insertar
// value_length: longitud en bytes del valor
//
// RESULT
// return: devuelve un numero negativo si se produce un error y 0 en caso contrario
// 
// ERRORES
// no hay suficiente espacio, incluso liberando otras entradas
int kv_store_put(struct kv_store* store, char const* key, int key_length, char const* value, int value_length);

// pide que se inserte un par (clave, valor)
// puede liberar otras entradas del almacenamiento, incluso en el caso de error
//
// PARAMS
// key: la clave a buscar
// key_length: longitud en bytes de la clave
//
// RESULT
// out_value: guarda la direccion del valor que corresponde a la clave. Se debe liberar usando free().
// return: devuelve un numero negativo si se produce un error y 0 en caso contrario.
// 
// ERRORES
// no hay suficiente espacio, incluso liberando otras entradas
int kv_store_get(struct kv_store* store, char const* key, int key_length, char** out_value, int* out_value_length);

// toma ownership del key y el value
int kv_store_put(struct kv_store* store, char const* key, int key_length);

int kv_store_take(struct kv_store* store, char const* key, int key_length, char** out_value, int* out_value_length);

int kv_store_stat(struct kv_store* store, int* out_stat);