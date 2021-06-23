#pragma once

#include <stdbool.h>

typedef int (*Comparator)(void const*, void const*);

struct _BstNode {
	struct _BstNode* lhs;
	struct _BstNode* rhs;
	void const* datum;
	int height;
};


struct _Bst {
	struct _BstNode* root;
	Comparator const comparator;
};
typedef struct _Bst Bst;


struct _BstInsertResult {
	bool success;
	struct _BstNode* position;
};
typedef struct _BstInsertResult BstInsertResult;


Bst bst_create(Comparator comparator);

BstInsertResult bst_insert(Bst* tree, void* datum);

