#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto_de_escucha>", argv[0]);
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

    socklen_t tam_direccion = sizeof(direccion);
    if ((resultado = bind(s, (struct sockaddr *) &direccion, tam_direccion)) != 0) {
        perror("Error en el bind");
        return(EXIT_FAILURE);
    }

    if (listen(s, SOMAXCONN) == -1) {
        perror("Error en el listen");
        return(EXIT_FAILURE);
    }

    while (1) {
        if ((sd = accept(s, 0, 0)) == -1) {
            perror("Error en el accept");
            return(EXIT_FAILURE);
        }

        char buffer[1024];
        int recibidos = read(sd, buffer, sizeof(buffer));

        do {
            fprintf(stdout,"Recibido: %s", buffer);
            int enviado = write(sd, buffer, recibidos);
            fprintf(stdout,"Enviado: %s", buffer);
            int recibidos = read(sd, buffer, sizeof(buffer));
            
        } while (recibidos > 0);
        if (close(sd) != 0){
            perror("Fallo en el close");
            return(EXIT_FAILURE);
        }

    }



}