#include "index.h"

#include "storage.h"

#include <string.h>
#include <assert.h>

static int
cmp_impl(void const* arg0, void const* arg1, void* metadata) {

	ContactId lhs_id = *(ContactId const*)arg0;
	ContactId rhs_id = *(ContactId const*)arg1;
	Storage* storage = metadata;

	Contact* lhs = storage_at(storage, lhs_id);
	Contact* rhs = storage_at(storage, rhs_id);

	assert(lhs);
	assert(rhs);

	int name_cmp_result = strcmp(lhs->name, rhs->name);
	if (name_cmp_result != 0)
		return name_cmp_result;

	int surname_cmp_result = strcmp(lhs->surname, rhs->surname);
	if (surname_cmp_result != 0)
		return surname_cmp_result;

	return 0;
}

static void
dtor_impl(void* arg0, void* metadata) {
	ContactId id = *(ContactId*)arg0;
	Storage* storage = metadata;

	storage_at(storage, id)->indexed = false;
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
	assert(!storage_at(index->storage, id)->indexed);

	storage_increase_refcount(index->storage, id);
	storage_at(index->storage, id)->indexed = true;

	BstInsertResult insert_result = bst_insert(&index->bst, id);
	if (!insert_result.success) {
		storage_at(index->storage, id)->indexed = false;
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
