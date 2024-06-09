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
        return(EXIT_FAILURE);
    }

    int s;
    int puerto = atoi(argv[1]);
    struct sockaddr_in direccion;
    char buffer[1024];

    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    direccion.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Fallo al crear el socket");
        return(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr *) &direccion, sizeof(direccion)) != 0) {
        perror("Error en el bind");
        return(EXIT_FAILURE);
    }

    while (1) {
        int recibidos;
        struct sockaddr_in dir_desde;
        static int tamanio = sizeof(dir_desde);

        if ((recibidos = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr *) &dir_desde, &tamanio)) < 0) {
            perror("Error en rcvfrom");
            return(EXIT_FAILURE);
        }
        sendto(s, buffer, recibidos, 0, (struct sockaddr *) &dir_desde, sizeof(dir_desde));
        fprintf(stdout, "Recibido y enviado: %s", buffer);
    }


}