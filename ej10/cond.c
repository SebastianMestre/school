#include "cond.h"

#include <pthread.h>
#include <semaphore.h>

// This file implements condition variables (CV).
//
// To acomplish this, it uses a mutex lock, a counter, and two semaphores.
//
// The counter keeps track of how many threads are waiting on the condition, and
// the mutex lock is used to keep this counter atomic.
//
// The first semaphore, here called `sem`, ensures that the right amount of
// threads are awakened on each signal or broadcast.
//
// To expand on this, each thread that waits on the CV will bump the counter and
// wait on `sem`. Then, on broadcast, the broadcasting thread will post `sem` as
// many times as indicated by the counter, and decrement the counter accordingly.
//
// The posting of `sem` will awaken the threads that were waiting on it, so they
// can continue their execution.
//
// Note 1: In `cond_sleep`, the releasing of the given mutex, and the waiting
// `sem` are not performed atomically, but this is actually OK.
//
// Note 2: In `cond_sleep`, the awakening from `sem` and taking of the given lock
// are not performed atomically, but this is actually OK.
//
// Note 3: As described, the algorithm has a subtle bug that I learned of after
// reading this paper: http://birrell.org/andrew/papers/ImplementingCVs.pdf
//
// Let's say `n` threads that called wait are suspended just before waiting on
// `sem` (so the counter must be `n`).
//
// Then suppose another thread calls broadcast (so `sem` gets posted `n` times),
// but none of the `n` threads awakens yet. Instead, yet another thread calls
// sleep, and manages to wait on `sem`.
//
// At this point, that thread will be able to continue executing, decrement the
// semaphore, and leave one of the original `n` threads still waiting.
//
// This is clearly wrong, so we add another semaphore (named `bar`, due to it
// acting similarly to a barrier) to ensure that the broadcasting thread will only
// advance once all the waiting threads have resumed their execution.
//
// This makes the broadcasting thread hold the counter lock for as long as it
// takes for the waiting threads to awaken, which blocks any other threads from
// waiting on the CV, preventing the bug.

struct cond {
	int waiting;
	sem_t sem;
	sem_t bar;
	pthread_mutex_t m;
};

struct cond* cond_create() {
	struct cond* cond = malloc(sizeof(*cond));
	cond->waiting = 0;
	sem_init(&cond->sem, 0, 0);
	sem_init(&cond->bar, 0, 0);
	pthread_mutex_init(&cond->m, NULL);
	return cond;
}

void cond_wait(struct cond* cond, pthread_mutex_t* m) {
	pthread_mutex_lock(&cond->m);
	cond->waiting += 1;
	pthread_mutex_unlock(&cond->m);

	pthread_mutex_unlock(m);

	// ---- This is the problematic point ----

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

	for (int i = cond->waiting; i--;) {
		sem_post(&cond->sem);
	}

	for (int i = cond->waiting; i--;) {
		cond->waiting -= 1;
		sem_wait(&cond->bar);
	}

	pthread_mutex_unlock(&cond->m);
}

