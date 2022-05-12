#include "wlock.h"

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>

// This file implements a write-preferring lock.
//
// To accomplish this, it keeps track of the amount of readers and writers that
// want to take the lock.
//
// A reader may only take the lock while there is no writer holding the lock, and
// no writers waiting to take it.
//
// A writer may take the lock while the lock is not being held (by either a reader
// or a writer).

struct wlock {
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
	// - the variables are only read and modified when 'lock' is taken
};

struct wlock* wlock_create() {
	struct wlock* lock = malloc(sizeof(*lock));
	lock->writing = lock->reading = 0;
	lock->waiting_readers = lock->waiting_writers = 0;
	pthread_cond_init(&lock->writer_cond, NULL);
	pthread_cond_init(&lock->reader_cond, NULL);
	pthread_mutex_init(&lock->lock, NULL);
	return lock;
}

void wlock_destroy(struct wlock* lock) {
	pthread_cond_destroy(&lock->writer_cond);
	pthread_cond_destroy(&lock->reader_cond);
	pthread_mutex_destroy(&lock->lock);
	free(lock);
}

void wlock_lock_writer(struct wlock* lock) {
	pthread_mutex_lock(&lock->lock);

	// a writer tries to take the lock

	if (lock->reading > 0 || lock->writing > 0) {
		// if the lock is held, we wait until it is free again
		lock->waiting_writers += 1;
		while (lock->reading > 0 || lock->writing > 0)
			pthread_cond_wait(&lock->writer_cond, &lock->lock);
		lock->waiting_writers -= 1;
	}

	// when the lock is free, we take it by updating the writer counter
	lock->writing += 1;

	pthread_mutex_unlock(&lock->lock);
}

void wlock_lock_reader(struct wlock* lock) {
	pthread_mutex_lock(&lock->lock);

	// a reader tries to take the lock

	if (lock->writing || lock->waiting_writers > 0) {
		// if the lock is held by a writer, or the are writers waiting to hold it, we wait it is ready to be taken
		lock->waiting_readers += 1;
		while (lock->writing || lock->waiting_writers > 0)
			pthread_cond_wait(&lock->reader_cond, &lock->lock);
		lock->waiting_readers -= 1;
	}

	// when the lock is ready to be taken, we take it by updating the reader counter
	lock->reading += 1;

	pthread_mutex_unlock(&lock->lock);
}

static void awaken(struct wlock* lock) {
	if (lock->waiting_writers != 0) {
		// if there are any pending writers, we awaken one of then
		pthread_cond_signal(&lock->writer_cond);
	} else if (lock->waiting_readers != 0) {
		// if there are no pending writers, but there are pending readers, we
		// awaken all of then
		pthread_cond_broadcast(&lock->reader_cond);
	}

}

void wlock_unlock_writer(struct wlock* lock) {
	pthread_mutex_lock(&lock->lock);

	// a writer drops the lock
	lock->writing -= 1;

	assert(lock->writing == 0);
	awaken(lock);

	pthread_mutex_unlock(&lock->lock);
}

void wlock_unlock_reader(struct wlock* lock) {
	pthread_mutex_lock(&lock->lock);

	lock->reading -= 1;

	if (lock->reading == 0) {
		awaken(lock);
	}

	pthread_mutex_unlock(&lock->lock);
}
