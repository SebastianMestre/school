#include "list.h"

void list_init(struct list* list) {
	list->next = list->prev = list;
}

void list_push_start(struct list* list, struct list* node) {
	struct list* next = list->next;

	node->prev = list;
	node->next = next;

	next->prev = node;
	list->next = node;
}

void list_remove(struct list* node) {
	struct list* next = node->next;
	struct list* prev = node->prev;

	prev->next = next;
	next->prev = prev;

	node->next = node->prev = node;
}
