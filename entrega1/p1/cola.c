#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cola.h"

void inicializar_cola(Cola *cola, int tam_cola)
{
    register int i;


    if (tam_cola < 1)
    {
        perror("Error: El tamaño de la cola debe ser mayor o igual que 1");
        exit(1);
    }
    if (cola == NULL)
    {
        perror("Error: El puntero a la cola es NULL en inicializar_cola");
        exit(2);
    }

    // A RELLENAR el resto de la inicialización de la cola
    // Inicialización de la cola
    cola->datos = (dato_cola **)malloc(sizeof(dato_cola *) * tam_cola);
    if (cola->datos == NULL)
    {
        perror("Error: No se pudo reservar memoria para la cola");
        exit(3);
    }
    cola->tam_cola = tam_cola;
    cola->head = cola->tail = 0;

    // Inicialización de mutex y semáforos
    if (pthread_mutex_init(&(cola->mutex_head), NULL) != 0)
    {
        perror("Error: No se pudo inicializar el mutex de la cola");
        exit(4);
    }
    if (pthread_mutex_init(&(cola->mutex_tail), NULL) != 0)
    {
        perror("Error: No se pudo inicializar el mutex de la cola");
        exit(5);
    }
    if (sem_init(&(cola->num_huecos), 0, tam_cola) != 0)
    {
        perror("Error: No se pudo inicializar el semáforo de cola vacía");
        exit(6);
    }
    if (sem_init(&(cola->num_ocupados), 0, 0) != 0)
    {
        perror("Error: No se pudo inicializar el semáforo de cola llena");
        exit(7);
    }

     // Inicializar cada elemento del array a NULL
    for (i = 0; i < tam_cola; i++)
    {
        cola->datos[i] = NULL;
    }
}

void destruir_cola(Cola *cola)
{
    // Debe liberarse la memoria apuntada por cada puntero guardado en la cola
    // y la propia memoria donde se guardan esos punteros, así como
    // destruir los semáforos y mutexes
    
    // A RELLENAR
    // Liberar la memoria de cada elemento de la cola
    for (int i = 0; i < cola->tam_cola; i++)
    {
        if (cola->datos[i] != NULL)
        {
            free(cola->datos[i]);
            cola->datos[i] = NULL;
        }
    }

    // Liberar la memoria de la cola y destruir mutex y semáforos
    free(cola->datos);
    pthread_mutex_destroy(&(cola->mutex_head));
    pthread_mutex_destroy(&(cola->mutex_tail));
    sem_destroy(&(cola->num_huecos));
    sem_destroy(&(cola->num_ocupados));
}

void insertar_dato_cola(Cola *cola, dato_cola *dato)
{
    // A RELLENAR
    sem_wait(&(cola->num_huecos)); // Esperar a que haya espacio en la cola

    pthread_mutex_lock(&(cola->mutex_tail)); // Bloquear el acceso al final de la cola

    cola->datos[cola->tail] = dato;
    cola->tail = (cola->tail + 1) % cola->tam_cola;

    pthread_mutex_unlock(&(cola->mutex_tail)); // Desbloquear el acceso al final de la cola

    sem_post(&(cola->num_ocupados)); // Incrementar el contador de elementos en la cola
}

dato_cola *obtener_dato_cola(Cola *cola)
{
    dato_cola *p;
    
    // A RELLENAR
    sem_wait(&(cola->num_ocupados)); // Esperar a que haya elementos en la cola

    pthread_mutex_lock(&(cola->mutex_head)); // Bloquear el acceso al inicio de la cola

    p = cola->datos[cola->head];
    cola->head = (cola->head + 1) % cola->tam_cola;

    pthread_mutex_unlock(&(cola->mutex_head)); // Desbloquear el acceso al inicio de la cola

    sem_post(&(cola->num_huecos)); // Incrementar el contador de espacios libres en la cola

    return p;
}
