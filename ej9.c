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

int padre(int rank) { return (rank - 1) / 2; }
int hijo1(int rank) { return (rank * 2) + 1; }
int hijo2(int rank) { return (rank * 2) + 2; }

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int mi_valor = rank + 1;

	int suma = mi_valor;

	suma += recibir(hijo1(rank));
	suma += recibir(hijo2(rank));

	int suma_total;
	if (rank == 0) {
		suma_total = suma;
	} else {
		enviar(padre(rank), suma);
		suma_total = recibir(padre(rank));
	}

	printf("Proc %d: la suma total es %d\n", rank, suma_total);

	enviar(hijo1(rank), suma_total);
	enviar(hijo2(rank), suma_total);

	MPI_Finalize();
}
