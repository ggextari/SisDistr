#include "calculadora.h"
// Faltarían más includes...

int main(int argc, char *argv[])
{
    CLIENT *clnt;   // contendrá información sobre el servicio remoto que buscamos
    Operandos op;   // Esta variable será el parámetro al servicio
    int *resultado; // Y aquí guardaremos la respuesta del servicio
    Resultado *resultado_dividir;

    // Inicializar la estructura con la información del servicio buscado
    // Hay que pasarle:
    //  1. IP o nombre de la máquina donde está el servidor (la obtenemos
    //     en este caso de la línea de comandos)
    //  2. Número de programa (o la constante que lo representa)
    //  3. Número de versión (o la constante que lo representa)
    //  4. Protocolo "tcp" o "udp"
    clnt = clnt_create(argv[1], CALCULADORA, BASICA, "udp");
    if (clnt == NULL)
    {
        clnt_pcreateerror("No puedo inicializar cliente");
        exit(1);
    }

    // ... A completar: inicialización de la variable op
    //     pidiéndole al usuario un par de números a sumar
    printf("Operando 1:");
    scanf("%d", &op.o1);
    printf("Operando 2:");
    scanf("%d", &op.o2);

    // Realizar invocación remota
    resultado = sumar_1(&op, clnt);
    // Mostrar resultado, recordar que devuelve un puntero
    if (resultado == NULL)
    {
        clnt_perror(clnt, "Fallo en la invocación remota");
        exit(1);
    }
    // ... A completar, imprime el valor recibido
    printf("Resultado sumar: %d\n", *resultado);

    resultado = restar_1(&op, clnt);
    if (resultado == NULL)
    {
        clnt_perror(clnt, "Fallo en la invocación remota");
        exit(1);
    }
    printf("Resultado restar: %d\n", *resultado);

    resultado = multiplicar_1(&op, clnt);
    if (resultado == NULL)
    {
        clnt_perror(clnt, "Fallo en la invocación remota");
        exit(1);
    }
    printf("Resultado multiplicar: %d\n", *resultado);

    resultado_dividir = dividir_1(&op, clnt);
    if (resultado_dividir == NULL)
    {
        clnt_perror(clnt, "Fallo en la invocación remota");
        exit(1);
    }
    printf("Resultado dividir: ");
    switch (resultado_dividir->caso)
    {
    case 1:
        printf("%d\n", resultado_dividir->Resultado_u.n);
        break;
    case 2:
        printf("%f\n", resultado_dividir->Resultado_u.x);
        break;
    case 3:
        printf("%s\n", resultado_dividir->Resultado_u.error);
        break;
    }

    clnt_destroy(clnt);
    return 0;
}