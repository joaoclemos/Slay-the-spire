/*
 * renderer.h
 *
 * Modificado para incluir DOIS tipos de inimigo (Fraco e Forte).
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h> 
#include "constants.h"
#include "game_structs.h"

typedef struct {
    ALLEGRO_DISPLAY* display;
    ALLEGRO_FONT* font;
    
    // --- IMAGENS ---
    ALLEGRO_BITMAP* img_player;
    ALLEGRO_BITMAP* img_enemy_weak;   // Inimigo Comum
    ALLEGRO_BITMAP* img_enemy_strong; // Inimigo Forte (Boss)
    ALLEGRO_BITMAP* img_card;       
    ALLEGRO_BITMAP* img_background; 
} Renderer;

void FillRenderer(Renderer* renderer);
void ClearRenderer(Renderer* renderer);

void Render(Renderer* renderer,
            GameState current_state,
            Player player,
            Enemy inimigos[],
            int carta_sel, int inimigo_sel, int selecionando_alvo);

#endif