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

int main(int argc, char *argv[])
{
    int fds[2];
    pipe(fds);  /* Crea un pipe */

    if (argc != 2) {
        fprintf(stderr, "Uso %s <nombre_del_fichero>\n", argv[0]);
    }

    /*Este proceso deberá enviar el fichero al otro proceso a través del pipe */
    if (fork() == 0)
    {
        close(fds[0]); //Cierra el extremo de lectura del pipe
        
        int fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
            perror("Error al abrir el fichero");
            exit(EXIT_FAILURE);
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytesRead;
        while((bytesRead = read(fd, buffer, BUFFER_SIZE)) > 0) {
            write(fds[1], buffer, bytesRead);
        }

        close(fd);
        close(fds[1]); // Cierra el extremo de escritura del pipe
        exit(EXIT_SUCCESS);
    }
    else if (fork() == 0)
    {
        close(fds[1]); // Cierra el extremo de escritura del pipe
        
        char buffer[BUFFER_SIZE];
        ssize_t bytesRead;
        while ((bytesRead = read(fds[0], buffer, BUFFER_SIZE)) > 0) {
            write(STDOUT_FILENO, buffer, bytesRead);
        }

        close(fds[0]);  // Cierra el extremo de lectura del pipe
        exit(EXIT_SUCCESS);
    }
    /* El padre cierra ambos descriptores del pipe y espera la finalización de sus hijos */
    else
    {
        close(fds[0]);  // Cierra ambos extremos del pipe en el proceso padres
        close(fds[1]);
        wait(0);  // Espera la finalización de ambos hijos
        wait(0);
    }

    return 0;
}