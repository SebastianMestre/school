#pragma once

#include "span.h"
#include "vector.h"

struct _SlotMap {
	Vector holes;
	Vector slots;
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
