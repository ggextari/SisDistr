#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto_de_escucha>", argv[0]);
        exit(EXIT_FAILURE);
    }

    int puerto = atoi(argv[1]);
    struct sockaddr_in direccion;
    int resultado;

    int s;
    s = socket(PF_INET, SOCK_STREAM, 0);

    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    direccion.sin_addr.s_addr = htonl(INADDR_ANY);



}