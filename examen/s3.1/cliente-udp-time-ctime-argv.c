#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stdout, "Uso: %s <ip_servidor>", argv[0]);
        exit(0);
    }

    int s;
    char bufferE[4] = {0};
    char bufferR[4];
    struct sockaddr_in direccion;
    int puerto = 37;
    char *ip = argv[1];
    int recibidos;

    direccion.sin_family = AF_INET;
    direccion.sin_port = htons(puerto);
    inet_aton(ip, &direccion.sin_addr);

    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Error al crear el socket");
        exit(-1);
    }

    if (sendto(s, bufferE, sizeof(bufferE), 0, (struct sockaddr *)&direccion, sizeof(direccion)) == -1)
    {
        perror("Error en el sendto");
        exit(-1);
    }

    if ((recibidos = recvfrom(s, bufferR, sizeof(bufferR), 0, NULL, NULL)) == -1)
    {
        perror("Error en el recvfrom");
        exit(-1);
    }

    // Asegurarse de que recibimos exactamente 4 bytes
    if (recibidos != 4)
    {
        fprintf(stderr, "Error: se esperaban 4 bytes, pero se recibieron %d bytes", recibidos);
        exit(-1);
    }

    // Imprimir los bytes recibidos para verificar su contenido
    printf("Bytes recibidos: %02x %02x %02x %02x\n",
           (unsigned char)bufferR[0],
           (unsigned char)bufferR[1],
           (unsigned char)bufferR[2],
           (unsigned char)bufferR[3]);

    // Interpretar los bytes recibidos como un entero de 32 bits en big-endian
    uint32_t numero = ((uint32_t)(unsigned char)bufferR[0] << 24) |
                      ((uint32_t)(unsigned char)bufferR[1] << 16) |
                      ((uint32_t)(unsigned char)bufferR[2] << 8) |
                      (uint32_t)(unsigned char)bufferR[3];

    // Convertir el número a hexadecimal
    printf("Número en hexadecimal: %08x\n", numero);
    const long int num = (const long int) numero - 2208988800;

    char *fecha = ctime(&num);
    printf("Años recibidos: %s", fecha);

    close(s); // Cerrar el socket
    return 0;
}