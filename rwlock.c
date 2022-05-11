#include "rwlock.h"

#include "queue.h"

#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

struct rwlock {
	// cola
	int reading;
	int writing;

	struct queue* queue;

	pthread_mutex_t m;
};

struct queue_entry {
	pthread_cond_t cond;
	char type;
};

struct rwlock* rwlock_create() {
	struct rwlock* lock = malloc(sizeof(*lock));
	lock->reading = lock->writing = 0;
	lock->queue = queue_create();
	pthread_mutex_init(&lock->m, NULL);
	return lock;
}

void rwlock_destroy(struct rwlock* lock) {
	pthread_mutex_destroy(&lock->m);
	queue_destroy(lock->queue);
	free(lock);
}

// PRE: lock->m esta tomado
static void wait_for_turn(struct rwlock* lock, char type) {
	struct queue_entry* entry = malloc(sizeof(*entry));
	entry->type = type;
	pthread_cond_init(&entry->cond, NULL);

	queue_push(lock->queue, entry);

	pthread_cond_wait(&entry->cond, &lock->m);

	pthread_cond_destroy(&entry->cond);
	free(entry);
}

// PRE: lock->m esta tomado
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
	pthread_mutex_lock(&lock->m);

	lock->reading -= 1;
	if (lock->reading == 0)
		end_turn(lock);

	pthread_mutex_unlock(&lock->m);
}

void rwlock_unlock_writer(struct rwlock* lock) {
	assert(lock);
	pthread_mutex_lock(&lock->m);

	lock->writing -= 1;
	assert(lock->writing == 0);
	end_turn(lock);

	pthread_mutex_unlock(&lock->m);
}

void rwlock_lock_reader(struct rwlock* lock) {
	assert(lock);
	pthread_mutex_lock(&lock->m);

	if (lock->writing != 0 || !queue_empty(lock->queue))
		wait_for_turn(lock, 'R');
	else
		lock->reading += 1;

	pthread_mutex_unlock(&lock->m);
}

void rwlock_lock_writer(struct rwlock* lock) {
	assert(lock);
	pthread_mutex_lock(&lock->m);

	if (lock->writing != 0 || lock->reading != 0 || !queue_empty(lock->queue))
		wait_for_turn(lock, 'W');
	else 
		lock->writing += 1;

	pthread_mutex_unlock(&lock->m);
}
