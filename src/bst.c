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
create(Node* lhs, Node* rhs, Span datum) {
	Span buffer = span_malloc(sizeof(Node) + span_width(datum));

	Span node_location = span_slice(buffer, 0, sizeof(Node));
	Span data_location = remove_prefix(buffer, sizeof(Node));

	assert(node_location.end == data_location.begin);

	Node node = {
		.lhs = lhs,
		.rhs = rhs,
		.height = 0,
		.datum = data_location,
	};

	span_write(node_location.begin, SPANOF(node));
	span_write(data_location.begin, datum);

	return buffer.begin;
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
rebalance(Node** node) {

	assert(node != nullptr);
	assert(*node != nullptr);

	assert(-3 < balance_factor(*node));
	assert(balance_factor(*node) <  3);

	Node* old_root = *node;
	int bf = balance_factor(old_root);
	if (bf == -2) {
		assert(balance_factor(old_root->lhs) < 2);
		if (balance_factor(old_root->lhs) == 1) {
			rot_left(&old_root->lhs);
		}
		rot_right(node);
	} else if (bf >= 2) {
		assert(balance_factor(old_root->rhs) > -2);
		if (balance_factor(old_root->rhs) == -1) {
			rot_right(&old_root->rhs);
		}
		rot_left(node);
	}
}

static BstInsertResult
insert(Node** node, Span datum, Comparator cmp) {
	assert(node);

	if (*node == nullptr) {
		*node = create(nullptr, nullptr, datum);
		return (BstInsertResult) {
			.success = true,
			.position = *node,
		};
	}

	int const cmp_result = call_cmp(cmp, datum.begin, (*node)->datum.begin);

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
	rebalance(node);

	return result;
}

static Node*
find(Node* node, Span datum, Comparator cmp) {
	if (node == nullptr) {
		return nullptr;
	}

	int cmp_result = call_cmp(cmp, datum.begin, node->datum.begin);

	if (cmp_result == 0) {
		return node;
	}

	Node* next = cmp_result < 0 ? node->lhs : node->rhs;

	return find(next, datum, cmp);
}

static void
free_node(Node* node, Destructor dtor) { 
	call_dtor(dtor, node->datum.begin);
	free(node);
}

static Node*
steal_leftmost(Node** p) {
	assert(p != nullptr);

	Node* node = *p;
	assert(node != nullptr);

	if (node->lhs == nullptr) {
		*p = node->rhs;

		// sanity: null en el puntero
		node->rhs = nullptr;

		return node;
	} else {
		Node* result = steal_leftmost(&node->lhs);
		recompute_height_shallow(node);
		return result;
	}
}

static void
erase(Node** p, Span datum, Comparator cmp) {
	assert(p != nullptr);

	// no permitimos que se borren cosas que no estan el el arbol
	Node* node = *p;
	assert(node != nullptr);

	int const cmp_result = call_cmp(cmp, datum.begin, node->datum.begin);

	if (cmp_result == 0) {
		Node* lhs = node->lhs;
		Node* rhs = node->rhs;

		// el nodo que se borra es una hoja
		if (lhs == nullptr && rhs == nullptr) {
			*p = nullptr;
		} else if (lhs == nullptr) {
			// el nodo que se borra tiene un subarbol
			*p = rhs;
		} else if (rhs == nullptr) {
			// el nodo que se borra tiene un subarbol
			*p = lhs;
		} else {
			// el nodo que se borra tiene dos subarboles
			Node* succ = steal_leftmost(&node->rhs);

			succ->lhs = node->lhs;
			succ->rhs = node->rhs;
			*p = succ;
		}

		free_node(node, /* TODO */ nop_dtor);
	} else if (cmp_result < 0) {
		erase(&node->lhs, datum, cmp);
	} else {
		erase(&node->rhs, datum, cmp);
	}

	if (*p != nullptr) {
		recompute_height_shallow(*p);
		rebalance(p);
	}
}

/*
Destruimos el arbol binario sin recursionar.

Se rota la raiz hacia la derecha repetidas veces, hasta
no tener un subarbol izquierdo.
Llegado este punto, se elimina la raiz, y se camina hacia
el subarbol derecho (el unico que hay), donde se repite
el proceso.

Complejidad:
tiempo: O(Nodos).
espacio: O(1).
*/
static void
release(Node* node, Destructor dtor) {
	while (node != nullptr) {
		while (node->lhs != nullptr)
			rot_right(&node);

		// no hay subarbol izquierdo, es facil liberar
		Node* rhs = node->rhs;
		call_dtor(dtor, node->datum.begin);
		free(node);
		node = rhs;
	}
}

Bst
bst_create(size_t element_width, Comparator cmp, Destructor dtor) {
	assert(cmp.call != nullptr);
	assert(dtor.call != nullptr);
	return (Bst) {
		.root = nullptr,
		.element_width = element_width,
		.cmp = cmp,
		.dtor = dtor,
	};
}

BstInsertResult
bst_insert(Bst* bst, Span datum) {
	assert(bst != nullptr);
	assert(bst->element_width == span_width(datum));
	return insert(&bst->root, datum, bst->cmp);
}

Node*
bst_find(Bst const bst, Span datum) {
	assert(bst.element_width == span_width(datum));
	return find(bst.root, datum, bst.cmp);
}

void
bst_erase(Bst* bst, Span datum) {
	assert(bst != nullptr);
	assert(bst->element_width == span_width(datum));
	return erase(&bst->root, datum, bst->cmp);
}

void
bst_release(Bst* bst) {
	release(bst->root, bst->dtor);
	bst->root = nullptr;
}
