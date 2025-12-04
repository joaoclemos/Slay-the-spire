#include "utils.h"
#include <allegro5/allegro5.h>
#include <stdio.h>

// Implementação da verificação de erro
void must_init(_Bool test, const char* description) {
    if (test) return; // Se o teste deu verdadeiro (sucesso), volta normal
    
    // Se deu falso (erro), imprime uma mensagem no terminal
    fprintf(stderr, "couldn't initialize %s\n", description);
    exit(1); // Fecha o programa com código de erro 1
}

// Implementação da limpeza de teclas
void ClearKeyboardKeys(unsigned char* keyboard_keys) {
    // Passa por todas as teclas possíveis e coloca zero (não pressionada)
    for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
        keyboard_keys[i] = 0;
    }
}