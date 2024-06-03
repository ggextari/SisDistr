#include <stdio.h>
#include <rpc/xdr.h>
#include "tipos.h"

int main() {
    FILE *archivo_texto, *archivo_persona, *archivo_resultado;
    XDR xdr_stream_texto, xdr_stream_persona, xdr_stream_resultado;
    Texto texto;
    Persona persona;
    Resultado resultado;

    // Abrir archivos
    archivo_texto = fopen("Texto.dat", "r");
    archivo_persona = fopen("Persona.dat", "r");
    archivo_resultado = fopen("Resultado.dat", "r");

    // Inicializar operaciones XDR
    xdrstdio_create(&xdr_stream_texto, archivo_texto, XDR_DECODE);
    xdrstdio_create(&xdr_stream_persona, archivo_persona, XDR_DECODE);
    xdrstdio_create(&xdr_stream_resultado, archivo_resultado, XDR_DECODE);

    // Leer datos de los archivos usando filtros XDR
    xdr_Texto(&xdr_stream_texto, &texto);
    xdr_Persona(&xdr_stream_persona, &persona);
    xdr_Resultado(&xdr_stream_resultado, &resultado);

    // Mostrar datos leídos
    printf("Texto: %s\n", texto);
    printf("Persona: Nombre - %s, Edad - %d\n", persona.nombre, persona.edad);
    switch (resultado.caso) {
        case 1:
            printf("Resultado: Entero - %d\n", resultado.Resultado_u.n);
            break;
        case 2:
            printf("Resultado: Real - %f\n", resultado.Resultado_u.x);
            break;
        case 3:
            printf("Resultado: Texto - %s\n", resultado.Resultado_u.error);
            break;
    }

    // Cerrar archivos
    fclose(archivo_texto);
    fclose(archivo_persona);
    fclose(archivo_resultado);

    return 0;
}
