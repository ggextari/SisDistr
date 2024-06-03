

// Fichero: echo-tcp-udp-select.c
#include <stdio.h>       // printf()
#include <stdlib.h>      // exit()
#include <sys/socket.h>  // socket(), bind(), listen(), recv(), send(), etc
#include <arpa/inet.h>   // sockaddr_in
#include <errno.h>       // perror()
#include <sys/select.h>  // select() y fd_set
#include <unistd.h>      // close()

#define NOHAYPOSICIONLIBRE -1
#define MAXCLIENTES 5
#define CIERTO 1
#define FALSO 0

int clientes[MAXCLIENTES]

int CrearSocketTCP(int puerto)
{
    int s;
    struct sockaddr_in dir;
    int r;

    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s==-1) {
        perror("En socket TCP");
        exit(1);
    }
    dir.sin_family = AF_INET;
    dir.sin_port   = htons(puerto);
    dir.sin_addr.s_addr = htonl(INADDR_ANY);
    r = bind(s, (struct sockaddr *) &dir, sizeof(dir));
    if (r==-1) {
        perror("En bind TCP");
        exit(1);
    }
    r = listen(s, SOMAXCONN);
    if (r==-1) {
        perror("En listen");
        exit(1);
    }
    return s;
}

int dar_servicio_TCP(int s)
{
    // Lee datos del socket s y si lee distinto de cero, envia eco
    // Retorna el numero de datos leidos

   char buffer[100];
   int leidos;

   leidos = recv(s, buffer, 100, 0);
   if (leidos>0)
       send(s, buffer, leidos, 0);
   return leidos;
}

int buscar_maximo(int tcp, int *lista, int num)
{
    int resultado=tcp;
    int i;

    for (i=0;i<num;i++)
    {
        if (lista[i]>resultado) resultado=lista[i];
    }

    return resultado;
}

int BuscarPosicionLibre(int *lista, int num)
{
    int resultado = NOHAYPOSICIONLIBRE;
    int i;

    for (i=0;i<num;i++)
    {
        if (lista[i]==0)
        {
            resultado=i;
            break;
        }
    }
}

int main(int argc, char * argv[])
{
    int puerto;
    int s_tcp; // sockets "de escucha"
    int s_datos;      // Para la conexion TCP
    fd_set conjunto;  // Para select
    int    maximo;    // Para select
    int pos;

    if (argc<2) {
        printf("Uso: %s puerto\n", argv[0]);
        exit(0);
    }

    puerto = atoi(argv[1]);
    s_tcp = CrearSocketTCP(puerto);
    s_datos = 0;  // De momento no tenemos cliente

    InicializarArrayClientes(clientes,MAXCLIENTES);

    while (1) {  // Bucle infinito del servidor

        // Vaciar conjunto de descriptores a vigilar
        FD_ZERO(&conjunto);

        // Meter solo los que haya que vigilar
        if (HayPosicionesLibres(clientes,num))
            FD_SET(s_tcp,&conjunto);

        for (int i = 0; i<MAXCLIENTES;i++)
        {
            if (clientes[i]!=0) FD_SET(clientes[i],&conjunto);
        }

        maximo = buscar_maximo(s_tcp, clientes, MAXCLIENTES);

        // Esperar a que ocurra "algo"
        select(maximo + 1, &conjunto, NULL, NULL, NULL);
        printf("Ha ocurrido algo\n");

        if (HayPosicionesLibres(clientes,MAXCLIENTES) && FD_ISSET(s_tcp,&conjunto))
        {
            printf("Ha llegado un cliente al socket TCP\n");
            s_datos = accept(s_tcp, NULL, NULL);
            pos=BuscarPosicionLibre(clientes,MAXCLIENTES);
            if (pos!=NOHAYPOSICIONLIBRE)
                clientes[pos]=s_datos;
        }

        for (int i=0;i<MAXCLIENTES;i++)
        {

            if (FD_ISSET(s_datos,&conjunto))
            {
                int n;
                printf("Han llegado datos por la conexión TCP\n");
                n = dar_servicio_TCP(clientes[i]);
                if (n==0) {
                    printf("El cliente TCP ha desconectado\n");
                    close(clientes[i]);
                    clientes[i]=0;
                }
            }
        }
    } // del while(1)
    return 0;   // Nunca se ejecuta, pero para que el compilador no proteste
} // de main()

