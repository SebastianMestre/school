#include "slot_map.h"

#include <assert.h>
#include <stdint.h>

struct _Slot {
	uint32_t refcount;
	bool active;
	// rellenamos para que data quede alineado a 8 bytes
	char PADD[3];

	char data[];
};
typedef struct _Slot Slot;

SlotMap
slot_map_create(size_t element_width, Destructor dtor) {
	size_t slot_width = sizeof(Slot) + element_width;
	// redondeo a multiplo de 8
	slot_width = ((slot_width + 8 - 1) / 8) * 8;

	return (SlotMap){
		.holes = vector_create(sizeof(size_t)),
		.slots = vector_create(slot_width),
		.element_width = element_width,
		.dtor = dtor,
	};
}

void
slot_map_release(SlotMap* slot_map) {
	vector_release(&slot_map->holes);
	vector_release(&slot_map->slots);
}

static Slot*
get_slot(SlotMap* map, size_t id) {
	Span span = vector_at(map->slots, id);
	return (Slot*)span.begin;
}

Span
slot_map_at(SlotMap* map, size_t id) {
	Slot* slot = get_slot(map, id);
	assert(slot->active);
	return span_create(slot->data, map->element_width);
}

size_t
slot_map_insert(SlotMap* map, Span data) {
	if (map->holes.size > 0) {
		size_t position = 0;
		span_write(&position, vector_last(map->holes));
		// TODO vector_pop(&map->holes);

		Slot* slot = get_slot(map, position);
		assert(!slot->active);
		assert(slot->refcount == 0);

		slot->active = true;
		span_write(slot->data, data);

		return position;
	} else {
		size_t result = map->slots.size;

		Slot to_push = {
			.refcount = 0,
			.active = true,
		};
		Span slot_space = vector_push_incomplete(&map->slots, SPANOF(to_push));
		Slot* slot = slot_space.begin;
		span_write(slot->data, data);

		return result;
	}
}

static void
delete(SlotMap* map, Slot* slot, size_t id) {
	assert(slot->active);
	assert(slot->refcount == 0);

	call_dtor(map->dtor, slot->data);
	slot->active = false;
	vector_push(&map->holes, SPANOF(id));
}

void
slot_map_delete(SlotMap* map, size_t id) {
	Slot* slot = get_slot(map, id);
	delete(map, slot, id);
}

void
slot_map_increase_refcount(SlotMap* map, size_t id) {
	Slot* slot = get_slot(map, id);

	assert(slot->active);

	slot->refcount += 1;
}

void
slot_map_decrease_refcount(SlotMap* map, size_t id) {
	Slot* slot = get_slot(map, id);

	assert(slot->active);
	assert(slot->refcount > 0);

	slot->refcount -= 1;

	if (slot->refcount == 0) {
		delete(map, slot, id);
	}
}
