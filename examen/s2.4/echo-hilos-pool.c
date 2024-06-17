// Implementación de un servidor que implementa el servicio ECHO
// RFC 0862
// Se implementa solo el servicio bajo TCP. Se logra la concurrencia
// creando un pool de hilos que atienden a los clientes. Cada hilo
// proporciona el servicio echo y el padre espera por otro cliente.
// Cuando el hilo finaliza de dar el servicio, mira la cola para
// ver si hay clientes.

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

#define PUERTO 7777  // Puerto por defecto (el estándar 7 está protegido)
#define MAX_LINEA 80
#define TAM_POOL 3
#define TAM_COLA 10

/**** Definición de tipos de datos ****/
// Datos de la conexión (que se le pasan al hilo)
struct conex {
    char *origen;       //IP origen
    int puerto_origen;  //Puerto de origen
    int sock;           //Socket de conexión con el cliente
};
typedef struct conex datos_conexion;

struct cola {
    datos_conexion *cola;
    int tam_cola;
    int head;
    int tail;
    pthread_mutex_t mutex;
    sem_t sem_num_huecos;
    sem_t sem_num_ocupados;
};
typedef struct cola cola_trabajos;
/*************************************/

//Socket de servicio y socket de respuesta
int sockEscucha, sockDatos;
cola_trabajos cola;

/* Un manejador para cuando se pulse Ctrl-C en el servidor*/
void SalirBien(int s)
{
    printf("Interrupción desde teclado. Terminando servidor. \n");
    close(sockEscucha);
    exit(0);
}

// Funciones para simplificar el acceso a la API de sockets
// Se ocupan de todo el manejo de errores
// ********************************************************
int CrearSocketServidorTCP(int puerto)
{
    int ret;
    int sock;
    struct sockaddr_in dir;

    ret=socket(PF_INET, SOCK_STREAM, 0);
    if(ret == 1) {
        perror("Al crear socket");
        exit(-1);
    }
    sock = ret;
    dir.sin_family = AF_INET;
    dir.sin_port = htons(puerto);
    dir.sin_addr.s_addr = htonl(INADDR_ANY);
    ret = bind(sock, (struct sockaddr *) &dir, sizeof(dir));
    if(ret == -1) {
        perror("Al asignar dirección");
        close(sock);
        exit(-1);
    }
    ret = listen(sock, SOMAXCONN);
    if(ret == -1) {
        perror("Al poner en modo escucha");
        close(sock);
        exit(-1);
    }
    return sock;
}

int AceptarConexion(int sock, char **ip, int *puerto)
{
    int ret;
    struct sockaddr_in dir;
    static socklen_t longitud = sizeof(dir);

    ret = accept(sock, (struct sockaddr *) &dir, &longitud);
    if (ret == -1) {
        perror("Al aceptar");
        exit(-1);
    }
    *ip = inet_ntoa(dir.sin_addr);
    *puerto = ntohs(dir.sin_port);
    return ret;
}

int Enviar(int sock, char *buff, int longitud)
{
    int ret;
    ret = send(sock, buff, longitud, 0);
    if (ret == -1) {
        perror("Al enviar");
        exit(-1);
    }
    return ret;
}

int Recibir(int sock, char *buff, int longitud)
{
    int ret;
    ret = recv(sock, buff, longitud, 0);
    if (ret == -1) {
        perror("Al recibir");
        exit(-1);
    }
    return ret;
}

void CerrarSocket(int sock)
{
    if (close(sock) == -1) {
        perror("Al cerrar el socket");
        exit(-1);
    }
}

void InicializarCola(int tam_cola)
{
    cola.cola = malloc(sizeof(datos_conexion) * cola.tam_cola);
    cola.tam_cola = tam_cola;
    cola.head = 0;
    cola.tail = 0;
    if (pthread_mutex_init(&(cola.mutex), NULL) != 0) {
        perror("Al inicializar el mutex");
        exit(-1);
    }
    if (sem_init(&(cola.sem_num_huecos),0,TAM_COLA) != 0) {
        perror("Al iniciar el semáforo de huecos");
        exit(-1);
    }
    if (sem_init(&(cola.sem_num_ocupados), 0, 0) != 0) {
        perror("Al inicializar el semáforo de ocupados");
        exit(-1);
    }
}

void DestruirCola()
{
    free(cola.cola);
    pthread_mutex_destroy(&(cola.mutex));
    sem_destroy(&(cola.sem_num_huecos));
    sem_destroy(&(cola.sem_num_ocupados));
}

void InsertarDatoCola(datos_conexion con)
{
    sem_wait(&(cola.sem_num_huecos));
    pthread_mutex_lock(&(cola.mutex));
    cola.cola[cola.head] = con;
    cola.head = (cola.head + 1) % cola.tam_cola;
    pthread_mutex_unlock(&(cola.mutex));
    sem_post(&(cola.sem_num_ocupados));
}

datos_conexion ObtenerDatoCola()
{
    datos_conexion dato;
    sem_wait(&(cola.sem_num_ocupados));
    pthread_mutex_lock(&(cola.mutex));
    dato = cola.cola[cola.tail];
    cola.tail = (cola.tail + 1) % cola.tam_cola;
    pthread_mutex_unlock(&(cola.mutex));
    sem_post(&(cola.sem_num_huecos));
    return dato;
}

void servicioEcho(datos_conexion s)
{
    int recibidos;
    int enviados;
    char buffer[MAX_LINEA];
    printf("Recibida conexión desde %s(%d)\n",
    s.origen, s.puerto_origen);
    //Leemos datos del socket mientras que el cliente no cierre la conexión
    do {
        recibidos = Recibir(s.sock, buffer, MAX_LINEA);
        printf("Recibida un mensaje de longitud %d\n",
        recibidos);
        buffer[recibidos] = 0;  // Añadir terminador
        printf("Contenido: %s\n", buffer);
        enviados = Enviar(s.sock, buffer, recibidos);
    } while (recibidos != 0);
    CerrarSocket(s.sock);
    return;
}


// ***********************************************

//Función que ejecuta el hilo que atiende cada petición
void * HiloJefe(int *s)
{
    datos_conexion *con;
    char * desde;
    int puerto_desde;
    int n_hilos = 0;
    while(1)
    {
        sockDatos = AceptarConexion(*s, &desde, &puerto_desde);
        con = (datos_conexion *) malloc(sizeof(datos_conexion));
        con -> origen = (char *) malloc(strlen(desde) + 1);
        strcpy(con -> origen, desde);
        con -> puerto_origen = puerto_desde;
        con -> sock = sockDatos;
        InsertarDatoCola(*con);
    }
}

void * HiloTrabajador()
{
    datos_conexion s;
    while(1)
    {
        s = ObtenerDatoCola();
        servicioEcho(s);
    }
}

int main(int argc, char *argv[])
{
    int tam_pool = TAM_POOL;
    int tam_cola = TAM_COLA;
    int puerto = PUERTO;
    char *desde;
    int puerto_desde;
    int n_hilos = 0;
    pthread_t hilo_jefe;
    datos_conexion *con;

    if (argc > 4) {
        printf("Uso: %s [tam_pool] [tam_cola] [puerto]", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (argc > 1)
        tam_pool = atoi(argv[1]);
    if (argc > 2)
        tam_cola = atoi(argv[2]);
    if (argc > 3)
        puerto = atoi(argv[3]);

    pthread_t hilo_trabajador[tam_pool];

    // Tratar el Ctrl-C para cerrar bien el socket de escucha
    signal(SIGINT, SalirBien);

    InicializarCola(tam_cola);

    sockEscucha = CrearSocketServidorTCP(puerto);
    //Creamos un nuevo hilo que atenderá la nueva conexión
    if (pthread_create(&hilo_jefe, NULL, (void *) HiloJefe, (void *) &sockEscucha) != 0) {
        perror("Al crear el hilo jefe");
        exit(-1);
    }
    for (int i = 0; i < tam_pool; i++)
        if (pthread_create(&hilo_trabajador[i], NULL, (void *) HiloTrabajador, NULL) != 0) {
            fprintf(stderr, "Error al crear el hilo %d", i);
            exit(-1);
        }
    
    for (int i = 0; i < tam_pool; i++)
        pthread_join(hilo_trabajador[i], NULL);
    DestruirCola();
    return 0;
}