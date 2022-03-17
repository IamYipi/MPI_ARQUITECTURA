// Compilar --> mpicc busca_cadena.c -o busca_cadena -lm
// Ejecutar --> mpirun -np 3 --oversubscribe busca_cadena 1 1
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#define PESO_COMPROBAR  5000000
#define PESO_GENERAR    10000000
#define CHAR_NF 32 // Para marcar no encontrado (espacio, por simplificar)
#define CHAR_MAX 127
#define CHAR_MIN 33
#define INICIAR_ARRAY(valor, array, tam) for (i=0; i<tam; i++) \
                      				array[i] = valor;

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

	int id, nprocs, winner;
	char palabra[CHAR_MAX];
	int pista = 0;

	MPI_Status status;
	MPI_Request request;

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
		int comprob[numComp + 1];
		INICIAR_ARRAY(0, comprob, numComp + 1);

		// Bucle para asignar comprobadores a los generadores

		for(i = j; i < nprocs; i++){

			fprintf(stdout, "0%d) %d\n", i, comp);

			// Envía Comprobador que le corresponde a cada Generador

			MPI_Send(&comp, 1, MPI_INT, i, gen_tag, MPI_COMM_WORLD);

			// Array para guardar a cada id(Comprobador) cuantos generadores le corresponde

			comprob[comp]=comprob[comp]+1;

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

		// Envia a los comprobadores cuantos Generadores les corresponde

		int c;

		for(i = 1; i <= numComp; i++){
			c = comprob[i];
			MPI_Send(&c,1,MPI_INT,i,compr_tag,MPI_COMM_WORLD);

		}
		
		/* Espera a recibir de los Generadores
		    -> Cadena con los caracteres ya encontrados
				-> Si hay pistas, se distribuyen al resto de Generadores
			-> Palabra ya encontrada
		*/

		// Variables flag y de todo

		int encontrado = 0;
		char palabra_aux[CHAR_MAX];
		int idgen,idcomp;
		int ngenex = 0;
		int exit = 0;

		fprintf(stdout,"\nBUSCANDO\n");

		// Mientras numero de Generadores Salidos sea distinto del numero de Generadores total

		while(ngenex != numGen){

			// Recibe id y palabra de un Generador

			MPI_Recv(&idgen,1,MPI_INT,MPI_ANY_SOURCE,gen_tag,MPI_COMM_WORLD,&status);
			MPI_Recv(&palabra_aux, long_palabra, MPI_CHAR, idgen, gen_tag, MPI_COMM_WORLD, &status);

			// Si ya esta encontrada la palabra se lo salta

			if(encontrado != 1){

				// Comparacion palabra recibida por el Generador

				if(strcmp(palabra_aux,palabra) == 0){

					// Guarda la variable flag encontrado y el id del Generador en winner

					encontrado = 1;
					winner = idgen;

					// Muestra por pantalla ganador y palabra buscada y encontrada

					fprintf(stdout,"\nPALABRA ENCONTRADA POR %d\n",winner);
					fprintf(stdout,"BUSCADA...: %s\n",palabra);
					fprintf(stdout,"ENCONTRADA: %s\n",palabra_aux);

				}else{

					// Imprime palabra que comprueba del generador

					fprintf(stdout,"0%d) NO PISTA.....: %s\n",idgen,palabra_aux);

				}
			}

			// -----------------------------------------------------------------------------------------------
			// Si pista activada bucle for envio palabra a cada generador. NO IMPLEMENTADO MODO PISTA

			if(pista == 1){

				j = numComp + 1;

				for(i = j; i < nprocs; i++){

					MPI_Isend(&palabra_aux, long_palabra, MPI_CHAR, 80, gen_tag, MPI_COMM_WORLD,&request);

				}

			}
			// -----------------------------------------------------------------------------------------------

			// Envio al generador de comprobacion palabra y recepcion de confirmacion que deja de generar

			MPI_Send(&encontrado,1,MPI_INT,idgen,gen_tag,MPI_COMM_WORLD);
			MPI_Recv(&exit,1,MPI_INT,idgen,gen_tag,MPI_COMM_WORLD,&status);

			// Si se sale sumamos uno a la variable que guarda cuantos generadores hay ya salidos

			if(exit == 1){

				ngenex++;

			}

			// Recibe el id del comprobador asignado al Generador que ha tramitado sus datos
			// Envia flag exit indicandole al comprobador que un Generador suyo ha terminado o no

			MPI_Recv(&idcomp,1,MPI_INT,idgen,gen_tag,MPI_COMM_WORLD,&status);
			MPI_Send(&exit,1,MPI_INT,idcomp,compr_tag,MPI_COMM_WORLD);

			// Reset flag exit

			exit = 0;

		}
		
		// Si palabra está encontrada recibe estadisticas
		
		
	}else{

		int longitud, rol;

		// Recibe el rol del grupo al que pertenece. COMPROBADORES = 0 GENERADORES = 1

		MPI_Recv(&rol, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		// Recibe el Bcast con la longitud de la palabra ya que su id != sender 

		MPI_Bcast(&longitud, 1, MPI_INT, 0, MPI_COMM_WORLD);

		// Comprobadores

		if(rol == 0){

			// Palabra recibida

			MPI_Recv(&palabra, longitud, MPI_CHAR, 0, rol, MPI_COMM_WORLD, &status);

			// Recibe numero de generadores que le corresponde al Comprobador

			int numGenCorr = 0;

			MPI_Recv(&numGenCorr,1,MPI_INT,0,rol,MPI_COMM_WORLD,&status);

			char palabra_rcv[CHAR_MAX];
			int encontrado = 0;
			int genid, ext;
			int genext = 0;
			int i,j;
			char a,b;

			// Si no tiene asignados Generadores se lo salta

			if(numGenCorr != 0){

				// Mientras numero de Generadores que le corresponde sea distinto de los generadores
				// que ya han finalizado de generar palabras

				while(numGenCorr != genext){

					// Recibe id del generador y despues la palabra

					MPI_Recv(&genid,1,MPI_INT,MPI_ANY_SOURCE,id,MPI_COMM_WORLD,&status);
					MPI_Recv(&palabra_rcv,longitud,MPI_CHAR,genid,genid,MPI_COMM_WORLD,&status);


					// Bucle for que comprueba letra a letra la palabra recibida con la palabra buena

					for(i = 0; i < longitud; i++){

						a = palabra_rcv[i];
						b = palabra[i];

						if(a != b){

							palabra_rcv[i] = CHAR_NF; // Sustitucion por espacio

						}

					}

					// Forzamos espera para dar peso al calculo

					//fuerza_espera(5);

					// Envia los resultados de la comprobacion

					MPI_Send(&palabra_rcv, longitud, MPI_CHAR, genid, genid, MPI_COMM_WORLD);


					// Recibe si se sale o no por parte del proceso 0 E/S

					MPI_Recv(&ext,1,MPI_INT,0,rol,MPI_COMM_WORLD,&status);

					// Si indica que se sale, aumenta en uno la variable generadores salidos

					if(ext == 1){

						genext++;
						ext = 0;

					}	

				}

			}

			// Envio estadisticas

		}

		// Generadores

		if(rol == 1){

			srand( id * time(NULL)); // Semilla en base al tiempo y el id de cada proceso.

			// Variable que guarda el comprobador asignado al generador

			int compr_assig;
			char palabra_pist[CHAR_MAX];

			// Recibe Comprobador asignado

			MPI_Recv(&compr_assig, 1, MPI_INT, 0, rol, MPI_COMM_WORLD, &status);

			// Variables flag

			int encontrado = 0;
			int exit = 0;

			// Variable palabraAleatoria que se va obteniendo segun la generacion de palabras

			char palabraAleatoria[longitud];

			// Tener en cuenta que si se cambia la palabra a descubrir hay que añadir aqui los caracteres que posea al menos una vez.

			char caracteresPosibles[] = "ABCDEFGHIJKLRRRMNIOQUTJAKSGHAJHGPFSGPfghafsghafsghafstjharfasdhjshdjh216531823612678&&!1281asdasdjahsdkjhadsljhaejuj";
			

			// Bucle envio palabras generadoras

			int x = 0;
			int i,j;
			char a,b;
			// Lleno el array de espacios vacios

			INICIAR_ARRAY(CHAR_NF,palabraAleatoria,longitud);

			while(encontrado != 1){
				// Genera palabra aleatoria
				// Segundo for del script de German, con este solo ya es suficiente y necesario

				for( i = 0; i < longitud; i++){ // Este bucle solo generará en las posiciones que tengan un CHAR_NF, es decir, las erroneas
					j = rand() % (strlen(caracteresPosibles)+1);
					if(palabraAleatoria[i] == CHAR_NF){
						palabraAleatoria[i] = caracteresPosibles[j];
					}
					//fuerza_espera(PESO_GENERAR);
				}

			

				// Esto es como laboratorio de pruebas queda generar la palabra bien

				//fuerza_espera(5);


				// Primero enviamos id, despues se envia la palabra al comprobador asignado

				MPI_Send(&id,1,MPI_INT,compr_assig,compr_assig,MPI_COMM_WORLD);
				MPI_Send(&palabraAleatoria,longitud,MPI_CHAR,compr_assig,id,MPI_COMM_WORLD);

				// Recibe la palabra con caracteres validos de su comprobador asignado

				MPI_Recv(&palabraAleatoria,longitud,MPI_CHAR,compr_assig,id,MPI_COMM_WORLD,&status);


				// Envio id y palabra a proceso 0 E/S para comprobar si se termina

				MPI_Send(&id, 1, MPI_INT,0,rol,MPI_COMM_WORLD);
				MPI_Send(&palabraAleatoria, longitud, MPI_CHAR, 0, rol, MPI_COMM_WORLD);

				// MPI_Recv(&palabra,longitud,MPI_CHAR,0,0,MPI_COMM_WORLD,&status); //Modo pista


				// Recibe flag encontrado para salir del bucle

				MPI_Recv(&encontrado, 1, MPI_INT, 0,rol, MPI_COMM_WORLD,&status);

				if(encontrado == 1){

					exit = 1;

				}

				// Envio resultado a proceso 0 E/S si termina su proceso o sigue
				// Despues envia el id del comprobador que tiene asignado, tambien al proceso 0 E/S

				MPI_Send(&exit,1,MPI_INT,0,rol,MPI_COMM_WORLD);
				MPI_Send(&compr_assig,1,MPI_INT,0,rol,MPI_COMM_WORLD);


				// PISTA NO IMPLEMENTADO
				// ----------------------------------------------------------------------------

				if(pista == 1){

					MPI_Irecv(&palabra_pist,longitud,MPI_CHAR,0,80,MPI_COMM_WORLD,&request);
					
					for(i = 0; i < longitud; i++){

						a = palabra_pist[i];
						b = palabra[i];

						if(a != b){

							palabra[i] = a; // Sustitucion por letra pista

						}
					}

				}
				// ------------------------------------------------------------------------------	

			}

		}
	}

	MPI_Barrier(MPI_COMM_WORLD);


	MPI_Finalize();
	return 0;	
}


void fuerza_espera(unsigned long peso)
{
  for (unsigned long i=1; i<1*peso; i++) sqrt(i);
}
