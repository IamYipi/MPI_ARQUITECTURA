#include <stdio.h>
#include <mpi.h>
#define PESO_COMPROBAR  5000000
#define PESO_GENERAR    10000000
#define COMPROBADORES 1
#define GENERADORES 1
#define CONJUNTO "ABCDEFGHIJKLMNIOQUJAKSGHAJHGSFfghafsghafsghafstjharfasdhjshdjh216531237613&&!123adawawdytfaxwada"
#define CHAR_NF 32
#define CHAR_MAX 127
#define CHAR_MIN 33
#define INICIAR_ARRAY(valor, array, tam) for (i=0; i<tam; i++) array[i] = valor;

void fuerza_espera(unsigned long);
void notificar(int numComp, int numGen, MPI_Comm_group group_world, MPI_Comm_group compr_group, MPI_Comm_group gen_group, MPI_Comm  comm_compr, MPI_Comm comm_gen);

int main(int argc, char ** argv){
	if(argc != 2){
		fprintf(stdout, "Error: Numero invalido de argumentos\n");
		fprintf(stdout, "Usar: mpirun -np X [--oversubscribe] %s numero\n", argv[0]);
		return 1;
	}

	int id, i, j, nprocs, pista, numGen, longitud_word = 0;
	int etiqueta = 50;
	char palabra[CHAR_MAX];

	// Asignamos variables para la creacion de grupos Generadores y Comprobadores
	// Con sus respectivos comunicadores
	MPI_Group group_world,compr_group,gen_group;
	MPI_Comm  comm_compr, comm_gen;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_group(MPI_COMM_WORLD,&group_world);

	if(nprocs < 3){
		fprintf(stdout,"Error: Numero invalido de procesos, debe ser mayor o igual a 3\n");
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

		//Primero obtiene el numero de procesos generadores a partir del numero de comprobadores
		numComp = COMPROBADORES;
		numGen = nprocs - (numComp + 1);

		// Funcion que notifica a cada proceso el rol que le corresponde
		notificar(numComp,numGen,group_world,compr_group,gen_group);

		longitud_word = strlen(palabra);

		// Envía mensaje para asegurar que se han establecido los grupos correctamente
		MPI_Bcast(&i, 1, MPI_INT, 0, MPI_COMM_WORLD );

		// Envia mensaje con la longitud a los COMPROBADORES
		MPI_Bcast(&longitud_word,1,MPI_INT,0,comm_compr);

	}else{
		// Quizas habria que pensar un metodo que asegure la recepcion del Bcast 
		// Pero en principio asi ya deberia funcionar


	}

	MPI_Finalize();
	return 0;	
}


void fuerza_espera(unsigned long peso)
{
  for (unsigned long i=1; i<1*peso; i++) sqrt(i);
}

// Asignacion de los procesos segun su id a un grupo. 
// Comprobadores --> id = 1 hasta COMPROBADORES.
// Generadores   --> id = COMPROBADORES + 1 hasta numGen
void notificar(int numComp,int numGen,MPI_Comm_group group_world,MPI_Comm_group compr_group,MPI_Comm_group gen_group, MPI_Comm  comm_compr, MPI_Comm comm_gen){
	
	int i,j,compr[COMPROBADORES],gen[numGen];

	INICIAR_ARRAY(0,compr,COMPROBADORES);
	INICIAR_ARRAY(0,gen,numGen);

	for(i = 1; i <= numComp; i++){
		compr[i-1]= i;
	}

	j = numComp + 1;

	for(i = 0; i < numGen; i++){
		gen[i] = j;
		j++;
	}

	// Establecemos los grupos

	MPI_Group_incl(group_world,COMPROBADORES,compr,&compr_group);
	MPI_Group_incl(group_world,numGen,gen,&gen_group);

	//Establecemos los comunicadores

	MPI_Comm_create(group_world,compr_group,&comm_compr);
	MPI_Comm_create(group_world,gen_group,&comm_gen);

}
