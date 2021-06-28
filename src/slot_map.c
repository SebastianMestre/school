#include "slot_map.h"

#include <assert.h>
#include <stdint.h>

struct _CellData {
	size_t slot;
};
typedef struct _CellData CellData;

struct _SlotData {
	size_t cell;
	uint32_t refcount;
	bool active;
};
typedef struct _SlotData SlotData;

SlotMap
slot_map_create(size_t element_width, Destructor dtor) {
	return (SlotMap){
		.cells = vector_create(element_width),
		.cell_data = vector_create(sizeof(CellData)),
		.slot_data = vector_create(sizeof(SlotData)),
		.holes = vector_create(sizeof(size_t)),

		.element_width = element_width,
		.dtor = dtor,
	};
}

void
slot_map_release(SlotMap* slot_map) {
	vector_release(&slot_map->cells);
	vector_release(&slot_map->cell_data);
	vector_release(&slot_map->slot_data);
	vector_release(&slot_map->holes);
}

static Span
get_cell(SlotMap* map, size_t cell) {
	return vector_at(map->cells, cell);
}

static CellData*
get_cell_data(SlotMap* map, size_t cell) {
	return (CellData*)vector_at(map->cell_data, cell).begin;
}

static SlotData*
get_slot_data(SlotMap* map, size_t id) {
	return (SlotData*)vector_at(map->slot_data, id).begin;
}

Span
slot_map_at(SlotMap* map, size_t id) {
	return get_cell(map, get_slot_data(map, id)->cell);
}

static size_t
pop_hole(SlotMap* map) {
	assert(map->holes.size > 0);
	size_t result = 0;
	span_write(&result, vector_last(map->holes));
	vector_pop(&map->holes);
	return result;
}

size_t
slot_map_insert(SlotMap* map, Span data) {
	assert(span_width(data) == map->element_width);

	size_t cell = map->cells.size;
	size_t id;
	if (map->holes.size > 0) {
		id = pop_hole(map);

		assert(!get_slot_data(map, id)->active);
		assert(get_slot_data(map, id)->refcount == 0);

		*get_slot_data(map, id) = (SlotData){
			.cell = cell,
			.active = true,
			.refcount = 0,
		};
	} else {
		id = map->slot_data.size;

		SlotData slot_data = {
			.cell = cell,
			.active = true,
			.refcount = 0,
		};

		vector_push(&map->slot_data, SPANOF(slot_data));
	}

	CellData cell_data = {
		.slot = id,
	};
	vector_push(&map->cell_data, SPANOF(cell_data));
	vector_push(&map->cells, data);

	return id;
}

static void
delete(SlotMap* map, size_t i) {
	assert(i < map->slot_data.size);
	assert(get_slot_data(map, i)->active);
	assert(get_slot_data(map, i)->refcount == 0);

	size_t i_cell = get_slot_data(map, i)->cell;
	call_dtor(map->dtor, vector_at(map->cells, i_cell).begin);
	get_slot_data(map, i)->active = false;
	vector_push(&map->holes, SPANOF(i));

	if(map->cells.size != 1) {
		size_t j_cell = map->cells.size - 1;
		vector_put_at(&map->cells, i_cell, vector_at(map->cells, j_cell));
		size_t j = get_cell_data(map, j_cell)->slot;

		get_slot_data(map, j)->cell = i_cell;
		get_cell_data(map, i_cell)->slot = j;
	}

	vector_pop(&map->cells);
}

void
slot_map_delete(SlotMap* map, size_t id) {
	delete(map, id);
}

void
slot_map_increase_refcount(SlotMap* map, size_t id) {
	SlotData* slot = get_slot_data(map, id);
	assert(slot->active);
	slot->refcount += 1;
}

void
slot_map_decrease_refcount(SlotMap* map, size_t id) {
	SlotData* slot = get_slot_data(map, id);

	assert(slot->active);
	assert(slot->refcount > 0);

	slot->refcount -= 1;

	if (slot->refcount == 0) {
		delete(map, id);
	}
}
