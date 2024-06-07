#include <stdio.h>

int main()
{
    int *p_i = 0;  // 0 representa al "puntero nulo"
    printf("El puntero apunta a %p\n", p_i);
    printf("Intento de desreferenciación\n");
    *p_i = 100;
}