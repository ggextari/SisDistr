#include <stdlib.h>

int main(){
    int *p_i;   // Se declara pero no se inicializa

    *p_i = 100;  // Sin embargo, se intenta desreferenciar ¿donde se guardará este 100?
}
