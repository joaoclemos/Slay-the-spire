#include "logic.h"
#include "setup.h"
#include "constants.h" // Importante para o CUSTO_X
#include <stdio.h>

// Move uma carta de um vetor para outro
void move_card(PilhaCartas* origem, PilhaCartas* destino, int index) {
    if (origem->num_cartas <= 0) return;
    // Copia
    destino->cartas[destino->num_cartas] = origem->cartas[index];
    destino->num_cartas++;
    // Remove e reorganiza o vetor original
    for (int i = index; i < origem->num_cartas - 1; i++) {
        origem->cartas[i] = origem->cartas[i + 1];
    }
    origem->num_cartas--;
}

// Compra cartas, reciclando o descarte se acabar
void draw_cards(Player* player, int n) {
    for (int i = 0; i < n; i++) {
        if (player->pilha_compra.num_cartas == 0) {
            // Se não tem carta nem no descarte, para.
            if (player->pilha_descarte.num_cartas == 0) break;
            
            // Move descarte para compra e embaralha
            while (player->pilha_descarte.num_cartas > 0) {
                move_card(&player->pilha_descarte, &player->pilha_compra, 0);
            }
            shuffle_pilha(&player->pilha_compra); 
        }
        // Pega do topo
        int topo = player->pilha_compra.num_cartas - 1;
        move_card(&player->pilha_compra, &player->mao, topo);
    }
}

// Aplica efeitos que acontecem no inicio do turno (veneno, regen)
void apply_turn_start_effects(Creature* c) {
    // Dano de veneno
    if (c->veneno > 0) {
        c->hp_atual -= c->veneno;
        if (c->hp_atual < 0) c->hp_atual = 0;
        c->veneno--; 
    }
    // Cura de regeneração
    if (c->regeneracao > 0) {
        c->hp_atual += 5; 
        if (c->hp_atual > c->hp_max) c->hp_atual = c->hp_max;
        c->regeneracao--;
    }
    // Reduz duração de status
    if (c->vulneravel > 0) c->vulneravel--;
    if (c->fraco > 0) c->fraco--;
    if (c->dormindo > 0) c->dormindo--; 
    
    // Zera escudo (regra do jogo)
    c->escudo = 0;
}

// Calcula dano final considerando buffs e debuffs
int calculate_damage(int base, Creature* atacante, Creature* alvo) {
    float dano = (float)base;
    
    dano += atacante->forca; // Adiciona força
    
    if (atacante->fraco > 0) dano *= 0.75f; // Reduz se estiver fraco
    if (alvo->vulneravel > 0) dano *= 1.50f; // Aumenta se alvo vulnerável
    
    if (dano < 0) dano = 0;
    return (int)dano;
}

// Inicia o turno do jogador
void start_player_turn(Player* player) {
    apply_turn_start_effects(&player->stats);
    player->energia_atual = player->energia_max;
    
    // Descarta mão anterior
    while (player->mao.num_cartas > 0) {
        move_card(&player->mao, &player->pilha_descarte, 0);
    }
    // Compra novas
    draw_cards(player, 5);
}

// Tenta jogar uma carta
int play_card(Player* player, int card_index, Enemy* target) {
    Card carta = player->mao.cartas[card_index];
    
    int custo_real = carta.custo_energia;
    int valor_X = 0;

    // Se for custo variável, gasta tudo que tem
    if (carta.custo_energia == CUSTO_X) {
        valor_X = player->energia_atual;
        custo_real = valor_X;
        if (valor_X <= 0) return 0; // Sem energia, não joga
    } else {
        if (player->energia_atual < custo_real) return 0;
    }

    // Paga o custo
    player->energia_atual -= custo_real;

    // Aplica efeito da carta
    switch (carta.tipo) {
        case ATAQUE:
            if (target != NULL) {
                int dano = calculate_damage(carta.efeito_valor, &player->stats, &target->stats);
                int dano_vampirico = dano; // Guarda valor original para cura

                // Dano no escudo
                if (target->stats.escudo > 0) {
                    if (target->stats.escudo >= dano) {
                        target->stats.escudo -= dano;
                        dano = 0;
                    } else {
                        dano -= target->stats.escudo;
                        target->stats.escudo = 0;
                    }
                }
                // Dano na vida
                target->stats.hp_atual -= dano;
                if (target->stats.hp_atual < 0) target->stats.hp_atual = 0;

                // Se for vampírica, cura o jogador
                if (carta.is_vampiric) {
                    player->stats.hp_atual += dano_vampirico; 
                    if (player->stats.hp_atual > player->stats.hp_max) 
                        player->stats.hp_atual = player->stats.hp_max;
                }
            }
            break;
            
        case DEFESA:
            player->stats.escudo += (carta.efeito_valor + player->stats.destreza);
            break;
            
        case BUFF:
            if (carta.efeito_valor == ID_FORCA) player->stats.forca += carta.magnitude;
            if (carta.efeito_valor == ID_DESTREZA) player->stats.destreza += carta.magnitude;
            
            if (carta.efeito_valor == ID_CURA_INSTANT) {
                int cura = carta.magnitude;
                // Cura X escala com energia gasta
                if (carta.custo_energia == CUSTO_X) cura = valor_X * 5; 
                player->stats.hp_atual += cura;
                if (player->stats.hp_atual > player->stats.hp_max) player->stats.hp_atual = player->stats.hp_max;
            }
            
            if (carta.efeito_valor == ID_REGEN_RODADAS) {
                int turnos = carta.magnitude;
                // Duração escala com energia gasta
                if (carta.custo_energia == CUSTO_X) turnos = valor_X;
                player->stats.regeneracao += turnos;
            }
            break;

        case DEBUFF:
            if (target != NULL) {
                if (carta.efeito_valor == ID_VENENO) target->stats.veneno += carta.magnitude;
                if (carta.efeito_valor == ID_VULNERAVEL) target->stats.vulneravel += carta.magnitude;
                if (carta.efeito_valor == ID_FRAQUEZA) target->stats.fraco += carta.magnitude;
                if (carta.efeito_valor == ID_SONO) target->stats.dormindo += carta.magnitude;
            }
            break;

        case ESPECIAL: 
             // Descarta tudo menos a carta atual (que será descartada no fim da função)
             while (player->mao.num_cartas > 1) {
                 int idx_to_remove = (0 == card_index) ? 1 : 0;
                 move_card(&player->mao, &player->pilha_descarte, idx_to_remove);
                 if (idx_to_remove < card_index) card_index--;
            }
            draw_cards(player, 5);
            break;
    }
    
    // Carta jogada vai pro descarte
    move_card(&player->mao, &player->pilha_descarte, card_index);
    return 1; 
}

// Turno da IA
void execute_enemy_turn(Player* player, Enemy inimigos[]) {
    // Aplica efeitos
    for (int i=0; i<2; i++) {
        if (inimigos[i].stats.hp_atual > 0) apply_turn_start_effects(&inimigos[i].stats);
    }

    for (int i = 0; i < 2; i++) {
        if (inimigos[i].stats.hp_atual > 0) {
            
            // Se dormindo, pula a vez
            if (inimigos[i].stats.dormindo > 0) {
                continue; 
            }

            AI_Action acao = inimigos[i].ia_ciclo[inimigos[i].acao_ia_atual];

            if (acao.tipo_acao == ATAQUE) {
                int dano = calculate_damage(acao.valor_efeito, &inimigos[i].stats, &player->stats);
                
                if (player->stats.escudo > 0) {
                    if (player->stats.escudo >= dano) {
                        player->stats.escudo -= dano;
                        dano = 0;
                    } else {
                        dano -= player->stats.escudo;
                        player->stats.escudo = 0;
                    }
                }
                player->stats.hp_atual -= dano;
                if (player->stats.hp_atual < 0) player->stats.hp_atual = 0;

            } else {
                // Defesa
                inimigos[i].stats.escudo += (acao.valor_efeito + inimigos[i].stats.destreza);
            }

            // Avança o ciclo
            inimigos[i].acao_ia_atual++;
            if (inimigos[i].acao_ia_atual >= inimigos[i].num_acoes_ia) {
                inimigos[i].acao_ia_atual = 0;
            }
        }
    }
}