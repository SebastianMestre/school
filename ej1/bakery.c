#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

// I copied Lamport's algorithm from the class slides and added one `mfence`
// after each write into the `eligiendo` array (which indicates whether a
// thread is choosing its priority at a given point in time).

#define THREAD_COUNT 4
int numero[THREAD_COUNT] = {};
bool eligiendo[THREAD_COUNT] = {};

void lock(int i){  

	eligiendo[i] = true;
	asm("mfence");

	int max_numero = numero[0];
	for (int j = 1; j < THREAD_COUNT; ++j) {
		int k = numero[j];
		if (max_numero < k)
			max_numero = k;
	}
	numero[i] = max_numero + 1;

	eligiendo[i] = false;  
	asm("mfence");

	for (int j = 0; j < THREAD_COUNT; ++j) {     

		while ( eligiendo[j] ) { 
			/* busy waiting */
		}    

		while ((numero[j] != 0) &&
		       ((numero[j] < numero[i]) || ((numero[j] == numero[i]) && (j < i)))) { 
		}
	}
}

void unlock(int i) {
	numero[i] = 0;
}

#define N_VISITANTES 1000000

int visitantes = 0;

void *molinete(void *arg) {
	int thread_number = arg - NULL;
	for (int i = 0; i < N_VISITANTES; i++) {
		lock(thread_number);
		visitantes++;
		unlock(thread_number);
	}
}

int main() {
	pthread_t m1, m2, m3, m4;
	pthread_create(&m1, NULL, molinete, NULL + 0);
	pthread_create(&m2, NULL, molinete, NULL + 1);
	pthread_create(&m3, NULL, molinete, NULL + 2);
	pthread_create(&m4, NULL, molinete, NULL + 3);
	pthread_join(m1, NULL);
	pthread_join(m2, NULL);
	pthread_join(m3, NULL);
	pthread_join(m4, NULL);

	printf("Hoy hubo %d visitantes!\n", visitantes);
	return 0;
}
