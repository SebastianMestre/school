#pragma once

struct _BstNode {
	struct _BstNode* lhs;
	struct _BstNode* rhs;
	void const* datum;
	int height;
};
typedef struct _BstNode BstNode;

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
struct _BstNode* bst_find(Bst tree, void const* datum);

