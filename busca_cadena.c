#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#define PESO_COMPROBAR  5000000
#define PESO_GENERAR    10000000
#define CHAR_NF 32 // Para marcar no encontrado (espacio, por simplificar)
#define CHAR_MAX 127
#define CHAR_MIN 33

void fuerza_espera(unsigned long);

int main(int argc, char ** argv){

	if(argc != 2){
		fprintf(stdout, "Error: Numero invalido de argumentos\n");
		fprintf(stdout, "Usar: mpirun -np X [--oversubscribe] %s numero\n", argv[0]);
		// Siendo "numero" el nº de procesos Comprobadores
		return 1;
	}

	int id, nprocs;
	char palabra[CHAR_MAX];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	if(nprocs < 3){
		fprintf(stdout,"Error: Numero invalido de procesos\n");
		fprintf(stdout,"Finalizando programa...\n");
		MPI_Finalize();
		return 2;
	}
	/*Tareas a realizar por el Proceso 0*/
	
	if(id == 0){
	
		strcpy(palabra,"PRACTICAMPI2122");

		/* El proceso E/S tiene en la variable palabra, la palabra correcta */
		
		printf("La longitud de la palabra es: %ld y hay %d comprobadores\n",
							strlen(palabra), atoi(argv[1]));
		int i;
		i= strlen(palabra);
		MPI_Bcast( &i, 1, MPI_INT, 0, MPI_COMM_WORLD ); // Envía mensaje con la longitud a todos
		
		
	}else{
		int longitud;
		
		MPI_Bcast( &longitud, 1, MPI_INT, 0, MPI_COMM_WORLD );
		// Recibe el Bcast ya que su id != sender 
		
		printf("Soy el proceso %d y la longitud es %d\n",id, longitud);
	}

	MPI_Finalize();
	return 0;	
}


void fuerza_espera(unsigned long peso)
{
  for (unsigned long i=1; i<1*peso; i++) sqrt(i);
}
