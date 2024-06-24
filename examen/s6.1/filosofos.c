/**************************************************************************
 *
 *                      filosofos.c
 *
 * En esta versión distribuída del problema de los filósofos requiere de un
 * maestro que controle los recursos compartidos (palillos).
 *
 *************************************************************************/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Funciones para simplificar la API de sockets. Se declaran aquí
// pero no se implementan. El alumno puede copiar su implementación
// de otros ejercicios, o implementarlas él mismo

// Crea un socket y si no hay errores lo retorna
int CrearSocketClienteTCP(void){
    int s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1)
    {
        perror("Al crear socket cliente");
        exit(1);
    }
    return s;
}

// Conecta a la IP y puertos dados. La IP se le pasa como cadena
// Si hay errores termina el programa
void ConectarConServidor(int s, char *ip, int puerto){
    int ret;
    struct sockaddr_in dir;

    dir.sin_family = AF_INET;
    dir.sin_port = htons(puerto);
    ret = inet_aton(ip, &dir.sin_addr);
    if (ret == -1){
        perror("Al asignar la ip");
        exit(1);
    }
    ret = connect(s, (struct sockaddr *) &dir, sizeof(dir));
    if (ret == -1)
    {
        perror("Al conectar");
        exit(1);
    }
    return;
}

// Las dos siguientes funciones equivalen a send() y recv() pero
// si detectan un error terminan el programa
int Enviar(int s, char *buff, int longitud)
{
    int enviados = send(s, buff, longitud, 0);
    if (enviados == -1)
    {
        perror("Al hacer send");
        exit(1);
    }
    return enviados;
}
int Recibir(int s, char *buff, int longitud)
{
    int recibidos = recv(s, buff, longitud, 0);
    if (recibidos == -1)
    {
        perror("Al hacer recv");
        exit(1);
    }
    return recibidos;
}

// Cierra el socket
void CerrarSocket(int s){
    int ret = close(s);
    if (ret == -1)
    {
        perror("Al cerrar el socket");
        exit(1);
    }
}

#define MAX_BOCADOS 10
#define LOCK 1
#define UNLOCK 2

// Prototipos de las funciones
void filosofo(int numfilo, char *ip, int puerto, int maxfilo);
int controlMutex(char *ipmaster, int puerto, char op, int nummutex);

void main(int argc, char *argv[])
{
    int numfilo;
    int puerto;
    int maxfilo;

    // Comprobación del número de argumentos
    if (argc < 5)
    {
        fprintf(stderr, "Forma de uso: %s numfilo ip_maestro puerto maxfilo\n",
                argv[0]);
        exit(1);
    }
    /*** Lectura de valores de los argumentos ***/
    numfilo = atoi(argv[1]);
    if ((numfilo < 0) || (numfilo > 4))
    {
        fprintf(stderr, "El número de filósofo debe ser >=0 y <=4\n");
        exit(2);
    }
    puerto = atoi(argv[3]);
    if ((puerto < 0) || (puerto > 65535))
    {
        fprintf(stderr, "El número de puerto debe ser >=0 y <=65535\n");
        exit(3);
    }
    maxfilo = atoi(argv[4]);
    if (maxfilo < 0)
    {
        fprintf(stderr, "El número de filósofos debe ser mayor que 0\n");
        exit(3);
    }
    /************************************************/
    // Lanzamiento del filósofo
    filosofo(numfilo, argv[2], puerto, maxfilo);
    exit(0);
}

// función que implementa las tareas a realizar por el filósofo
void filosofo(int numfilo, char *ip, int puerto, int maxfilo)
{
    int veces = 0;

    printf("Filósofo %d sentado a la mesa.\n", numfilo);
    while (veces < MAX_BOCADOS)
    {
        // mientras el acuse de la solicitud de palillo derecho sea 0
        // seguimos intentándolo
        while (controlMutex(ip, puerto, LOCK, numfilo) == 0)
            ;
        // mientras el acuse de la solicitud de palillo izquierdo sea 0
        // seguimos intentándolo
        while (controlMutex(ip, puerto, LOCK, (numfilo + 1) % maxfilo) == 0)
            ;
        // ya tenemos ambos palillos y por tanto podemos comer
        printf("El filósofo %d está comiendo.\n", numfilo);
        sleep(3);
        // mientras el acuse de liberación de palillo izquierdo sea 0
        // seguimos intentando
        // liberar el palillo
        while (controlMutex(ip, puerto, UNLOCK, (numfilo + 1) % maxfilo) == 0)
            ;
        // mientras el acuse de liberación de palillo derecho sea 0
        // seguimos intentando
        // liberar el palillo
        while (controlMutex(ip, puerto, UNLOCK, numfilo) == 0)
            ;
        // el filósofo ha soltado ambos palillos y puede dedicarse a pensar
        printf("El filósofo %d está pensando.\n", numfilo);
        sleep(5);
        // incrementamos el número de veces que el filósofo
        // ha podido completar el ciclo
        veces++;
    }
    // el filósofo ha completado el número de bocados y se levanta de la mesa
    printf("El filósofo %d se ha levantado de la mesa.\n", numfilo);
}

// esta función le permite al filósofo solicitar-liberar un recurso
// gestionado por el coordinador
int controlMutex(char *ipmaster, int puerto, char op, int nummutex)
{
    // Necesita de las funciones de libsockets
    int sock;
    char buffer[10];
    char *ptr;

    // crea el socket de conexión con el maestro de recursos
    sock = CrearSocketClienteTCP();
    // abre conexión con el maestro de recursos
    ConectarConServidor(sock, ipmaster, puerto);
    // en buffer confecciona el mensaje en función de la operación
    // a realizar y el palillo a solicitar (LOCK) o liberar (UNLOCK)
    sprintf(buffer, "%c", '\0');
    switch (op)
    {
    case LOCK:
        sprintf(buffer, "L %d", nummutex);
        break;
    case UNLOCK:
        sprintf(buffer, "U %d", nummutex);
        break;
    }
    // se envía el mensaje al maestro de recursos
    Enviar(sock, buffer, strlen(buffer));
    // se inicializa el buffer para la recepción
    sprintf(buffer, "%c", '\0');
    // se espera síncronamente la respuesta del mensaje
    Recibir(sock, buffer, 10);
    // se obtiene en ptr un puntero al primer token del mensaje
    ptr = strtok(buffer, " \t\n");
    // se cierra el socket de comunicación con el servidor
    CerrarSocket(sock);
    // se retorna el valor entero del token recibido
    //1-OK, 0-NO OK
    return(atoi(ptr));
}