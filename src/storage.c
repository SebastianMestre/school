#include "storage.h"

#include <assert.h>

Storage
storage_create() {
	return (Storage){
		.slot_map = slot_map_create(sizeof(Contact)),
	};
}

Contact*
storage_at(Storage storage, ContactId id) {
	return (Contact*)slot_map_at(&storage.slot_map, id).begin;
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
