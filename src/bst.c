#include "bst.h"

#include <assert.h>
#include <stdlib.h>

typedef struct _BstNode Node;

struct _InsertResult {
	Node* root;
	Node* position;
	bool success;
};
typedef struct _InsertResult InsertResult;

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

static Node*
rebalance(Node* node) {

	// TODO: implement double rotations.

	int bf = balance_factor(node);

	// assert(bf > -3);
	// assert(bf < 3);

	Node* new_root = node;

	if (bf > 1) {
		Node* new_root = node->rhs;

		Node* t1 = node->lhs;
		Node* t2 = new_root->lhs;
		Node* t3 = new_root->rhs;

		new_root->lhs = node;
		new_root->rhs = t3;
		node->lhs = t1;
		node->rhs = t2;

	} else if (bf < -1) {
		Node* new_root = node->lhs;

		Node* t1 = new_root->lhs;
		Node* t2 = new_root->rhs;
		Node* t3 = node->rhs;

		new_root->lhs = t1;
		new_root->rhs = node;
		node->lhs = t2;
		node->rhs = t3;
	}

	recompute_height_shallow(node);
	recompute_height_shallow(new_root);

	return new_root;
}

static InsertResult
insert(Node* node, void* datum, Comparator cmp) {
	if (node == nullptr) {
		return (InsertResult) {
			.success = true,
			.position = create(nullptr, nullptr, datum),
			.root = node,
		};
	}

	int const cmp_result = cmp(datum, node->datum);

	if (cmp_result == 0) {
		return (InsertResult) {
			.success = false,
			.position = node,
			.root = node,
		};
	}

	bool const go_left = cmp_result < 0;
	Node** next = go_left ? &node->lhs : &node->rhs;
	InsertResult result = insert(*next, datum, cmp);
	*next = result.root;

	result.root = node;

	if (!result.success) {
		return result;
	}

	recompute_height_shallow(node);

	result.root = rebalance(node);

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
	InsertResult result = insert(bst->root, datum, bst->comparator);
	bst->root = result.root;
	return (BstInsertResult) {
		.success = result.success,
		.position = result.position,
	};
}

Node*
bst_find(Bst const bst, void const* datum) {
	return find(bst.root, datum, bst.comparator);
}

