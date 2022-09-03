#pragma once

struct list {
	struct list* next;
	struct list* prev;
};

void list_init(struct list* list);
void list_push_start(struct list* list, struct list* node);
void list_remove(struct list* node);
