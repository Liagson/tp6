/* los includes que necesiteis */
#include <pthread.h> /* Necesario para el multithreading */
#include <fcntl.h>   /* Necesario para el O_RDONLY, O_CREAT... */
#include <stdlib.h> /* Necesario para el malloc */

#define SIZE_OF_INT 4 //Valido para punteros
#define SIZE_OF_FLOAT 4 

/* las variables que necesiteis */
float** M; //Esta es la matriz donde se colocan los valores del fichero
float** R; //Esta es la matriz donde quedara el resultado de la convolucion

extern int columnas;
extern int filas;


/* funciones auxiliales que necesiteis */

float** crear_matriz(int filas, int columnas){
	//Se devuelve una matriz con el espacio demandado
	
	int i = 0; 	
	float **matriz = malloc(filas * SIZE_OF_FLOAT);
	for(; i < filas; i++) matriz[i] = malloc(columnas * SIZE_OF_FLOAT);
		
	return matriz;	
}


void *convolucion(void* arg){
	//Parametros de entrada fijan los limites de la zona de la matriz que va a ser convolucionada
	//Estos parametros marcan la esquina superior izquierda y la esquina inferior derecha de dicha area
	
	
	int x_orig = ((int *)arg)[0];
	int y_orig = ((int *)arg)[1];
	int x_fin = ((int *)arg)[2];
	int y_fin = ((int *)arg)[3];
		
	float K[5]={1.0f, 4.0f , 6.0f, 4.0f, 1.0f};
	float suma = 16.0f;
	float centro;
	
	int i = y_orig + 2;
	int j = x_orig + 2; 
	x_fin += 2; y_fin += 2;
	
	for(; j < x_fin; j++){
		for(; i < y_fin; i++){			
			centro = M[i][j]*K[2];
			R[i - 2][j - 2] = (M[i][j-2]*K[0]+M[i][j-1]*K[1]+centro+M[i][j+1]*K[3]+M[i][j+2]*K[4])/suma;
			R[i - 2][j - 2] += (M[i-2][j]*K[0]+M[i-1][j]*K[1]+centro+M[i+1][j]*K[3]+M[i+2][j]*K[4])/suma;
			//R[i - 2][j - 2] = R[i - 2][j - 2] / suma; //Sorprendentemente es menos eficiente			
		}
		i = y_orig + 2;
	}
	
	pthread_exit(NULL);
}

/* funciones principales que teneis que implementar */
	
int lectura_y_creacion(char *f_in, char *f_out, int *filas, int *columnas){
	/* esta funcion debe crear las estructuras de datos necesaria e inicializarlas par poder proceder al calculo del filtro 
	   f_in (parametro de entrada): puntero al nombre del fichero de entrada donde estan los datos de la matriz origen 
	   f_out (parametro de entrada): puntero al nombre del fichero de salida donde se alamacenara la matrix resultado en el mismo formato que el fichero entrada 
	   filas (parametro de salida): entero con el numero de filas de f_in 
	   columnas (parametro de salida): entero con el numero de columnas de  f_in 
	   devuelve  0 si todo ha ido bien y -1 en caso contrario */
	
	int arch_lectura, error  = 0; 
	int j = 0, i = 0, posicion = 0;
	int filas_M, columnas_M; //la matriz M tiene distinto numero de filas y columnas
	int parametros[2];
	float* buf_num; //Aqui vuelco el contenido (salvo los 8 primeros bytes) del archivo de lectura
	
	arch_lectura = open(f_in, O_RDONLY);
	if (arch_lectura == -1) return arch_lectura;
	
	error = read(arch_lectura, &parametros, SIZE_OF_INT * 2);
	if (error == -1) return error;
	
	*filas = parametros[0];
	*columnas = parametros[1]; 
	buf_num = malloc(parametros[0] * parametros[1] * SIZE_OF_FLOAT);
	
	read(arch_lectura, buf_num, SIZE_OF_FLOAT * parametros[0] * parametros[1]);
	close(arch_lectura);
	
	R = crear_matriz(*filas, *columnas); //Reservo el espacio necesario para R
	
	filas_M = *filas + 4; //Los margenes superior e inferior de la matriz (de grosor 2 filas y valor '0') son necesarios para la convolucion
	columnas_M = *columnas + 4;  //Los margenes laterales de la matriz (de grosor 2 columnas y valor '0') son necesarios para la convolucion
	
	M = crear_matriz(filas_M, columnas_M); //Reservo el espacio necesario para M
	
	for(; i < filas_M ; i++){ //Asigno los valores del fichero a M
		for(; j < columnas_M; j++){
			if(i == 0 || i == 1 || i == (filas_M - 1) || i == (filas_M - 2)) M[i][j] = 0;
			else if(j == 0 || j == 1 || j == (columnas_M - 1) || j == (columnas_M - 2)) M[i][j] = 0;
			else{												
				M[i][j] = buf_num[posicion];
				posicion++;
			}
		}		
		j = 0;
	}
	
	for(i = 0; i < filas_M; i++) free(M[i]);
	free(M);
	free(buf_num);
}
	
void calculo_filtro(int numero_threads){
	/* para la matriz de entrada calcula la convolucion utilizando tantos threads como los indicados en numero_threads */
		
	pthread_t* thread;
	thread = malloc(numero_threads * sizeof(pthread_t*));
	
	int i = 0,  posicion, anchura;	

	int** parametros; /*Aqui vendran los parametros de entrada para la convolucion */
	parametros = malloc(numero_threads * SIZE_OF_INT);
	for(;i < numero_threads; i++) parametros[i] = malloc(4 * SIZE_OF_INT);
	
	anchura = columnas / numero_threads;
	for (i = 0; i < numero_threads - 1; i++){
		posicion = i * anchura;	
		parametros[i][0] = posicion;		
		parametros[i][1] = 0;
		parametros[i][2] = posicion + anchura;
		parametros[i][3] = filas;		
	
		pthread_create(&thread[i], NULL, convolucion, parametros[i]);
	}
	
	if(numero_threads > 1) parametros[i][0] = posicion + anchura; 
	else parametros[i][0] = 0;
	parametros[i][1] = 0;
	parametros[i][2] = columnas; 
	parametros[i][3] = filas;
	
	pthread_create(&thread[i], NULL, convolucion, parametros[i]);
	for(i = 0; i < numero_threads; i++) pthread_join(thread[i], NULL);

	free(thread);
	for(i = 0; i < numero_threads; i++) free(parametros[i]);
	free(parametros);
	
}

void escribir_resultado(char *f_out){
	/* escribe la matriz resultante en el fichero f_out (parametro de entrada) */
	/* devuelve un 0 si todo ha ido bien y -1 en caso contrario */
	
	int fichero;
	int i = 0, j = 0, contador = 0;
	int parametros[] = {filas, columnas};
	float* num = malloc(filas * columnas * SIZE_OF_FLOAT);
		
	for(; i < filas; i++){
		for(; j < columnas; j++){
			num[contador] = R[i][j];
			contador++;			
		}
		j = 0;
	}
		
	fichero = creat(f_out, 0666);
	write(fichero, parametros, SIZE_OF_INT * 2);		
	write(fichero, num, SIZE_OF_FLOAT * filas * columnas);

	free(num);
	close(fichero);
}
