// Fichero: echo-tcp-multi.c
#include <stdio.h>      // printf()
#include <stdlib.h>     // exit()
#include <sys/socket.h> // socket(), bind(), listen(), recv(), send(), etc
#include <arpa/inet.h>  // sockaddr_in
#include <errno.h>      // perror()
#include <sys/select.h> // select() y fd_set
#include <unistd.h>     // close()

#define TAM_ARRAY 5

int CrearSocketTCP(int puerto)
{
    int s;
    struct sockaddr_in dir;
    int r;

    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        perror("En socket TCP");
        exit(1);
    }
    dir.sin_family  = AF_INET;
    dir.sin_port    = htons(puerto);
    dir.sin_addr.s_addr = htonl(INADDR_ANY);
    r = bind(s, (struct sockaddr *) &dir, sizeof(dir));
    if (r == -1) {
        perror("En el bind TCP");
        exit(1);
    }
    r = listen(s, SOMAXCONN);
    if (r == -1) {
        perror("En listen");
        exit(1);
    }
    return s;
}

int dar_servicio_TCP(int *sd, int n, int i)
{
    // Lee los datos del socket s y si lee distinto de cero, envía eco
    // Retorna el número de datos leídos

    char buffer[100];
    int leidos;

    leidos = recv(sd[i], buffer, 100, 0);
    if (leidos > 0)
        for (int j = 0; j < n; j++)
            send(sd[j], buffer, leidos, 0);
    return leidos;
}

int max(int a, int b)
{
    // Devuelve el mayor entre a y b
    if (a > b) return a;
    else return b;
}

int buscar_maximo(int s, int *sd)
{
    // Devuelve el mayor de los sockets, bien el de escucha o bien alguno de datos
    int resultado = s;
    int sock;
    for (int i = 0; i < TAM_ARRAY; i++) {
        sock = sd[i];
        resultado = max(resultado, sock);
    }
    return resultado;
}

int quitar_ceros(int *s_conjunto, int sd[])
{
    int c = 0;
    for (int i = 0; i < TAM_ARRAY; i++)
        if (sd[i] != 0) {
            s_conjunto[c] = sd[i];
            c++;
        }
    return c;
}

void anadir_sd(int *sd, int socket)
{
    for (int i = 0; i < TAM_ARRAY; i++)
        if (sd[i] == 0) {
            sd[i] = socket;
            return;
        }
    perror("No hay huecos en el array de sockets");
    exit(-1);
}

void eliminar_socket(int *sd, int socket)
{
    for (int i = 0; i < TAM_ARRAY; i++)
        if (sd[i] == socket) {
            sd[i] = 0;
            return;
        }
}

int main(int argc, char * argv[]) {
    int puerto;
    int s;              // socket de escucha
    int sd[5];          // array de sockets de datos
    fd_set  conjunto;   // Para select
    int     maximo;     // Para select

    if (argc < 2) {
        printf("Uso: %s puerto\n", argv[0]);
        exit(0);
    }

    puerto = atoi(argv[1]);
    s = CrearSocketTCP(puerto);
    for (int i = 0; i < TAM_ARRAY; i++)
        sd[i] = 0; // Iniciamos a 0 porque no hay clientes de momento
    
    while (1) {  // Bucle infinito del servidor

        // Vaciar conjunto de descriptores a vigilar
        FD_ZERO(&conjunto);

        // Meter solo los que haya que vigilar
        int *sockets_conjunto = (int *) malloc(sizeof(int) * TAM_ARRAY);
        int n_sockets = quitar_ceros(sockets_conjunto, sd);
        for (int i = 0; i < n_sockets; i++)
            FD_SET(sockets_conjunto[i], &conjunto);
        if (n_sockets < TAM_ARRAY)
            FD_SET(s, &conjunto);

        maximo = buscar_maximo(s, sockets_conjunto);

        // Esperar a que ocurra "algo"
        select(maximo + 1, &conjunto, NULL, NULL, NULL);
        printf("Ha ocurrido algo\n");

        // Averiguar qué ocurrió
        if (FD_ISSET(s, &conjunto))
        {
            printf("Ha ocurrido algo en el socket TCP\n");
            anadir_sd(sd, accept(s, NULL, NULL));
        }
        for (int i = 0; i < n_sockets; i++)
            if (FD_ISSET(sockets_conjunto[i], &conjunto)) {
                int n;
                printf("Han llegado datos por un socket de datos\n");
                n = dar_servicio_TCP(sockets_conjunto, n_sockets, i);
                if (n == 0) {
                    printf("El socket se ha desconectado\n");
                    eliminar_socket(sd, sockets_conjunto[i]);
                }
            }
    } // del while(1)
    return 0;
} // del main()