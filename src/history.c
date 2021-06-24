#include "history.h"

#include "contacts.h"

#define HISTORY_SIZE (10)

struct _Action {
	ContactId forwards;
	ContactId backwards;
};
typedef struct _Action Action;

static void
action_dtor_impl(void* action_ptr, void* contacts_ptr) {
	Contacts* contacts = (Contacts*)contacts_ptr;
	Action* action = (Action*)action_ptr;

	contacts_unset_flags(contacts, action->forwards, CONTACT_REFERENCED_IN_HISTORY_FWD);
	contacts_unset_flags(contacts, action->backwards, CONTACT_REFERENCED_IN_HISTORY_BWD);
}

History
history_create(Contacts* contacts) {
	return (History){
		.actions = circular_buffer_create(
			sizeof(ContactId),
			HISTORY_SIZE,
			(Destructor){
				action_dtor_impl,
				contacts
			}
		),
	};
}
