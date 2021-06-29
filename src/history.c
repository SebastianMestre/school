#include "history.h"

#include "storage.h"

#include <assert.h>

#define HISTORY_SIZE (10)

typedef HistoryEvent Event;

static void
dtor_impl(void* action_ptr, void* storage_ptr) {
	Storage* storage = (Storage*)storage_ptr;
	Event* action = (Event*)action_ptr;

	if (action->forwards.active) {
		storage_decrease_refcount(storage, action->forwards.id);
	}

	if (action->backwards.active) {
		storage_decrease_refcount(storage, action->backwards.id);
	}
}

History
history_create(Storage* storage) {
	History result = {
		.storage = storage,
		.actions = circular_buffer_create(
			sizeof(Event),
			HISTORY_SIZE,
			(Destructor){
				dtor_impl,
				storage
			}
		),
		.next_action = nullptr,
	};
	result.next_action = result.actions.begin;
	return result;
}

void
history_release(History* history) {
	circular_buffer_release(&history->actions);
}

void
history_clear(History* history) {
	circular_buffer_clear(&history->actions);
}

void
history_clear_future(History* history) {
	if (history->actions.size == 0)
		return;

	while (history->next_action != history->actions.end) {
		circular_buffer_pop_back(&history->actions);
	}
}

void
history_record_inserted(History* history, ContactId id) {
	storage_increase_refcount(history->storage, id);
	Event action = {
		.forwards = (OptionalContactId){true, id},
		.backwards = {},
	};
	circular_buffer_push_back(&history->actions, SPANOF(action));
}

void
history_record_deleted(History* history, ContactId id) {
	storage_increase_refcount(history->storage, id);
	Event action = {
		.backwards = (OptionalContactId){true, id},
	};
	circular_buffer_push_back(&history->actions, SPANOF(action));
}

void
history_record_updated(History* history, ContactId old_id, ContactId new_id) {
	storage_increase_refcount(history->storage, new_id);
	storage_increase_refcount(history->storage, old_id);
	Event action = {
		.forwards = (OptionalContactId){true, new_id},
		.backwards = (OptionalContactId){true, old_id},
	};
	circular_buffer_push_back(&history->actions, SPANOF(action));
}

void history_advance_cursor(History* history) {
	assert(!history_cursor_at_end(history));

	history->next_action += sizeof(Event);
	if (history->next_action == history->actions.data.end) {
		history->next_action = history->actions.data.begin;
	}
}

void history_retreat_cursor(History* history) {
	assert(!history_cursor_at_begin(history));

	if (history->next_action == history->actions.data.begin) {
		history->next_action = history->actions.data.end;
	}
	history->next_action -= sizeof(Event);
}

Event*
history_next_action(History* history) {
	assert(history->next_action != history->actions.end);
	return history->next_action;
}

bool
history_cursor_at_begin(History const* history) {
	return history->next_action == history->actions.begin;
}

bool
history_cursor_at_end(History const* history) {
	return history->next_action == history->actions.end;
}
