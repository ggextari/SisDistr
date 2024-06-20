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

    t = "Probando";
    f = fopen("Texto.dat", "w");
    if (f == NULL)
    {
        perror("Error al abrir el archivo");
        exit(-1);
    }
    xdrstdio_create(&operacion, f, XDR_ENCODE);
    xdr_Texto(&operacion, &t);
    fclose(f);

    f = fopen("Persona.dat", "w");
    if (f == NULL)
    {
        perror("Error al abrir el archivo");
        exit(-1);
    }
    p.nombre = "Maria";
    p.edad = 22;
    xdr_Persona(&operacion, &p);
    fclose(f);

    printf("Elige una opción:\n");
    printf("    1. Entero\n");
    printf("    2. Real\n");
    printf("    3. Texto\n");
    printf("Tu elección: ");
    scanf("%d", &r.caso);
    if (r.caso == 1) {
        printf("Dame un entero: ");
        scanf("%d", &r.Resultado_u.n);
    } else if (r.caso == 2) {
        printf("Dame un número real: ");
        scanf("%f", &r.Resultado_u.x);
    } else if (r.caso == 3) {
        printf("Dame una cadena de texto: ");
        scanf("%s", r.Resultado_u.error);
    } else {
        perror("Opción incorrecta");
        exit(-1);
    }
    f = fopen("Resultado.dat", "w");
    if (f == NULL) {
        perror("Fallo al abrir el fichero");
        exit(-1);
    }
    xdr_Resultado(&operacion, &r);

    xdr_destroy(&operacion);
    return 0;
}