/*
 * logic.c - VERSÃO DE CORREÇÃO E DEBUG
 */

#include "logic.h"
#include "setup.h"
#include <stdio.h>

void move_card(PilhaCartas* origem, PilhaCartas* destino, int index_origem) {
    if (origem->num_cartas <= 0) return;
    destino->cartas[destino->num_cartas] = origem->cartas[index_origem];
    destino->num_cartas++;
    for (int i = index_origem; i < origem->num_cartas - 1; i++) {
        origem->cartas[i] = origem->cartas[i + 1];
    }
    origem->num_cartas--;
}

void draw_cards(Player* player, int n) {
    for (int i = 0; i < n; i++) {
        if (player->pilha_compra.num_cartas == 0) {
            if (player->pilha_descarte.num_cartas == 0) break;
            while (player->pilha_descarte.num_cartas > 0) {
                move_card(&player->pilha_descarte, &player->pilha_compra, 0);
            }
            shuffle_pilha(&player->pilha_compra);
        }
        int topo = player->pilha_compra.num_cartas - 1;
        move_card(&player->pilha_compra, &player->mao, topo);
    }
}

void start_player_turn(Player* player) {
    player->energia_atual = player->energia_max;
    player->stats.escudo = 0;
    while (player->mao.num_cartas > 0) {
        move_card(&player->mao, &player->pilha_descarte, 0);
    }
    draw_cards(player, 5);
}

int play_card(Player* player, int card_index, Enemy* target) {
    if (card_index < 0 || card_index >= player->mao.num_cartas) return 0;
    Card carta = player->mao.cartas[card_index];

    if (player->energia_atual < carta.custo_energia) {
        printf("Sem energia!\n");
        return 0; 
    }
    player->energia_atual -= carta.custo_energia;

    switch (carta.tipo) {
        case ATAQUE:
            if (target != NULL) {
                int dano = carta.efeito_valor;
                if (target->stats.escudo > 0) {
                    if (target->stats.escudo >= dano) {
                        target->stats.escudo -= dano;
                        dano = 0;
                    } else {
                        dano -= target->stats.escudo;
                        target->stats.escudo = 0;
                    }
                }
                target->stats.hp_atual -= dano;
                if (target->stats.hp_atual < 0) target->stats.hp_atual = 0;
            }
            break;
        case DEFESA:
            player->stats.escudo += carta.efeito_valor;
            break;
        case ESPECIAL:
             while (player->mao.num_cartas > 0) {
                 if (0 == card_index) {
                     if (player->mao.num_cartas > 1) move_card(&player->mao, &player->pilha_descarte, 1);
                     else break; 
                 } else {
                     move_card(&player->mao, &player->pilha_descarte, 0);
                     card_index--; 
                 }
            }
            draw_cards(player, 5);
            break;
    }
    move_card(&player->mao, &player->pilha_descarte, card_index);
    return 1; 
}

/*
 * FUNÇÃO QUE ESTAVA DANDO PROBLEMA?
 * Adicionei prints para vermos o que está acontecendo.
 */
void execute_enemy_turn(Player* player, Enemy inimigos[]) {
    printf("\n--- INICIO TURNO INIMIGOS ---\n");
    
    for (int i = 0; i < 2; i++) {
        if (inimigos[i].stats.hp_atual > 0) {
            
            int indice_acao = inimigos[i].acao_ia_atual;
            AI_Action acao = inimigos[i].ia_ciclo[indice_acao];

            // --- DEBUG: Mostra qual ação o inimigo escolheu ---
            if (acao.tipo_acao == ATAQUE) {
                printf("Inimigo %d vai ATACAR com forca %d\n", i, acao.valor_efeito);
                
                // Lógica de Dano
                int dano = acao.valor_efeito;
                
                // Print Vida Antes
                printf("  > Vida do Jogador ANTES: %d (Escudo: %d)\n", player->stats.hp_atual, player->stats.escudo);

                // 1. Escudo
                if (player->stats.escudo > 0) {
                    if (player->stats.escudo >= dano) {
                        player->stats.escudo -= dano;
                        dano = 0;
                        printf("  > Escudo absorveu todo o dano.\n");
                    } else {
                        dano -= player->stats.escudo;
                        player->stats.escudo = 0;
                        printf("  > Escudo quebrou! Dano restante: %d\n", dano);
                    }
                }

                // 2. Vida
                if (dano > 0) {
                    player->stats.hp_atual -= dano;
                    if (player->stats.hp_atual < 0) player->stats.hp_atual = 0;
                    printf("  > Dano aplicado! Vida do Jogador AGORA: %d\n", player->stats.hp_atual);
                }

            } else {
                printf("Inimigo %d vai DEFENDER (ganha %d escudo)\n", i, acao.valor_efeito);
                inimigos[i].stats.escudo += acao.valor_efeito;
            }

            // Avança ciclo
            inimigos[i].acao_ia_atual++;
            if (inimigos[i].acao_ia_atual >= inimigos[i].num_acoes_ia) {
                inimigos[i].acao_ia_atual = 0;
            }
        } else {
            printf("Inimigo %d esta morto e nao age.\n", i);
        }
    }
    printf("--- FIM TURNO INIMIGOS ---\n\n");
}