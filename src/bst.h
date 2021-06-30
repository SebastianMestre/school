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

// inserta un elemento. Si ya hay uno igual segun el comparador, no inserta
// y devuelve false, con un puntero al nodo ya existente
// (en caso contrario devuelve true y un puntero al nuevo nodo)
BstInsertResult
bst_insert(Bst* tree, ContactId datum);

// encuentra el elemento que sea igual a datum, segun el comparador
// si no hay uno, devuelve NULL
BstNode*
bst_find(Bst tree, ContactId datum);

// elimina el elemento que sea igual a datum, segun el comparador
void
bst_erase(Bst* tree, ContactId datum);

// libera los nodos el arbol, y llama el destructor para cada elemento
void
bst_release(Bst* tree);
