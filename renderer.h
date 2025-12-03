#ifndef RENDERER_H
#define RENDERER_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h> 
#include "constants.h"
#include "game_structs.h"

// Estrutura que guarda tudo necessário para desenhar
typedef struct {
    ALLEGRO_DISPLAY* display; // A Janela
    ALLEGRO_FONT* font;       // A Fonte do texto
    
    // Imagens que vamos carregar do disco
    ALLEGRO_BITMAP* img_player;
    ALLEGRO_BITMAP* img_enemy_weak;
    ALLEGRO_BITMAP* img_enemy_strong;
    ALLEGRO_BITMAP* img_card;
    ALLEGRO_BITMAP* img_background;
} Renderer;

// Inicia a janela e carrega imagens
void FillRenderer(Renderer* renderer);

// Limpa a memória ao fechar
void ClearRenderer(Renderer* renderer);

// Função PRINCIPAL que desenha tudo na tela a cada frame
void Render(Renderer* renderer, GameState current_state, Player player, Enemy inimigos[], 
            int carta_sel, int inimigo_sel, int selecionando_alvo);

#endif