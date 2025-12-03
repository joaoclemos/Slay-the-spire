#include "utils.h"
#include <allegro5/allegro5.h>
#include <stdio.h>

// Função que checa erros. Se algo der errado (teste for falso), fecha o jogo e avisa.
void must_init(_Bool test, const char* description) {
    if (test) return; // Se deu certo, continua
    
    // Se deu errado, imprime o erro e sai
    fprintf(stderr, "couldn't initialize %s\n", description);
    exit(1);
}

// Zera todas as teclas (diz que nenhuma está apertada)
void ClearKeyboardKeys(unsigned char* keyboard_keys) {
    for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
        keyboard_keys[i] = 0;
    }
}