#include <stdio.h>  // Para FILE*
#include <errno.h>  // Para perror()
#include <stdlib.h> // Para exit()
#include <rpc/types.h>
#include <rpc/xdr.h>

#include "tipos.h"

void imprime_datos_lista(Lista *primero)
{
    Lista *elemento;
    elemento = primero;
    while (elemento != NULL) {
        printf("Dato: %d\n", elemento->dato);
        elemento = elemento->siguiente;
    }
    printf("No hay más elementos");
}

int main()
{
    FILE *f;
    XDR operacion;
    Lista lista;
    lista.siguiente = NULL;

    f = fopen("Lista.dat", "r");
    if (f == NULL) {
        perror("Error al abrir el fichero");
        exit(-1);
    }
    xdrstdio_create(&operacion, f, XDR_DECODE);
    xdr_Lista(&operacion, &lista);
    printf("Primer elemento: %d", lista.dato);
    imprime_datos_lista(&lista);

    xdr_destroy(&operacion);
    fclose(f);
    xdr_free((xdrproc_t) xdr_Lista, &lista);
}