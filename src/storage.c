#include "storage.h"

#include <assert.h>

struct _Slot {
	Contact data;
	uint8_t flags;
};
typedef struct _Slot Slot;

#define CONTACT_REFERENCED (CONTACT_REFERENCED_IN_INDEX | CONTACT_REFERENCED_IN_HISTORY_FWD | CONTACT_REFERENCED_IN_HISTORY_BWD)

static Slot*
get_slot(Storage storage, ContactId id) {
	Span span = vector_at(storage.slots, id);
	return (Slot*)span.begin;
}

void
storage_set_flags(Storage storage, ContactId id, uint8_t flags) {
	assert(!(flags & CONTACT_ACTIVE));

	Slot* slot = get_slot(storage, id);
	assert(slot->flags & CONTACT_ACTIVE);

	slot->flags |= flags;
}

void
storage_unset_flags(Storage* storage, ContactId id, uint8_t flags) {
	assert(!(flags & CONTACT_ACTIVE));

	Slot* slot = get_slot(*storage, id);
	assert(slot->flags & CONTACT_ACTIVE);

	slot->flags &= ~flags;

	if (!(slot->flags & CONTACT_REFERENCED)) {
		// TODO: contact_dtor(slot->data);

		slot->flags &= ~CONTACT_ACTIVE;
		vector_push(&storage->holes, SPANOF(id));
	}
}

Contact*
storage_at(Storage storage, ContactId id) {
	Slot* result = get_slot(storage, id);
	assert(result);
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
