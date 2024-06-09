#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto_de_escucha>", argv[1]);
        exit(EXIT_FAILURE);
    }

    int puerto = atoi(argv[1]);
    struct sockaddr_in direccion;
    int resultado;

    int s;
    int sd;
    s = socket(PF_INET, SOCK_STREAM, 0);

    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    direccion.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((resultado = bind(s, (struct sockaddr *) &direccion, sizeof(direccion))) != 0) {
        perror("Error en el bind");
        return(EXIT_FAILURE);
    }

    if (listen(s, SOMAXCONN) == -1) {
        perror("Error en el listen");
        return(EXIT_FAILURE);
    }

    while(1) {
        if ((sd = accept(s, 0, 0)) == -1) {
            perror("Error en el accept");
            return(EXIT_FAILURE);
        }

        char buffer[1024];
        int recibidos;
        int enviados;
        if ((recibidos = read(sd, buffer, sizeof(buffer))) < 0) {
            perror("Error en el read");
            return(EXIT_FAILURE);
        }

        do {
            int i = 0;
            while (buffer[i] != '\0') {
                buffer[i] = toupper(buffer[i]);
                i++;
            }
            if ((enviados = write(sd, buffer, recibidos)) < 0) {
                perror("Fallo en el write");
                return(EXIT_FAILURE);
            }
            fprintf(stdout, "Enviado: %s", buffer);
            if ((recibidos = read(sd, buffer, sizeof(buffer))) < 0) {
                perror("Fallo en el read");
                return(EXIT_FAILURE);
            }

        } while (recibidos > 0);
        if (close(sd) != 0) {
            perror("Fallo en el close");
            return(EXIT_FAILURE);
        }
    }
}