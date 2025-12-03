#ifndef _UTILS_H_
#define _UTILS_H_
#include <stdlib.h>

// Verifica se o Allegro iniciou corretamente
void must_init(_Bool test, const char* description);

// Limpa o estado das teclas do teclado
void ClearKeyboardKeys(unsigned char* keyboard_keys);

#endif