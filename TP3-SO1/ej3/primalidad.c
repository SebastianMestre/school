#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "timing.h"
#include "primalidad.h"

int primo_paralelo(long long n) {
	int es_compuesto = 0;
	int k = sqrt(n);

	#pragma omp parallel num_threads(W)
	{
		int rank = omp_get_thread_num();

		int i = (rank == 0) ? 2 : (long long)rank * k / W;
		int tope = ((long long)(rank+1) * k) / W;
		
		for (; i <= tope && !es_compuesto; i++) {
			if (n % i == 0) {
				es_compuesto = 1;
			}
		}
	}

	return !es_compuesto;
}

int primo_secuencial(long long n) {
	int es_compuesto = 0;
	int k = sqrt(n);
	for (int i = 2; i <= k && !es_compuesto; i++) {
		if (n % i == 0) {
			es_compuesto = 1;
		}
	}
	return !es_compuesto;
}

int main() {
	long long n = (long long)random() * RAND_MAX + (long long)random();
	// long long n = 1000000000000000003ll;

	int secuencial = TIME(primo_secuencial(n), NULL);
	int paralelo = TIME(primo_paralelo(n), NULL);

	printf("Paralelo: %d\nSecuencial: %d\n", paralelo, secuencial);
}
