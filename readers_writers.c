#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define MODE 1

#if MODE == 1
#include "rlock.h"
reader_lock_t lock;
#elif MODE == 2
#include "fair_rwlock.h"
struct fair_rwlock* lock;
#endif

#define M 5
#define N 5
#define ARRLEN 10240

int arr[ARRLEN];

void lock_writer() {
#if MODE == 1
	reader_lock_take_writer(&lock);
#elif MODE == 2
	fair_rwlock_lock_writer(lock);
#endif
}

void unlock_writer() {
#if MODE == 1
	reader_lock_drop_writer(&lock);
#elif MODE == 2
	fair_rwlock_unlock_writer(lock);
#endif
}

void lock_reader() {
#if MODE == 1
	reader_lock_take_reader(&lock);
#elif MODE == 2
	fair_rwlock_lock_reader(lock);
#endif
}

void unlock_reader() {
#if MODE == 1
	reader_lock_drop_reader(&lock);
#elif MODE == 2
	fair_rwlock_unlock_reader(lock);
#endif
}

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
		v = arr[0];
		for (i = 1; i < ARRLEN; i++) {
			if (arr[i] != v)
				break;
		}
		unlock_reader();
		if (i < ARRLEN)
			printf("Lector %d, error de lectura\n", num);
		else
			printf("Lector %d, dato %d\n", num, v);
	}
	return NULL;
}

int main() {
	pthread_t lectores[M], escritores[N];

#if MODE == 1
	reader_lock_init(&lock);
#elif MODE == 2
	lock = fair_rwlock_create();
#endif

	for (int i = 0; i < M; i++)
		pthread_create(&lectores[i], NULL, lector, NULL + i);

	for (int i = 0; i < N; i++)
		pthread_create(&escritores[i], NULL, escritor, NULL + i);

	pthread_join(lectores[0], NULL); /* Espera para siempre */
	return 0;
}
