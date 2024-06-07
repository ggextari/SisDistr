#include<stdio.h>
#include<stdlib.h>
int main(int argc, char **argv)
{
  int i;

  if (argc<3) {
    printf("Ejecuta este programa con al menos tres parámetros\n");
    return 2;
  }
  printf("argv apunta a %p\n", argv);
  printf("En esa dirección hay un array de punteros que son:\n");
  for (i=0;i<argc;i++) {
    printf("argv[%d] apunta a %p\n", i, *(argv+i));
    printf("  donde hay una cadena, cuyo primer carácter es: %c\n", *(*(argv+i)+0));
  }
  return 0;
}