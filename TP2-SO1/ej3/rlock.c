#include "rlock.h"

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>

// This file implements a read-preferring lock.
//
// To accomplish this, it keeps track of the amount of readers and writers that
// want to take the lock.
//
// A reader may take the lock while there is no writer holding the lock.
//
// A writer may take the lock while the lock is not being held (by either a reader
// or a writer), and there are no readers waiting to take it.

struct rlock {
	// amount of readers/writers that are holding the lock
	int reading;
	int writing;

	// amount of readers/writers that are waiting to take the lock
	int waiting_readers;
	int waiting_writers;

	// fires when writing == 0 and waiting_writers > 0
	pthread_cond_t writer_cond;

	// fires when writing == 0 and waiting_writers == 0 and waiting_readers > 0
	pthread_cond_t reader_cond;

	pthread_mutex_t lock;

	// invariants:
	// - 0 <= reading
	// - 0 <= writing <= 1
	// - 0 <= waiting_readers
	// - 0 <= waiting_writers
	// - the variables are only modified when 'lock' is taken
};

struct rlock* rlock_create() {
	struct rlock* lock = malloc(sizeof(*lock));
	lock->writing = lock->reading = 0;
	lock->waiting_readers = lock->waiting_writers = 0;
	pthread_cond_init(&lock->writer_cond, NULL);
	pthread_cond_init(&lock->reader_cond, NULL);
	pthread_mutex_init(&lock->lock, NULL);
	return lock;
}

void rlock_destroy(struct rlock* lock) {
	pthread_cond_destroy(&lock->writer_cond);
	pthread_cond_destroy(&lock->reader_cond);
	pthread_mutex_destroy(&lock->lock);
	free(lock);
}

void rlock_lock_writer(struct rlock* lock) {
	pthread_mutex_lock(&lock->lock);

	// si no puedo agarrar el lock, entonces espero y anoto que estoy esperando.
	if (lock->reading > 0 || lock->writing) {
		lock->waiting_writers += 1;
		while (lock->reading > 0 || lock->writing)
			pthread_cond_wait(&lock->writer_cond, &lock->lock);
		lock->waiting_writers -= 1;
	}

	lock->writing += 1;

	pthread_mutex_unlock(&lock->lock);
}

void rlock_lock_reader(struct rlock* lock) {
	pthread_mutex_lock(&lock->lock);

	if (lock->writing) {
		lock->waiting_readers += 1;
		while (lock->writing)
			pthread_cond_wait(&lock->reader_cond, &lock->lock);
		lock->waiting_readers -= 1;
	}

	lock->reading += 1;

	pthread_mutex_unlock(&lock->lock);
}

static void awaken(struct rlock* lock) {
	if (lock->waiting_readers != 0) {
		pthread_cond_broadcast(&lock->reader_cond);
	} else if (lock->waiting_writers != 0) {
		pthread_cond_signal(&lock->writer_cond);
	}
}

void rlock_unlock_writer(struct rlock* lock) {
	pthread_mutex_lock(&lock->lock);

	lock->writing -= 1;

	assert(lock->writing == 0);
	awaken(lock);

	pthread_mutex_unlock(&lock->lock);
}

void rlock_unlock_reader(struct rlock* lock) {
	pthread_mutex_lock(&lock->lock);

	lock->reading -= 1;

	if (lock->reading == 0) {
		awaken(lock);
	}

	pthread_mutex_unlock(&lock->lock);
}
