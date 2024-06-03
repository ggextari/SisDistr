#include <stdio.h>

void mostrar_array(int *p, int num_elem)
{
   int i;
   for (i=0;i<num_elem;i++)
       printf("[%d]: %d\n", i, *(p+i));  // O podríamos poner p[i]
}

int main()
{
    int a[] = {1,2,3,4,5};

    mostrar_array(a, 5);
    return 0;
}
