#include "queue.h"

#include <stdlib.h>
#include <stdbool.h>

struct node {
	struct node* next;
	void* data;
};

struct queue {
	struct node* first;
	struct node* last;
};

struct queue* queue_create() {
	struct queue* queue;
	queue->first = queue->last = NULL;
	return queue;
}

void queue_destroy(struct queue* queue) {
	while (!queue_empty(queue)) {
		queue_pop(queue);
	}
}

void queue_push(struct queue* queue, void* data) {
	struct node* node = malloc(sizeof(*node));
	node->next = NULL;
	node->data = data;
	if (queue->last == NULL) {
		queue->first = node;
	} else {
		queue->last->next = node;
	}
	queue->last = node;
}

void* queue_peek(struct queue* queue) {
	return queue->first->data;
}

void* queue_pop(struct queue* queue) {
	void* data = queue->first->data;
	struct node* to_free = queue->first;
	queue->first = queue->first->next;
	if (queue->first == NULL) {
		queue->last = NULL;
	}
	free(to_free);
	return data;
}

bool queue_empty(struct queue* queue) {
	return queue->first == NULL;
}

