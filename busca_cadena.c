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

	// Variables comunes a todos los procesos

	int id, nprocs, winner;
	char palabra[CHAR_MAX];
	int flag = 0;
	double tiempoTotal;

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
		// El proceso E/S tiene en la variable palabra, la palabra correcta

		strcpy(palabra,"ABCDEFGHIJKLRRGPFSPRACTICAMPI2122hjash&YhdBcLwFdSHHsdyhj22kylkudaghkdjawgdalwADasdhjshdjh21658&&!1281asdasdjahsdkjhadsljhaejuj");

		// Dependiendo del nprocs y del nºComprobadores, habrá un nºGeneradores
		
		int numComp = atoi(argv[1]);
		int numGen = nprocs - (numComp + 1); // +1 pq seria E/S
		int pista = atoi(argv[2]);

		// Primera salida por pantalla

		fprintf(stdout,"\nNUMERO DE PROCESOS: Total %d: E/S: 1, Comprobadores: %d, Generadores: %d\n\n",nprocs,numComp,numGen);
 
		
		// Sabe cuantos Comprobadores existe, y si es posible seguir adelante con el nº de procs

		if(numGen < 1){

			fprintf(stdout,"Error: no hay ningun Generador\n");
			fprintf(stdout,"Aumente el numero de procesos, o disminuya el numero de Comprobadores\n");
			fprintf(stdout,"Finalizando programa...\n");

			MPI_Abort(MPI_COMM_WORLD, 2);

		}

		int i,j;

		// Variables tag para establecer comunicaciones con cada grupo especifico

		int compr_tag = 0;
		int gen_tag   = 1;

		// Envia rol a los procesos comprobadores
		//   -> Indica a cada proceso su rol, si es Generador, le dice cual es su Comprobador

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

			comprob[comp] = comprob[comp] + 1;

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

		// Variables tiempo cada grupo de procesos y comprobaciones efectuadas por cada comprobador

			// Generadores 

		double tiempoGeneradores[numGen];
		double tiempoGeneradoresEspera[numGen];
		int iteracionesGen[numGen];

			// Comprobadores

		double tiempoComprobadores[numComp+1];
		double tiempoComp[numComp+1];
		int comprobaciones[numComp+1];

		// Comienzo busqueda

		fprintf(stdout,"\nBUSCANDO\n");

		// Mientras numero de Generadores Salidos sea distinto del numero de Generadores total
		// Bucle proceso 0

		tiempoTotal = MPI_Wtime();

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
					fflush(stdin);
					fprintf(stdout,"BUSCADA...:  ");
					fflush(stdin);
					for( i = 0; i < long_palabra; i++){
						fprintf(stdout,"%c",palabra_aux[i]);
						fflush(stdin);
					}
					fprintf(stdout,"\n");
					fflush(stdin);
					fprintf(stdout,"ENCONTRADA:  ");
					for( i = 0; i < long_palabra; i++){
						fprintf(stdout,"%c",palabra_aux[i]);
						fflush(stdin);
					}
					fprintf(stdout,"\n");
					fflush(stdin);

				}else{

					// Imprime palabra que comprueba del generador

					if(pista == 1){
						fprintf(stdout,"0%d)\t PISTA.....:\t",idgen);
						fflush(stdin);
						for( i = 0; i < long_palabra; i++){
							fprintf(stdout,"%c",palabra_aux[i]);
							fflush(stdin);
						}
						fprintf(stdout,"\n");
						fflush(stdin);
					}else{
						fprintf(stdout,"0%d)\t NO PISTA.....:\t",idgen);
						fflush(stdin);
						for( i = 0; i < long_palabra; i++){
							fprintf(stdout,"%c",palabra_aux[i]);
							fflush(stdin);
						}
						fprintf(stdout,"\n");
						fflush(stdin);
					}
				}
			}

			// MODO PISTAAAAAAAAAAA 
			// -----------------------------------------------------------------------------------------------
			// Si pista activada bucle for envio palabra a cada generador.IMPLEMENTADO MODO PISTA

			if(pista == 1){

				j = numComp + 1;

				for(i = j; i < nprocs; i++){

					MPI_Isend(&palabra_aux, long_palabra, MPI_CHAR, i, 80, MPI_COMM_WORLD,&request);

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
		// Primero recibe estadisticas de Generadores, pues son los primeros en salir

		MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD); // Aseguramos que todos han finalizado

		ngenex = 0;

		fprintf(stdout,"\n-----------------------------------------ESTADISTICAS--------------------------------------------\n");

		while(ngenex != numGen){

			MPI_Recv(&idgen,1,MPI_INT,MPI_ANY_SOURCE,gen_tag,MPI_COMM_WORLD,&status);
			MPI_Recv(&iteracionesGen[idgen],1,MPI_INT,idgen,gen_tag,MPI_COMM_WORLD,&status);
			MPI_Recv(&tiempoGeneradores[idgen],1,MPI_DOUBLE,idgen,gen_tag,MPI_COMM_WORLD,&status);
			MPI_Recv(&tiempoGeneradoresEspera[idgen],1,MPI_DOUBLE,idgen,gen_tag,MPI_COMM_WORLD,&status);
			ngenex++;

		}

		
		int ncompex = 0;

		while(ncompex != numComp){

			MPI_Recv(&idcomp,1,MPI_INT,MPI_ANY_SOURCE,compr_tag,MPI_COMM_WORLD,&status);
			MPI_Recv(&comprobaciones[idcomp],1,MPI_INT,idcomp,compr_tag,MPI_COMM_WORLD,&status);
			MPI_Recv(&tiempoComprobadores[idcomp],1,MPI_DOUBLE,idcomp,compr_tag,MPI_COMM_WORLD,&status);
			MPI_Recv(&tiempoComp[idcomp],1,MPI_DOUBLE,idcomp,compr_tag,MPI_COMM_WORLD,&status);
			ncompex++;

		}

		tiempoTotal = MPI_Wtime() - tiempoTotal;

		// Imprimo por pantalla resultados estadisticos

		// GENERADORES

		fprintf(stdout,"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~GENERADORES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		fprintf(stdout,"\n*************************************************************************************************\n");
		fprintf(stdout,"|\tIteraciones \t|\t TiempoTotal \t|\tTiempoGenerado  |\tTiempoEspera\t|\n");
		fprintf(stdout,"|-----------------------------------------------------------------------------------------------|\n");
		
		j = numComp + 1;
		int itTotal = 0;

		for(i = 0; i < numGen; i++){

			itTotal += iteracionesGen[j];
			fprintf(stdout,"|0%d) %10d \t| %15f \t| %15f \t| %15f \t|\n",j,iteracionesGen[j],tiempoGeneradores[j],tiempoGeneradores[j]- tiempoGeneradoresEspera[j],tiempoGeneradoresEspera[j]);
			j++;

		}

		fprintf(stdout,"|-----------------------------------------------------------------------------------------------|\n");
		fprintf(stdout,"| Iteraciones Totales: %d\n",itTotal);

		// COMPROBADORES
		itTotal = 0;

		fprintf(stdout,"\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~COMPROBADORES~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		fprintf(stdout,"\n*************************************************************************\n");
		fprintf(stdout,"|\tIteraciones \t|\t TiempoTotal \t|    TiempoComprobado\t|\n");
		fprintf(stdout,"|-----------------------------------------------------------------------|\n");

		for (i = 1; i <= numComp; i++){

			itTotal += comprobaciones[i];
			fprintf(stdout,"|0%d) %10d \t| %15f \t| %15f \t|\n",i,comprobaciones[i],tiempoComprobadores[i],tiempoComp[i]);

		}

		fprintf(stdout,"|-----------------------------------------------------------------------|\n");
		fprintf(stdout,"| Iteraciones Totales: %d\n",itTotal);

		// ESTADISTICAS TOTALES DE PROCESO 0

		fprintf(stdout,"\n>------------------------ ESTADISTICAS TOTALES --------------------------<\n");
		fprintf(stdout,"| Numero de procesos: 	%d\n",nprocs);
		fprintf(stdout,"| Tiempo procesamiento: %f\n",tiempoTotal);
		fprintf(stdout,"\n>------------------------------ FIN  -------------------------------<\n");

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

			// Variables estadisticas

			double tiempoComprob, tiempoAux;
			double tiempoEspecifico = 0;
			int numComprobaciones = 0;

			// Inicio tiempo Comprobador

			tiempoComprob = MPI_Wtime();

			// Si no tiene asignados Generadores se lo salta

			if(numGenCorr != 0){

				// Mientras numero de Generadores que le corresponde sea distinto de los generadores
				// que ya han finalizado de generar palabras

				while(numGenCorr != genext){

					// Recibe id del generador y despues la palabra

					MPI_Recv(&genid,1,MPI_INT,MPI_ANY_SOURCE,id,MPI_COMM_WORLD,&status);
					MPI_Recv(&palabra_rcv,longitud,MPI_CHAR,genid,genid,MPI_COMM_WORLD,&status);


					// Bucle for que comprueba letra a letra la palabra recibida con la palabra buena
					// Tiempo Auxiliar para sumar al total de tiempo especifico en comprobaciones

					tiempoAux = MPI_Wtime();

					for(i = 0; i < longitud; i++){

						a = palabra_rcv[i];
						b = palabra[i];

						if(a != b){

							palabra_rcv[i] = CHAR_NF; // Sustitucion por espacio

						}

					}

					// Variable que guarda cuantas veces comprueba 

					numComprobaciones++;

					// Forzamos espera para dar peso al calculo

					fuerza_espera(PESO_COMPROBAR);

					// Fin tiempo auxiliar y lo sumamos a una variable que acumula tiempos

					tiempoAux = MPI_Wtime() - tiempoAux;

					tiempoEspecifico += tiempoAux;

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

			tiempoComprob = MPI_Wtime() - tiempoComprob;

			MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD); // Asegura finalizacion

			// Envio estadisticas

			MPI_Send(&id,1,MPI_INT,0,rol,MPI_COMM_WORLD);
			MPI_Send(&numComprobaciones,1,MPI_INT,0,rol,MPI_COMM_WORLD);
			MPI_Send(&tiempoComprob,1,MPI_DOUBLE,0,rol,MPI_COMM_WORLD);
			MPI_Send(&tiempoEspecifico,1,MPI_DOUBLE,0,rol,MPI_COMM_WORLD);

		}

		// Generadores

		if(rol == 1){

			srand( id * time(NULL)); // Semilla en base al tiempo y el id de cada proceso.

			// Variable que guarda el comprobador asignado al generador

			int compr_assig;
			char palabra_pist[longitud];

			// Variables estadistica tiempo

			double tiempoGen, tiempoAux;
			double tiempoEsp = 0;

			int iteraciones = 0;

			// Recibe Comprobador asignado

			MPI_Recv(&compr_assig, 1, MPI_INT, 0, rol, MPI_COMM_WORLD, &status);

			// Variables flag

			int encontrado = 0;
			int exit = 0;

			// Variable palabraAleatoria que se va obteniendo segun la generacion de palabras

			char palabraAleatoria[CHAR_MAX];

			// Tener en cuenta que si se cambia la palabra a descubrir hay que añadir aqui los caracteres que posea al menos una vez.

			char caracteresPosibles[CHAR_MAX] = "ABCDEFGHIJKLRRGPFSPRACTICAMPI2122hjash&&YhdBcLwFdSHHsdyhjkylkudaghkdjawgdalwADasdhjshdjh21658&&!1281asdasdjahsdkjhadsljhaejuj";
			

			// Bucle envio palabras generadoras

			int i,j;
			char a,b;
			int recv = 0;

			// Lleno el array de espacios vacios

			INICIAR_ARRAY(CHAR_NF,palabraAleatoria,longitud);

			// Inicializa tiempo

			tiempoGen = MPI_Wtime();

			while(encontrado != 1){

				// Genera palabra aleatoria
				// Segundo for del script de German, con este solo ya es suficiente y necesario

				for( i = 0; i < longitud; i++){ // Este bucle solo generará en las posiciones que tengan un CHAR_NF, es decir, las erroneas
					
					j = rand() % (strlen(caracteresPosibles)+1);

					if(palabraAleatoria[i] == CHAR_NF){

						palabraAleatoria[i] = caracteresPosibles[j];

					}				
				}

				// Forzamos espera

				fuerza_espera(PESO_GENERAR);

				// Variable que guarda el numero de veces que itera el generador

				iteraciones++;

				// Primero enviamos id, despues se envia la palabra al comprobador asignado

				MPI_Send(&id,1,MPI_INT,compr_assig,compr_assig,MPI_COMM_WORLD);
				MPI_Send(&palabraAleatoria,longitud,MPI_CHAR,compr_assig,id,MPI_COMM_WORLD);

				// Recibe la palabra con caracteres validos de su comprobador asignado

				tiempoAux = MPI_Wtime();
				MPI_Recv(&palabraAleatoria,longitud,MPI_CHAR,compr_assig,id,MPI_COMM_WORLD,&status);
				tiempoAux = MPI_Wtime() - tiempoAux;
				tiempoEsp += tiempoAux;

				// Envio id y palabra a proceso 0 E/S para comprobar si se termina

				MPI_Send(&id, 1, MPI_INT,0,rol,MPI_COMM_WORLD);
				MPI_Send(&palabraAleatoria, longitud, MPI_CHAR, 0, rol, MPI_COMM_WORLD);

				// Recibe flag encontrado para salir del bucle

				MPI_Recv(&encontrado, 1, MPI_INT, 0,rol, MPI_COMM_WORLD,&status);

				if(encontrado == 1){

					exit = 1;

				}

				// Envio resultado a proceso 0 E/S si termina su proceso o sigue
				// Despues envia el id del comprobador que tiene asignado, tambien al proceso 0 E/S

				MPI_Send(&exit,1,MPI_INT,0,rol,MPI_COMM_WORLD);
				MPI_Send(&compr_assig,1,MPI_INT,0,rol,MPI_COMM_WORLD);


				// PISTA IMPLEMENTADO
				// ----------------------------------------------------------------------------

				MPI_Iprobe(0,80,MPI_COMM_WORLD,&recv,&status);

				if(recv == 1){

					MPI_Irecv(&palabra_pist,longitud,MPI_CHAR,0,80,MPI_COMM_WORLD,&request);

					for(i = 0; i < longitud; i++){

						a = palabra_pist[i];
						b = palabraAleatoria[i];

						// Si a != espacio en blanco, entonces tendra una letra por lo que es letra valida acertada
						// Se guarda directamente la letra en la palabra

						if(a != CHAR_NF){

							if(a != b){

								palabraAleatoria[i] = a; // Sustitucion por letra pista

							}
						}
					}

				}
				recv = 0;
			// ------------------------------------------------------------------------------	

		}

			// Tiempo de generador

			tiempoGen = MPI_Wtime() - tiempoGen;

			MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD); //Asegura finalizacion

			// Envio estadisticas generadores al proceso cero

			MPI_Send(&id,1,MPI_INT,0,rol,MPI_COMM_WORLD);
			MPI_Send(&iteraciones,1,MPI_INT,0,rol,MPI_COMM_WORLD);
			MPI_Send(&tiempoGen,1,MPI_DOUBLE,0,rol,MPI_COMM_WORLD);
			MPI_Send(&tiempoEsp,1,MPI_DOUBLE,0,rol,MPI_COMM_WORLD);

		}
	}

	// Se quedan esperando procesos generadores y comprobadores hasta que proceso 0 termine de imprimir en pantalla

	MPI_Barrier(MPI_COMM_WORLD);

	// Fin programa

	MPI_Finalize();
	return 0;	
}


void fuerza_espera(unsigned long peso)
{
  for (unsigned long i=1; i<1*peso; i++) sqrt(i);
}
