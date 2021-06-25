#include "history.h"

#include "storage.h"

#define HISTORY_SIZE (10)

struct _Action {
	ContactId forwards;
	ContactId backwards;
};
typedef struct _Action Action;

static void
action_dtor_impl(void* action_ptr, void* storage_ptr) {
	Storage* storage = (Storage*)storage_ptr;
	Action* action = (Action*)action_ptr;

	storage_unset_flags(storage, action->forwards, CONTACT_REFERENCED_IN_HISTORY_FWD);
	storage_unset_flags(storage, action->backwards, CONTACT_REFERENCED_IN_HISTORY_BWD);
}

History
history_create(Storage* storage) {
	return (History){
		.actions = circular_buffer_create(
			sizeof(ContactId),
			HISTORY_SIZE,
			(Destructor){
				action_dtor_impl,
				storage
			}
		),
	};
}
