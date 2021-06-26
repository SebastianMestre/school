#include "history.h"

#include "storage.h"

#define HISTORY_SIZE (10)

struct _Action {
	OptionalContactId forwards;
	OptionalContactId backwards;
};
typedef struct _Action Action;

static void
action_dtor_impl(void* action_ptr, void* storage_ptr) {
	Storage* storage = (Storage*)storage_ptr;
	Action* action = (Action*)action_ptr;

	if (action->forwards.active) {
		storage_decrease_refcount(storage, action->forwards.id);
	}

	if (action->backwards.active) {
		storage_decrease_refcount(storage, action->backwards.id);
	}
}

History
history_create(Storage* storage) {
	return (History){
		.storage = storage,
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

void
history_record_inserted(History* history, ContactId id) {
	storage_increase_refcount(history->storage, id);
	Action action = {
		.forwards = (OptionalContactId){true, id},
	};
	circular_buffer_push_back(&history->actions, SPANOF(action));
}

void
history_record_deleted(History* history, ContactId id) {
	storage_increase_refcount(history->storage, id);
	Action action = {
		.backwards = (OptionalContactId){true, id},
	};
	circular_buffer_push_back(&history->actions, SPANOF(action));
}

void
history_record_updated(History* history, ContactId old_id, ContactId new_id) {
	storage_increase_refcount(history->storage, new_id);
	storage_increase_refcount(history->storage, old_id);
	Action action = {
		.forwards = (OptionalContactId){true, new_id},
		.backwards = (OptionalContactId){true, old_id},
	};
	circular_buffer_push_back(&history->actions, SPANOF(action));
}
