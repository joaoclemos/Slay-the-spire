#include "renderer.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>

// Função interna para desenhar uma criatura (Herói ou Inimigo)
void draw_creature(Renderer* renderer, Creature creature, float x, float y, const char* label) {
    ALLEGRO_BITMAP* img = NULL;
    
    // Decide qual imagem usar
    if (strcmp(label, "Jogador") == 0) img = renderer->img_player;
    else if (creature.hp_max > 35) img = renderer->img_enemy_strong; // Se tiver muita vida, é Forte
    else img = renderer->img_enemy_weak; // Se tiver pouca vida, é Fraco

    // Desenha a imagem (Sprite)
    float altura = 100;
    if (img) {
        altura = al_get_bitmap_height(img);
        // Desenha centralizado
        al_draw_bitmap(img, x - (al_get_bitmap_width(img)/2), y - altura, 0);
    } else {
        // Se a imagem falhou, desenha um quadrado cinza
        al_draw_filled_rectangle(x-30, y-100, x+30, y, al_map_rgb(150,150,150));
    }

    // Desenha o Nome
    al_draw_text(renderer->font, al_map_rgb(255,255,255), x, y - altura - 25, ALLEGRO_ALIGN_CENTER, label);
    
    // Barra de Vida (Vermelha fundo, Verde frente)
    al_draw_filled_rectangle(x-50, y+10, x+50, y+30, al_map_rgb(255,0,0)); 
    float pct = (float)creature.hp_atual / creature.hp_max; // Calcula porcentagem
    if(pct < 0) pct = 0;
    al_draw_filled_rectangle(x-50, y+10, (x-50) + (100*pct), y+30, al_map_rgb(0,255,0)); 
    
    // Texto da Vida (ex: 100/100)
    char hp[20];
    sprintf(hp, "%d/%d", creature.hp_atual, creature.hp_max);
    al_draw_text(renderer->font, al_map_rgb(255,255,255), x, y+12, ALLEGRO_ALIGN_CENTER, hp);

    // Texto do Escudo (Azul)
    if (creature.escudo > 0) {
        sprintf(hp, "[%d]", creature.escudo);
        al_draw_text(renderer->font, al_map_rgb(0,200,255), x+60, y+12, ALLEGRO_ALIGN_LEFT, hp);
    }
}

// Função interna para desenhar uma Carta
void draw_card(Renderer* r, Card c, float x, float y) {
    // Desenha o fundo da carta
    if (r->img_card) al_draw_bitmap(r->img_card, x, y, 0);
    else {
        al_draw_filled_rectangle(x, y, x+CARD_W, y+CARD_H, al_map_rgb(50,50,100)); // Azul se não tiver imagem
        al_draw_rectangle(x, y, x+CARD_W, y+CARD_H, al_map_rgb(255,255,255), 2); // Borda branca
    }

    // Custo de Energia
    char txt[30];
    sprintf(txt, "%d", c.custo_energia);
    al_draw_text(r->font, al_map_rgb(255,255,0), x+20, y+10, ALLEGRO_ALIGN_CENTER, txt);

    // Texto descritivo (Ataque, Defesa, Valor)
    if (c.tipo == ATAQUE) {
        al_draw_text(r->font, al_map_rgb(255,100,100), x+60, y+40, ALLEGRO_ALIGN_CENTER, "ATAQUE");
        sprintf(txt, "%d DANO", c.efeito_valor);
    } else if (c.tipo == DEFESA) {
        al_draw_text(r->font, al_map_rgb(100,100,255), x+60, y+40, ALLEGRO_ALIGN_CENTER, "DEFESA");
        sprintf(txt, "%d DEF", c.efeito_valor);
    } else {
        al_draw_text(r->font, al_map_rgb(200,100,200), x+60, y+40, ALLEGRO_ALIGN_CENTER, "ESPECIAL");
        sprintf(txt, "COMPRA 5");
    }
    al_draw_text(r->font, al_map_rgb(255,255,255), x+60, y+80, ALLEGRO_ALIGN_CENTER, txt);
}

// Configura a tela e carrega os arquivos
void FillRenderer(Renderer* r) {
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    r->display = al_create_display(SCREEN_W, SCREEN_H);
    must_init(r->display, "display");
    
    // Inicia plugins do Allegro
    must_init(al_init_font_addon(), "font");
    must_init(al_init_ttf_addon(), "ttf");
    must_init(al_init_image_addon(), "image");

    // Tenta carregar a fonte 'font.ttf', senão usa a padrão
    r->font = al_load_font("font.ttf", 24, 0);
    if (!r->font) r->font = al_create_builtin_font();

    // Carrega as imagens. Se não achar, não trava o jogo, só avisa.
    r->img_player = al_load_bitmap("player.png");
    r->img_enemy_weak = al_load_bitmap("enemy_weak.png");
    r->img_enemy_strong = al_load_bitmap("enemy_strong.png");
    r->img_card = al_load_bitmap("card.png");
    r->img_background = al_load_bitmap("background.png");
}

// Limpa memória
void ClearRenderer(Renderer* r) {
    if(r->img_player) al_destroy_bitmap(r->img_player);
    // ... destroi as outras imagens ...
    al_destroy_font(r->font);
    al_destroy_display(r->display);
}

// FUNÇÃO PRINCIPAL: É chamada no loop do jogo para desenhar tudo
void Render(Renderer* r, GameState state, Player p, Enemy inimigos[], int sel_c, int sel_e, int mirando) {
    al_clear_to_color(al_map_rgb(0,0,0)); // Limpa a tela com preto

    // 1. Tela de Vitória
    if (state == GAME_STATE_VICTORY) {
        al_draw_text(r->font, al_map_rgb(0,255,0), SCREEN_W/2, SCREEN_H/2, ALLEGRO_ALIGN_CENTER, "VITORIA!");
    } 
    // 2. Tela de Derrota
    else if (state == GAME_STATE_GAME_OVER) {
        al_draw_text(r->font, al_map_rgb(255,0,0), SCREEN_W/2, SCREEN_H/2, ALLEGRO_ALIGN_CENTER, "GAME OVER");
    }
    // 3. Jogo Rodando
    else {
        // Desenha Fundo
        if (r->img_background) al_draw_bitmap(r->img_background, 0, 0, 0);

        // Desenha Jogador
        draw_creature(r, p.stats, PLAYER_X, PLAYER_Y, "Jogador");

        // Desenha Inimigos
        for(int i=0; i<2; i++) {
            if (inimigos[i].stats.hp_atual > 0) { // Só desenha se estiver vivo
                float ex = (i==0) ? ENEMY_1_X : ENEMY_2_X;
                draw_creature(r, inimigos[i].stats, ex, 400, "Inimigo");
                
                // Mostra a Intenção (o que ele vai fazer)
                AI_Action acao = inimigos[i].ia_ciclo[inimigos[i].acao_ia_atual];
                char txt[30];
                sprintf(txt, "%s %d", (acao.tipo_acao==ATAQUE ? "ATK" : "DEF"), acao.valor_efeito);
                al_draw_text(r->font, al_map_rgb(255,255,255), ex, 250, ALLEGRO_ALIGN_CENTER, txt);

                // Desenha a Mira Vermelha se o jogador estiver escolhendo alvo
                if (mirando && sel_e == i) {
                    al_draw_rectangle(ex-40, 290, ex+40, 440, al_map_rgb(255,0,0), 3);
                }
            }
        }

        // Desenha as Cartas na Mão
        for(int i=0; i<p.mao.num_cartas; i++) {
            float y = HAND_Y;
            if (i == sel_c) y -= 30; // Levanta a carta que está selecionada
            draw_card(r, p.mao.cartas[i], HAND_X + (i*130), y);
        }
        
        // Desenha Energia
        char en[20];
        sprintf(en, "Energia: %d/%d", p.energia_atual, p.energia_max);
        al_draw_text(r->font, al_map_rgb(255,255,0), 20, 20, 0, en);
    }
    
    // "Vira" a página para mostrar o desenho novo
    al_flip_display();
}