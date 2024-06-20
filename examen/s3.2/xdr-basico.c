// xdr-basico.c
#include <stdio.h>      // Para FILE*
#include <errno.h>      // Para perror()
#include <stdlib.h>     // para exit()
#include <rpc/types.h>
#include <rpc/xdr.h>

int main()
{
    int n;
    char c;
    char buf[100];
    FILE *fichero;
    XDR operacion;

    fprintf(stdout, "Introduce un número y un caracter: ");
    fgets(buf, sizeof(buf), stdin);
    n = atoi(buf);
    fgets(buf, sizeof(buf), stdin);
    c = buf[0];

    fichero = fopen("basicos.dat", "w");    // Abrir para "w"rite
    if (fichero == NULL) {                  // Comprobar errores
        perror("Al abrir fichero");
        exit(1);
    }

    // Inicializar variable operacion para filtros subsiguientes
    xdrstdio_create(&operacion, fichero, XDR_ENCODE);

    // Escribir la variable n en el fichero, codificada en representación externa
    xdr_int(&operacion, &n);

    // Escribir la variable c en el fichero, codificada en representación externa
    xdr_char(&operacion, &c);

    // Terminado, labores finales "domésticas"
    xdr_destroy(&operacion);    // Destruir la variable operacion
    fclose(fichero);            // Cerrar fichero
    return 0;
}