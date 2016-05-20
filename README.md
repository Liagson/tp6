#Trabajo tp6 - Arquitectura de sistemas
#Desenfoque gaussiano
Compilación
-----------
`gcc -lpthread main.c funciones.c -o main`
* Hay versiones de gcc donde se debe escribir:  `gcc -pthread main.c funciones.c -o main`

Ejecución
-----------
`./main (numero de threads) (fichero lectura) (fichero salida)`
* Por ejemplo: `./main 10 entrada.in salida.out`

Descripción
-----------
El problema consiste en calcular el producto de convolución para cada uno de los elementos de una matriz de 2 dimensiones. Para cada elemento la convolucion tendrá en cuenta el propio elementos de la matriz y los 8 elementos adyacentes en de la misma fila y columna. El cálculo para cada elemento de la matriz (**R**) será el siguiente:

```C
float K[5]={1.0f, 4.0f , 6.0f, 4.0f, 1.0f};

float suma = 16.0f;

R[i][j] = (M[i][j-2]*K[0]+M[i][j-1]*K[1]+M[i][j]*K[2]+M[i][j+1]*K[3]+M[i][j+2]*K[4])/suma;

R[i][j] += (M[i-2][j]*K[0]+M[i-1][j]*K[1]+M[i][j]*K[2]+M[i+1][j]*K[3]+M[i+2][j]*K[4])/suma;
```

En los contornos de la matriz, aquellos elementos que esta fuera de la misma valen 0.0f.

La matriz de origen **M** estará contenida en un fichero de que tendrá el siguiente formato:
*int1int2float1float2 ... float((int1xint2)-1)floatint1xint2EOF*

*int1* es el número entero que indica el número de filas de la matriz. *int2* es el número entero que indica el número de columnas de la matriz. A continuación se encuentran los números flotantes para cada uno de los elementos de la matriz almacenados por filas, es decir, los *int2* primeros números flotantes corresponden a la primera fila de la matriz. En el fichero únicamente se almacenan las codificaciones en binario de cada uno de los elementos y no hay ningún tipo de carácter de separación entre ellos. El tamaño los enteros(int) y flotantes(float) del fichero origen es el mismo que el que utiliza hendrix (4 bytes)

El fichero resultado correspondiente a la matriz R tiene el mismo formato que el de la matriz origen.

Threads y rendimiento
-----------
Se realizó a través de este programa un estudio sobre el rendimiento del servidor de prácticas Hendrix de la Universidad de Zaragoza (Solaris) a la hora de ir incrementando el número de hilos empleados. Para ello empleé una matriz de 22MB y realicé la convulicón sobre ella con distinto número de threads. El estudio reveló que aunque el número óptimo de threads soportados por la máquina era de 30 hilos (siendo 32 el máximo) y tardando sólo 300ms, la mejora no era muy percetible pasados los 10 hilos.

![Captura](http://i.imgur.com/iKLpqNA.png?1)
