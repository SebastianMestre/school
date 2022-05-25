#pragma once
#include <pthread.h>

/********** UNBOUND STACK **********/
typedef struct stack_element_t {
  void *val;
  struct stack_element_t *next;
} stack_element_t;

typedef struct stack_t {
  int size;
  stack_element_t *top;
} stack_t;

void stack_init(stack_t *stack);
void stack_push(stack_t *stack, void *pushed);
void *stack_pop(stack_t *stack);
int  stack_empty(stack_t stack);
void stack_destroy(stack_t stack);

/********** UNBOUND CONCURRENT STACK **********/
typedef struct conc_stack_t {
  stack_t stack;
  pthread_mutex_t mutex;
  pthread_cond_t non_empty;
} conc_stack_t;

void conc_stack_init(conc_stack_t *conc_stack);
int  conc_stack_empty(conc_stack_t conc_stack);
void conc_stack_push(conc_stack_t *conc_stack, void *pushed);
void *conc_stack_pop(conc_stack_t *conc_stack);
void conc_stack_destroy(conc_stack_t conc_stack);

/********** BOUND CONCURRENT QUEUE **********/
typedef struct conc_queue_t {
  int size;
  int count;
  int begin;
  int end;
  void **data;
  pthread_mutex_t mutex;
  pthread_cond_t non_full;
  pthread_cond_t non_empty;
} conc_queue_t;

void conc_queue_init(conc_queue_t *conc_queue, int queue_size);
int conc_queue_empty(conc_queue_t cq);
void conc_queue_push(conc_queue_t *cq, void *pushed);
void *conc_queue_pop(conc_queue_t *cq);
void conc_queue_destroy(conc_queue_t cq);
