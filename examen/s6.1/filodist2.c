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
        puerto_siguiente_chain = (unsigned short)atoi(lista[4]);
        puerto_local = (unsigned short)atoi(lista[5]);
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
        printf("Filósofo %d   Puerto siguiente filósofo: %d\n",
               idfilo, puerto_siguiente_chain);
        printf("Filósofo %d   Puerto local: %d\n", idfilo, puerto_local);
        printf("Filósofo %d   Delay conexión: %d\n", idfilo, delay);
    }
}
// inicializa el mutex, la variable condicional y el estado del filósofo
void inicializaciones(void)
{
    pthread_mutex_init(&mestado, NULL);
    pthread_cond_init(&condestado, NULL);
    estado = no_sentado;
}
// hilo principal del filósofo
void *filosofo(void)
{
    int numbocados = 0;

    while (numbocados < MAX_BOCADOS)
    {
        fprintf(stderr, "Filósofo %d: cambiando estado a"
                        "queriendo comer\n",
                idfilo);
        cambiarEstado(queriendo_comer);
        esperarPalillos();
        // comiendo
        fprintf(stderr, "Filósofo %d: Comiendo\n", idfilo);
        sleep(5);
        numbocados++;
        cambiarEstado(dejando_comer);
        soltarPalillos();
        fprintf(stderr, "Filósofo %d: Pensando\n", idfilo);
        sleep(10);
    }
    fprintf(stderr, "Filósofo %d: Levantándose de la mesa\n", idfilo);
    // Levantándose de la mesa
}

// sincronización con el cambio de estado a comiendo
void esperarPalillos(void)
{
    pthread_mutex_lock(&mestado);
    while (estado != comiendo)
        pthread_cond_wait(&condestado, &mestado);
    pthread_mutex_unlock(&mestado);
}
// sincronización con el cambio de estado a "pensando"
void soltarPalillos(void)
{
    pthread_mutex_lock(&mestado);
    while (estado != pensando)
        pthread_cond_wait(&condestado, &mestado);
    pthread_mutex_unlock(&mestado);
}
// Modificando el estado del filósofo
void cambiarEstado(estado_filosofo nuevoestado)
{
    pthread_mutex_lock(&mestado);
    estado = nuevoestado;
    pthread_mutex_unlock(&mestado);
}
// comprueba el estado de los palillos necesarios
// para que el filósofo pueda comer
char palillosLibres(unsigned char token)
{
    int pos;
    unsigned char ocupado = 1;
    unsigned char tokenorg = token;

    pos = idfilo;
    // desplazamiento a la derecha
    // se rellena con ceros por la
    // izquierda
    token = token >> pos;
    ocupado &= token;
    if (!ocupado)
    {
        ocupado = 1;
        if (idfilo > 0)
            pos = idfilo - 1;
        else
            pos = numfilo - 1;
        token = tokenorg >> pos;
        ocupado &= token;
    }
    return (!ocupado);
}
// cambia el token reservando o liberando los recursos que el filósofo
// utiliza en función del nuevo estado al que pasa
void alterarToken(unsigned char *tok, estado_filosofo nuevoestado)
{
    int pos;
    unsigned char bit;
    unsigned char tokenaux;

    switch (nuevoestado)
    {
    case comiendo:
        pos = idfilo;
        bit = 1;
        bit = bit << pos;
        *tok |= bit;
        if (idfilo > 0)
            pos = idfilo - 1;
        else
            pos = numfilo - 1;
        bit = 1;
        bit = bit << pos;
        *tok |= bit;
        break;
    case pensando:
        tokenaux = ~*tok;
        pos = idfilo;
        bit = 1;
        bit = bit << pos;
        tokenaux |= bit;
        if (idfilo > 0)
            pos = idfilo - 1;
        else
            pos = numfilo - 1;
        bit = 1;
        bit = bit << pos;
        tokenaux |= bit;
        *tok = ~tokenaux;
        break;
    default:
    }
}
// hilo de comunicaciones
void *comunicaciones(void)
{
    int ret;
    unsigned char token = 0; // sus posiciones marcan si los palillos
                             // están ocupados-1 o libres-0
    struct sockaddr_in next;
    struct hostent *host_info;
    int sockserver, sockant;
    int socknext;
    struct sockaddr_in servidor, anterior;
    int anterior_len;

    // 1-crear_socket_comunicacion_con_anterior y listen
    sockserver = socket(AF_INET, SOCK_STREAM, 0);
    if (sockserver < 0)
    {
        fprintf(stderr, "Filósofo %d: No se pudo crear "
                        "el socket de comunicación con el anterior "
                        "en el anillo.\n",
                idfilo);
        exit(3);
    }
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = htonl(INADDR_ANY);
    servidor.sin_port = htons(puerto_local);
    if (bind(sockserver, (struct sockaddr *)&servidor, sizeof(servidor)) < 0)
    {
        fprintf(stderr, "Filósofo %d: Error vinculando el "
                        "socket de comunicación con el anterior en el anillo.\n",
                idfilo);
        exit(4);
    }
    listen(sockserver, SOMAXCONN);

    // 2-esperar-delay para permitir que el resto de procesos
    // se lancen y lleguen a crear su socket servidor
    sleep(delay);
    // 3-conectar_con_siguiente
    socknext = socket(AF_INET, SOCK_STREAM, 0);
    if (socknext < 0)
    {
        fprintf(stderr, "Filósofo %d: Error creando el"
                        "socket de conexión con el siguiente. \n",
                idfilo);
        exit(5);
    }

    fprintf(stderr, "Filósofo %d: Dirección de conexión "
                    "del siguiente filósofo %s  puerto: %d\n",
            idfilo, siguiente_chain, puerto_siguiente_chain);

    host_info = gethostbyname(siguiente_chain);
    if (host_info == NULL)
    {
        fprintf(stderr, "Filósofo %d: nombre de host desconocido: %s\n",
                idfilo, siguiente_chain);
        exit(3);
    }
    next.sin_family = host_info->h_addrtype;
    memcpy((char *)&next.sin_addr, host_info->h_addr, host_info->h_length);
    next.sin_port = htons(puerto_siguiente_chain);

    if (connect(socknext, (struct sockaddr *)&next, sizeof(next)) < 0)
    {
        fprintf(stderr, "Filósofo %d: Error %d conectando "
                        "con el filósofo siguiente\n",
                idfilo, errno);
        perror("Error conectando\n");
        exit(7);
    }
    // 4-esperamos a que se haya aceptado la conexión del anterior
    anterior_len = sizeof(anterior);
    sockant = accept(sockserver, (struct sockaddr *)&anterior,
                     (socklen_t *)&anterior_len);
    fprintf(stderr, "Filósofo %d: Llega conexión valor %d\n", idfilo, sockant);

    // si llegamos a este punto el ciclo está completo
    // 5-si idfilosofo = 0 inyectar token
    if (idfilo == 0)
    {
        write(socknext, &token, (size_t)sizeof(unsigned char));
    }
    // mientras no fin
    while (1)
    {
        // 6- esperar token
        ret = read(sockant, &token, sizeof(unsigned char));
        if (ret != 1)
        {
            fprintf(stderr, "FIlósofo %d: Error de lectura "
                            "en el socket de conexión con el anterior nodo Ret=%d",
                    idfilo, ret);
        }
        pthread_mutex_lock(&mestado);
        if (estado == queriendo_comer)
        {
            // si no si estado = queriendo_comer
            // alterar token cuando estén libres y avanzar
            // cambiar estado a comiendo y señalar la condición
            if (palillosLibres(token))
            {
                alterarToken(&token, comiendo);
                estado = comiendo;
                pthread_cond_signal(&condestado);
            }
        }
        // si no si estado=dejando_comer
        else if (estado == dejando_comer)
        {
            // alterar token y avanzar
            // cambiar estado a pensando y señalar la condición
            alterarToken(&token, pensando);
            estado = pensando;
            pthread_cond_signal(&condestado);
        }
        pthread_mutex_unlock(&mestado);
        if (ret == 1) // si se leyó bien
        {
            ret = write(socknext, &token, sizeof(char));
            if (ret != 1)
            {
                fprintf(stderr, "Error de escritura "
                                "en el socket de conexión con el siguiente nodo\n");
            }
        }
    } // fin mientras
}
