#include <stdio.h>  // Para FILE*
#include <errno.h>  // Para perror()
#include <stdlib.h> // para exit()
#include <rpc/types.h>
#include <rpc/xdr.h>

#include "tipos.h"

int main()
{
    FILE *f;
    XDR operacion;
    Texto t;
    Persona p;
    Resultado r;

    f = fopen("Texto.dat", "r");
    if (f == NULL)
    {
        perror("Al abrir el archivo");
        exit(-1);
    }
    xdrstdio_create(&operacion, f, XDR_DECODE);
    t = NULL;
    xdr_Texto(&operacion, &t);
    fclose(f);

    printf("Texto recibido: %s\n", t);
    xdr_free((xdrproc_t) xdr_Texto, &t);

    f = fopen("Persona.dat", "r");
    if (f == NULL)
    {
        perror("Al abrir el archivo");
        exit(-1);
    }
    xdrstdio_create(&operacion, f, XDR_DECODE);
    p.nombre = NULL;
    xdr_Persona(&operacion, &p);
    fclose(f);

    printf("\nNombre: %s\nEdad: %d\n", p.nombre, p.edad);
    xdr_free((xdrproc_t) xdr_Persona, &p);

    f = fopen("Resultado.dat", "r");
    if (f == NULL)
    {
        perror("Abriendo el fichero");
        exit(-1);
    }
    xdrstdio_create(&operacion, f, XDR_DECODE);
    r.Resultado_u.error = NULL;
    xdr_Resultado(&operacion, &r);
    fclose(f);

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
}