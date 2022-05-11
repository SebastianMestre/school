#include "rlock.h"

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

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

void rlock_lock_writer(struct rlock* rlock) {
	pthread_mutex_lock(&rlock->lock);

	// si no puedo agarrar el lock, entonces espero y anoto que estoy esperando.
	if (rlock->reading > 0 || rlock->writing) {
		rlock->waiting_writers += 1;
		while (rlock->reading > 0 || rlock->writing)
			pthread_cond_wait(&rlock->writer_cond, &rlock->lock);
		rlock->waiting_writers -= 1;
	}

	rlock->writing += 1;

	pthread_mutex_unlock(&rlock->lock);
}

void rlock_lock_reader(struct rlock* rlock) {
	pthread_mutex_lock(&rlock->lock);

	if (rlock->writing) {
		rlock->waiting_readers += 1;
		while (rlock->writing)
			pthread_cond_wait(&rlock->reader_cond, &rlock->lock);
		rlock->waiting_readers -= 1;
	}

	rlock->reading += 1;

	pthread_mutex_unlock(&rlock->lock);
}

void rlock_unlock_writer(struct rlock* rlock) {
	pthread_mutex_lock(&rlock->lock);

	rlock->writing -= 1;

	if (rlock->waiting_readers != 0) {
		pthread_cond_broadcast(&rlock->reader_cond);
	} else if (rlock->waiting_writers != 0) {
		pthread_cond_signal(&rlock->writer_cond);
	}

	pthread_mutex_unlock(&rlock->lock);
}

void rlock_unlock_reader(struct rlock* rlock) {
	pthread_mutex_lock(&rlock->lock);

	rlock->reading -= 1;

	if (rlock->reading == 0) {
		if (rlock->waiting_writers != 0) {
			pthread_cond_signal(&rlock->writer_cond);
		}
	}

	pthread_mutex_unlock(&rlock->lock);
}
