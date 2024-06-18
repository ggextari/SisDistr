#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char * argv[]) {
    int s;
    char bufferE[4] = {0};
    char bufferR[4];
    struct sockaddr_in direccion;
    int puerto = 37;
    char *ip = "132.163.97.2";
    int recibidos;

    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    inet_aton(ip, &direccion.sin_addr);

    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(-1);
    }

    // strcpy(bufferE, "\0");
    if (sendto(s, bufferE, sizeof(bufferE), 0, (struct sockaddr *) &direccion, sizeof(direccion)) < 0) {
        perror("Error en el sendto");
        exit(-1);
    }
    if ((recibidos = recvfrom(s, bufferR, sizeof(bufferR), 0, NULL, NULL)) < 0) {
        perror("Error en recvfrom");
        exit(-1);
    }

    // Asegurarse de que recibimos exactamente 4 bytes
    if (recibidos != 4) {
        fprintf(stderr, "Error: se esperaban 4 bytes, pero se recibieron %d bytes\n", recibidos);
        exit(-1);
    }

    // Imprimir los bytes recibidos

    // Interpretar los bytes recibidos como un entero de 32 bits en big-endian
    unsigned long int numero = (unsigned char) bufferR[0] << 24 | 
                               (unsigned char) bufferR[1] << 16 |
                               (unsigned char) bufferR[2] << 8 | 
                               (unsigned char) bufferR[3];

    // Convertir el número a hexadecimal
    printf("Número en hexadecimal: %lx\n", numero);

    double segundos_por_anyo = 365.25 * 24 * 60 * 60;
    double anyos = numero / segundos_por_anyo;
    printf("Años recibidos: %f\n", anyos);
    return 0;
}