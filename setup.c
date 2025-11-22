/*
 * setup.c
 *
 * Arquivo de implementação para as funções de setup.
 * CORRIGIDO: Garante que todo inimigo tenha pelo menos um ataque.
 */

#include "setup.h"
#include <stdlib.h> // rand(), NULL
#include <time.h>   // time()

/*
 * Função auxiliar para gerar um número aleatório dentro de um intervalo
 */
int random_int_in_range(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

/*
 * Implementação da função que cria uma carta aleatória.
 */
Card create_random_card(TipoCarta tipo, int custo) {
    Card nova_carta;
    nova_carta.tipo = tipo;
    nova_carta.custo_energia = custo;

    if (tipo == ESPECIAL) {
        nova_carta.custo_energia = 0;
        nova_carta.efeito_valor = 0;
        return nova_carta;
    }

    switch (custo) {
        case 0: nova_carta.efeito_valor = random_int_in_range(1, 5); break;
        case 1: nova_carta.efeito_valor = random_int_in_range(5, 10); break;
        case 2: nova_carta.efeito_valor = random_int_in_range(10, 15); break;
        case 3: nova_carta.efeito_valor = random_int_in_range(15, 30); break;
    }
    return nova_carta;
}

/*
 * Implementação da função que embaralha uma pilha.
 */
void shuffle_pilha(PilhaCartas *pilha) {
    for (int i = pilha->num_cartas - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = pilha->cartas[i];
        pilha->cartas[i] = pilha->cartas[j];
        pilha->cartas[j] = temp;
    }
}

/*
 * Monta o baralho inicial.
 */
PilhaCartas create_initial_deck() {
    PilhaCartas baralho;
    baralho.num_cartas = 0;
    int i = 0;

    // --- 1. Ataques ---
    baralho.cartas[i++] = create_random_card(ATAQUE, 0);
    baralho.cartas[i++] = create_random_card(ATAQUE, 1);
    baralho.cartas[i++] = create_random_card(ATAQUE, 1);
    baralho.cartas[i++] = create_random_card(ATAQUE, 1);
    baralho.cartas[i++] = create_random_card(ATAQUE, 2);
    baralho.cartas[i++] = create_random_card(ATAQUE, 3);
    for (int j = 0; j < 4; j++) {
        baralho.cartas[i++] = create_random_card(ATAQUE, random_int_in_range(0, 3));
    }

    // --- 2. Defesas ---
    baralho.cartas[i++] = create_random_card(DEFESA, 0);
    baralho.cartas[i++] = create_random_card(DEFESA, 1);
    baralho.cartas[i++] = create_random_card(DEFESA, 1);
    baralho.cartas[i++] = create_random_card(DEFESA, 1);
    baralho.cartas[i++] = create_random_card(DEFESA, 2);
    baralho.cartas[i++] = create_random_card(DEFESA, 3);
    for (int j = 0; j < 2; j++) {
        baralho.cartas[i++] = create_random_card(DEFESA, random_int_in_range(0, 2));
    }

    // --- 3. Especiais ---
    baralho.cartas[i++] = create_random_card(ESPECIAL, 0);
    baralho.cartas[i++] = create_random_card(ESPECIAL, 0);

    baralho.num_cartas = 20;
    shuffle_pilha(&baralho);
    return baralho;
}

/*
 * Inicializa jogador.
 */
Player setup_player() {
    Player jogador;
    jogador.stats.hp_atual = 100;
    jogador.stats.hp_max = 100;
    jogador.stats.escudo = 0;
    jogador.energia_atual = 3;
    jogador.energia_max = 3;
    jogador.baralho_completo = create_initial_deck();
    jogador.pilha_compra.num_cartas = 0;
    jogador.mao.num_cartas = 0;
    jogador.pilha_descarte.num_cartas = 0;
    return jogador;
}

/*
 * Cria inimigo aleatório.
 */
Enemy create_enemy() {
    Enemy inimigo;
    float chance = (float)rand() / RAND_MAX;
    int tem_ataque = 0; // Flag para verificar se gerou ataque

    if (chance <= 0.05) {
        // --- FORTE ---
        inimigo.tipo = FORTE;
        inimigo.stats.hp_max = random_int_in_range(40, 100);
        inimigo.num_acoes_ia = random_int_in_range(2, 3);
        
        int custo1_usado = 0;
        for (int i = 0; i < inimigo.num_acoes_ia; i++) {
            int custo_acao;
            if (custo1_usado == 0) {
                custo_acao = random_int_in_range(1, 3);
                if (custo_acao == 1) custo1_usado = 1;
            } else {
                custo_acao = random_int_in_range(2, 3);
            }
            
            TipoCarta tipo_acao = (rand() % 2 == 0) ? ATAQUE : DEFESA;
            Card acao_card = create_random_card(tipo_acao, custo_acao);
            
            inimigo.ia_ciclo[i].tipo_acao = acao_card.tipo;
            inimigo.ia_ciclo[i].valor_efeito = acao_card.efeito_valor;
            
            if (tipo_acao == ATAQUE) tem_ataque = 1;
        }

    } else {
        // --- FRACO ---
        inimigo.tipo = FRACO;
        inimigo.stats.hp_max = random_int_in_range(10, 30);
        inimigo.num_acoes_ia = random_int_in_range(1, 2);

        for (int i = 0; i < inimigo.num_acoes_ia; i++) {
            int custo_acao = random_int_in_range(0, 1);
            TipoCarta tipo_acao = (rand() % 2 == 0) ? ATAQUE : DEFESA;
            
            Card acao_card = create_random_card(tipo_acao, custo_acao);
            
            inimigo.ia_ciclo[i].tipo_acao = acao_card.tipo;
            inimigo.ia_ciclo[i].valor_efeito = acao_card.efeito_valor;
            
            if (tipo_acao == ATAQUE) tem_ataque = 1;
        }
    }

    // --- CORREÇÃO: GARANTE PELO MENOS 1 ATAQUE ---
    if (!tem_ataque) {
        // Escolhe um passo aleatório do ciclo para virar ataque
        int indice_para_trocar = rand() % inimigo.num_acoes_ia;
        inimigo.ia_ciclo[indice_para_trocar].tipo_acao = ATAQUE;
        // O valor do efeito não muda, pois ataque e defesa têm o mesmo range de valores
    }

    inimigo.stats.hp_atual = inimigo.stats.hp_max;
    inimigo.stats.escudo = 0;
    inimigo.acao_ia_atual = 0;

    return inimigo;
}