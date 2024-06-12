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

#define MAX_HIJOS 4

// Manejador de la señal SIGCHLD para limpiar procesos hijos terminados
void manejador_sigchld(int signo) {
    (void)signo; // Evitar advertencias de variable no usada
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <puerto_de_escucha>", argv[0]);
        exit(EXIT_FAILURE);
    }

    int s, sd;
    struct sockaddr_in direccion;
    int puerto;
    char buffer[1024];
    int recibidos;
    int enviados;

    puerto = atoi(argv[1]);

    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    direccion.sin_addr.s_addr = htonl(INADDR_ANY);

    fprintf(stdout, "Soy el proceso %d y voy a crear el socket pasivo\n", getpid());
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error al crear el socket pasivo");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Soy el proceso %d y voy a hacer el bind\n", getpid());
    if (bind(s, (struct sockaddr *) &direccion, sizeof(direccion)) < 0) {
        perror("Error en el bind");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Soy el proceso %d y voy a hacer el listen\n", getpid());
    if (listen(s, SOMAXCONN) < -1) {
        perror("Error al hacer el listen");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Soy el proceso %d y voy a configurar el manejador de la señal SIGCHLD\n", getpid())
    // Configurar el manejador de la señal SIGCHLD
    struct sigaction sa;
    sa.sa_handler = manejador_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Error al configurar el manejador de SIGCHLD");
        exit(EXIT_FAILURE);
    }

    while(1) {
        fprintf(stdout, "Soy el proceso %d y voy a hacer el accept\n", getpid());
        if((sd = accept(s, 0, 0)) < 0) {
            perror("Error en el accept");
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "Soy el proceso %d y voy a hacer el fork()\n", getpid());
        
    }

}