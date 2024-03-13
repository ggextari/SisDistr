// servicios.c
#include "calculadora.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

int * sumar_1_svc(Operandos *op, struct svc_req *r) // No usaremos r
{
    static int resultado; // Variable que contendrá la respuesta del servicio

    // Lógica del servicio de suma
    resultado = op->op1 + op->op2;

    return (&resultado); // Se retorna por referencia
}

// Implementación del resto de funciones de la calculadora

int * restar_1_svc(Operandos *op, struct svc_req *r)
{
    static int resultado; // Variable que contendrá la respuesta del servicio

    // Lógica del servicio de resta
    resultado = op->op1 - op->op2;

    return (&resultado); // Se retorna por referencia
}

int * multiplicar_1_svc(Operandos *op, struct svc_req *r)
{
    static int resultado; // Variable que contendrá la respuesta del servicio

    // Lógica del servicio de multiplicación
    resultado = op->op1 * op->op2;

    return (&resultado); // Se retorna por referencia
}

Resultado * dividir_1_svc(Operandos *op, struct svc_req *r)
{
    static Resultado res; // Variable que contendrá la respuesta del servicio

    // Lógica del servicio de división
    if (op->op2 != 0)
    {
        if (op->op1 % op->op2 == 0)
        {
            res.caso = 1; // Caso de división exacta
            res.Resultado_u.n = op->op1 / op->op2;
        }
        else
        {
            res.caso = 2; // Caso de división no exacta
            res.Resultado_u.x = op->op1 * 1.0 / op->op2;
        }
    }
    else
    {
        res.caso = 3; // Caso de división por cero
        res.Resultado_u.error = "Error: División por cero";
    }

    return (&res); // Se retorna por referencia
}
