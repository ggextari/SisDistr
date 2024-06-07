#include <stdio.h>

int main(){
    int *p_i;  // Puntero sin inicializar

    printf("El puntero apunta a la dirección: %p\n", p_i);

    *p_i = 100;
}