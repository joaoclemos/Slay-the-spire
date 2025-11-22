/*
 * renderer.c
 *
 * Implementação da lógica de renderização.
 * AGORA SUPORTA INIMIGOS FRACOS E FORTES DISTINTOS.
 */

#include "renderer.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

#include "game_structs.h"
#include "constants.h"
#include "utils.h"

// --- FUNÇÕES AUXILIARES DE DESENHO ---

/*
 * Desenha uma criatura. 
 * Escolhe a imagem correta (Player, Enemy Weak, Enemy Strong) automaticamente.
 */
void draw_creature(Renderer* renderer, Creature creature, float x, float y, const char* label) {
    
    int desenhou_imagem = 0;
    float altura_img = 100; // Altura padrão

    // --- 1. Escolher qual imagem desenhar ---
    ALLEGRO_BITMAP* img_to_draw = NULL;

    if (strcmp(label, "Jogador") == 0) {
        img_to_draw = renderer->img_player;
    } else {
        // É um Inimigo. Vamos ver se é Forte ou Fraco.
        // No setup.c: Fraco tem HP até 30. Forte tem HP acima de 40.
        // Vamos usar 35 como divisor.
        if (creature.hp_max > 35) {
            img_to_draw = renderer->img_enemy_strong;
            // Se não tiver imagem de forte, tenta usar a de fraco como fallback
            if (!img_to_draw) img_to_draw = renderer->img_enemy_weak;
        } else {
            img_to_draw = renderer->img_enemy_weak;
        }
    }

    // --- 2. Desenhar a Imagem ---
    if (img_to_draw != NULL) {
        // Pega o tamanho real da imagem para centralizar
        int w = al_get_bitmap_width(img_to_draw);
        int h = al_get_bitmap_height(img_to_draw);
        altura_img = h; 

        // Desenha centralizado no X e com os pés no Y
        al_draw_bitmap(img_to_draw, x - (w / 2), y - h, 0);
        desenhou_imagem = 1;
    }

    // Se não tem imagem, desenha retângulo
    if (!desenhou_imagem) {
        // Cor diferente para forte/fraco no modo retângulo também
        if (strcmp(label, "Jogador") != 0 && creature.hp_max > 35) {
            al_draw_filled_rectangle(x - 40, y - 120, x + 40, y, al_map_rgb(100, 0, 0)); // Vermelho escuro pro forte
            altura_img = 120;
        } else {
            al_draw_filled_rectangle(x - 30, y - 100, x + 30, y, al_map_rgb(150, 150, 150)); // Cinza normal
            altura_img = 100;
        }
    }
    
    // --- 3. Texto do Nome ---
    al_draw_text(renderer->font, al_map_rgb(255, 255, 255), x, y - altura_img - 20, ALLEGRO_ALIGN_CENTER, label);

    // --- 4. Barra de Vida ---
    float hp_percent = (float)creature.hp_atual / (float)creature.hp_max;
    if (hp_percent < 0) hp_percent = 0;
    float hp_bar_width = 100.0 * hp_percent;

    al_draw_filled_rectangle(x - 50, y + 10, x + 50, y + 30, al_map_rgb(255, 0, 0)); // Fundo
    al_draw_filled_rectangle(x - 50, y + 10, (x - 50) + hp_bar_width, y + 30, al_map_rgb(0, 255, 0)); // Frente

    char hp_text[20];
    sprintf(hp_text, "%d/%d", creature.hp_atual, creature.hp_max);
    al_draw_text(renderer->font, al_map_rgb(255, 255, 255), x, y + 12, ALLEGRO_ALIGN_CENTER, hp_text);

    // --- 5. Escudo ---
    if (creature.escudo > 0) {
        char shield_text[20];
        sprintf(shield_text, "[%d]", creature.escudo);
        al_draw_text(renderer->font, al_map_rgb(0, 200, 255), x + 60, y + 12, ALLEGRO_ALIGN_LEFT, shield_text);
    }
}

/*
 * Desenha uma carta.
 */
void draw_card(Renderer* renderer, Card card, float x, float y) {
    // Fundo
    if (renderer->img_card != NULL) {
        al_draw_bitmap(renderer->img_card, x, y, 0);
    } else {
        al_draw_filled_rectangle(x, y, x + CARD_W, y + CARD_H, al_map_rgb(50, 50, 100));
        al_draw_rectangle(x, y, x + CARD_W, y + CARD_H, al_map_rgb(255, 255, 255), 2.0);
    }

    // Textos
    char card_text[50];
    sprintf(card_text, "%d", card.custo_energia);
    al_draw_filled_circle(x + 20, y + 20, 15, al_map_rgb(0, 0, 0));
    al_draw_text(renderer->font, al_map_rgb(255, 255, 0), x + 20, y + 12, ALLEGRO_ALIGN_CENTER, card_text);

    switch (card.tipo) {
        case ATAQUE:
            sprintf(card_text, "ATAQUE");
            al_draw_text(renderer->font, al_map_rgb(255, 150, 150), x + (CARD_W / 2), y + 40, ALLEGRO_ALIGN_CENTER, card_text);
            sprintf(card_text, "%d DANO", card.efeito_valor);
            al_draw_text(renderer->font, al_map_rgb(255, 255, 255), x + (CARD_W / 2), y + 80, ALLEGRO_ALIGN_CENTER, card_text);
            break;
        case DEFESA:
            sprintf(card_text, "DEFESA");
            al_draw_text(renderer->font, al_map_rgb(150, 150, 255), x + (CARD_W / 2), y + 40, ALLEGRO_ALIGN_CENTER, card_text);
            sprintf(card_text, "%d ESCUDO", card.efeito_valor);
            al_draw_text(renderer->font, al_map_rgb(255, 255, 255), x + (CARD_W / 2), y + 80, ALLEGRO_ALIGN_CENTER, card_text);
            break;
        case ESPECIAL:
            sprintf(card_text, "ESPECIAL");
            al_draw_text(renderer->font, al_map_rgb(200, 100, 200), x + (CARD_W / 2), y + 40, ALLEGRO_ALIGN_CENTER, card_text);
            al_draw_text(renderer->font, al_map_rgb(255, 255, 255), x + (CARD_W / 2), y + 80, ALLEGRO_ALIGN_CENTER, "COMPRA 5");
            break;
    }
}

/*
 * Desenha a tela de combate completa.
 */
void draw_combat_screen(Renderer* renderer, Player player, Enemy inimigos[], 
                        int carta_sel, int inimigo_sel, int selecionando_alvo) {
    
    // Background
    if (renderer->img_background != NULL) {
        al_draw_bitmap(renderer->img_background, 0, 0, 0);
    }

    // Jogador
    draw_creature(renderer, player.stats, PLAYER_X, PLAYER_Y, "Jogador");

    // Inimigos
    for(int i=0; i<2; i++) {
        if (inimigos[i].stats.hp_atual > 0) {
            float ex = (i==0) ? ENEMY_1_X : ENEMY_2_X;
            float ey = (i==0) ? ENEMY_1_Y : ENEMY_2_Y;
            
            char nome[20];
            sprintf(nome, "Inimigo %d", i+1);
            draw_creature(renderer, inimigos[i].stats, ex, ey, nome);

            // Intenção
            AI_Action acao = inimigos[i].ia_ciclo[inimigos[i].acao_ia_atual];
            char intent[30];
            if (acao.tipo_acao == ATAQUE) sprintf(intent, "ATK %d", acao.valor_efeito);
            else sprintf(intent, "DEF %d", acao.valor_efeito);
            
            al_draw_text(renderer->font, 
                         (acao.tipo_acao == ATAQUE) ? al_map_rgb(255,100,100) : al_map_rgb(100,100,255), 
                         ex, ey - 140, ALLEGRO_ALIGN_CENTER, intent);

            // Mira
            if (selecionando_alvo && inimigo_sel == i) {
                al_draw_rectangle(ex - 40, ey - 110, ex + 40, ey + 40, al_map_rgb(255, 0, 0), 3.0);
                al_draw_text(renderer->font, al_map_rgb(255, 0, 0), ex, ey - 170, ALLEGRO_ALIGN_CENTER, "ALVO");
            }
        }
    }

    // UI (Energia e Pilhas)
    char txt[30];
    sprintf(txt, "Energia: %d/%d", player.energia_atual, player.energia_max);
    al_draw_text(renderer->font, al_map_rgb(255, 255, 0), 30, 30, ALLEGRO_ALIGN_LEFT, txt);

    al_draw_filled_rectangle(DECK_X, DECK_Y, DECK_X + CARD_W, DECK_Y + CARD_H, al_map_rgb(0, 50, 0));
    sprintf(txt, "%d", player.pilha_compra.num_cartas);
    al_draw_text(renderer->font, al_map_rgb(255, 255, 255), DECK_X + CARD_W/2, DECK_Y + 50, ALLEGRO_ALIGN_CENTER, txt);
    al_draw_text(renderer->font, al_map_rgb(255, 255, 255), DECK_X + CARD_W/2, DECK_Y + 80, ALLEGRO_ALIGN_CENTER, "COMPRA");

    al_draw_filled_rectangle(DISCARD_X, DISCARD_Y, DISCARD_X + CARD_W, DISCARD_Y + CARD_H, al_map_rgb(50, 0, 0));
    sprintf(txt, "%d", player.pilha_descarte.num_cartas);
    al_draw_text(renderer->font, al_map_rgb(255, 255, 255), DISCARD_X + CARD_W/2, DISCARD_Y + 50, ALLEGRO_ALIGN_CENTER, txt);
    al_draw_text(renderer->font, al_map_rgb(255, 255, 255), DISCARD_X + CARD_W/2, DISCARD_Y + 80, ALLEGRO_ALIGN_CENTER, "DESCARTE");

    // Mão
    for (int i = 0; i < player.mao.num_cartas; i++) {
        float cx = HAND_X + (i * (CARD_W + 10));
        float cy = HAND_Y;
        if (i == carta_sel) {
            cy -= 30;
            al_draw_rectangle(cx - 3, cy - 3, cx + CARD_W + 3, cy + CARD_H + 3, al_map_rgb(255, 215, 0), 4.0);
        }
        draw_card(renderer, player.mao.cartas[i], cx, cy);
    }
}

void FillRenderer(Renderer* renderer) {
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    renderer->display = al_create_display(SCREEN_W, SCREEN_H);
    must_init(renderer->display, "display");

    must_init(al_init_font_addon(), "font addon");
    must_init(al_init_ttf_addon(), "ttf addon");
    must_init(al_init_image_addon(), "image addon");

    renderer->font = al_create_builtin_font();

    // Carrega Imagens
    renderer->img_player       = al_load_bitmap("player.png");
    renderer->img_enemy_weak   = al_load_bitmap("enemy_weak.png");
    renderer->img_enemy_strong = al_load_bitmap("enemy_strong.png");
    renderer->img_card         = al_load_bitmap("card.png");
    renderer->img_background   = al_load_bitmap("background.png");

    if (!renderer->img_player) printf("AVISO: player.png nao encontrado.\n");
    if (!renderer->img_enemy_weak) printf("AVISO: enemy_weak.png nao encontrado.\n");
}

void ClearRenderer(Renderer* renderer) {
    if (renderer->img_player) al_destroy_bitmap(renderer->img_player);
    if (renderer->img_enemy_weak) al_destroy_bitmap(renderer->img_enemy_weak);
    if (renderer->img_enemy_strong) al_destroy_bitmap(renderer->img_enemy_strong);
    if (renderer->img_card) al_destroy_bitmap(renderer->img_card);
    if (renderer->img_background) al_destroy_bitmap(renderer->img_background);

    al_destroy_font(renderer->font);
    al_destroy_display(renderer->display);
}

void Render(Renderer* renderer, GameState current_state, Player player, Enemy inimigos[], 
            int carta_sel, int inimigo_sel, int selecionando_alvo) 
{
    al_clear_to_color(al_map_rgb(0, 0, 0));

    switch (current_state) {
        case GAME_STATE_PLAYER_TURN:
        case GAME_STATE_ENEMY_TURN:
            draw_combat_screen(renderer, player, inimigos, carta_sel, inimigo_sel, selecionando_alvo);
            break;

        case GAME_STATE_VICTORY:
            if (renderer->img_background) al_draw_bitmap(renderer->img_background, 0, 0, 0);
            al_draw_text(renderer->font, al_map_rgb(0, 255, 0), SCREEN_W / 2, SCREEN_H / 2, ALLEGRO_ALIGN_CENTER, "VITORIA SUPREMA!");
             al_draw_text(renderer->font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 + 40, ALLEGRO_ALIGN_CENTER, "Pressione Q");
            break;

        case GAME_STATE_GAME_OVER:
            al_clear_to_color(al_map_rgb(50, 0, 0));
            al_draw_text(renderer->font, al_map_rgb(255, 0, 0), SCREEN_W / 2, SCREEN_H / 2, ALLEGRO_ALIGN_CENTER, "GAME OVER");
             al_draw_text(renderer->font, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 + 40, ALLEGRO_ALIGN_CENTER, "Pressione Q");
            break;
            
        default:
            break;
    }
    al_flip_display();
}