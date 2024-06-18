// Fichero: convierte-a-anyos.c
#include <stdio.h>
int main()
{
    unsigned long int numero = 0xEA1C4A3F;
    double segundos_por_anyo = 365.25 * 24 * 60 * 60;
    double anyos = numero / segundos_por_anyo;
    printf("%f", anyos);
    return 0;
}