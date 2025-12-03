#ifndef CONSTANTS_H
#define CONSTANTS_H

// --- TAMANHO DA TELA ---
#define SCREEN_W 1280
#define SCREEN_H 720

// --- TAMANHO DAS CARTAS ---
#define CARD_W 120
#define CARD_H 180

// --- POSIÇÕES NA TELA (X = Horizontal, Y = Vertical) ---

// Onde o JOGADOR fica
#define PLAYER_X 200
#define PLAYER_Y 400

// Onde o INIMIGO 1 fica
#define ENEMY_1_X 900
#define ENEMY_1_Y 400

// Onde o INIMIGO 2 fica
#define ENEMY_2_X 1100
#define ENEMY_2_Y 400

// Posições das pilhas de cartas no chão
#define DECK_X 50      // Pilha de Compra
#define DECK_Y 550
#define DISCARD_X 1100 // Pilha de Descarte
#define DISCARD_Y 550

// Onde a primeira carta da MÃO é desenhada
#define HAND_X 300
#define HAND_Y 550

// Códigos para o teclado (para saber se apertou a tecla)
#define GAME_KEY_SEEN 1
#define GAME_KEY_DOWN 2

#endif