#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#ifndef MODE
#define MODE 0
#endif

#if MODE == 0

#include "rlock.h"
struct rlock* lock;

void create_lock() { lock = rlock_create(); }
void destroy_lock() { rlock_destroy(lock); }

void lock_reader() { rlock_lock_reader(lock); }
void lock_writer() { rlock_lock_writer(lock); }
void unlock_reader() { rlock_unlock_reader(lock); }
void unlock_writer() { rlock_unlock_writer(lock); }

#elif MODE == 1

#include "wlock.h"
struct wlock* lock;

void create_lock() { lock = wlock_create(); }
void destroy_lock() { wlock_destroy(lock); }

void lock_reader() { wlock_lock_reader(lock); }
void lock_writer() { wlock_lock_writer(lock); }
void unlock_reader() { wlock_unlock_reader(lock); }
void unlock_writer() { wlock_unlock_writer(lock); }


#elif MODE == 2

#include "rwlock.h"
struct rwlock* lock;

void create_lock() { lock = rwlock_create(); }
void destroy_lock() { rwlock_destroy(lock); }

void lock_reader() { rwlock_lock_reader(lock); }
void lock_writer() { rwlock_lock_writer(lock); }
void unlock_reader() { rwlock_unlock_reader(lock); }
void unlock_writer() { rwlock_unlock_writer(lock); }

#endif

#define sleep(x) usleep((x) * 1000000)

#define M 5
#define N 5
#define ARRLEN 10240

int arr[ARRLEN];

void* escritor(void *arg) {
	int i;
	int num = arg - NULL;
	while (1) {
		sleep(random() % 3);
		lock_writer();
		printf("Escritor %d escribiendo\n", num);
		for (i = 0; i < ARRLEN; i++)
			arr[i] = num;
		unlock_writer();
	}
	return NULL;
}

void* lector(void *arg) {
	int v, i;
	int num = arg - NULL;
	while (1) {
		sleep(random() % 3);
		lock_reader();
		v = arr[ARRLEN-1];
		for (i = ARRLEN-2; i >= 0; i--) {
			if (arr[i] != v)
				break;
		}
		unlock_reader();
		if (i >= 0)
			printf("Lector %d, error de lectura\n", num);
		else {
			printf("Lector %d, dato %d\n", num, v);
		}
	}
	return NULL;
}

int main() {
	pthread_t lectores[M], escritores[N];

	create_lock();

	for (int i = 0; i < M; i++)
		pthread_create(&lectores[i], NULL, lector, NULL + i);

	for (int i = 0; i < N; i++)
		pthread_create(&escritores[i], NULL, escritor, NULL + i);

	pthread_join(lectores[0], NULL); /* Espera para siempre */

	destroy_lock();

	return 0;
}
