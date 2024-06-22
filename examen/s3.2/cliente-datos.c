#include <stdio.h>  // Para FILE*
#include <errno.h>  // Para perror()
#include <stdlib.h> // Para exit()
#include <sys/socket.h> //Funciones de sockets
#include <arpa/inet.h>  //Funciones de sockets
#include <unistd.h>     //dup, close
#include <rpc/types.h>
#include <rpc/xdr.h>

#include "tipos.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Uso %s <IP> <puerto>", argv[0]);
        exit(-1);
    }

    char *ip = argv[1];
    int puerto = atoi(argv[2]);
    int s;
    struct sockaddr_in dir_servidor;

    FILE *f;
    XDR operacion;
    Texto t;
    Persona p;
    Resultado r;

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Al crear el socket");
        exit(-1);
    }
    dir_servidor.sin_family = AF_INET;
    dir_servidor.sin_port = htons(puerto);
    dir_servidor.sin_addr.s_addr = inet_addr(ip);
    if (connect(s, (struct sockaddr *) &dir_servidor, sizeof(dir_servidor)) == -1)
    {
        perror("Al hacer connect");
        exit(-1);
    }

    f = fdopen(s, "r");
    xdrstdio_create(&operacion, f, XDR_DECODE);
    t = NULL;
    xdr_Texto(&operacion, &t);

    printf("Texto recibido: %s\n", t);
    xdr_free((xdrproc_t)xdr_Texto, &t);

    p.nombre = NULL;
    xdr_Persona(&operacion, &p);

    printf("\nNombre :%s\nEdad: %d\n", p.nombre, p.edad);
    xdr_free((xdrproc_t)xdr_Persona, &p);

    r.Resultado_u.error = NULL;
    xdr_Resultado(&operacion, &r);

    switch (r.caso)
    {
    case 1:
        printf("\nEntero recibido: %d\n", r.Resultado_u.n);
        break;
    case 2:
        printf("\nNúmero real recibido: %f\n", r.Resultado_u.x);
        break;
    case 3:
        printf("\nCadena recibida: %s\n", r.Resultado_u.error);
        break;
    }
    xdr_free((xdrproc_t) xdr_Resultado, &r);

    xdr_destroy(&operacion);
    fclose(f);
}