#include <stdio.h>
#include <rpc/xdr.h>

int main() {
    FILE *fichero;
    XDR xdrs;
    int entero;
    char caracter;

    // Abrir el fichero para escritura binaria
    fichero = fopen("basicos.dat", "wb");
    if (fichero == NULL) {
        perror("Error al abrir el fichero");
        return 1;
    }

    // Pedir al usuario el valor para la variable int
    printf("Introduce un número entero: ");
    scanf("%d", &entero);

    // Pedir al usuario el valor para la variable char
    printf("Introduce un caracter: ");
    scanf(" %c", &caracter);

    // Inicializar el stream XDR
    xdrstdio_create(&xdrs, fichero, XDR_ENCODE);

    // Volcar el entero al fichero usando el filtro xdr_int()
    if (!xdr_int(&xdrs, &entero)) {
        fprintf(stderr, "Error al volcar el entero al fichero\n");
        return 1;
    }

    // Volcar el caracter al fichero usando el filtro xdr_char()
    if (!xdr_char(&xdrs, &caracter)) {
        fprintf(stderr, "Error al volcar el caracter al fichero\n");
        return 1;
    }

    // Liberar recursos
    xdr_destroy(&xdrs);
    fclose(fichero);

    printf("Datos volcados correctamente al fichero basicos.dat\n");

    return 0;
}
