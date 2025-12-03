#ifndef SETUP_H
#define SETUP_H
#include "game_structs.h"

// Cria uma carta com valores aleatórios
Card create_random_card(TipoCarta tipo, int custo);

// Cria o baralho inicial do jogador
PilhaCartas create_initial_deck();

// Prepara o jogador (vida cheia, energia cheia)
Player setup_player();

// Cria um inimigo (pode ser fraco ou forte)
Enemy create_enemy();

// Mistura as cartas de uma pilha
void shuffle_pilha(PilhaCartas *pilha);

#endif