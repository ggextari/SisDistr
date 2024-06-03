// Fichero: convierte-a-decimal.c
#include <stdio.h>
int main()
{
    unsigned long int numero = 0xe9897569;  // Pon el tuyo
    double segundos_por_anyo = 365.25 * 24 * 60 * 60;
    double anyos = numero / segundos_por_anyo;
    printf("Valor en años: %f\n", anyos);
    printf("Valor: %lu\n", numero);
    return 0;
}