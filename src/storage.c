#include "storage.h"

#include <assert.h>

struct _Slot {
	Contact data;
	bool active;
	uint8_t refcount;
};
typedef struct _Slot Slot;

Storage
storage_create() {
	return (Storage){
		.holes = vector_create(sizeof(int)),
		.slots = vector_create(sizeof(Slot)),
	};
}

static Slot*
get_slot(Storage storage, ContactId id) {
	Span span = vector_at(storage.slots, id);
	return (Slot*)span.begin;
}

void
storage_increase_refcount(Storage storage, ContactId id) {
	Slot* slot = get_slot(storage, id);
	assert(slot->active);
	slot->refcount += 1;
}

void
storage_decrease_refcount(Storage* storage, ContactId id) {
	Slot* slot = get_slot(*storage, id);
	assert(slot->active);
	slot->refcount -= 1;

	if (slot->refcount == 0) {
		// TODO: contact_dtor(slot->data);
		slot->active = false;
		vector_push(&storage->holes, SPANOF(id));
	}
}

Contact*
storage_at(Storage storage, ContactId id) {
	Slot* result = get_slot(storage, id);
	return &result->data;
}

ContactId
storage_insert(
	Storage* storage,
	char* name,
	char* surname,
	unsigned age,
	char* phone_number
) {
	Contact to_insert = {
		.name = name,
		.surname = surname,
		.age = age,
		.phone_number = phone_number,
	};

	if (storage->holes.size > 0) {
		ContactId position = 0;
		span_write(&position, vector_last(storage->holes));
		vector_put_at(storage->slots, position, SPANOF(to_insert));
		return position;
	} else {
		ContactId result = storage->slots.size;
		vector_push(&storage->slots, SPANOF(to_insert));
		return result;
	}
}
