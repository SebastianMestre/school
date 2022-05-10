#include "cond.h"

#include <pthread.h>
#include <semaphore.h>

struct cond {
	int waiting;
	sem_t sem;
	sem_t bar;
	pthread_mutex_t m;
};

void cond_wait(struct cond* cond, pthread_mutex_t* m) {
	pthread_mutex_lock(&cond->m);
	cond->waiting += 1;
	pthread_mutex_unlock(&cond->m);

	pthread_mutex_unlock(m);
	sem_wait(&cond->sem);
	sem_post(&cond->bar);
	pthread_mutex_lock(m);
}

void cond_signal(struct cond* cond) {
	pthread_mutex_lock(&cond->m);

	if (cond->waiting > 0) {
		sem_post(&cond->sem);
		sem_wait(&cond->bar);
		cond->waiting -= 1;
	}

	pthread_mutex_unlock(&cond->m);
}

void cond_broadcast(struct cond* cond) {
	pthread_mutex_lock(&cond->m);

	for (int i = cond->waiting; i--;) sem_post(&cond->sem);
	for (int i = cond->waiting; i--;) sem_wait(&cond->bar);
	cond->waiting = 0;

	pthread_mutex_unlock(&cond->m);
}

