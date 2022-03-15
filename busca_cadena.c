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
/*
ejemplo compilacion: 	mpicc busca_cadena.c -o nombre_ejecutable -lm
ejemplo ejecucion:   	mpirun -np 7 --oversubscribe nombre_ejecutable 2 0
	 
	 argumentos: 
	 					argv[0] = nombre_ejecutable
						argv[1] = numeroComprobadores
						argv[2] = 0 si no hay pistas, 1 si hay pistas
*/
	if(argc != 3){
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
		
		int numComp = atoi(argv[1]);
		int numGen = nprocs - (numComp + 1); // +1 pq seria E/S 
		
		// Sabe cuantos Comprobadores existe, y si es posible seguir adelante con el nº de procs
		if(numGen < 1){
			fprintf(stdout,"Error: no hay ningun Generador\n");
			fprintf(stdout,"Aumente el numero de procesos, o disminuya el numero de Comprobadores\n");
			fprintf(stdout,"Finalizando programa...\n");
			MPI_Finalize();
			return 3;
		}
		
		// Dependiendo del nprocs y del nºComprobadores, habrá un nºGeneradores
		//   -> Indica a cada proceso su rol, si es Generador, le dice cual es su Comprobador
		
		
		// Sabra si se usan pistas o no segun argv[2]
		
		// Envía tipo de proceso
		
		// Envía longitud
		int i;
		i= strlen(palabra);
		MPI_Bcast( &i, 1, MPI_INT, 0, MPI_COMM_WORLD ); // Envía mensaje con la longitud a todos
		// Envía la palabra a los Comprobadores
		
		
		/* Espera a recibir de los Generadores
		    -> Cadena con los caracteres ya encontrados
				-> Si hay pistas, se distribuyen al resto de Generadores
			-> Palabra ya encontrada
		*/
		
		// Si palabra está encontrada manda terminar a Comprobadores y Generadores y recibe estadisticas
		
		
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
