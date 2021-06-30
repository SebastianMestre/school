#pragma once

#include "span.h"
#include "vector.h"

struct _SlotMap {
	Vector cells;
	Vector cell_data;

	Vector slots;
	Vector holes;

	size_t highlighted_count;
	size_t element_width;
	Destructor dtor;
};
typedef struct _SlotMap SlotMap;


SlotMap
slot_map_create(size_t element_width, Destructor dtor);

void
slot_map_release(SlotMap* slot_map);

Span
slot_map_at(SlotMap* map, size_t id);

size_t
slot_map_insert(SlotMap* map, Span data);

void
slot_map_delete(SlotMap* map, size_t id);

void
slot_map_increase_refcount(SlotMap* map, size_t id);

void
slot_map_decrease_refcount(SlotMap* map, size_t id);

// devuelve el slot que le corresponde a una celda
size_t
slot_map_get_cell_slot(SlotMap* map, size_t cell);

// 'resalta' un elemento
void
slot_map_highlight(SlotMap* map, size_t i);

// 'des-resalta' un elemento
void
slot_map_unhighlight(SlotMap* map, size_t i);

// devuelve true sii el elemento esta 'resaltado'
bool
slot_map_is_highlighted(SlotMap* map, size_t i);

// aplica el callback a cada celda
void
slot_map_for_each(SlotMap const* map, Callback cb);

// aplica el callback a cada celda resaltada
void
slot_map_for_each_highlighted(SlotMap const* map, Callback cb);
