#ifndef _UTILS_H_
#define _UTILS_H_
#include <stdlib.h>

// Função para verificar se o Allegro iniciou bem. Se falhar, fecha o programa.
void must_init(_Bool test, const char* description);

// Função para limpar o array que guarda quais teclas estão apertadas
void ClearKeyboardKeys(unsigned char* keyboard_keys);

#endif