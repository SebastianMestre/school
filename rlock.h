#pragma once

#include <pthread.h>
#include <stdbool.h>

typedef struct reader_lock {
	bool writing;
	int active_readers;

	int buffered_readers;
	int buffered_writers;

	pthread_cond_t writer_cond;
	pthread_cond_t reader_cond;

	pthread_mutex_t lock;
} reader_lock_t;

void reader_lock_init(reader_lock_t* rlock);

void reader_lock_take_writer(reader_lock_t* rlock);
void reader_lock_take_reader(reader_lock_t* rlock);
void reader_lock_drop_writer(reader_lock_t* rlock);
void reader_lock_drop_reader(reader_lock_t* rlock);
