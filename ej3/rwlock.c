#include "rwlock.h"

#include "queue.h"

#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

// This file implements a fair, order-preserving, reader-writer lock.
//
// To accomplish this, it keeps a queue of readers and writers that want to take
// the lock.
//
// A reader may only take the lock while there is no writer holding the lock, and
// no writers waiting to take it.
//
// A writer may take the lock while the lock is not being held (by either a reader
// or a writer).
//
// In any other case, the thread that wants to take the lock creates a condition
// variable, pushes into the queue, and waits on it.
//
// Once the lock is dropped, the thread that drops it will remove one entry from
// the queue, and signal the condition variable that is held in it.

struct rwlock {
	// amount of readers/writers that are holding the lock
	int reading;
	int writing;

	// queue that holds all the threads that want to take the lock
	struct queue* queue;

	pthread_mutex_t lock;

	// invariants:
	// - 0 <= reading
	// - 0 <= writing <= 1
	// - if the queue is not empty, then the lock is held by a writer, or there are pending writers.
	// - the variables are only modified when `lock` is taken
};

struct queue_entry {
	pthread_cond_t cond;
	char type;
};

struct rwlock* rwlock_create() {
	struct rwlock* lock = malloc(sizeof(*lock));
	lock->reading = lock->writing = 0;
	lock->queue = queue_create();
	pthread_mutex_init(&lock->lock, NULL);
	return lock;
}

void rwlock_destroy(struct rwlock* lock) {
	pthread_mutex_destroy(&lock->lock);
	queue_destroy(lock->queue);
	free(lock);
}

// PRE: lock->lock esta tomado
static void wait_for_turn(struct rwlock* lock, char type) {
	struct queue_entry* entry = malloc(sizeof(*entry));
	entry->type = type;
	pthread_cond_init(&entry->cond, NULL);

	queue_push(lock->queue, entry);

	pthread_cond_wait(&entry->cond, &lock->lock);

	pthread_cond_destroy(&entry->cond);
	free(entry);
}

// PRE: lock->lock esta tomado
static void end_turn(struct rwlock* lock) {
	if (queue_empty(lock->queue))
		return;

	struct queue_entry* entry = queue_pop(lock->queue);
	char entry_type = entry->type;

	pthread_cond_broadcast(&entry->cond);
	if (entry_type == 'R')
		lock->reading += 1;
	else
		lock->writing += 1;

	if (entry_type == 'R') {
		while (!queue_empty(lock->queue)) {
			entry = queue_peek(lock->queue);
			if (entry->type != 'R')
				break;
			queue_pop(lock->queue);
			pthread_cond_broadcast(&entry->cond);
			lock->reading += 1;
		}
	}
}

void rwlock_unlock_reader(struct rwlock* lock) {
	assert(lock);
	pthread_mutex_lock(&lock->lock);

	lock->reading -= 1;
	if (lock->reading == 0)
		end_turn(lock);

	pthread_mutex_unlock(&lock->lock);
}

void rwlock_unlock_writer(struct rwlock* lock) {
	assert(lock);
	pthread_mutex_lock(&lock->lock);

	lock->writing -= 1;
	assert(lock->writing == 0);
	end_turn(lock);

	pthread_mutex_unlock(&lock->lock);
}

void rwlock_lock_reader(struct rwlock* lock) {
	assert(lock);
	pthread_mutex_lock(&lock->lock);

	if (lock->writing != 0 || !queue_empty(lock->queue))
		wait_for_turn(lock, 'R');
	else
		lock->reading += 1;

	pthread_mutex_unlock(&lock->lock);
}

void rwlock_lock_writer(struct rwlock* lock) {
	assert(lock);
	pthread_mutex_lock(&lock->lock);

	if (lock->writing != 0 || lock->reading != 0 || !queue_empty(lock->queue))
		wait_for_turn(lock, 'W');
	else 
		lock->writing += 1;

	pthread_mutex_unlock(&lock->lock);
}
