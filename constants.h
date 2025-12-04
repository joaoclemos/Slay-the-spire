#ifndef CONSTANTS_H // Se CONSTANTS_H não foi definido ainda...
#define CONSTANTS_H // ...define agora. (Isso evita erros de incluir o arquivo 2 vezes)

// --- CONFIGURAÇÕES DA TELA ---
#define SCREEN_W 1280 // Define a LARGURA da janela do jogo em pixels
#define SCREEN_H 720  // Define a ALTURA da janela do jogo em pixels

// --- TAMANHOS ---
#define CARD_W 120 // Largura visual de uma carta
#define CARD_H 180 // Altura visual de uma carta

// --- POSIÇÕES (Coordenadas X, Y na tela) ---
// (0,0) é o canto superior esquerdo da tela.

#define PLAYER_X 200 // Posição horizontal onde o Jogador será desenhado
#define PLAYER_Y 400 // Posição vertical (chão) do Jogador

// Posições dos inimigos
#define ENEMY_1_X 900  // Inimigo da esquerda
#define ENEMY_1_Y 400  // Altura do chão
#define ENEMY_2_X 1100 // Inimigo da direita (mais ao fundo)
#define ENEMY_2_Y 400  // Altura do chão

// Posições das pilhas de cartas
#define DECK_X 50      // Onde fica o monte de comprar (canto esquerdo)
#define DECK_Y 550     // Altura do monte
#define DISCARD_X 1100 // Onde fica o monte de descarte (canto direito)
#define DISCARD_Y 550  // Altura do monte

// Onde a mão do jogador começa a ser desenhada
#define HAND_X 300 // Começa um pouco depois do monte de compra
#define HAND_Y 550 // Mesma altura dos montes

// --- CUSTO ESPECIAL ---
#define CUSTO_X -1  // Valor numérico para representar cartas que gastam "toda a energia"

// --- CONTROLES ---
// Códigos internos para ajudar a saber se uma tecla foi apertada ou segurada
#define GAME_KEY_SEEN 1 // A tecla foi "vista" pelo sistema
#define GAME_KEY_DOWN 2 // A tecla está "pressionada" neste momento

#endif // Fim do bloco de proteção do arquivo