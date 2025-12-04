#ifndef SETUP_H
#define SETUP_H
#include "game_structs.h"

// Declaração das funções para que outros arquivos saibam que elas existem
Card create_random_card(TipoCarta tipo, int custo);
PilhaCartas create_initial_deck();
Player setup_player();
Enemy create_enemy();
Enemy create_boss(); // Função especial pro Boss
void shuffle_pilha(PilhaCartas *pilha);

#endif