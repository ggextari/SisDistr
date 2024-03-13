#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char* argv[]) {
    int sock_pasivo, sock_datos;
    struct sockaddr_in d_local;
    char mensaje[100];
    int leidos;
    int puerto;
    pid_t pid;

    puerto=atoi(argv[1]);

    sock_pasivo = socket(PF_INET, SOCK_STREAM, 0);

    d_local.sin_family = AF_INET;
    d_local.sin_addr.s_addr = htonl(INADDR_ANY);
    d_local.sin_port = htons(puerto);

    bind(sock_pasivo, (struct sockaddr *)&d_local, sizeof(d_local));
    listen(sock_pasivo, SOMAXCONN);

    //generamos el pool de procesos hijo
    for (int i=0;i<3;i++){
        if ((pid=fork())<0)//fallo del fork
        {
            perror("Fallo del fork(). No se pudo generar subproceso hijo");
            continue;
        }
        else if(pid==0)
        {
            break;
        }
    }


    while (1) { // Bucle infinito de atención a clientes
        printf("Soy el proceso (%d) antes del accept()\n",getpid());
        sock_datos = accept(sock_pasivo, 0, 0);
        printf("Soy el proceso (%d) después del accept()\n",getpid());
        printf("Soy el proceso (%d) antes del bucle de lectura del socket\n",getpid());
        while ((leidos=read(sock_datos,mensaje,sizeof(mensaje)))>0)
        {
            for (int i=0;i<leidos;i++) mensaje[i]=(char) toupper(mensaje[i]);
            write(sock_datos,mensaje,leidos);
        }
        close(sock_datos);
    }
}