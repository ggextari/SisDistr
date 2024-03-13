#include <stdio.h>

void main(int argc,char *argv[])
{
	int i;
	printf("Numero de argumentos recibidos: %d\n", argc);
	for (i=0;i<argc;i++)
		printf("Argumento %d vale: %s\n", i, argv[i]);
}
