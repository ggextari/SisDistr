#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int s;
    char bufferE[1024];
    char bufferR[1024];
    struct sockaddr_in direccion;
    int puerto = atoi(argv[2]);

    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    inet_aton(argv[1], &direccion.sin_addr);

    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(-1);
    }

    do {
        printf("->");
        fgets(bufferE, 1023, stdin);
        if (strcmp(bufferE, "FIN\0") == 0)
            break;
        else {
            if (sendto(s, bufferE, sizeof(bufferE), 0, (struct sockaddr *) &direccion, sizeof(direccion)) < 0) {
                perror("Error en el sendto");
                exit(-1);
            }
            if (recvfrom(s, bufferR, sizeof(bufferR), 0, 0, 0) < 0) {
                perror("Error en recvfrom");
                exit(-1);
            }
            fprintf(stdout, "Recibido: %s", bufferR);
        }

    } while (strcmp(bufferE, "FIN\n") != 0);

}