// Código de servicios.c
#include "calculadora.h"

int * sumar_1_svc(Operandos * op, struct svc_req *r) // No usaremos r
{
    static int resultado; // Variable que contendrá la respuesta del servicio

    // Lógica del servicio, algoritmo, etc..
    resultado = op->o1 + op->o2;

    return(&resultado); // se retorna por referencia
}

// Etc... de igual modo el resto de servicios

int * restar_1_svc(Operandos * op, struct svc_req *r)
{
    static int resultado;
    resultado = op->o1 - op->o2;
    return(&resultado);
}

int * multiplicar_1_svc(Operandos * op, struct svc_req *r)
{
    static int resultado;
    resultado = op->o1 * op->o2;
    return(&resultado);
}

Resultado * dividir_1_svc(Operandos * op, struct svc_req *r)
{
    static Resultado resultado;
    if (op->o2 == 0) {
        resultado.caso = 3;
        resultado.Resultado_u.error = "Operando 2 igual a 0";
    }
    else if (op->o1 % op->o2 == 0) {
        resultado.caso = 1;
        resultado.Resultado_u.n = (int) op->o1 / op->o2;
    } else {
        resultado.caso = 2;
        resultado.Resultado_u.x = (double) op->o1 / op->o2;
    }
    return(&resultado);
    
}