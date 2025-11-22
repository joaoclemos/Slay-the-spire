#ifndef LOGIC_H
#define LOGIC_H

#include "game_structs.h"

// Move 'n' cartas da pilha de compras para a mão.
// Se acabar a pilha de compras, recicla o descarte.
void draw_cards(Player* player, int n);

// Prepara o jogador para um novo turno (reseta energia, compra mão nova)
void start_player_turn(Player* player);

// Joga uma carta da mão (aplica efeito e descarta)
// Retorna 1 se deu certo, 0 se falhou (ex: sem energia)
int play_card(Player* player, int card_index, Enemy* target);

// Executa as ações de todos os inimigos vivos e passa o turno
void execute_enemy_turn(Player* player, Enemy inimigos[]);

#endif