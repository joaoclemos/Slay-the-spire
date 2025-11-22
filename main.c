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
    // --- 1. INICIALIZAÇÃO DO ALLEGRO ---
    must_init(al_init(), "allegro");
    must_init(al_init_image_addon(), "allegro");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0); // 60 FPS
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    // --- 2. INICIALIZAÇÃO DO JOGO ---

    srand(time(NULL)); // Inicializa números aleatórios

    // Variáveis de controle do Allegro
    unsigned char keyboard_keys[ALLEGRO_KEY_MAX];
    ClearKeyboardKeys(keyboard_keys);
    ALLEGRO_EVENT event;

    // Inicializa o display (definido em renderer.c)
    Renderer renderer;
    FillRenderer(&renderer);

    // Registra as fontes de eventos
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_display_event_source(renderer.display));

    // --- 3. VARIÁVEIS DE ESTADO DO JOGO ---

    GameState current_state = GAME_STATE_START;
    
    Player player;
    Enemy inimigos[2];
    
    // Variáveis de Seleção e Controle
    int selected_card_index = 0;
    int selected_enemy_index = 0;
    int is_selecting_target = 0; // 0 = false, 1 = true
    int combates_vencidos = 0;

    bool running = true;
    bool redraw = true; 

    // --- 4. O LOOP PRINCIPAL DO JOGO ---
    al_start_timer(timer);

    while (running) {
        
        // --- 4A. PROCESSAMENTO DE EVENTOS (Input) ---
        al_wait_for_event(queue, &event);

        switch (event.type) {
            case ALLEGRO_EVENT_TIMER:
                redraw = true;
                
                // Tecla Q para sair
                if (keyboard_keys[ALLEGRO_KEY_Q]) {
                     running = false;
                }
                
                // Limpa o estado "visto" das teclas
                for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
                    keyboard_keys[i] &= ~GAME_KEY_SEEN;
                }
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                keyboard_keys[event.keyboard.keycode] = GAME_KEY_SEEN | GAME_KEY_DOWN;
                int key = event.keyboard.keycode;

                if (current_state == GAME_STATE_PLAYER_TURN) {
                    
                    // --- MODO DE SELEÇÃO DE CARTA ---
                    if (!is_selecting_target) {
                        
                        if (key == ALLEGRO_KEY_LEFT) {
                            if (selected_card_index > 0) selected_card_index--;
                        }
                        
                        if (key == ALLEGRO_KEY_RIGHT) {
                            if (selected_card_index < player.mao.num_cartas - 1) selected_card_index++;
                        }

                        // Confirmar Carta (ENTER)
                        if (key == ALLEGRO_KEY_ENTER) {
                            if (player.mao.num_cartas > 0) {
                                Card carta = player.mao.cartas[selected_card_index];

                                if (carta.tipo == ATAQUE) {
                                    // Entra no modo mira
                                    is_selecting_target = 1;
                                    selected_enemy_index = 0;
                                    // Se o inimigo 0 estiver morto, mira no 1
                                    if (inimigos[0].stats.hp_atual <= 0) selected_enemy_index = 1;
                                } else {
                                    // Defesa ou Especial
                                    int sucesso = play_card(&player, selected_card_index, NULL);
                                    if (sucesso) {
                                        if (selected_card_index >= player.mao.num_cartas) selected_card_index = player.mao.num_cartas - 1;
                                        if (selected_card_index < 0) selected_card_index = 0;
                                    }
                                }
                            }
                        }
                    } 
                    // --- MODO DE SELEÇÃO DE ALVO (MIRA) ---
                    else {
                        if (key == ALLEGRO_KEY_LEFT || key == ALLEGRO_KEY_RIGHT) {
                            selected_enemy_index = !selected_enemy_index;
                        }

                        if (key == ALLEGRO_KEY_ESCAPE) {
                            is_selecting_target = 0;
                        }

                        // Confirmar Ataque (ENTER)
                        if (key == ALLEGRO_KEY_ENTER) {
                            if (inimigos[selected_enemy_index].stats.hp_atual > 0) {
                                int sucesso = play_card(&player, selected_card_index, &inimigos[selected_enemy_index]);
                                if (sucesso) {
                                    is_selecting_target = 0;
                                    if (selected_card_index >= player.mao.num_cartas) selected_card_index = player.mao.num_cartas - 1;
                                    if (selected_card_index < 0) selected_card_index = 0;
                                }
                            }
                        }
                    }

                    // Encerrar Turno (ESC fora do modo de mira)
                    if (!is_selecting_target && key == ALLEGRO_KEY_ESCAPE) {
                        printf("Turno do jogador encerrado!\n");
                        current_state = GAME_STATE_ENEMY_TURN;
                    }
                    
                    // Atalhos de Debug
                    if (key == ALLEGRO_KEY_X) player.stats.hp_atual = 1;
                    if (key == ALLEGRO_KEY_SPACE) { inimigos[0].stats.hp_atual = 0; inimigos[1].stats.hp_atual = 0; }
                }
                break;

            case ALLEGRO_EVENT_KEY_UP:
                keyboard_keys[event.keyboard.keycode] &= ~GAME_KEY_DOWN;
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                running = false;
                break;
        }

        if (!running) break;

        // --- 4B. PROCESSAMENTO DA LÓGICA DO JOGO (Máquina de Estados) ---
        
        switch (current_state) {
            
            case GAME_STATE_START:
                printf("Carregando jogo... Criando jogador...\n");
                player = setup_player();
                combates_vencidos = 0;
                current_state = GAME_STATE_NEW_COMBAT;
                break;

            case GAME_STATE_NEW_COMBAT:
                printf("Iniciando combate %d...\n", combates_vencidos + 1);

                // 1. Cria inimigos
                inimigos[0] = create_enemy();
                inimigos[1] = create_enemy();

                // 2. Prepara o baralho
                player.pilha_compra = player.baralho_completo;
                shuffle_pilha(&player.pilha_compra);
                player.mao.num_cartas = 0;
                player.pilha_descarte.num_cartas = 0;

                // 3. Inicia o turno
                start_player_turn(&player);
                current_state = GAME_STATE_PLAYER_TURN; 
                break;

            case GAME_STATE_PLAYER_TURN:
                // Verifica VITORIA (se todos inimigos morreram)
                 if (inimigos[0].stats.hp_atual <= 0 && inimigos[1].stats.hp_atual <= 0) {
                     // Animação ou pausa poderia vir aqui
                     al_rest(0.5);
                     
                     combates_vencidos++;
                     printf("Combate vencido! Total de vitorias: %d\n", combates_vencidos);
                     
                     if (combates_vencidos >= 10) {
                         current_state = GAME_STATE_VICTORY;
                     } else {
                         current_state = GAME_STATE_NEW_COMBAT;
                     }
                 }
                break;

            case GAME_STATE_ENEMY_TURN:
                al_rest(0.5); 

                // 1. Remove escudo dos inimigos
                if (inimigos[0].stats.hp_atual > 0) inimigos[0].stats.escudo = 0;
                if (inimigos[1].stats.hp_atual > 0) inimigos[1].stats.escudo = 0;

                // 2. Inimigos agem
                execute_enemy_turn(&player, inimigos);

                // 3. Verifica se jogador morreu
                if (player.stats.hp_atual <= 0) {
                    current_state = GAME_STATE_GAME_OVER;
                } else {
                    start_player_turn(&player);
                    current_state = GAME_STATE_PLAYER_TURN;
                }
                break;

            case GAME_STATE_VICTORY:
                // Apenas espera (Render desenha a tela de vitoria)
                break;

            case GAME_STATE_GAME_OVER:
                // Apenas espera (Render desenha a tela de game over)
                break;
        }

        // --- 4C. RENDERIZAÇÃO ---
        if (redraw && al_is_event_queue_empty(queue)) {
            Render(&renderer, current_state, player, inimigos, 
                   selected_card_index, selected_enemy_index, is_selecting_target);
            redraw = false;
        }
    }

    // --- 5. LIMPEZA E FINALIZAÇÃO ---
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    ClearRenderer(&renderer);
    
    return 0;
}