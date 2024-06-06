/*
EPI GIJÓN
GRADO EN INGENIERÍA INFORMÁTICA
SISTEMAS DISTRIBUIDOS - CURSO 3º
MATERIAL DE LA ASIGNATURA
-------------------------
MECANISMO   : PIPES
FICHERO     : prodcons.c
DESCRIPCIÓN : Se propone la realización de un programa en el que dos
procesos se envíen a través de un pipe un fichero de texto donde
el que recibe debe mostrarlo por su salida standard.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

main(int argc, char *argv[])
{

    if (argc != 2) {
        printf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fds[2];
    pipe(fds);

    if (fork() == 0)
    {
        dup2(fds[0],0);
        close(fds[1]);
        while ( (count = read(fd, buffer, 100)))
    }
    else if (fork() == 0)
    {
        dup2(fds[1],1);
        close(fds[0]);
        FILE *f = fopen(argv[1], "r");
        execlp()
    }
    else
    {
        close(fds[0]);
        close(fds[1]);
        wait(0);
        wait(0);
    }
}