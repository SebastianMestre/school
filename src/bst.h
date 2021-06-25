#pragma once

#include "span.h"

struct _BstNode {
	struct _BstNode* lhs;
	struct _BstNode* rhs;
	int height;
	Span datum;
};
typedef struct _BstNode BstNode;

struct _Bst {
	struct _BstNode* root;
	size_t const element_width;
	Comparator const cmp;
	Destructor const dtor;
};
typedef struct _Bst Bst;

struct _BstInsertResult {
	bool success;
	struct _BstNode* position;
};
typedef struct _BstInsertResult BstInsertResult;


Bst
bst_create(size_t element_width, Comparator cmp, Destructor dtor);

BstInsertResult
bst_insert(Bst* tree, Span datum);

BstNode*
bst_find(Bst tree, Span datum);
