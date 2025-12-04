/*
 * renderer.c
 *
 * Responsável por desenhar tudo na tela.
 * VERSÃO FINAL: Inclui Menu Inicial, correção de alinhamento da Base,
 * e suporte a todos os ícones de Buff/Debuff e Custo.
 */

#include "renderer.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include "utils.h"

// --- DESENHA ÍCONES DE STATUS ---
void draw_status_icons(Renderer* r, Creature c, float x, float y) {
    float offset_x = 0;
    
    if (c.forca > 0) {
        al_draw_filled_circle(x + offset_x, y, 10, al_map_rgb(200, 50, 50));
        al_draw_textf(r->font, al_map_rgb(255,255,255), x + offset_x, y-5, ALLEGRO_ALIGN_CENTER, "F:%d", c.forca);
        offset_x += 25;
    }
    if (c.destreza > 0) {
        al_draw_filled_circle(x + offset_x, y, 10, al_map_rgb(50, 50, 200));
        al_draw_textf(r->font, al_map_rgb(255,255,255), x + offset_x, y-5, ALLEGRO_ALIGN_CENTER, "D:%d", c.destreza);
        offset_x += 25;
    }
    if (c.veneno > 0) {
        al_draw_filled_circle(x + offset_x, y, 10, al_map_rgb(50, 200, 50));
        al_draw_textf(r->font, al_map_rgb(0,0,0), x + offset_x, y-5, ALLEGRO_ALIGN_CENTER, "V:%d", c.veneno);
        offset_x += 25;
    }
    if (c.dormindo > 0) {
        al_draw_filled_circle(x + offset_x, y, 10, al_map_rgb(200, 200, 255));
        al_draw_textf(r->font, al_map_rgb(0,0,0), x + offset_x, y-5, ALLEGRO_ALIGN_CENTER, "Zz:%d", c.dormindo);
        offset_x += 25;
    }
    if (c.regeneracao > 0) {
        al_draw_filled_circle(x + offset_x, y, 10, al_map_rgb(255, 100, 200));
        al_draw_textf(r->font, al_map_rgb(0,0,0), x + offset_x, y-5, ALLEGRO_ALIGN_CENTER, "Rg:%d", c.regeneracao);
        offset_x += 25;
    }
}

// --- DESENHA CRIATURAS ---
void draw_creature(Renderer* renderer, Creature creature, float x, float y, const char* label) {
    ALLEGRO_BITMAP* img_char = NULL;
    float char_w = 0, char_h = 0;
    float base_w = 0, base_h = 0;
    
    // 1. Lógica de detecção (Boss, Forte, Fraco)
    if (strcmp(label, "Jogador") == 0) img_char = renderer->img_player;
    else if (creature.hp_max > 200) img_char = renderer->img_boss; 
    else if (creature.hp_max > 40) img_char = renderer->img_enemy_strong; 
    else img_char = renderer->img_enemy_weak; 

    if (img_char) {
        char_w = al_get_bitmap_width(img_char);
        char_h = al_get_bitmap_height(img_char);
    }
    if (renderer->img_base) {
        base_w = al_get_bitmap_width(renderer->img_base);
        base_h = al_get_bitmap_height(renderer->img_base);
    } else { base_h = 30; }

    // 2. BASE DE PEDRA
    if (renderer->img_base) al_draw_bitmap(renderer->img_base, x - base_w/2, y, 0);
    else al_draw_filled_ellipse(x, y + 40, 60, 20, al_map_rgb(100, 100, 100));

    // 3. PERSONAGEM (Ajustado +15px para baixo)
    if (img_char) al_draw_bitmap(img_char, x - char_w/2, y - char_h + 15, 0);
    else al_draw_filled_rectangle(x-30, y-100, x+30, y, al_map_rgb(150,150,150));

    // 4. INTERFACE
    al_draw_text(renderer->font, al_map_rgb(255,255,255), x, y - char_h - 25 + 15, ALLEGRO_ALIGN_CENTER, label);
    
    float hp_y_start = y + base_h + 15;
    float hp_y_end = hp_y_start + 24;

    al_draw_filled_rectangle(x-60, hp_y_start, x+60, hp_y_end, al_map_rgb(80,0,0)); 
    float pct = (float)creature.hp_atual / creature.hp_max;
    if(pct < 0) pct = 0; if(pct > 1) pct = 1;
    al_draw_filled_rectangle(x-60, hp_y_start, (x-60) + (120*pct), hp_y_end, al_map_rgb(0,220,0)); 
    al_draw_rectangle(x-60, hp_y_start, x+60, hp_y_end, al_map_rgb(200,200,200), 2);
    al_draw_textf(renderer->font, al_map_rgb(255,255,255), x, hp_y_start + 2, ALLEGRO_ALIGN_CENTER, "%d/%d", creature.hp_atual, creature.hp_max);

    if (creature.escudo > 0) {
        al_draw_filled_circle(x+85, hp_y_start + 12, 18, al_map_rgb(50,100,255));
        al_draw_circle(x+85, hp_y_start + 12, 18, al_map_rgb(200,200,255), 2);
        al_draw_textf(renderer->font, al_map_rgb(255,255,255), x+85, hp_y_start + 2, ALLEGRO_ALIGN_CENTER, "%d", creature.escudo);
    }
    draw_status_icons(renderer, creature, x - 40, hp_y_end + 20);
}

// --- DESENHA A CARTA ---
void draw_card(Renderer* r, Card c, float x, float y) {
    ALLEGRO_BITMAP* img_icone = NULL;
    ALLEGRO_COLOR cor_texto = al_map_rgb(0,0,0);
    char titulo[20];
    char descricao[30];

    // Lógica de seleção do ícone e texto
    switch (c.tipo) {
        case ATAQUE:
            if (c.is_vampiric) { img_icone = r->icon_vampiro; sprintf(titulo, "VAMPIRO"); sprintf(descricao, "%d DMG+CURA", c.efeito_valor); } 
            else { img_icone = r->img_ataque; sprintf(titulo, "ATAQUE"); sprintf(descricao, "%d DANO", c.efeito_valor); }
            break;
        case DEFESA:
            img_icone = r->img_defesa; sprintf(titulo, "DEFESA"); sprintf(descricao, "%d DEF", c.efeito_valor);
            break;
        case ESPECIAL:
            img_icone = r->img_especial; sprintf(titulo, "TROCA"); sprintf(descricao, "NOVA MAO");
            break;
        case BUFF:
            if (c.efeito_valor == ID_FORCA) { img_icone = r->icon_forca; sprintf(titulo, "FORCA"); sprintf(descricao, "+%d FORCA", c.magnitude); }
            else if (c.efeito_valor == ID_REGEN_RODADAS) { 
                img_icone = r->icon_supercura; sprintf(titulo, "SUPER CURA"); 
                if (c.custo_energia == -1) sprintf(descricao, "X TURNOS");
                else sprintf(descricao, "%d TURNOS", c.magnitude); 
            }
            else if (c.efeito_valor == ID_CURA_INSTANT) { 
                img_icone = r->icon_cura; sprintf(titulo, "CURA"); sprintf(descricao, "+%d VIDA", c.magnitude); 
            }
            else { // Destreza
                img_icone = r->icon_destreza; 
                sprintf(titulo, "DESTREZA"); sprintf(descricao, "+%d DEF+", c.magnitude); 
            }
            break;
        case DEBUFF:
            if (c.efeito_valor == ID_VENENO) { 
                img_icone = r->icon_veneno; sprintf(titulo, "VENENO"); sprintf(descricao, "%d VEN", c.magnitude); 
            }
            else if (c.efeito_valor == ID_SONO) { 
                img_icone = r->icon_sono; sprintf(titulo, "SONO"); sprintf(descricao, "%d TURNO", c.magnitude); 
            }
            else if (c.efeito_valor == ID_VULNERAVEL) { 
                img_icone = r->icon_vulneravel; sprintf(titulo, "VULNERAVEL"); sprintf(descricao, "50%% +DANO"); 
            }
            else { 
                img_icone = r->icon_fraqueza;
                sprintf(titulo, "FRAQUEZA"); sprintf(descricao, "25%% -ATK"); 
            }
            break;
    }

    if (r->img_verso) al_draw_bitmap(r->img_verso, x, y, 0);
    else al_draw_filled_rounded_rectangle(x, y, x+CARD_W, y+CARD_H, 8, 8, al_map_rgb(220,220,220));

    if (img_icone) {
        int iw = al_get_bitmap_width(img_icone);
        al_draw_bitmap(img_icone, x + (CARD_W - iw) / 2.0, y + 20, 0);
    }

    float cx = x + 22; float cy = y + 22; 
    float radius = 11; 

    al_draw_filled_circle(cx + 2, cy + 2, radius, al_map_rgba(0, 0, 0, 100));
    al_draw_filled_circle(cx, cy, radius, al_map_rgb(218, 165, 32)); 
    al_draw_filled_circle(cx, cy, radius - 4, al_map_rgb(255, 255, 255)); 
    al_draw_circle(cx, cy, radius - 4, al_map_rgb(139, 69, 19), 2); 

    char custo[5]; 
    if (c.custo_energia == -1) sprintf(custo, "X");
    else sprintf(custo, "%d", c.custo_energia);
    
    al_draw_text(r->font, al_map_rgb(0,0,0), cx, cy - 5, ALLEGRO_ALIGN_CENTER, custo);

    al_draw_text(r->font, cor_texto, x + CARD_W/2, y + 90, ALLEGRO_ALIGN_CENTER, titulo);
    al_draw_text(r->font, cor_texto, x + CARD_W/2, y + 115, ALLEGRO_ALIGN_CENTER, descricao);
}

// --- DESENHA MONTES DE CARTAS ---
void draw_deck_pile(Renderer* r, int x, int y, int qtd, const char* label) {
    if (qtd > 0) {
        if (r->img_verso) {
            al_draw_bitmap(r->img_verso, x, y, 0);
            if (qtd > 1) al_draw_bitmap(r->img_verso, x-2, y-2, 0);
            if (qtd > 5) al_draw_bitmap(r->img_verso, x-4, y-4, 0);
        } else {
            al_draw_filled_rounded_rectangle(x, y, x+CARD_W, y+CARD_H, 8, 8, al_map_rgb(100,50,50));
        }
        al_draw_textf(r->font, al_map_rgb(255,255,255), x + CARD_W/2, y + CARD_H/2, ALLEGRO_ALIGN_CENTER, "%d", qtd);
    } else {
        al_draw_rounded_rectangle(x, y, x+CARD_W, y+CARD_H, 5, 5, al_map_rgb(80,80,80), 2);
    }
    al_draw_text(r->font, al_map_rgb(200,200,200), x + CARD_W/2, y + CARD_H + 10, ALLEGRO_ALIGN_CENTER, label);
}

// --- DESENHA TELA DE MENU ---
void draw_menu_screen(Renderer* r) {
    if (r->img_background) {
        al_draw_bitmap(r->img_background, 0, 0, 0);
    } else {
        al_clear_to_color(al_map_rgb(20, 20, 40)); 
    }

    al_draw_text(r->font, al_map_rgb(255, 255, 0), SCREEN_W / 2, SCREEN_H / 4, ALLEGRO_ALIGN_CENTER, "TRABALHO PRATICO PDS1");
    al_draw_text(r->font, al_map_rgb(200, 200, 255), SCREEN_W / 2, SCREEN_H / 2, ALLEGRO_ALIGN_CENTER, "PRESSIONE ENTER PARA INICIAR A ESCALADA");
    al_draw_text(r->font, al_map_rgb(150, 150, 150), SCREEN_W / 2, SCREEN_H * 0.75, ALLEGRO_ALIGN_CENTER, "Q: Sair | ESC: Encerrar Turno");
}

// --- INICIALIZAÇÃO E LIMPEZA ---

void FillRenderer(Renderer* r) {
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    r->display = al_create_display(SCREEN_W, SCREEN_H);
    must_init(r->display, "display");
    
    must_init(al_init_font_addon(), "font");
    must_init(al_init_ttf_addon(), "ttf");
    must_init(al_init_image_addon(), "image");

    r->font = al_load_font("font.ttf", 20, 0); 
    if (!r->font) r->font = al_create_builtin_font();

    // --- CARREGAMENTO DAS IMAGENS --- (Usando fotos/path)
    r->img_player       = al_load_bitmap("fotos/player.png");
    r->img_enemy_weak   = al_load_bitmap("fotos/enemy_weak.png");
    r->img_enemy_strong = al_load_bitmap("fotos/enemy_strong.png");
    r->img_boss         = al_load_bitmap("fotos/boss.png");
    r->img_base         = al_load_bitmap("fotos/base.png");
    r->img_background   = al_load_bitmap("fotos/background.png");

    r->img_verso    = al_load_bitmap("fotos/verso.png"); 
    r->img_ataque   = al_load_bitmap("fotos/ataque.png");
    r->img_defesa   = al_load_bitmap("fotos/defesa.png");
    r->img_especial = al_load_bitmap("fotos/especial.png"); 
    // Removidas as referências a img_buff/debuff genéricos que causavam erro
    
    // Ícones
    r->icon_forca      = al_load_bitmap("fotos/forca.png");
    r->icon_vulneravel = al_load_bitmap("fotos/vulneravel.png");
    r->icon_veneno     = al_load_bitmap("fotos/veneno.png");
    r->icon_cura       = al_load_bitmap("fotos/cura.png");
    r->icon_supercura  = al_load_bitmap("fotos/supercura.png");
    r->icon_troca      = al_load_bitmap("fotos/trocar.png"); 
    r->icon_sono       = al_load_bitmap("fotos/sono.png");
    r->icon_fraqueza   = al_load_bitmap("fotos/fraqueza.png"); 
    r->icon_vampiro    = al_load_bitmap("fotos/vampiro.png"); 
    r->icon_destreza   = al_load_bitmap("fotos/destreza.png"); 

    if (!r->img_player) printf("AVISO: player.png nao encontrado.\n");
    if (!r->img_especial) printf("AVISO: especial.png nao encontrado.\n");
}

void ClearRenderer(Renderer* r) {
    if(r->img_player) al_destroy_bitmap(r->img_player);
    if(r->img_enemy_weak) al_destroy_bitmap(r->img_enemy_weak);
    if(r->img_enemy_strong) al_destroy_bitmap(r->img_enemy_strong);
    if(r->img_background) al_destroy_bitmap(r->img_background);
    if(r->img_base) al_destroy_bitmap(r->img_base);
    if(r->img_boss) al_destroy_bitmap(r->img_boss);

    if(r->img_verso) al_destroy_bitmap(r->img_verso);
    if(r->img_ataque) al_destroy_bitmap(r->img_ataque);
    if(r->img_defesa) al_destroy_bitmap(r->img_defesa);
    if(r->img_especial) al_destroy_bitmap(r->img_especial);

    if(r->icon_forca) al_destroy_bitmap(r->icon_forca);
    if(r->icon_vulneravel) al_destroy_bitmap(r->icon_vulneravel);
    if(r->icon_veneno) al_destroy_bitmap(r->icon_veneno);
    if(r->icon_cura) al_destroy_bitmap(r->icon_cura);
    if(r->icon_supercura) al_destroy_bitmap(r->icon_supercura);
    if(r->icon_troca) al_destroy_bitmap(r->icon_troca);
    if(r->icon_sono) al_destroy_bitmap(r->icon_sono);
    if(r->icon_fraqueza) al_destroy_bitmap(r->icon_fraqueza);
    if(r->icon_vampiro) al_destroy_bitmap(r->icon_vampiro);
    if(r->icon_destreza) al_destroy_bitmap(r->icon_destreza);

    al_destroy_font(r->font);
    al_destroy_display(r->display);
}

void Render(Renderer* r, GameState state, Player p, Enemy inimigos[], int sel_c, int sel_e, int mirando) {
    al_clear_to_color(al_map_rgb(20,20,20)); 

    if (state == GAME_STATE_VICTORY) {
        if (r->img_background) al_draw_bitmap(r->img_background, 0, 0, 0);
        al_draw_text(r->font, al_map_rgb(0,255,0), SCREEN_W/2, SCREEN_H/2, ALLEGRO_ALIGN_CENTER, "VITORIA SUPREMA!");
        al_draw_text(r->font, al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2 + 40, ALLEGRO_ALIGN_CENTER, "Pressione Q");
    } 
    else if (state == GAME_STATE_GAME_OVER) {
        al_clear_to_color(al_map_rgb(50, 0, 0));
        al_draw_text(r->font, al_map_rgb(255,0,0), SCREEN_W/2, SCREEN_H/2, ALLEGRO_ALIGN_CENTER, "GAME OVER");
        al_draw_text(r->font, al_map_rgb(255,255,255), SCREEN_W/2, SCREEN_H/2 + 40, ALLEGRO_ALIGN_CENTER, "Pressione Q");
    }
    else if (state == GAME_STATE_MENU) { // NOVO CASE
        draw_menu_screen(r);
    }
    else {
        if (r->img_background) al_draw_bitmap(r->img_background, 0, 0, 0);

        draw_creature(r, p.stats, PLAYER_X, PLAYER_Y, "Jogador");
        for(int i=0; i<2; i++) {
            if (inimigos[i].stats.hp_atual > 0) {
                float ex = (i==0) ? ENEMY_1_X : ENEMY_2_X;
                draw_creature(r, inimigos[i].stats, ex, 400, "Inimigo");
                
                if (inimigos[i].stats.dormindo > 0) {
                    al_draw_text(r->font, al_map_rgb(200,200,255), ex, 250, ALLEGRO_ALIGN_CENTER, "Zzz...");
                } else {
                    AI_Action acao = inimigos[i].ia_ciclo[inimigos[i].acao_ia_atual];
                    char txt[30];
                    sprintf(txt, "%s %d", (acao.tipo_acao==ATAQUE ? "ATK" : "DEF"), acao.valor_efeito);
                    al_draw_text(r->font, al_map_rgb(255,255,255), ex, 250, ALLEGRO_ALIGN_CENTER, txt);
                }

                if (mirando && sel_e == i) {
                    al_draw_rectangle(ex-70, 220, ex+70, 480, al_map_rgb(255,0,0), 3);
                }
            }
        }

        draw_deck_pile(r, DECK_X, DECK_Y, p.pilha_compra.num_cartas, "COMPRA");
        draw_deck_pile(r, DISCARD_X, DISCARD_Y, p.pilha_descarte.num_cartas, "DESCARTE");

        for(int i=0; i<p.mao.num_cartas; i++) {
            float y = HAND_Y;
            if (i == sel_c) {
                y -= 30; 
                al_draw_rounded_rectangle(HAND_X + (i*130) - 2, y - 2, HAND_X + (i*130) + CARD_W + 2, y + CARD_H + 2, 10, 10, al_map_rgb(255,255,0), 3);
            }
            draw_card(r, p.mao.cartas[i], HAND_X + (i*130), y);
        }
        
        char en[20];
        sprintf(en, "Energia: %d/%d", p.energia_atual, p.energia_max);
        al_draw_text(r->font, al_map_rgb(255,255,0), 20, 20, 0, en);
    }
    al_flip_display();
}