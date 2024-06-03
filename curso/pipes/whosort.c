/* File whosort.c */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

main()
{
    int fds[2];
	pid_t pid;
    pipe(fds);                  /*Crea un pipe */
    /* El primer hijo reconecta su entrada estándar (stdin) al flujo
       de salida del pipe y cierra el descriptor de la entrada del pipe
    */
    if ((pid=fork())<0) {
		perror("Fallo del fork al crear el primer hijo");
		exit(1);
	}
	else if(pid ==0) {
		dup2(fds[0],0);
		close(fds[1]);
		execlp("sort","sort",NULL);
	}
    /* El segundo hijo reconecta su salida estándar (stdout) a la
       entrada del pipe y cierra el descriptor de la salida del pipe
    */
	else /*retorno del fork del padre*/
	{
		if ((pid=fork())<0)
		{
			perror("Fallo en  el segundo fork()");
			exit(2);
		}
    		else if ( fork() == 0) {
        		dup2(fds[1], 1);
        		close(fds[0]);
        		execlp("who", "who", NULL);
    		}
		else //retorno del segundo fork ene l padre
    /* El padre cerrará ambos descriptores del pipe y esperará
       la finalización de sus hijos
    */
    		{
    			close(fds[0]);
    			close(fds[1]);
    			wait(0);
    			wait(0);
    		}
	}
}
