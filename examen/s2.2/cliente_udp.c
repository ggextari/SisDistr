#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {
    int s;
    char buffer[1024];
    struct sockaddr_in direccion;
    int puerto = atoi(argv[2]);

    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    direccion.sin_addr.s_addr = htonl(argv[1]);

    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(-1);
    }
    if (connect(s, (struct sockaddr *) &direccion, sizeof(direccion)) < 0) {
        perror("Error en el connect");
        exit(-1);
    }

    do {
        print("->");
        gets(buffer);
        if (buffer == "FIN")
            break;
        else {
            if (sendto(s, buffer, strlen(buffer), 0, ))
        }

    } while (buffer != "FIN");

}