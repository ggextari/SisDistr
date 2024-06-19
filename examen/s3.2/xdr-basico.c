#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n;
    char c;
    char *buf;

    fprintf(stdin, "Introduce un número y un entero: ");
    fgets(buf, 1, stdin);
    n = atoi(buf);
    fgets(buf, 1, stdin);
    c = buf[0];

    
}