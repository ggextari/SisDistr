/************************************************************************/
/* Versión 2 de los filósofos distribuídos con paso de testigo e anillo */
/* 2 hilos, uno para comunicaciones y otro para simular el filósofo     */
/*                                                                      */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "filodist.h"

/* variables globales */
// identificador del filósofo
int idfilo;
// número de filósofos en la simulación
int numfilo;
// dir IP o nombre FQDN del siguiente filósofo
// en el anillo lógico
char siguiente_chain[45];
// puerto donde enviar en testigo al siguiente filósofo
unsigned short int puerto_siguiente_chain;
// puerto local en donde debemos recibir el testigo
unsigned short int puerto_local;
// delay inicial antes de conectar con el siguiente
// filósofo en el anillo lógico. Este delay permite
// que el siguiente filosófico haya creado, vinculado(bind)
// y hecho el listen en su socket servidor
int delay;
// estado del filósofo
estado_filosofo estado;
// mutex que protege las modificaciones al valor
// del estado del filósofo
pthread_mutex_t mestado;
// variable condicional que permite suspender al filósofo
// hasta que se produce el cambio de estado efectivo
pthread_cond_t condestado;

/* prototipos funciones */
void procesaLineaComandos(int numero, char *lista[]);
void inicializaciones(void);
void *filosofo(void);
void esperarPalillos(void);
void soltarPalillos(void);
void cambiarEstado(estado_filosofo nuevoestado);
char palillosLibres(unsigned char token);
void alterarToken(unsigned char *tok, estado_filosofo nuevoestado);
void *comunicaciones(void);
void *esperarConexion(void);

int main(int argc, char *argv[])
{
    int ret;
    // objetos de datos de hilo
    pthread_t h1, h2;

    procesaLineaComandos(argc, argv);
    inicializaciones();
    // Lanzamiento del hilo de comunicaciones de filósofo
    ret = pthread_create(&h1, NULL, (void *)comunicaciones, (void *)NULL);
    if (ret != 0)
    {
        fprintf(stderr, "Filósofo %d: Falló al lanzar "
                        "el hilo de comunicaciones\n",
                idfilo);
        exit(10);
    }
    // Lanzamiento del hilo principal de funcionamiento del filósofo
    ret = pthread_create(&h2, NULL, (void *)filosofo, (void *)NULL);
    if (ret != 0)
    {
        fprintf(stderr, "Filósofo: %d: Falló al lanzar el hilo filósofo\n",
                idfilo);
        exit(10);
    }
    // sincronización con la terminación del hilo de comunicaciones y el
    // hilo que ejecuta la función filósofo
    pthread_join(h1, NULL);
    pthread_join(h2, NULL);
    return 0;
}

// procesa la línea de comandos, almacena los valores leídos en variables
// globales e imprime los valores leídos
void procesaLineaComandos(int numero, char *lista[])
{
    if (numero != 7)
    {
        fprintf(stderr, "Forma de uso: %s id_filosofo num_filosofos "
                        "ip_siguiente puerto_siguiente "
                        "puerto_local delay_conexion\n",
                lista[0]);
        fprintf(stderr, "Donde id_filosofo es un valor de 0 a n. "
                        "El iniciador del anillo debe ser "
                        "el filósofo con id=0\n");
        exit(1);
    }
    else
    {
        idfilo = atoi(lista[1]);
        numfilo = atoi(lista[2]);
        strcpy(siguiente_chain, lista[3]);
        puerto_siguiente_chain = (unsigned short) atoi(lista[4]);
        puerto_local = (unsigned short) atoi(lista[5]);
        delay = atoi(lista[6]);
        if ((numfilo < 2) || (numfilo > 8))
        {
            fprintf(stderr, "El número de filósofos debe ser >=2 y <8\n");
            exit(2);
        }
        printf("Filósofo %d Valores leídos:\n", idfilo);
        printf("Filósofo %d   Número filósofos: %d\n", idfilo, numfilo);
        printf("Filósofo %d   Dir. IP siguiente filósofo: %s\n",
        idfilo, siguiente_chain);
        printf
    }
}