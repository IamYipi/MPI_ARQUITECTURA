#include<stdio.h>
 #include<time.h>
 #include<stdlib.h>
 #include<ctype.h>
 #define CHAR_MAX 127
 
 int main(){
 
 char palabra[CHAR_MAX];
  
strcpy (palabra, "PRACTICAMPI2122");

int long_palabra;

long_palabra=strlen(palabra);
  
 int i=0;
 
 char cadena_aleatoria[long_palabra];
 char cad[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

 printf("*******CADENA ALEATORIA*******\n");
 srand(time(NULL));

 for(i=0;i<long_palabra;i++){
 	
 	cadena_aleatoria[i]=cad[rand()%(sizeof(cad)-1)];
 	
	printf("%c",cadena_aleatoria[i]);   

  }
 
 return 0;
 }
