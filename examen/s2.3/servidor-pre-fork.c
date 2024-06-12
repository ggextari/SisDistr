#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto_de_escucha>", argv[0]);
        exit(EXIT_FAILURE);
    }

    int s;
    struct sockaddr_in direccion;
    int puerto = atoi(argv[1]);

    direccion.sin_port = htons(puerto);
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = htonl(INADDR_ANY);

    fprintf(stdout, "Soy el proceso %d y voy a crear el socket pasivo\n", getpid());

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error al crear el socket pasivo");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Soy el proceso %d y voy a hacer bind\n", getpid());

    if (bind(s, (struct sockaddr *) &direccion, sizeof(direccion)) < 0) {
        perror("Error al hacer bind");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Soy el proceso %d y voy a hacer el listen\n", getpid());
    if (listen(s, SOMAXCONN) < 0){
        perror("Error en el listen");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Soy el proceso %d y voy a hacer fork\n", getpid());

    if (fork() == 0) {
        fprintf(stdout, "Soy el proceso %d y voy a hacer fork\n", getpid());
        if (fork() == 0) {
            fprintf(stdout, "Soy el proceso %d y voy a hacer fork\n", getpid());
            fork();
        }
    }

    while(1) {
        int sd;
        char buffer[1024];
        int recibidos;
        int enviados;

        fprintf(stdout, "Soy el proceso %d y voy a esperar en accept()\n", getpid());
        if ((sd = accept(s, 0, 0)) < 0) {
            perror("Error en el accept");
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "Soy el proceso %d y me entró un cliente\n", getpid());

        while(recibidos != 0) {

            fprintf(stdout, "Soy el proceso %d y voy a hacer read\n", getpid());
            if ((recibidos = read(sd, buffer, sizeof(buffer))) < 0) {
                perror("Error en el read");
                exit(EXIT_FAILURE);
            }

            fprintf(stdout, "Soy el proceso %d y voy a hacer write\n", getpid());
            if ((enviados = write(sd, buffer, recibidos)) < 0) {
                perror("Error en el write");
                exit(EXIT_FAILURE);
            }
        }

        fprintf(stdout, "Soy el proceso %d y voy a cerrar el socket de datos", getpid());
        close(sd);

    }    



}