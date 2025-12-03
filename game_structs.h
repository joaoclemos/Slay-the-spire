#ifndef GAME_STRUCTS_H
#define GAME_STRUCTS_H

// --- TIPOS DE COISAS ---

// Tipo da Carta: É de bater, defender ou especial?
typedef enum {
    ATAQUE,
    DEFESA,
    ESPECIAL
} TipoCarta;

// Tipo do Inimigo: É fraco ou forte (Boss)?
typedef enum {
    FRACO,
    FORTE
} TipoInimigo;

// Estados do Jogo: O que está acontecendo agora?
typedef enum {
    GAME_STATE_START,       // Carregando o jogo
    GAME_STATE_NEW_COMBAT,  // Preparando uma nova luta
    GAME_STATE_PLAYER_TURN, // É a vez do jogador
    GAME_STATE_ENEMY_TURN,  // É a vez do inimigo
    GAME_STATE_VICTORY,     // Ganhou o jogo todo
    GAME_STATE_GAME_OVER    // Perdeu o jogo
} GameState;


// --- ESTRUTURAS (DADOS) ---

// 1. A CARTA
typedef struct {
    TipoCarta tipo;    // Qual o tipo (Ataque/Defesa)
    int custo_energia; // Quanto custa para usar
    int efeito_valor;  // Quanto de dano ou escudo ela dá
} Card;

// 2. UM MONTE DE CARTAS (Serve para o baralho, mão e descarte)
typedef struct {
    Card cartas[50]; // Guarda até 50 cartas
    int num_cartas;  // Quantas cartas tem agora
} PilhaCartas;

// 3. UMA CRIATURA (Base para Jogador e Inimigos)
typedef struct {
    int hp_atual; // Vida agora
    int hp_max;   // Vida máxima
    int escudo;   // Escudo atual
} Creature;

// 4. O JOGADOR (Tem vida + cartas + energia)
typedef struct {
    Creature stats;      // Vida e Escudo
    int energia_atual;   // Energia sobrando
    int energia_max;     // Energia total

    PilhaCartas baralho_completo; // Todas as cartas que possui
    PilhaCartas pilha_compra;     // Cartas para comprar
    PilhaCartas mao;              // Cartas na mão agora
    PilhaCartas pilha_descarte;   // Cartas já usadas
} Player;

// 5. AÇÃO DA IA (O que o inimigo planeja fazer)
typedef struct {
    TipoCarta tipo_acao; // Vai atacar ou defender?
    int valor_efeito;    // Com que força?
} AI_Action;

// 6. O INIMIGO (Tem vida + inteligência artificial)
typedef struct {
    Creature stats;      // Vida e Escudo
    TipoInimigo tipo;    // Fraco ou Forte

    AI_Action ia_ciclo[3]; // Lista de ações que ele repete (ex: Ataca -> Defende)
    int num_acoes_ia;      // Quantas ações diferentes ele tem
    int acao_ia_atual;     // Qual ação ele vai fazer agora
} Enemy;

#endif