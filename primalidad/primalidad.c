#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "timing.h"

int primo_paralelo(int n) {
	int es_primo = 0;
	#pragma omp parallel for reduction(|:es_primo)
	for (int i = 2; i < (int) sqrt(n); i++) {
		es_primo |= (n % i == 0);
	}
	return es_primo;
}

int primo_secuencial(int n) {
	int es_primo = 0;
	for (int i = 2; i < (int) sqrt(n); i++) {
		es_primo |= (n % i == 0);
	}
	return es_primo;
}

int main() {
	int n = random();

	int paralelo = TIME(primo_paralelo(n), NULL);
	int secuencial = TIME(primo_secuencial(n), NULL);

	printf("Paralelo: %d\nSecuencial: %d\n", paralelo, secuencial);
}
