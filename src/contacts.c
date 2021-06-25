#include "contacts.h"

#include <assert.h>

struct _Slot {
	Contact data;
	uint8_t flags;
};
typedef struct _Slot Slot;

#define CONTACT_REFERENCED (CONTACT_REFERENCED_IN_INDEX | CONTACT_REFERENCED_IN_HISTORY_FWD | CONTACT_REFERENCED_IN_HISTORY_BWD)

static Slot*
get_slot(Contacts contacts, ContactId id) {
	Span span = vector_at(contacts.slots, id);
	return (Slot*)span.begin;
}

void
contacts_set_flags(Contacts contacts, ContactId id, uint8_t flags) {
	assert(!(flags & CONTACT_ACTIVE));

	Slot* slot = get_slot(contacts, id);
	assert(slot->flags & CONTACT_ACTIVE);

	slot->flags |= flags;
}

void
contacts_unset_flags(Contacts* contacts, ContactId id, uint8_t flags) {
	assert(!(flags & CONTACT_ACTIVE));

	Slot* slot = get_slot(*contacts, id);
	assert(slot->flags & CONTACT_ACTIVE);

	slot->flags &= ~flags;

	if (!(slot->flags & CONTACT_REFERENCED)) {
		// TODO: contact_dtor(slot->data);

		slot->flags &= ~CONTACT_ACTIVE;
		vector_push(&contacts->holes, SPANOF(id));
	}
}

Contact*
contacts_at(Contacts contacts, ContactId id) {
	Slot* result = get_slot(contacts, id);
	assert(result);
	return &result->data;
}
