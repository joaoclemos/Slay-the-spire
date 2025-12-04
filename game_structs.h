#ifndef GAME_STRUCTS_H
#define GAME_STRUCTS_H

// --- TIPOS DE CARTA ---
typedef enum {
    ATAQUE,
    DEFESA,
    ESPECIAL, 
    BUFF,     
    DEBUFF    
} TipoCarta;

// --- IDENTIFICADORES DE EFEITO ---
#define ID_FORCA 1
#define ID_DESTREZA 2
#define ID_VENENO 3
#define ID_VULNERAVEL 4
#define ID_FRAQUEZA 5
#define ID_CURA_INSTANT 6
#define ID_REGEN_RODADAS 7
#define ID_SONO 8
#define CUSTO_X -1  // Custo variável

// --- A CARTA ---
typedef struct {
    TipoCarta tipo;    
    int custo_energia; 
    int efeito_valor;  
    int magnitude;     
    int is_vampiric;   
} Card;

// --- PILHA DE CARTAS ---
typedef struct {
    Card cartas[50]; 
    int num_cartas;  
} PilhaCartas;

// --- CRIATURA ---
typedef struct {
    int hp_atual; 
    int hp_max;   
    int escudo;
    
    // Status
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
    Creature stats;      
    int energia_atual;   
    int energia_max;     

    PilhaCartas baralho_completo; 
    PilhaCartas pilha_compra;     
    PilhaCartas mao;              
    PilhaCartas pilha_descarte;   
} Player;

// --- TIPOS DE INIMIGO ---
typedef enum {
    FRACO,
    FORTE,
    BOSS
} TipoInimigo;

// --- AÇÃO DA IA ---
typedef struct {
    TipoCarta tipo_acao; 
    int valor_efeito;    
} AI_Action;

// --- O INIMIGO ---
typedef struct {
    Creature stats;      
    TipoInimigo tipo;    

    AI_Action ia_ciclo[3]; 
    int num_acoes_ia;      
    int acao_ia_atual;     
} Enemy;

// --- ESTADOS DO JOGO (COM MENU) ---
typedef enum {
    GAME_STATE_MENU,      // <--- NOVO ESTADO
    GAME_STATE_START,       
    GAME_STATE_NEW_COMBAT,  
    GAME_STATE_PLAYER_TURN, 
    GAME_STATE_ENEMY_TURN,  
    GAME_STATE_VICTORY,     
    GAME_STATE_GAME_OVER    
} GameState;

#endif