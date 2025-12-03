#ifndef LOGIC_H
#define LOGIC_H
#include "game_structs.h"

// Compra 'n' cartas da pilha
void draw_cards(Player* player, int n);

// Começa o turno do jogador (reseta energia, etc)
void start_player_turn(Player* player);

// O Jogador joga uma carta contra um alvo
// Retorna 1 se funcionou, 0 se falhou (sem energia)
int play_card(Player* player, int card_index, Enemy* target);

// Faz os inimigos agirem (atacarem ou defenderem)
void execute_enemy_turn(Player* player, Enemy inimigos[]);

#endif