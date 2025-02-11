#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <pthread.h>
#include <semaphore.h>

sem_t tabaco, papel, fosforos, otra_vez;
sem_t sem_fumador1, sem_fumador2, sem_fumador3;
int tengo_tabaco, tengo_papel, tengo_fosforos;
pthread_mutex_t counters_mutex = PTHREAD_MUTEX_INITIALIZER;

void agente()
{
	for (;;) {
		int caso = random() % 3;
		sem_wait(&otra_vez);
		switch (caso) {
			case 0:
				sem_post(&tabaco);
				sem_post(&papel);
				break;
			case 1:
				sem_post(&fosforos);
				sem_post(&tabaco);
				break;
			case 2:
				sem_post(&papel);
				sem_post(&fosforos);
				break;
		}
	}
}

void fumar(int fumador)
{
	printf("Fumador %d: Puf! Puf! Puf!\n", fumador);
	sleep(1);
}

void *fumador1(void *arg)
{
	for (;;) {
		sem_wait(&sem_fumador1);
		fumar(1);
		sem_post(&otra_vez);
	}
}

void *fumador2(void *arg)
{
	for (;;) {
		sem_wait(&sem_fumador2);
		fumar(2);
		sem_post(&otra_vez);
	}
}

void *fumador3(void *arg)
{
	for (;;) {
		sem_wait(&sem_fumador3);
		fumar(3);
		sem_post(&otra_vez);
	}
}

void intentar_despertar_fumador() {
	if (tengo_tabaco && tengo_papel) {
		tengo_tabaco--;
		tengo_papel--;
		sem_post(&sem_fumador1);
		return;
	}

	if (tengo_fosforos && tengo_tabaco) {
		tengo_fosforos--;
		tengo_tabaco--;
		sem_post(&sem_fumador2);
		return;
	}

	if (tengo_papel && tengo_fosforos) {
		tengo_papel--;
		tengo_fosforos--;
		sem_post(&sem_fumador3);
		return;
	}
}

void counters_lock() {
	pthread_mutex_lock(&counters_mutex);
}

void counters_unlock() {
	pthread_mutex_unlock(&counters_mutex);
}

void esperar_recurso(sem_t* sem, int* flag) {
	while (1) {
		sem_wait(sem);

		counters_lock();

		(*flag)++;

		intentar_despertar_fumador();

		counters_unlock();
	}
}

void* thread_tabaco(void* arg) {
	esperar_recurso(&tabaco, &tengo_tabaco);
	return NULL;
}

void* thread_papel(void* arg) {
	esperar_recurso(&papel, &tengo_papel);
	return NULL;
}

void* thread_fosforos(void* arg) {
	esperar_recurso(&fosforos, &tengo_fosforos);
	return NULL;
}

int main() {
	pthread_t s1, s2, s3;
	sem_init(&tabaco, 0, 0);
	sem_init(&papel, 0, 0);
	sem_init(&fosforos, 0, 0);
	sem_init(&otra_vez, 0, 1);

	sem_init(&sem_fumador1, 0, 0);
	sem_init(&sem_fumador2, 0, 0);
	sem_init(&sem_fumador3, 0, 0);

	pthread_create(&s1, NULL, fumador1, NULL);
	pthread_create(&s2, NULL, fumador2, NULL);
	pthread_create(&s3, NULL, fumador3, NULL);

	pthread_t w1, w2, w3;
	pthread_create(&w1, NULL, thread_tabaco, NULL);
	pthread_create(&w2, NULL, thread_papel, NULL);
	pthread_create(&w3, NULL, thread_fosforos, NULL);

	agente();

	return 0;
}

