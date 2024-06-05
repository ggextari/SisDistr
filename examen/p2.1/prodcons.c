#include <stdlib.h>
#include <stdio.h>

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