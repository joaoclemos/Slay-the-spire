/*
 * main.c
 *
 * RESPONSABILIDADE: Controlar o jogo.
 * - Inicia o Allegro.
 * - Lê o teclado (setas, enter, esc).
 * - Controla a Máquina de Estados (Menu -> Jogo -> Vitória).
 * - Chama a lógica e o desenho 60 vezes por segundo.
 */

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "constants.h"
#include "renderer.h"
#include "utils.h"
#include "game_structs.h"
#include "setup.h"
#include "logic.h"

int main() {
    // 1. INICIALIZAÇÃO TÉCNICA (ALLEGRO)
    must_init(al_init(), "allegro");
    must_init(al_init_image_addon(), "image");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0); // Timer de 60 frames por segundo
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    
    // Carrega as imagens e fontes
    Renderer renderer;
    FillRenderer(&renderer);

    // Liga os eventos à fila
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_display_event_source(renderer.display));

    srand(time(NULL)); // Semente aleatória para sorteios diferentes

    // 2. VARIÁVEIS DE ESTADO (Onde guardamos o jogo)
    GameState current_state = GAME_STATE_MENU; // Começa no Menu
    Player player;
    Enemy inimigos[2];
    int selected_card = 0;
    int selected_enemy = 0;
    int mirando = 0; // 0 = escolhendo carta, 1 = escolhendo alvo
    int vitorias = 0;
    bool running = true;
    bool redraw = true;

    al_start_timer(timer);

    // 3. LOOP PRINCIPAL (Roda até fechar o jogo)
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event); // Espera algo acontecer

        // A. EVENTO DE TEMPO (60fps): Hora de redesenhar
        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
        } 
        // B. FECHAR JANELA (X)
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } 
        // C. TECLADO
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            int k = event.keyboard.keycode;
            if (k == ALLEGRO_KEY_Q) running = false; // Q sempre sai

            // -- Lógica no MENU --
            if (current_state == GAME_STATE_MENU) {
                if (k == ALLEGRO_KEY_ENTER) current_state = GAME_STATE_START;
                continue; // Pula o resto do loop
            }

            // -- Lógica no TURNO DO JOGADOR --
            if (current_state == GAME_STATE_PLAYER_TURN) {
                if (!mirando) {
                    // Navega nas cartas
                    if (k == ALLEGRO_KEY_LEFT && selected_card > 0) selected_card--;
                    if (k == ALLEGRO_KEY_RIGHT && selected_card < player.mao.num_cartas - 1) selected_card++;
                    
                    // Seleciona carta
                    if (k == ALLEGRO_KEY_ENTER) {
                        // Se for ataque/debuff, precisa mirar
                        if (player.mao.cartas[selected_card].tipo == ATAQUE || 
                            player.mao.cartas[selected_card].tipo == DEBUFF) {
                            mirando = 1; 
                            // Mira no primeiro inimigo vivo
                            selected_enemy = (inimigos[0].stats.hp_atual > 0) ? 0 : 1;
                        } else {
                            // Se for defesa/buff/especial, joga direto
                            play_card(&player, selected_card, NULL); 
                            if (selected_card >= player.mao.num_cartas) selected_card--;
                        }
                    }
                    // Passar turno
                    if (k == ALLEGRO_KEY_ESCAPE) current_state = GAME_STATE_ENEMY_TURN;
                    
                    // Cheat de DEBUG (Espaço mata inimigos)
                    if (k == ALLEGRO_KEY_SPACE) { inimigos[0].stats.hp_atual = 0; inimigos[1].stats.hp_atual = 0; }
                } else {
                    // MODO MIRA: Escolhe inimigo
                    if (k == ALLEGRO_KEY_LEFT || k == ALLEGRO_KEY_RIGHT) selected_enemy = !selected_enemy;
                    if (k == ALLEGRO_KEY_ESCAPE) mirando = 0; // Cancela
                    if (k == ALLEGRO_KEY_ENTER) {
                        // Confirma ataque no alvo
                        if (inimigos[selected_enemy].stats.hp_atual > 0) {
                            if (play_card(&player, selected_card, &inimigos[selected_enemy])) {
                                mirando = 0; 
                                if (selected_card >= player.mao.num_cartas) selected_card--;
                            }
                        }
                    }
                }
            }
        }

        if (!running) break;

        // --- MÁQUINA DE ESTADOS ---
        switch (current_state) {
            case GAME_STATE_MENU: break; // Espera
            
            case GAME_STATE_START: // Configuração Inicial
                player = setup_player();
                vitorias = 0;
                current_state = GAME_STATE_NEW_COMBAT;
                break;

            case GAME_STATE_NEW_COMBAT: // Novo Round
                // Desenha a tela antes de resetar (para ver os inimigos mortos)
                Render(&renderer, GAME_STATE_PLAYER_TURN, player, inimigos, -1, -1, 0);
                al_rest(1.0); // Espera 1 segundo
                printf("Iniciando combate %d...\n", vitorias + 1);
                
                if (vitorias == 10) { // Se venceu 10, vem o Boss
                    inimigos[0] = create_boss();
                    inimigos[1].stats.hp_atual = 0; // Só um inimigo (Boss)
                } else {
                    inimigos[0] = create_enemy();
                    inimigos[1] = create_enemy();
                }
                
                // Reseta baralho para o combate
                player.pilha_compra = player.baralho_completo;
                shuffle_pilha(&player.pilha_compra);
                player.mao.num_cartas = 0;
                player.pilha_descarte.num_cartas = 0;
                start_player_turn(&player);
                current_state = GAME_STATE_PLAYER_TURN;
                break;

            case GAME_STATE_PLAYER_TURN: // Vez do Jogador
                // Verifica vitória
                if (inimigos[0].stats.hp_atual <= 0 && inimigos[1].stats.hp_atual <= 0) {
                    vitorias++;
                    if (vitorias >= 11) current_state = GAME_STATE_VICTORY;
                    else current_state = GAME_STATE_NEW_COMBAT;
                }
                break;

            case GAME_STATE_ENEMY_TURN: // Vez do Inimigo
                Render(&renderer, current_state, player, inimigos, -1, -1, 0);
                al_rest(0.8); // Espera para dar tempo de ver
                
                // Zera escudos dos inimigos
                if(inimigos[0].stats.hp_atual > 0) inimigos[0].stats.escudo = 0;
                if(inimigos[1].stats.hp_atual > 0) inimigos[1].stats.escudo = 0;
                
                execute_enemy_turn(&player, inimigos);
                
                // Verifica derrota
                if (player.stats.hp_atual <= 0) current_state = GAME_STATE_GAME_OVER;
                else {
                    start_player_turn(&player);
                    current_state = GAME_STATE_PLAYER_TURN;
                }
                break;
            
            case GAME_STATE_VICTORY:
            case GAME_STATE_GAME_OVER: break; // Só espera sair
        }

        // --- RENDERIZAÇÃO ---
        if (redraw && al_is_event_queue_empty(queue)) {
            Render(&renderer, current_state, player, inimigos, selected_card, selected_enemy, mirando);
            redraw = false;
        }
    }

    ClearRenderer(&renderer); // Limpa memória
    return 0;
}