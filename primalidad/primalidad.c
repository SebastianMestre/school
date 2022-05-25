#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "timing.h"

int primo_paralelo(long long n) {
	int es_compuesto = 0;
	#pragma omp parallel for reduction(|:es_compuesto)
	for (int i = 2; i <= (int) sqrt(n); i++) {
		es_compuesto |= (n % i == 0);
	}
	return !es_compuesto;
}

int primo_secuencial(long long n) {
	int es_compuesto = 0;
	for (int i = 2; i <= (int) sqrt(n); i++) {
		es_compuesto |= (n % i == 0);
	}
	return !es_compuesto;
}

int main() {
	long long n = (long long)random() * RAND_MAX + (long long)random();

	int paralelo = TIME(primo_paralelo(n), NULL);
	int secuencial = TIME(primo_secuencial(n), NULL);

	printf("Paralelo: %d\nSecuencial: %d\n", paralelo, secuencial);
}
