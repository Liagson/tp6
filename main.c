#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>  
       
/* numero de filas y de columnas de la matriz */
int filas, columnas;

/* Nombre de los ficheros entrada y salida */
char *nombre_fentrada;
char *nombre_fsalida;
  
/* numero de threads a generar */
int numero_threads;
                  
int main(int argc, char **argv){
   
	struct timeval inicio, fin;

	/* Comprobacion del numero de argumentos */
	if (argc != 4){
		fprintf(stderr, "Uso del programa: $> ./main numero_threads fichero_entrada fichero_salida\n");
		exit(-1);
	}
 
	/*Inicializacion variables simples */ 
	numero_threads	= atoi(argv[1]);
	nombre_fentrada	= argv[2];
	nombre_fsalida	= argv[3];
	
	/* Inicializacion de las matrices de entrada y salida */
	if (-1 == lectura_y_creacion(nombre_fentrada, nombre_fsalida, &filas, &columnas)){
		fprintf(stderr, "Error en la funcion lectura_y_creacion \n");
	}
	
	fprintf(stdout, "Iniciando el calculo con %d threads para la matriz %s de tamano: %d filas y %d columnas\n", numero_threads, nombre_fentrada, filas, columnas);
	
	gettimeofday(&inicio, NULL);
	
	/* calculo de la convolucion para los elementos de la matriz de entrada en la matriz de salida*/
	calculo_filtro(numero_threads);     

	gettimeofday(&fin, NULL);
	
	/* Escritura del resultado en el fichero de salida */
	escribir_resultado(nombre_fsalida);
	
	fprintf(stdout, "Calculo finalizado en %ld microsegundos y escrito en el fichero %s \n", ((fin.tv_sec * 1000000 + fin.tv_usec) - (inicio.tv_sec * 1000000 + inicio.tv_usec)), nombre_fsalida);
	

}/* end main */
