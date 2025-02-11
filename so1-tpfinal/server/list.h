/**
 * Este modulo define una lista circular doblemente enlazada intrusiva.
 * La utilizamos en la implementacion de la tabla hash (ver informe).
 */

#pragma once

struct list {
	struct list* next;
	struct list* prev;
};

// Inicializar lista.
void list_init(struct list* list);
// Insertar al comienzo.
void list_push_start(struct list* list, struct list* node);
// Remueve el nodo de la lista.
void list_remove(struct list* node);
