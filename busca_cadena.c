#include <stdio.h>
#include <mpi.h>

int main(int argc, char ** argv){

	if(argc != 2){
		fprintf(stdout, "Error: Numero invalido de argumentos\n");
		return 1;
	}
	
	int myrank, nprocs;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	printf("Hello from processor %d of %d\n",myrank, nprocs);

	MPI_Finalize();
	return 0;
	
	
}
