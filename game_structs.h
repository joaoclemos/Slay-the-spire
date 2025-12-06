#ifndef GAME_STRUCTS_H // Proteção contra inclusão dupla
#define GAME_STRUCTS_H // Define o símbolo

// --- TIPOS DE CARTA ---
// 'enum' cria uma lista de opções numéricas com nomes fáceis
typedef enum {
    ATAQUE,   // Vale 0
    DEFESA,   // Vale 1
    ESPECIAL, // Vale 2
    BUFF,     // Vale 3
    DEBUFF    // Vale 4
} TipoCarta;

// --- IDENTIFICADORES DE EFEITO (IDs) ---
// Números para diferenciar o que cada buff faz
#define ID_FORCA 1        // Aumenta dano
#define ID_DESTREZA 2     // Aumenta defesa
#define ID_VENENO 3       // Dano por turno
#define ID_VULNERAVEL 4   // Recebe mais dano
#define ID_FRAQUEZA 5     // Causa menos dano
#define ID_CURA_INSTANT 6 // Cura na hora
#define ID_REGEN_RODADAS 7// Cura por turnos
#define ID_SONO 8         // Perde a vez

// --- A CARTA ---
typedef struct {
    TipoCarta tipo;    // Qual o tipo (Ataque, Defesa...)
    int custo_energia; // Quanto custa para jogar? (se for -1 é Custo X)
    int efeito_valor;  // O valor principal (Dano, Escudo ou ID do efeito)
    int magnitude;     // A força do efeito (ex: quantos turnos dura o veneno)
    int is_vampiric;   // 1 se a carta rouba vida, 0 se não
} Card;

// --- PILHA DE CARTAS ---
typedef struct {
    Card cartas[50]; // Um vetor (lista) que cabe até 50 cartas
    int num_cartas;  // Contador de quantas cartas tem na lista agora
} PilhaCartas;

// --- CRIATURA ---
// Estrutura base para qualquer ser vivo (Jogador ou Inimigo)
typedef struct {
    int hp_atual; // Vida neste momento
    int hp_max;   // Vida máxima total
    int escudo;   // Armadura temporária
    
    // Status (Buffs e Debuffs) - Contadores
    int forca;      
    int destreza;   
    int veneno;     
    int vulneravel; 
    int fraco;      
    int regeneracao; 
    int dormindo;    
} Creature;

// --- O JOGADOR ---
typedef struct {
    Creature stats;      // Herda vida, escudo e status da Creature
    int energia_atual;   // Energia sobrando no turno
    int energia_max;     // Energia total por turno

    // As 4 zonas de cartas do jogo
    PilhaCartas baralho_completo; // Todas as cartas do deck
    PilhaCartas pilha_compra;     // Cartas viradas para comprar
    PilhaCartas mao;              // Cartas na mão agora
    PilhaCartas pilha_descarte;   // Cartas já usadas
} Player;

// --- TIPOS DE INIMIGO ---
typedef enum {
    FRACO, // Inimigo básico
    FORTE, // Inimigo mais difícil
    BOSS   // O Chefão final
} TipoInimigo;

// --- AÇÃO DA IA ---
// O que o inimigo planeja fazer?
typedef struct {
    TipoCarta tipo_acao; // Vai atacar ou defender?
    int valor_efeito;    // Quanto de dano ou escudo?
} AI_Action;

// --- O INIMIGO ---
typedef struct {
    Creature stats;      // Vida e status
    TipoInimigo tipo;    // Fraco/Forte/Boss

    AI_Action ia_ciclo[3]; // Lista de 3 ações que ele repete em loop
    int num_acoes_ia;      // Quantas ações tem esse ciclo (2 ou 3)
    int acao_ia_atual;     // Qual passo do ciclo ele está agora (0, 1 ou 2)
} Enemy;

// --- ESTADOS DO JOGO ---
// Controla em qual tela ou fase o jogo está
typedef enum {
    GAME_STATE_MENU,        // Tela de "Pressione Enter"
    GAME_STATE_START,       // Carregando/Criando personagem
    GAME_STATE_NEW_COMBAT,  // Criando novos inimigos
    GAME_STATE_PLAYER_TURN, // Sua vez de jogar
    GAME_STATE_ENEMY_TURN,  // Vez do inimigo
    GAME_STATE_VICTORY,     // Ganhou o jogo (tela verde)
    GAME_STATE_GAME_OVER    // Perdeu o jogo (tela vermelha)
} GameState;

#endif