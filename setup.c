#include "setup.h"
#include <stdlib.h> // Para usar rand() (sorteio)
#include <time.h>   

// Função auxiliar: Sorteia um número entre min e max
int random_int_in_range(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

// Cria uma carta sorteada
Card create_random_card(TipoCarta tipo, int custo) {
    Card nova_carta;
    nova_carta.tipo = tipo;
    nova_carta.custo_energia = custo;

    // Se for carta Especial, custo é 0
    if (tipo == ESPECIAL) {
        nova_carta.custo_energia = 0;
        nova_carta.efeito_valor = 0;
        return nova_carta;
    }

    // Define a força da carta baseada no custo (Custo maior = Mais forte)
    switch (custo) {
        case 0: nova_carta.efeito_valor = random_int_in_range(1, 5); break;
        case 1: nova_carta.efeito_valor = random_int_in_range(5, 10); break;
        case 2: nova_carta.efeito_valor = random_int_in_range(10, 15); break;
        case 3: nova_carta.efeito_valor = random_int_in_range(15, 30); break;
    }
    return nova_carta;
}

// Mistura as cartas (Algoritmo padrão de embaralhamento)
void shuffle_pilha(PilhaCartas *pilha) {
    for (int i = pilha->num_cartas - 1; i > 0; i--) {
        int j = rand() % (i + 1); // Escolhe uma carta aleatória
        
        // Troca a carta atual com a aleatória
        Card temp = pilha->cartas[i];
        pilha->cartas[i] = pilha->cartas[j];
        pilha->cartas[j] = temp;
    }
}

// Cria o baralho de 20 cartas
PilhaCartas create_initial_deck() {
    PilhaCartas baralho;
    baralho.num_cartas = 0;
    int i = 0;

    // Adiciona 10 Ataques
    for(int k=0; k<10; k++) {
        baralho.cartas[i++] = create_random_card(ATAQUE, random_int_in_range(0, 2));
    }
    // Adiciona 8 Defesas
    for(int k=0; k<8; k++) {
        baralho.cartas[i++] = create_random_card(DEFESA, random_int_in_range(0, 2));
    }
    // Adiciona 2 Especiais
    baralho.cartas[i++] = create_random_card(ESPECIAL, 0);
    baralho.cartas[i++] = create_random_card(ESPECIAL, 0);

    baralho.num_cartas = 20;
    shuffle_pilha(&baralho); // Mistura tudo
    return baralho;
}

// Cria o Jogador pronto para começar
Player setup_player() {
    Player jogador;
    jogador.stats.hp_atual = 100;
    jogador.stats.hp_max = 100;
    jogador.stats.escudo = 0;
    jogador.energia_atual = 3;
    jogador.energia_max = 3;
    
    // Cria o baralho mas começa com as outras pilhas vazias
    jogador.baralho_completo = create_initial_deck();
    jogador.pilha_compra.num_cartas = 0;
    jogador.mao.num_cartas = 0;
    jogador.pilha_descarte.num_cartas = 0;
    
    return jogador;
}

// Cria um Inimigo
Enemy create_enemy() {
    Enemy inimigo;
    // Sorteia se vai ser Forte (5% de chance) ou Fraco (95%)
    float chance = (float)rand() / RAND_MAX;
    int tem_ataque = 0;

    if (chance <= 0.05) {
        // INIMIGO FORTE (BOSS)
        inimigo.tipo = FORTE;
        inimigo.stats.hp_max = random_int_in_range(40, 100);
        inimigo.num_acoes_ia = 3; // Tem 3 passos no ciclo
    } else {
        // INIMIGO FRACO
        inimigo.tipo = FRACO;
        inimigo.stats.hp_max = random_int_in_range(10, 30);
        inimigo.num_acoes_ia = 2; // Tem 2 passos no ciclo
    }

    // Define o que o inimigo vai fazer (Atacar ou Defender)
    for (int i = 0; i < inimigo.num_acoes_ia; i++) {
        TipoCarta tipo = (rand() % 2 == 0) ? ATAQUE : DEFESA;
        Card acao = create_random_card(tipo, random_int_in_range(0, 2));
        
        inimigo.ia_ciclo[i].tipo_acao = acao.tipo;
        inimigo.ia_ciclo[i].valor_efeito = acao.efeito_valor;
        
        if (tipo == ATAQUE) tem_ataque = 1;
    }

    // GARANTIA: Se o inimigo foi criado só com defesa, força o primeiro golpe a ser Ataque
    if (!tem_ataque) {
        inimigo.ia_ciclo[0].tipo_acao = ATAQUE;
    }

    inimigo.stats.hp_atual = inimigo.stats.hp_max;
    inimigo.stats.escudo = 0;
    inimigo.acao_ia_atual = 0; // Começa na primeira ação

    return inimigo;
}