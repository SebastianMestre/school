#include "index.h"

#include "storage.h"

#include <string.h>
#include <assert.h>

static int
cmp_impl(void const* arg0, void const* arg1, void* metadata) {
	ContactId lhs = *(ContactId const*)arg0;
	ContactId rhs = *(ContactId const*)arg1;
	Storage* storage = metadata;

	int name_cmp_result = strcmp(
		storage_get_name(storage, lhs),
		storage_get_name(storage, rhs));

	if (name_cmp_result != 0)
		return name_cmp_result;

	int surname_cmp_result = strcmp(
		storage_get_surname(storage, lhs),
		storage_get_surname(storage, rhs));

	if (surname_cmp_result != 0)
		return surname_cmp_result;

	return 0;
}

static void
dtor_impl(void* arg0, void* metadata) {
	ContactId id = *(ContactId*)arg0;
	Storage* storage = metadata;

	storage_mark_not_indexed(storage, id);
	storage_decrease_refcount(storage, id);
}

Index
index_create(Storage* storage) {
	return (Index){
		.storage = storage,
		.bst = bst_create(
			(Comparator){ cmp_impl, storage },
			(Destructor){ dtor_impl, storage }
		),
	};
}

void
index_release(Index* index) {
	bst_release(&index->bst);
}

bool
index_insert(Index* index, ContactId id) {
	assert(!storage_is_indexed(index->storage, id));

	storage_increase_refcount(index->storage, id);
	storage_mark_indexed(index->storage, id);

	BstInsertResult insert_result = bst_insert(&index->bst, id);
	if (!insert_result.success) {
		storage_mark_not_indexed(index->storage, id);
		storage_decrease_refcount(index->storage, id);
	}
	return insert_result.success;
}

void
index_delete(Index* index, ContactId id) {
	bst_erase(&index->bst, id);
}

OptionalContactId
index_find(Index index, ContactId id) {
	BstNode* node = bst_find(index.bst, id);
	if (node == nullptr) {
		return (OptionalContactId){false, 0};
	}
	ContactId result = node->data;
	return (OptionalContactId){true, result};
}
