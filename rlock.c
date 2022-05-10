#include "rlock.h"

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

void reader_lock_init(reader_lock_t* rlock) {
	rlock->writing = false;
	rlock->active_readers = 0;
	rlock->buffered_readers = 0;
	rlock->buffered_writers = 0;
	pthread_cond_init(&rlock->writer_cond, NULL);
	pthread_cond_init(&rlock->reader_cond, NULL);
	pthread_mutex_init(&rlock->lock, NULL);
}

void reader_lock_take_writer(reader_lock_t* rlock) {
	pthread_mutex_lock(&rlock->lock);

	// si no puedo agarrar el lock, entonces espero y anoto que estoy esperando.
	if (rlock->active_readers > 0 || rlock->writing) {
		rlock->buffered_writers += 1;
		while (rlock->active_readers > 0 || rlock->writing)
			pthread_cond_wait(&rlock->writer_cond, &rlock->lock);
		rlock->buffered_writers -= 1;
	}

	rlock->writing = true;

	pthread_mutex_unlock(&rlock->lock);
}

void reader_lock_take_reader(reader_lock_t* rlock) {
	pthread_mutex_lock(&rlock->lock);

	if (rlock->writing) {
		rlock->buffered_readers += 1;
		while (rlock->writing)
			pthread_cond_wait(&rlock->reader_cond, &rlock->lock);
		rlock->buffered_readers -= 1;
	}

	rlock->active_readers += 1;

	pthread_mutex_unlock(&rlock->lock);
}

void reader_lock_drop_writer(reader_lock_t* rlock) {
	pthread_mutex_lock(&rlock->lock);

	rlock->writing = false;

	if (rlock->buffered_readers != 0) {
		pthread_cond_broadcast(&rlock->reader_cond);
	} else if (rlock->buffered_writers != 0) {
		pthread_cond_signal(&rlock->writer_cond);
	}

	pthread_mutex_unlock(&rlock->lock);
}

void reader_lock_drop_reader(reader_lock_t* rlock) {
	pthread_mutex_lock(&rlock->lock);

	rlock->active_readers -= 1;

	if (rlock->active_readers == 0) {
		if (rlock->buffered_writers != 0) {
			pthread_cond_signal(&rlock->writer_cond);
		}
	}

	pthread_mutex_unlock(&rlock->lock);
}
