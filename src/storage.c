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
