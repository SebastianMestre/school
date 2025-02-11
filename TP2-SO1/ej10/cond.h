#pragma once

#include <pthread.h>

struct cond;

void cond_wait(struct cond*, pthread_mutex_t*);
void cond_signal(struct cond*);
void cond_broadcast(struct cond*);
