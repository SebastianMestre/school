#include "storage.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static void
dtor_impl(void* arg0, void* metadata) {
	assert(metadata == nullptr);
	Contact* contact = arg0;
	free(contact->name);
	free(contact->surname);
	free(contact->phone_number);
}

Storage
storage_create() {
	return (Storage){
		.slot_map = slot_map_create(
			sizeof(Contact),
			(Destructor){ dtor_impl, nullptr }
		),
	};
}

void
storage_release(Storage* storage) {
	if (storage->slot_map.cells.size > 0) {
		fprintf(stderr, "No se limpio correctamente el storage.\n");
	}
	slot_map_release(&storage->slot_map);
}

Contact*
storage_at(Storage* storage, ContactId id) {
	return (Contact*)slot_map_at(&storage->slot_map, id).begin;
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

	return slot_map_insert(&storage->slot_map, SPANOF(to_insert));
}

void
storage_delete(Storage* storage, ContactId id) {
	slot_map_delete(&storage->slot_map, id);
}

void
storage_increase_refcount(Storage* storage, ContactId id) {
	slot_map_increase_refcount(&storage->slot_map, id);
}

void
storage_decrease_refcount(Storage* storage, ContactId id) {
	slot_map_decrease_refcount(&storage->slot_map, id);
}

static void
call_with_span_ptr(void* arg, void* metadata) {
	Callback* cb = metadata;
	Span* span = arg;
	call_cb(*cb, span->begin);
}

void
storage_for_each(Storage* storage, Callback cb) {
	slot_map_for_each(&storage->slot_map, (Callback){call_with_span_ptr, &cb});
}


char const*
storage_get_name(Storage* storage, ContactId id) {
	return storage_at(storage, id)->name;
}

char const*
storage_get_surname(Storage* storage, ContactId id) {
	return storage_at(storage, id)->name;
}

char const*
storage_get_phone_number(Storage* storage, ContactId id) {
	return storage_at(storage, id)->phone_number;
}

uint32_t
storage_get_age(Storage* storage, ContactId id) {
	return storage_at(storage, id)->age;
}

bool
storage_is_indexed(Storage* storage, ContactId id) {
	return storage_at(storage, id)->indexed;
}

void
storage_mark_indexed(Storage* storage, ContactId id) {
	storage_at(storage, id)->indexed = true;
}

void
storage_mark_not_indexed(Storage* storage, ContactId id) {
	storage_at(storage, id)->indexed = false;
}
