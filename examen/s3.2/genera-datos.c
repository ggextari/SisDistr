#include <stdio.h>  // Para FILE*
#include <errno.h>  // Para perror()
#include <stdlib.h> // para exit()
#include <rpc/types.h>
#include <rpc/xdr.h>

#include "tipos.h"

#define MAXIMO_TEXTO 100

int main()
{
    FILE *f;
    XDR operacion;
    Texto t;
    Persona p;
    Resultado r;

    t = "Esta tía es bollera: ";
    f = fopen("Texto.dat", "w");
    if (f == NULL)
    {
        perror("Error al abrir el archivo");
        exit(-1);
    }
    xdrstdio_create(&operacion, f, XDR_ENCODE);
    xdr_Texto(&operacion, &t);
    xdr_destroy(&operacion);
    fclose(f);

    f = fopen("Persona.dat", "w");
    if (f == NULL)
    {
        perror("Error al abrir el archivo");
        exit(-1);
    }
    xdrstdio_create(&operacion, f, XDR_ENCODE);
    p.nombre = "Sara Díaz Menéndez";
    p.edad = 25;
    xdr_Persona(&operacion, &p);
    xdr_destroy(&operacion);
    fclose(f);

    printf("Elige una opción:\n");
    printf("    1. Entero\n");
    printf("    2. Real\n");
    printf("    3. Texto\n");
    printf("Tu elección: ");
    scanf("%d", &r.caso);
    getchar(); // Consumir el newline restante después del scanf
    if (r.caso == 1) {
        printf("Dame un entero: ");
        scanf("%d", &r.Resultado_u.n);
    } else if (r.caso == 2) {
        printf("Dame un número real: ");
        scanf("%f", &r.Resultado_u.x);
    } else if (r.caso == 3) {
        r.Resultado_u.error = malloc(MAXIMO_TEXTO * sizeof(char));
        if (r.Resultado_u.error == NULL) {
            perror("Error al asignar memoria");
            exit(-1);
        }
        printf("Dame una cadena de texto: ");
        fgets(r.Resultado_u.error, MAXIMO_TEXTO, stdin);
        // Eliminar el newline al final de la cadena si está presente
        size_t len = strlen(r.Resultado_u.error);
        if (len > 0 && r.Resultado_u.error[len - 1] == '\n') {
            r.Resultado_u.error[len - 1] = '\0';
        }
    } else {
        perror("Opción incorrecta");
        exit(-1);
    }
    f = fopen("Resultado.dat", "w");
    if (f == NULL) {
        perror("Fallo al abrir el fichero");
        exit(-1);
    }
    xdrstdio_create(&operacion, f, XDR_ENCODE);
    xdr_Resultado(&operacion, &r);
    if (r.caso == 3) {
        free(r.Resultado_u.error);
        r.Resultado_u.error = NULL;
    }
    xdr_destroy(&operacion);
    fclose(f);
    return 0;
}