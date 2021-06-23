#include "bst.h"

#include <assert.h>
#include <stdlib.h>

typedef struct _BstNode Node;

static int height(Node* node);

static int
recompute_height_shallow(Node* node) {
	if (node == nullptr) {
		return 0;
	}

	int lhs_height = height(node->lhs);
	int rhs_height = height(node->rhs);
	int max_height = lhs_height < rhs_height ? rhs_height : lhs_height;

	return node->height = max_height + 1;
}

static int
height(Node* node) {
	if (node == nullptr) {
		return 0;
	}
	return node->height;
}

static int
balance_factor(Node* node) {
	if (node == nullptr) {
		return 0;
	}
	return height(node->rhs) - height(node->lhs);
}

static Node*
create(Node* lhs, Node* rhs, void* datum) {
	Node* result = malloc(sizeof(Node));
	*result = (Node) {
		.lhs = lhs,
		.rhs = rhs,
		.datum = datum,
	};
	return result;
}

static void
rot_left(Node** node) {
	Node* old_root = *node;
	Node* new_root = old_root->rhs;

	old_root->rhs = new_root->lhs;
	new_root->lhs = old_root;

	recompute_height_shallow(old_root);
	recompute_height_shallow(new_root);

	*node = new_root;
}

static void
rot_right(Node** node) {
	Node* old_root = *node;
	Node* new_root = old_root->lhs;

	old_root->lhs = new_root->rhs;
	new_root->rhs = old_root;

	recompute_height_shallow(old_root);
	recompute_height_shallow(new_root);

	*node = new_root;
}

static void
rebalance(Node** node, int cmp_result) {

	assert(node != nullptr);
	assert(*node != nullptr);

	assert(-3 < balance_factor(*node));
	assert(balance_factor(*node) <  3);

	assert(cmp_result != 0);

	Node* old_root = *node;
	if (cmp_result < 0) {
		if (balance_factor(old_root) == -2) {
			if (balance_factor(old_root->lhs) == 1) {
				rot_left(&old_root->lhs);
			}
			rot_right(node);
		}
	} else {
		if (balance_factor(old_root) == 2) {
			if (balance_factor(old_root->rhs) == -1) {
				rot_right(&old_root->rhs);
			}
			rot_left(node);
		}
	}
}

static BstInsertResult
insert(Node** node, void* datum, Comparator cmp) {
	assert(node);

	if (*node == nullptr) {
		*node = create(nullptr, nullptr, datum);
		return (BstInsertResult) {
			.success = true,
			.position = *node,
		};
	}

	int const cmp_result = cmp(datum, (*node)->datum);

	if (cmp_result == 0) {
		return (BstInsertResult) {
			.success = false,
			.position = *node,
		};
	}

	Node** next = cmp_result < 0 ? &(*node)->lhs : &(*node)->rhs;

	BstInsertResult result = insert(next, datum, cmp);

	if (!result.success) {
		return result;
	}

	recompute_height_shallow(*node);
	rebalance(node, cmp_result);

	return result;
}

static Node*
find(Node* node, void const* datum, Comparator cmp) {
	if (node == nullptr) {
		return nullptr;
	}

	int cmp_result = cmp(datum, node->datum);

	if (cmp_result == 0) {
		return node;
	}

	Node* next = cmp_result < 0 ? node->lhs : node->rhs;

	return find(next, datum, cmp);
}


Bst
bst_create(Comparator comparator) {
	assert(comparator != nullptr);
	return (Bst) {
		.root = nullptr,
		.comparator = comparator
	};
}

BstInsertResult
bst_insert(Bst* bst, void* datum) {
	assert(bst != nullptr);
	return insert(&bst->root, datum, bst->comparator);
}

Node*
bst_find(Bst const bst, void const* datum) {
	return find(bst.root, datum, bst.comparator);
}

