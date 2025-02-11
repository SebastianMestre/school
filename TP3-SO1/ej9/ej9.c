#include <mpi.h>
#include <stdio.h>

void enviar(int rank, int num) {
	int num_proc;
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	if (rank >= num_proc)
		return;

	MPI_Send(&num, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
}

int recibir(int rank) {
	int num_proc;
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	if (rank >= num_proc)
		return 0;

	int result;
	MPI_Status status;
	MPI_Recv(&result, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, &status);
	return result;
}

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);

	int num_proc;
	MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int mi_valor = rank + 1;

	int suma = mi_valor;

	int w = 1;
	while (w < num_proc) {
		int delta = (rank / w) % 2 == 0 ? w : -w;

		enviar(rank + delta, suma);
		suma += recibir(rank + delta);

		w *= 2;
	}

	printf("Proc %3d: la suma total es %d\n", rank, suma);

	MPI_Finalize();
}
