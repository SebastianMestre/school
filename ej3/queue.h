#pragma once

#include <stdbool.h>

struct queue;

struct queue* queue_create();
void queue_destroy(struct queue*);

void queue_push(struct queue*, void*);
void* queue_peek(struct queue*);
void* queue_pop(struct queue*);
bool queue_empty(struct queue*);
