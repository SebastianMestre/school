#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>


int mod(int x, int m) {
	return (x % m + m) % m;
}

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);

	int num_procs, my_id;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	int n = 1 * 2 * 3 * 4 * 1000000;
	assert(n % num_procs == 0);

	int* datos_separados = malloc((n / num_procs) * sizeof(*datos_separados));
	int return_code;

	if (my_id == 0) {

		uint32_t rng = 3141592;

		int* datos = malloc(n * sizeof(*datos));
		for (int i = 0; i < n; ++i) {
			rng += 23456349;
			rng *= 123457;
			datos[i] = rng % 100;
		}

		return_code = MPI_Scatter(
			datos,           n/num_procs, MPI_INT,
			datos_separados, n/num_procs, MPI_INT,
			0, MPI_COMM_WORLD);

		if (return_code != 0) {
			perror("Root error in scatter: ");
		}

		int suma = 0;
		for (int i = 0; i < n/num_procs; ++i) {
			suma += datos_separados[i];
		}

		int* sumas = malloc(num_procs * sizeof(int));
		return_code = MPI_Gather(
			&suma, 1, MPI_INT,
			sumas, 1, MPI_INT,
			0, MPI_COMM_WORLD);

		if (return_code != 0) {
			perror("Root error in gather: ");
		}

		int resultado = 0;
		for (int i = 0; i < num_procs; ++i) {
			resultado += sumas[i];
		}
		printf("resultado: %d\n", resultado);
	} else {

		return_code = MPI_Scatter(
			NULL,            0,   MPI_INT,
			datos_separados, n/num_procs, MPI_INT,
			0, MPI_COMM_WORLD);
		if (return_code != 0) {
			perror("Process error in scatter: ");
		}

		int suma = 0;
		for (int i = 0; i < n/num_procs; ++i) {
			suma += datos_separados[i];
		}

		return_code = MPI_Gather(
			&suma, 1, MPI_INT,
			NULL, 0, MPI_INT,
			0, MPI_COMM_WORLD);
		if (return_code != 0) {
			perror("Process error in gather: ");
		}

	}

	MPI_Finalize();
}
