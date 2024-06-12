#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto_de_escucha>", argv[0]);
        exit(-1);
    }

    int s;
    int sd;
    struct sockaddr_in direccion;
    int puerto = atoi(argv[1]);
    char buffer[1024];
    int recibidos;
    int enviados;


    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    direccion.sin_addr.s_addr = INADDR_ANY;

    fprintf(stdout, "Soy el proceso %d y voy a crear el socket pasivo\n", getpid());

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error en la creación del socket");
        exit(-1);
    }

        fprintf(stdout, "Soy el proceso %d y voy a hacer el bind\n", getpid());


    if (bind(s, (struct sockaddr *) &direccion, sizeof(direccion)) < 0) {
        perror("Fallo en el bind");
        exit(-1);
    }

        fprintf(stdout, "Soy el proceso %d y voy a hacer el listen\n", getpid());


    if (listen(s, SOMAXCONN) == -1) {
        perror("Error en el listen");
        exit(-1);
    }

    while(1) {

            fprintf(stdout, "Soy el proceso %d y voy a crear el socket de datos\n", getpid());

        if ((sd = accept(s, 0, 0)) < 0) {
            perror("Error en el accept");
            exit(-1);
        }

            fprintf(stdout, "Soy el proceso %d y voy a hacer fork\n", getpid());


        if (fork() == 0) { // Si ye fíu
            fprintf(stdout, "Soy el proceso %d y voy a cerrar el socket pasivo\n", getpid());
            close(s);
            do {

                fprintf(stdout, "Soy el proceso %d y voy a hacer read\n", getpid());
                if ((recibidos = read(sd, buffer, sizeof(buffer))) < 0) {
                    perror("Error en el read");
                    exit(-1);
                }
                if (recibidos > 0)
                    fprintf(stdout, "Soy el proceso %d y voy a hacer write\n", getpid());
                    if ((enviados = write(sd, buffer, recibidos)) < 0) {
                        perror("Error en el write");
                        exit(-1);
                    }
            } while (recibidos != 0);
            // fprintf(stdout, "Soy el proceso %d y voy a cerrar el socket pasivo", getpid());
            // close(s);
            exit(0);
        } else {
            fprintf(stdout, "Soy el proceso %d y voy a cerrar el socket de datos\n", getpid());
            close(sd);
        }
    }
}