/*
 * main.c
 *
 * Ponto de entrada principal do jogo.
 * Controla o loop de jogo, a máquina de estados e o loop de eventos.
 */

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

// --- NOSSOS ARQUIVOS DE CABEÇALHO ---
#include "constants.h"
#include "renderer.h"
#include "utils.h"
#include "game_structs.h"
#include "setup.h"
#include "logic.h"

int main() {
    // --- 1. INICIALIZAÇÃO TÉCNICA ---
    must_init(al_init(), "allegro");
    must_init(al_init_image_addon(), "image");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0); // 60 FPS
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    // Inicializa o display
    Renderer renderer;
    FillRenderer(&renderer); // Carrega janelas e imagens

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_display_event_source(renderer.display));

    srand(time(NULL)); // Inicializa sorteio aleatório

    // 2. VARIÁVEIS DE CONTROLE
    // O jogo começa no MENU
    GameState current_state = GAME_STATE_MENU; 
    
    Player player;
    Enemy inimigos[2];
    
    // Variáveis de Seleção e Controle (Inicializadas)
    int selected_card = 0;
    int selected_enemy = 0;
    int mirando = 0;      
    int vitorias = 0;

    bool running = true;  
    bool redraw = true;   

    al_start_timer(timer);

    // 3. LOOP PRINCIPAL DO JOGO
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        // A. EVENTO DE TEMPO
        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
        } 
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } 
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            int k = event.keyboard.keycode;
            
            if (k == ALLEGRO_KEY_Q) running = false; // Q para sair (Sempre funciona)
            
            // --- LÓGICA DO MENU ---
            if (current_state == GAME_STATE_MENU) {
                if (k == ALLEGRO_KEY_ENTER) {
                    current_state = GAME_STATE_START; // Começa o setup do jogo
                }
                // Sai do switch KEY_DOWN para não processar comandos de combate
                continue; 
            }

            // --- LÓGICA DE COMBATE E INPUT (Turno do Jogador) ---
            if (current_state == GAME_STATE_PLAYER_TURN) {
                if (!mirando) {
                    if (k == ALLEGRO_KEY_LEFT && selected_card > 0) selected_card--;
                    if (k == ALLEGRO_KEY_RIGHT && selected_card < player.mao.num_cartas - 1) selected_card++;
                    
                    if (k == ALLEGRO_KEY_ENTER) {
                        if (player.mao.cartas[selected_card].tipo == ATAQUE || 
                            player.mao.cartas[selected_card].tipo == DEBUFF) {
                            mirando = 1; 
                            selected_enemy = (inimigos[0].stats.hp_atual > 0) ? 0 : 1;
                        } else {
                            play_card(&player, selected_card, NULL); 
                            if (selected_card >= player.mao.num_cartas) selected_card--;
                        }
                    }
                    if (k == ALLEGRO_KEY_ESCAPE) current_state = GAME_STATE_ENEMY_TURN;

                    // DEBUG KEYS
                    if (k == ALLEGRO_KEY_SPACE) { inimigos[0].stats.hp_atual = 0; inimigos[1].stats.hp_atual = 0; }

                } else { // Mirando
                    if (k == ALLEGRO_KEY_LEFT || k == ALLEGRO_KEY_RIGHT) selected_enemy = !selected_enemy;
                    if (k == ALLEGRO_KEY_ESCAPE) mirando = 0; 
                    if (k == ALLEGRO_KEY_ENTER) {
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

        // --- LÓGICA DE ESTADOS (O QUE O JOGO FAZ AGORA) ---
        switch (current_state) {
            
            case GAME_STATE_MENU:
                // Aguarda ENTER para mudar para START
                break;
            
            case GAME_STATE_START:
                printf("Carregando jogo... Criando jogador...\n");
                // Inicializa o jogador (só uma vez)
                player = setup_player();
                vitorias = 0;
                current_state = GAME_STATE_NEW_COMBAT;
                break;

            case GAME_STATE_NEW_COMBAT:
                // TRUQUE VISUAL: Desenha a tela com os inimigos mortos antes de recriar
                Render(&renderer, GAME_STATE_PLAYER_TURN, player, inimigos, -1, -1, 0);
                al_rest(1.0); // Espera 1 segundo

                printf("Iniciando combate %d...\n", vitorias + 1);
                
                // LÓGICA DO BOSS (11ª RODADA)
                if (vitorias == 10) {
                    inimigos[0] = create_boss();
                    inimigos[1].stats.hp_atual = 0; // Inimigo 2 morto
                } else {
                    inimigos[0] = create_enemy();
                    inimigos[1] = create_enemy();
                }

                // Setup do Turno
                player.pilha_compra = player.baralho_completo;
                shuffle_pilha(&player.pilha_compra);
                player.mao.num_cartas = 0;
                player.pilha_descarte.num_cartas = 0;
                start_player_turn(&player);
                current_state = GAME_STATE_PLAYER_TURN;
                break;

            case GAME_STATE_PLAYER_TURN:
                // Verifica VITORIA do combate
                if (inimigos[0].stats.hp_atual <= 0 && inimigos[1].stats.hp_atual <= 0) {
                    vitorias++;
                    if (vitorias >= 11) current_state = GAME_STATE_VICTORY;
                    else current_state = GAME_STATE_NEW_COMBAT;
                }
                break;

            case GAME_STATE_ENEMY_TURN:
                Render(&renderer, current_state, player, inimigos, -1, -1, 0);
                al_rest(0.8);
                
                // Zera escudos, executa IA
                if(inimigos[0].stats.hp_atual > 0) inimigos[0].stats.escudo = 0;
                if(inimigos[1].stats.hp_atual > 0) inimigos[1].stats.escudo = 0;
                execute_enemy_turn(&player, inimigos);
                
                if (player.stats.hp_atual <= 0) current_state = GAME_STATE_GAME_OVER;
                else {
                    start_player_turn(&player);
                    current_state = GAME_STATE_PLAYER_TURN;
                }
                break;
                
            case GAME_STATE_VICTORY:
            case GAME_STATE_GAME_OVER:
                // Apenas espera o jogador apertar Q
                break;
        }

        // --- 4C. RENDERIZAÇÃO ---
        if (redraw && al_is_event_queue_empty(queue)) {
            Render(&renderer, current_state, player, inimigos, selected_card, selected_enemy, mirando);
            redraw = false;
        }
    }

    ClearRenderer(&renderer);
    return 0;
}