#include <stdio.h>

int main(int argc, char *argv[]) {
    // argc es el número de argumentos
    // argv es un array de cadenas que contiene los argumentos

    // Iterar sobre cada argumento y imprimirlo
    for(int i = 0; i < argc; i++) {
        printf("Argumento %d: %s\n", i, argv[i]);
    }

    return 0;
}