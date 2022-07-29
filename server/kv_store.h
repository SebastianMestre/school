#pragma once

#define KV_STORE_OK 0
#define KV_STORE_OOM (-1)
#define KV_STORE_NOTFOUND (-2)
#define KV_STORE_UNK (-3)

#include <stddef.h>
#include <stdint.h>

struct kv_store;

struct kv_store* kv_store_init();

// TODO falta marcar cuando un valor es binario

// pide que se libere espacio de memoria
//
// RESULT
// return: devuelve un numero negativo si se produce un error y 0 en caso contrario
//
// ERRORES
// no hay elementos en el almacenamiento
int kv_store_evict(struct kv_store* store);

// pide que se inserte un par (clave, valor)
// si tiene exito, toma ownership de la clave y el valor
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
// no hay suficiente espacio de heap para insertar un nodo nuevo
int kv_store_put(struct kv_store* store, char* key, uint32_t key_length, char* value, uint32_t value_length);

// pide el valor correspondiente a una clave
// da ownership sobre el valor que se devuelve
// no toma ownership de la clave
//
// PARAMS
// key: la clave a buscar
// key_length: longitud en bytes de la clave
//
// RESULT
// out_value: el valor asociado a la clave, 
// out_value_length: longitud en bytes del valor
// return: devuelve un numero negativo si se produce un error y 0 en caso contrario
// 
// ERRORES
// no existe la clave en la estructura
// no hay suficiente memoria para copiar el valor
int kv_store_get(struct kv_store* store, char* key, uint32_t key_length, char** out_value, uint32_t* out_value_length);

// pide que se se elimine la entrada correspondiente a una clave
// no toma ownership de la key
//
// PARAMS
// key: la clave a buscar
// key_length: longitud en bytes de la clave
//
// RESULT
// return: devuelve un numero negativo si se produce un error y 0 en caso contrario.
// 
// ERRORES
// no existe la clave en la estructura
int kv_store_del(struct kv_store* store, char* key, uint32_t key_length);

// pide el valor correspondiente a una clave, y que se elimine la entrada correspondiente
// no toma ownership de la key
// da ownership sobre la clave y el valor que se devuelven
//
// PARAMS
// key: la clave a buscar
// key_length: longitud en bytes de la clave
//
// RESULT
// out_value: el valor asociado a la clave, 
// out_value_length: longitud en bytes del valor
// return: devuelve un numero negativo si se produce un error y 0 en caso contrario
// 
// ERRORES
// no existe la clave en la estructura
int kv_store_take(struct kv_store* store, char* key, uint32_t key_length, char** out_value, uint32_t* out_value_length);

int kv_store_stat(struct kv_store* store, int* out_stat);