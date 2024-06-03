#include <stdio.h>

void mostrar_array(int p[], int num_elem)
{
   int i;
   for (i=0;i<num_elem;i++)
       printf("[%d]: %d\n", i, p[i]);
}

void poner_ceros(int p[], int num_elem)
{
    int i;
    for (i=0;i<num_elem;i++)
        p[i] = 0;
}

int main()
{
    int a[] = {1,2,3,4,5};

    printf("ANTES:\n");
    mostrar_array(a, 5);
    poner_ceros(a, 5);
    printf("DESPUÉS:\n");
    mostrar_array(a, 5);
    return 0;
}