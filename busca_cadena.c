#include <stdio.h>
#include <mpi.h>
#define PESO_COMPROBAR  5000000
#define PESO_GENERAR    10000000

void fuerza_espera(unsigned long);

int main(int argc, char ** argv){
	if(argc != 2){
		fprintf(stdout, "Error: Numero invalido de argumentos\n");
		fprintf(stdout, "Usar: mpirun -np X [--oversubscribe] %s numero\n", argv[0]);
		return 1;
	}

	int id, nprocs;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	if(nprocs < 3){
		fprintf(stdout,"Error: Numero invalido de procesos\n");
		fprintf(stdout,"Finalizando programa...\n");
		MPI_Finalize();
		return 2;
	}

	printf("Hello from processor %d of %d\n",id, nprocs);

	/*Tareas a realizar por el Proceso 0*/
	
	if(id == 0){


		
	}

	MPI_Finalize();
	return 0;	
}


void fuerza_espera(unsigned long peso)
{
  for (unsigned long i=1; i<1*peso; i++) sqrt(i);
}
