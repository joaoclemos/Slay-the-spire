#include "setup.h"
#include <stdlib.h> // Necessário para rand() (números aleatórios)
#include <time.h>   // Necessário para time()
#include "constants.h"

// Gera um número aleatório entre min e max (inclusive)
int random_int_in_range(int min, int max) {
    // rand() % N dá um número de 0 a N-1. Ajustamos a matemática aqui.
    return (rand() % (max - min + 1)) + min;
}

// Cria uma carta baseada no tipo e custo, definindo seus efeitos
Card create_random_card(TipoCarta tipo, int custo) {
    Card nova_carta;
    nova_carta.tipo = tipo;
    nova_carta.custo_energia = custo;
    nova_carta.magnitude = 0;    // Começa zerado
    nova_carta.is_vampiric = 0;  // Começa não-vampírica

    // Carta Especial (Troca Mão) sempre custa 0 e não tem efeito numérico
    if (tipo == ESPECIAL) {
        nova_carta.custo_energia = 0;
        nova_carta.efeito_valor = 0;
        return nova_carta;
    }

    // Lógica para Ataque e Defesa
    if (tipo == ATAQUE || tipo == DEFESA) {
        // Define o poder baseado no custo (Regras do Enunciado)
        switch (custo) {
            case 0: nova_carta.efeito_valor = random_int_in_range(1, 5); break;
            case 1: nova_carta.efeito_valor = random_int_in_range(5, 10); break;
            case 2: nova_carta.efeito_valor = random_int_in_range(10, 15); break;
            case 3: nova_carta.efeito_valor = random_int_in_range(15, 30); break;
            default: nova_carta.efeito_valor = 1; break;
        }
    }
    // Lógica para BUFFS (Coisas boas)
    else if (tipo == BUFF) {
        int sorteio = rand() % 4; // Sorteia entre 4 tipos
        if (sorteio == 0) { // Força
            nova_carta.efeito_valor = ID_FORCA; 
            nova_carta.magnitude = 1; 
        } 
        else if (sorteio == 1) { // Destreza
            nova_carta.efeito_valor = ID_DESTREZA; 
            nova_carta.magnitude = 1; 
        }
        else if (sorteio == 2) { // Cura Instantânea
            nova_carta.efeito_valor = ID_CURA_INSTANT; 
            // Cura 5x a energia gasta
            nova_carta.magnitude = (custo == 0 ? 1 : custo) * 5; 
        }
        else { // Regeneração
            nova_carta.efeito_valor = ID_REGEN_RODADAS;
            // 50% chance de ser Custo X (gasta tudo), 50% de ser normal
            if (rand() % 2 == 0) { 
                nova_carta.custo_energia = CUSTO_X; 
                nova_carta.magnitude = 0; 
            } else { 
                nova_carta.magnitude = (custo == 0 ? 2 : custo + 1); 
            }
        }
    }
    // Lógica para DEBUFFS (Coisas ruins pro inimigo)
    else if (tipo == DEBUFF) {
        int sorteio = rand() % 4;
        if (sorteio == 0) { // Veneno
            nova_carta.efeito_valor = ID_VENENO; 
            nova_carta.magnitude = random_int_in_range(3, 5); 
        } 
        else if (sorteio == 1) { // Vulnerável
            nova_carta.efeito_valor = ID_VULNERAVEL; 
            nova_carta.magnitude = 1; 
        } 
        else if (sorteio == 2) { // Fraqueza
            nova_carta.efeito_valor = ID_FRAQUEZA; 
            nova_carta.magnitude = 1; 
        }
        else { // Sono
            nova_carta.efeito_valor = ID_SONO; 
            nova_carta.magnitude = 1; 
        }
    }

    return nova_carta;
}

// Algoritmo Fisher-Yates para embaralhar o vetor de cartas
void shuffle_pilha(PilhaCartas *pilha) {
    for (int i = pilha->num_cartas - 1; i > 0; i--) {
        int j = rand() % (i + 1); // Sorteia índice
        // Troca carta i com j
        Card temp = pilha->cartas[i];
        pilha->cartas[i] = pilha->cartas[j];
        pilha->cartas[j] = temp;
    }
}

// Monta o baralho de 20 cartas com a distribuição pedida
PilhaCartas create_initial_deck() {
    PilhaCartas baralho;
    baralho.num_cartas = 0;
    int i = 0;

    // 7 Ataques (custos variados)
    baralho.cartas[i++] = create_random_card(ATAQUE, 0);
    baralho.cartas[i++] = create_random_card(ATAQUE, 1);
    baralho.cartas[i++] = create_random_card(ATAQUE, 2);
    baralho.cartas[i++] = create_random_card(ATAQUE, 3); // Obrigatório ter custo 3
    for(int k=0; k<3; k++) baralho.cartas[i++] = create_random_card(ATAQUE, random_int_in_range(0, 2));
    
    // 6 Defesas
    baralho.cartas[i++] = create_random_card(DEFESA, 0);
    baralho.cartas[i++] = create_random_card(DEFESA, 1);
    baralho.cartas[i++] = create_random_card(DEFESA, 2);
    baralho.cartas[i++] = create_random_card(DEFESA, 3); // Obrigatório ter custo 3
    for(int k=0; k<2; k++) baralho.cartas[i++] = create_random_card(DEFESA, random_int_in_range(0, 2));

    // 2 Especiais
    baralho.cartas[i++] = create_random_card(ESPECIAL, 0);
    baralho.cartas[i++] = create_random_card(ESPECIAL, 0);

    // 2 Buffs e 2 Debuffs
    baralho.cartas[i++] = create_random_card(BUFF, 1);
    baralho.cartas[i++] = create_random_card(BUFF, 1);
    baralho.cartas[i++] = create_random_card(DEBUFF, 1);
    baralho.cartas[i++] = create_random_card(DEBUFF, 1);

    // 1 Golpe Vampírico (Substitui o 8º ataque)
    baralho.cartas[i].tipo = ATAQUE;
    baralho.cartas[i].custo_energia = 2;
    baralho.cartas[i].efeito_valor = 8;
    baralho.cartas[i].magnitude = 0;
    baralho.cartas[i].is_vampiric = 1; // Marca como vampírica
    i++;

    // Define tamanho e embaralha
    baralho.num_cartas = 20;
    shuffle_pilha(&baralho);
    return baralho;
}

// Inicializa o jogador
Player setup_player() {
    Player jogador;
    // Stats iniciais
    jogador.stats.hp_atual = 100;
    jogador.stats.hp_max = 100;
    jogador.stats.escudo = 0;
    
    // Zera todos os status
    jogador.stats.forca = 0;
    jogador.stats.destreza = 0;
    jogador.stats.veneno = 0;
    jogador.stats.vulneravel = 0;
    jogador.stats.fraco = 0;
    jogador.stats.regeneracao = 0;
    jogador.stats.dormindo = 0;

    jogador.energia_atual = 3;
    jogador.energia_max = 3;
    
    // Cria o baralho e deixa as outras pilhas vazias
    jogador.baralho_completo = create_initial_deck();
    jogador.pilha_compra.num_cartas = 0;
    jogador.mao.num_cartas = 0;
    jogador.pilha_descarte.num_cartas = 0;
    
    return jogador;
}

// Cria um Inimigo Normal
Enemy create_enemy() {
    Enemy inimigo;
    float chance = (float)rand() / RAND_MAX;
    int tem_ataque = 0;

    // Zera status
    inimigo.stats.forca = 0;
    inimigo.stats.destreza = 0;
    inimigo.stats.veneno = 0;
    inimigo.stats.vulneravel = 0;
    inimigo.stats.fraco = 0;
    inimigo.stats.regeneracao = 0;
    inimigo.stats.dormindo = 0;

    // Decide se é Forte ou Fraco
    if (chance <= 0.05) { // 5% chance de ser Forte
        inimigo.tipo = FORTE;
        inimigo.stats.hp_max = random_int_in_range(50, 100);
        inimigo.num_acoes_ia = 3; 
    } else {
        inimigo.tipo = FRACO;
        inimigo.stats.hp_max = random_int_in_range(15, 35);
        inimigo.num_acoes_ia = 2; 
    }

    // Preenche o ciclo de IA
    for (int i = 0; i < inimigo.num_acoes_ia; i++) {
        TipoCarta tipo = (rand() % 2 == 0) ? ATAQUE : DEFESA;
        Card acao = create_random_card(tipo, random_int_in_range(0, 2));
        inimigo.ia_ciclo[i].tipo_acao = acao.tipo;
        inimigo.ia_ciclo[i].valor_efeito = acao.efeito_valor;
        if (tipo == ATAQUE) tem_ataque = 1;
    }

    // Garante que ele tenha pelo menos um ataque
    if (!tem_ataque) {
        inimigo.ia_ciclo[0].tipo_acao = ATAQUE;
    }

    inimigo.stats.hp_atual = inimigo.stats.hp_max;
    inimigo.stats.escudo = 0;
    inimigo.acao_ia_atual = 0; 

    return inimigo;
}

// Cria o Boss Final
Enemy create_boss() {
    Enemy boss;
    boss.tipo = BOSS;
    boss.stats.hp_max = 300; 
    boss.stats.hp_atual = 300;
    boss.stats.escudo = 0;
    
    // Boss começa com força extra
    boss.stats.forca = 2; 
    boss.stats.destreza = 0;
    boss.stats.veneno = 0;
    boss.stats.vulneravel = 0;
    boss.stats.fraco = 0;
    boss.stats.regeneracao = 0;
    boss.stats.dormindo = 0;

    // Ciclo fixo e poderoso
    boss.num_acoes_ia = 3;
    boss.ia_ciclo[0].tipo_acao = ATAQUE; boss.ia_ciclo[0].valor_efeito = 20; 
    boss.ia_ciclo[1].tipo_acao = DEFESA; boss.ia_ciclo[1].valor_efeito = 30; 
    boss.ia_ciclo[2].tipo_acao = ATAQUE; boss.ia_ciclo[2].valor_efeito = 25; 
    
    boss.acao_ia_atual = 0;
    return boss;
}