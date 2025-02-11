/**
 * Este modulo provee una funcion para intentar alocar memoria en el heap,
 * purgando la LRU si es necesario.
 * 
 */
#include "kv_store.h"

// -- intenta alocar memoria en el heap --
void* try_alloc(kv_store* store, size_t size);
