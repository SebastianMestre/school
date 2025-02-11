#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>

int suma (int* arr, int n) {
	int resultado = 0;
	for (int i = 0; i < n; ++i) {
		resultado += arr[i];
	}
	return resultado;
}


int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);

	int num_procs, my_id;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	int n = 31415926;
	int m = n / num_procs;

	int* datos_separados = malloc(m * sizeof(*datos_separados));

	int* datos = NULL;
	int* sumas = NULL;

	if (my_id == 0) {

		datos = malloc(n * sizeof(*datos));
		for (int i = 0; i < n; ++i) {
			datos[i] = rand();
		}

		sumas = malloc(num_procs * sizeof(int));
	}

	// reparto/recibo los datos
	MPI_Scatter(
		datos, m, MPI_INT,
		datos_separados, m, MPI_INT,
		0, MPI_COMM_WORLD);

	// sumo la parte que me corresponde
	int suma_parcial = suma(datos_separados, m);

	// junto los datos
	MPI_Gather(
		&suma_parcial, 1, MPI_INT,
		sumas, 1, MPI_INT,
		0, MPI_COMM_WORLD);


	if (my_id == 0) {
		int resultado = suma(sumas, num_procs);

		// hago la suma de lo que le falto hacer a scatter
		resultado += suma(datos + m * num_procs, n - m * num_procs);

		printf("resultado: %d\n", resultado);
	}

	MPI_Finalize();
}
