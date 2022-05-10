#include "fair_rwlock.h"

#include "queue.h"

#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

struct fair_rwlock {
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

struct fair_rwlock* fair_rwlock_create() {
	struct fair_rwlock* lock = malloc(sizeof(*lock));
	lock->reading = lock->writing = 0;
	lock->queue = queue_create();
	pthread_mutex_init(&lock->m, NULL);
	return lock;
}

void fair_rwlock_destroy(struct fair_rwlock* lock) {
	pthread_mutex_destroy(&lock->m);
	queue_destroy(lock->queue);
	free(lock);
}

static void wait_for_turn(struct fair_rwlock* lock, char type) {
	struct queue_entry* entry = malloc(sizeof(*entry));
	entry->type = type;
	pthread_cond_init(&entry->cond, NULL);

	queue_push(lock->queue, entry);

	pthread_cond_wait(&entry->cond, &lock->m);

	pthread_cond_destroy(&entry->cond);
	free(entry);
}

static void end_turn(struct fair_rwlock* lock) {
	if (queue_empty(lock->queue))
		return;

	struct queue_entry* entry = queue_pop(lock->queue);
	char entry_type = entry->type;

	pthread_cond_broadcast(&entry->cond);

	if (entry_type == 'W') {
		while (1) {
			entry = queue_peek(lock->queue);
			if (entry->type != 'W')
				break;
			queue_pop(lock->queue);
			pthread_cond_broadcast(&entry->cond);
		}
	}
}

void unlock_reader(struct fair_rwlock* lock) {
	assert(lock);
	pthread_mutex_lock(&lock->m);
	lock->reading -= 1;
	if (lock->reading == 0)
		end_turn(lock);
	pthread_mutex_unlock(&lock->m);
}

void unlock_writer(struct fair_rwlock* lock) {
	assert(lock);
	pthread_mutex_lock(&lock->m);
	lock->writing -= 1;
	end_turn(lock);
	pthread_mutex_unlock(&lock->m);
}

void lock_reader(struct fair_rwlock* lock) {
	assert(lock);
	pthread_mutex_lock(&lock->m);
	if (lock->writing == 0) {
		lock->reading += 1;
	} else {
		wait_for_turn(lock, 'R');
	}
	pthread_mutex_unlock(&lock->m);
}

void lock_writer(struct fair_rwlock* lock) {
	assert(lock);
	pthread_mutex_lock(&lock->m);
	if (lock->writing == 0 && lock->reading == 0) {
		lock->writing += 1;
	} else {
		wait_for_turn(lock, 'W');
	}
	pthread_mutex_unlock(&lock->m);
}
