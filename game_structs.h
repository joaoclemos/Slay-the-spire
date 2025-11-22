// Isso garante que este arquivo só seja "lido" uma vez pelo compilador
#ifndef GAME_STRUCTS_H
#define GAME_STRUCTS_H

// --- 📦 ESTRUTURAS DAS CARTAS ---

// Enum para os tipos de carta
typedef enum {
    ATAQUE,
    DEFESA,
    ESPECIAL
} TipoCarta;

// A struct de uma única carta
typedef struct {
    TipoCarta tipo;
    int custo_energia;
    int efeito_valor; // Dano do ataque, escudo da defesa, etc.
} Card;

// Struct para um grupo de cartas (baralho, mão, descarte, etc.)
typedef struct {
    Card cartas[50]; // Um limite de 50 cartas por pilha (pode ajustar)
    int num_cartas;
} PilhaCartas;


// --- 🛠️ ESTRUTURAS DAS CRIATURAS ---

// Uma struct base que serve para o jogador E para os inimigos
typedef struct {
    int hp_atual;
    int hp_max;
    int escudo;
} Creature;

// --- ESTRUTURA DO JOGADOR ---
typedef struct {
    Creature stats;      // Os stats base (HP, escudo)
    int energia_atual;
    int energia_max;

    PilhaCartas baralho_completo; // A lista mestre de 20 cartas
    PilhaCartas pilha_compra;
    PilhaCartas mao;
    PilhaCartas pilha_descarte;
} Player;


// --- ESTRUTURA DOS INIMIGOS ---

// Enum para os tipos de inimigo
typedef enum {
    FRACO,
    FORTE
} TipoInimigo;

// Uma ação da IA do inimigo
typedef struct {
    TipoCarta tipo_acao; // Só pode ser ATAQUE ou DEFESA
    int valor_efeito;
} AI_Action;

// A struct de um único inimigo
typedef struct {
    Creature stats;
    TipoInimigo tipo;

    AI_Action ia_ciclo[3]; // Inimigos têm de 1 a 3 ações
    int num_acoes_ia;
    int acao_ia_atual;    // Índice de qual ação ele fará no ciclo
} Enemy;

// --- MÁQUINA DE ESTADOS DO JOGO ---
// Colado do main.c
typedef enum {
    GAME_STATE_START,
    GAME_STATE_NEW_COMBAT,
    GAME_STATE_PLAYER_TURN,
    GAME_STATE_ENEMY_TURN,
    GAME_STATE_VICTORY,
    GAME_STATE_GAME_OVER
} GameState;


#endif // Fim da guarda de cabeçalho

