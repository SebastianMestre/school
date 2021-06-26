#include "slot_map.h"

#include <assert.h>
#include <stdint.h>

struct _Metadata {
	uint32_t refcount;
	bool active;
};
typedef struct _Metadata Metadata;

SlotMap
slot_map_create(size_t element_width, Destructor dtor) {
	return (SlotMap){
		.holes = vector_create(sizeof(size_t)),
		.slots = vector_create(element_width),
		.slot_metadata = vector_create(sizeof(Metadata)),
		.element_width = element_width,
		.dtor = dtor,
	};
}

void
slot_map_release(SlotMap* slot_map) {
	vector_release(&slot_map->holes);
	vector_release(&slot_map->slots);
	vector_release(&slot_map->slot_metadata);
}

static Metadata*
get_metadata(SlotMap* map, size_t id) {
	Span span = vector_at(map->slot_metadata, id);
	return (Metadata*)span.begin;
}

static Span
get_value(SlotMap* map, size_t id) {
	return vector_at(map->slots, id);
}

Span
slot_map_at(SlotMap* map, size_t id) {
	Metadata* slot = get_metadata(map, id);
	assert(slot->active);
	return get_value(map, id);
}

size_t
slot_map_insert(SlotMap* map, Span data) {
	assert(span_width(data) == map->element_width);
	if (map->holes.size > 0) {
		size_t position = 0;
		span_write(&position, vector_last(map->holes));
		// TODO vector_pop(&map->holes);

		Metadata* slot = get_metadata(map, position);
		assert(!slot->active);
		assert(slot->refcount == 0);

		slot->active = true;

		Span slot_data = get_value(map, position);
		span_write(slot_data.begin, data);

		return position;
	} else {
		size_t result = map->slots.size;

		Metadata metadata = {
			.refcount = 0,
			.active = true,
		};

		vector_push(&map->slot_metadata, SPANOF(metadata));
		vector_push(&map->slots, data);

		return result;
	}
}

static void
delete(SlotMap* map, Metadata* slot, size_t id) {
	assert(slot->active);
	assert(slot->refcount == 0);

	Span value = get_value(map, id);
	call_dtor(map->dtor, value.begin);

	slot->active = false;
	vector_push(&map->holes, SPANOF(id));
}

void
slot_map_delete(SlotMap* map, size_t id) {
	Metadata* slot = get_metadata(map, id);
	delete(map, slot, id);
}

void
slot_map_increase_refcount(SlotMap* map, size_t id) {
	Metadata* slot = get_metadata(map, id);

	assert(slot->active);

	slot->refcount += 1;
}

void
slot_map_decrease_refcount(SlotMap* map, size_t id) {
	Metadata* slot = get_metadata(map, id);

	assert(slot->active);
	assert(slot->refcount > 0);

	slot->refcount -= 1;

	if (slot->refcount == 0) {
		delete(map, slot, id);
	}
}
