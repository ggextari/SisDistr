#include <stdio.h>  // Para FILE*
#include <errno.h>  // Para perror()
#include <stdlib.h> // Para exit()
#include <sys/socket.h> // Funciones de sockets
#include <arpa/inet.h>  // Funciones de sockets
#include <unistd.h>     //dup, close
#include <rpc/types.h>
#include <rpc/xdr.h>

#include "tipos.h"

#define MAXIMO_TEXTO 100

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Uso: %s <puerto>", argv[1]);
        exit(1);
    }

    int puerto = atoi(argv[1]);
    int socket_pasivo, socket_datos;
    struct sockaddr_in direccion;

    FILE *f;
    XDR operacion;
    
    Texto t;
    Persona p;
    Resultado r;

    if ((socket_pasivo = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Al crear el socket pasivo");
        exit(-1);
    }

    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    direccion.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind (socket_pasivo, (struct sockaddr *) &direccion, sizeof(direccion)) == -1)
    {
        perror("En el bind");
        exit(-1);
    }

    if (listen(socket_pasivo, SOMAXCONN) == -1)
    {
        perror("En el listen");
        exit(-1);
    }
    while(1)
    {
        if ((socket_datos = accept(socket_pasivo, 0, 0)) == -1)
        {
            perror("En el accept");
            exit(-1);
        }
        
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

        f = fdopen(socket_datos, "w");
        if (f == NULL)
        {
            perror("En el fichero de escritura");
            exit(-1);
        }
        xdrstdio_create(&operacion, f, XDR_ENCODE);


        t = "Esta tía es bollera: ";
        xdr_Texto(&operacion, &t);

        p.nombre = "Sara Díaz Menéndez";
        p.edad = 25;
        xdr_Persona(&operacion, &p);

        xdr_Resultado(&operacion, &r);
        if (r.caso == 3) {
            free(r.Resultado_u.error);
            r.Resultado_u.error = NULL;
        }
        xdr_destroy(&operacion);
        fclose(f);
    }
    return 0;
}