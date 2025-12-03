#include "logic.h"
#include "setup.h"
#include <stdio.h>

// Função auxiliar: Move UMA carta de uma pilha para outra
void move_card(PilhaCartas* origem, PilhaCartas* destino, int index) {
    if (origem->num_cartas <= 0) return; // Se vazio, não faz nada

    // Copia a carta para o final da pilha de destino
    destino->cartas[destino->num_cartas] = origem->cartas[index];
    destino->num_cartas++;

    // Remove a carta da origem e puxa as outras para tapar o buraco
    for (int i = index; i < origem->num_cartas - 1; i++) {
        origem->cartas[i] = origem->cartas[i + 1];
    }
    origem->num_cartas--;
}

// Compra 'n' cartas
void draw_cards(Player* player, int n) {
    for (int i = 0; i < n; i++) {
        // Se a pilha de compra acabar...
        if (player->pilha_compra.num_cartas == 0) {
            // ...e o descarte também estiver vazio, não dá pra comprar.
            if (player->pilha_descarte.num_cartas == 0) break;
            
            // Senão, recicla o descarte: move tudo de volta para a compra
            while (player->pilha_descarte.num_cartas > 0) {
                move_card(&player->pilha_descarte, &player->pilha_compra, 0);
            }
            shuffle_pilha(&player->pilha_compra); // Embaralha de novo
        }
        // Pega a carta do topo da pilha e move para a mão
        int topo = player->pilha_compra.num_cartas - 1;
        move_card(&player->pilha_compra, &player->mao, topo);
    }
}

// Prepara o turno do jogador
void start_player_turn(Player* player) {
    player->energia_atual = player->energia_max; // Enche energia
    player->stats.escudo = 0; // Escudo some no inicio do turno
    
    // Descarta todas as cartas que sobraram na mão
    while (player->mao.num_cartas > 0) {
        move_card(&player->mao, &player->pilha_descarte, 0);
    }
    // Compra 5 novas cartas
    draw_cards(player, 5);
}

// O Jogador usa uma carta
int play_card(Player* player, int card_index, Enemy* target) {
    Card carta = player->mao.cartas[card_index];

    // Verifica se tem energia suficiente
    if (player->energia_atual < carta.custo_energia) {
        printf("Sem energia!\n");
        return 0; // Falhou, não faz nada
    }
    // Gasta a energia
    player->energia_atual -= carta.custo_energia;

    // Aplica o efeito da carta
    switch (carta.tipo) {
        case ATAQUE:
            if (target != NULL) {
                int dano = carta.efeito_valor;
                // Se o inimigo tem escudo, o dano bate no escudo primeiro
                if (target->stats.escudo > 0) {
                    if (target->stats.escudo >= dano) {
                        target->stats.escudo -= dano; // Escudo absorveu tudo
                        dano = 0;
                    } else {
                        dano -= target->stats.escudo; // Escudo quebrou, sobra dano
                        target->stats.escudo = 0;
                    }
                }
                // O resto do dano vai na vida
                target->stats.hp_atual -= dano;
                if (target->stats.hp_atual < 0) target->stats.hp_atual = 0;
            }
            break;
            
        case DEFESA:
            // Ganha escudo
            player->stats.escudo += carta.efeito_valor;
            break;
            
        case ESPECIAL: 
             // Carta Especial: Troca a mão toda
             // (Remove todas as cartas menos a que está sendo usada agora)
             while (player->mao.num_cartas > 1) {
                 int idx_to_remove = (0 == card_index) ? 1 : 0;
                 move_card(&player->mao, &player->pilha_descarte, idx_to_remove);
                 if (idx_to_remove < card_index) card_index--;
            }
            draw_cards(player, 5); // Compra 5 novas
            break;
    }
    
    // Joga a carta usada no descarte
    move_card(&player->mao, &player->pilha_descarte, card_index);
    return 1; // Sucesso
}

// Turno dos Inimigos (IA)
void execute_enemy_turn(Player* player, Enemy inimigos[]) {
    printf("\n--- INIMIGOS AGINDO ---\n");
    
    // Passa por cada inimigo (0 e 1)
    for (int i = 0; i < 2; i++) {
        if (inimigos[i].stats.hp_atual > 0) {
            
            // Descobre o que ele vai fazer agora (do ciclo)
            AI_Action acao = inimigos[i].ia_ciclo[inimigos[i].acao_ia_atual];

            if (acao.tipo_acao == ATAQUE) {
                printf("Inimigo %d ATACA com %d\n", i, acao.valor_efeito);
                int dano = acao.valor_efeito;
                
                // Dano bate no escudo do jogador primeiro
                if (player->stats.escudo > 0) {
                    if (player->stats.escudo >= dano) {
                        player->stats.escudo -= dano;
                        dano = 0;
                    } else {
                        dano -= player->stats.escudo;
                        player->stats.escudo = 0;
                    }
                }
                // Resto tira vida
                player->stats.hp_atual -= dano;
                if (player->stats.hp_atual < 0) player->stats.hp_atual = 0;

            } else {
                // Inimigo defende
                printf("Inimigo %d DEFENDE com %d\n", i, acao.valor_efeito);
                inimigos[i].stats.escudo += acao.valor_efeito;
            }

            // Avança para o próximo passo do ciclo
            inimigos[i].acao_ia_atual++;
            // Se chegou no fim do ciclo, volta pro começo
            if (inimigos[i].acao_ia_atual >= inimigos[i].num_acoes_ia) {
                inimigos[i].acao_ia_atual = 0;
            }
        }
    }
}