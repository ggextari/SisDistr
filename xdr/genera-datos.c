#include <stdio.h>
#include <rpc/xdr.h>
#include "tipos.h"

int main() {
    FILE *archivo_texto, *archivo_persona, *archivo_resultado;
    XDR xdr_stream_texto, xdr_stream_persona, xdr_stream_resultado;
    Texto texto = "Probando";
    Persona persona = {"Maria", 22};
    Resultado resultado;
    int opcion;

    // Abrir archivos
    archivo_texto = fopen("Texto.dat", "w");
    archivo_persona = fopen("Persona.dat", "w");
    archivo_resultado = fopen("Resultado.dat", "w");

    // Inicializar operaciones XDR
    xdrstdio_create(&xdr_stream_texto, archivo_texto, XDR_ENCODE);
    xdrstdio_create(&xdr_stream_persona, archivo_persona, XDR_ENCODE);
    xdrstdio_create(&xdr_stream_resultado, archivo_resultado, XDR_ENCODE);

    // Volcar datos a los archivos usando filtros XDR
    xdr_Texto(&xdr_stream_texto, &texto);
    xdr_Persona(&xdr_stream_persona, &persona);

    // Menú de opciones
    printf("Elige una opcion:\n");
    printf("    1. Entero\n");
    printf("    2. Real\n");
    printf("    3. Texto\n");
    printf("Tu eleccion: ");
    scanf("%d", &opcion);

    // Leer datos según la opción elegida por el usuario
    resultado.caso = opcion;
    switch (opcion) {
        case 1:
            printf("Introduce un numero entero: ");
            scanf("%d", &resultado.Resultado_u.n);
            break;
        case 2:
            printf("Introduce un numero real: ");
            scanf("%f", &resultado.Resultado_u.x);
            break;
        case 3:
            printf("Introduce un texto: ");
            scanf("%s", resultado.Resultado_u.error);
            break;
        default:
            printf("Opcion invalida\n");
            return 1;
    }

    // Volcar resultado a archivo usando filtro XDR
    xdr_Resultado(&xdr_stream_resultado, &resultado);

    // Cerrar archivos
    fclose(archivo_texto);
    fclose(archivo_persona);
    fclose(archivo_resultado);

    return 0;
}
