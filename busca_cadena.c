// Compilar --> mpicc busca_cadena.c -o busca_cadena -lm
// Ejecutar --> mpirun -np 3 --oversubscribe busca_cadena 1 1
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
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
	MPI_Status status;

	// Inicio MPI

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	
	/*Tareas a realizar por el Proceso 0*/
	
	if(id == 0){	

		if(nprocs < 3){

			fprintf(stdout,"Error: Numero invalido de procesos\n");
			fprintf(stdout,"Finalizando programa...\n");

			MPI_Abort(MPI_COMM_WORLD, 1);
			
	}

		// Se copia la palabra a descubrir en la variable

		strcpy(palabra,"PRACTICAMPI2122");

		/* El proceso E/S tiene en la variable palabra, la palabra correcta */
		
		int numComp = atoi(argv[1]);
		int numGen = nprocs - (numComp + 1); // +1 pq seria E/S

		// Primera salida por pantalla

		fprintf(stdout,"\nNUMERO DE PROCESOS: Total %d: E/S: 1, Comprobadores: %d, Generadores: %d\n\n",nprocs,numComp,numGen);
 
		
		// Sabe cuantos Comprobadores existe, y si es posible seguir adelante con el nº de procs

		if(numGen < 1){

			fprintf(stdout,"Error: no hay ningun Generador\n");
			fprintf(stdout,"Aumente el numero de procesos, o disminuya el numero de Comprobadores\n");
			fprintf(stdout,"Finalizando programa...\n");

			MPI_Abort(MPI_COMM_WORLD, 2);

		}
		
		// Dependiendo del nprocs y del nºComprobadores, habrá un nºGeneradores
		//   -> Indica a cada proceso su rol, si es Generador, le dice cual es su Comprobador

		int i,j;

		// Variables tag para establecer comunicaciones con cada grupo especifico

		int compr_tag = 0;
		int gen_tag   = 1;

		// Envia rol a los procesos comprobadores

		fprintf(stdout,"NOTIFICACION TIPO\n");

		for(i = 1; i <= numComp; i++){

			fprintf(stdout, "0%d) %d\n", i, compr_tag);
			MPI_Send(&compr_tag, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

		}

		// Se guarda el primer id de los generadores que sera el total de comprobadores mas uno

		j = numComp + 1;

		// Envia rol a los procesos generadores

		for(i = j; i < nprocs; i++){

			MPI_Send(&gen_tag, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

		}

		// Asignacion de roles
		
		// Sabra si se usan pistas o no segun argv[2]
			
		// Envía tipo de proceso
			
		// Envía longitud

		int long_palabra;
		long_palabra = strlen(palabra);
		MPI_Bcast(&long_palabra, 1, MPI_INT, 0, MPI_COMM_WORLD); // Envía mensaje con la longitud a todos

		// Inicializamos variable con el id del primer generador

		j = numComp + 1;

		// Inicializamos la variable con el id del primer comprobador

		int comp = 1;

		// Bucle para asignar comprobadores a los generadores

		for(i = j; i < nprocs; i++){

			fprintf(stdout, "0%d) %d\n", i, comp);

			// Envía Comprobador que le corresponde a cada Generador

			MPI_Send(&comp, 1, MPI_INT, i, gen_tag, MPI_COMM_WORLD);

			// Si la variable alcanza el numero de comprobadores total, se resetea
			// Sino aumenta uno

			if( comp == numComp){

				comp = 1;

			}else{

				comp++;

			}

		}

		// Envia la palabra a los Comprobadores

		for(i = 1; i <= numComp; i++){

			MPI_Send(&palabra, long_palabra, MPI_CHAR, i, compr_tag, MPI_COMM_WORLD);
		
		}

		fprintf(stdout, "\n\nNOTIFICACION PALABRA COMPROBADORES\n");

		for(i = 1; i <= numComp; i++){

			fprintf(stdout, "0%d) %s, %d\n", i, palabra, long_palabra);

		}

		fprintf(stdout, "\n");
		
		/* Espera a recibir de los Generadores
		    -> Cadena con los caracteres ya encontrados
				-> Si hay pistas, se distribuyen al resto de Generadores
			-> Palabra ya encontrada
		*/
		
		// Si palabra está encontrada manda terminar a Comprobadores y Generadores y recibe estadisticas
		
		
	}else{

		int longitud, rol;

		// Recibe el rol del grupo al que pertenece. COMPROBADORES = 0 GENERADORES = 1

		MPI_Recv(&rol, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		// Recibe el Bcast ya que su id != sender 

		MPI_Bcast(&longitud, 1, MPI_INT, 0, MPI_COMM_WORLD);

		// Comprobadores

		if(rol == 0){

			// Palabra recibida

			MPI_Recv(&palabra, longitud, MPI_CHAR, 0, rol, MPI_COMM_WORLD, &status);
			
		}

		// Generadores

		if(rol == 1){

			// Variable que guarda el comprobador asignado al generador
			
			srand( id * time(NULL)); // Semilla en base al tiempo y el id de cada proceso.
			int compr_assig,i;

			// Recibe Comprobador asignado

			MPI_Recv(&compr_assig, 1, MPI_INT, 0, rol, MPI_COMM_WORLD, &status);
			
			// Conozco id, longitud, compr_assig
			
			char palabraAleatoria[longitud];
			char caracteresPosibles[] = "ABCDEFGHIJKLMNIOQUJAKSGHAJHGFSGfghafsghafsghafstjharfasdhjshdjh216531823612678&&!1281asdasdjahsdkjhadsljhaejuj";
			

			/* Genera combinación aleatoria
					-> Ten en cuenta ya detectados
					-> random gen SEED
					-> Espera forzada
			*/
			int j;
			
			for( i = 0; i < longitud; i++){
				j= rand() % (strlen(caracteresPosibles)+1) ;
				palabraAleatoria[i] = caracteresPosibles[j];
				fuerza_espera(PESO_GENERAR);
			}


			for( i = 0; i < longitud; i++){
				fprintf(stderr,"%c",palabraAleatoria[i]);
			}
			fprintf(stderr,"\n");
			
			
			// El tramo entre asteriscos será un bucle hasta que E/S mande, esto se hará con un iProbe
// *******************************************************************************************************			
			// Consulta al comprobador
			
//			MPI_Send(&palabraAleatoria, 1, MPI_CHAR, compr_assig, 1, MPI_COMM_WORLD); // TODO Tengo dudas de este send
			
			/* 
				Recibo de compr una palabra, mantengo las que sean iguales a las que envié
		 			->Si la respuesta tiene nuevos caracteres acertados, mando a E/S
		 	 			Pista o no, pero ya depende del E/S 
			*/
			
			char palabraCorregida[longitud], palabraCorregidaAnterior[longitud];			
			
			// MSG_RCV PALABRACORREGIDA 
			
			if( strcmp(palabraCorregida,palabraCorregidaAnterior) != 0){ // TODO Quizas habria que ignorar la primera iteracion y asi no notificar al E/S
				// Envío mensaje a E/S
				// MPI_Send(&palabraCorregida, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
				strcpy(palabraCorregidaAnterior,palabraCorregida);
			}
			
			strcpy(palabraAleatoria,palabraCorregida); // Ahora en palabraAleatoria tengo la palabra corregida, con CHAR_NF en los fallados, y los acertados OK
			

			for( i = 0; i < longitud; i++){ // Este bucle solo generará en las posiciones que tengan un CHAR_NF, es decir, las erroneas
				j= rand() % (strlen(caracteresPosibles)+1) ;
				if(palabraAleatoria[i] == CHAR_NF){
					palabraAleatoria[i] = caracteresPosibles[j];
				}
				fuerza_espera(PESO_GENERAR);
			}
			
			for( i = 0; i < longitud; i++){
				fprintf(stderr,"%c",palabraAleatoria[i]);
			}
			fprintf(stderr,"\n");
// *******************************************************************************************************
			
			// E/S puede mandar una pista
			
			// Una vez mandado acabar, mandar estadisticas a E/S

		}
	}

	MPI_Finalize();
	return 0;	
}


void fuerza_espera(unsigned long peso)
{
  for (unsigned long i=1; i<1*peso; i++) sqrt(i);
}
