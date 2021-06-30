#pragma once

#include "contact.h"
#include "span.h"

// Arbol AVL, guarda Ids de contacto

struct _BstNode {
	struct _BstNode* lhs;
	struct _BstNode* rhs;
	int height;

	ContactId data;
};
typedef struct _BstNode BstNode;

struct _Bst {
	struct _BstNode* root;
	Comparator cmp;
	Destructor dtor;
};
typedef struct _Bst Bst;

struct _BstInsertResult {
	bool success;
	struct _BstNode* position;
};
typedef struct _BstInsertResult BstInsertResult;


Bst
bst_create(Comparator cmp, Destructor dtor);

BstInsertResult
bst_insert(Bst* tree, ContactId datum);

BstNode*
bst_find(Bst tree, ContactId datum);

void
bst_erase(Bst* tree, ContactId datum);

void
bst_release(Bst* tree);
