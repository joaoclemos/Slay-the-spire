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
    // 1. INICIALIZAÇÃO TÉCNICA
    must_init(al_init(), "allegro");
    must_init(al_init_image_addon(), "image");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0); // 60 FPS
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    Renderer renderer;
    FillRenderer(&renderer); // Carrega janelas e imagens

    // Registra os eventos (teclado, tempo, botão X da janela)
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_display_event_source(renderer.display));

    srand(time(NULL)); // Inicializa sorteio aleatório

    // 2. VARIÁVEIS DE CONTROLE
    GameState current_state = GAME_STATE_START;
    Player player;
    Enemy inimigos[2];
    int selected_card = 0;
    int selected_enemy = 0;
    int mirando = 0;      // 0 = não, 1 = sim (está escolhendo alvo)
    int vitorias = 0;
    bool running = true;  // Jogo está rodando?
    bool redraw = true;   // Precisa desenhar a tela?

    al_start_timer(timer);

    // 3. LOOP PRINCIPAL DO JOGO
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event); // Espera algo acontecer

        // A. EVENTO DE TEMPO (60 vezes por segundo)
        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
        } 
        // B. EVENTO DE FECHAR JANELA (Clicou no X)
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } 
        // C. EVENTO DE TECLADO (Apertou uma tecla)
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            int k = event.keyboard.keycode;
            
            if (k == ALLEGRO_KEY_Q) running = false; // Q para sair

            // Se for turno do jogador...
            if (current_state == GAME_STATE_PLAYER_TURN) {
                if (!mirando) {
                    // SELEÇÃO DE CARTAS
                    if (k == ALLEGRO_KEY_LEFT && selected_card > 0) selected_card--;
                    if (k == ALLEGRO_KEY_RIGHT && selected_card < player.mao.num_cartas - 1) selected_card++;
                    
                    // JOGAR CARTA (ENTER)
                    if (k == ALLEGRO_KEY_ENTER) {
                        // Se for ataque, entra no modo "mirando"
                        if (player.mao.cartas[selected_card].tipo == ATAQUE) {
                            mirando = 1; 
                            // Mira automaticamente no primeiro inimigo vivo
                            selected_enemy = (inimigos[0].stats.hp_atual > 0) ? 0 : 1;
                        } else {
                            // Se for defesa/especial, joga direto
                            play_card(&player, selected_card, NULL); 
                            if (selected_card >= player.mao.num_cartas) selected_card--;
                        }
                    }
                    // PASSAR TURNO (ESC)
                    if (k == ALLEGRO_KEY_ESCAPE) current_state = GAME_STATE_ENEMY_TURN;

                } else { // SE ESTIVER MIRANDO
                    // Troca de inimigo
                    if (k == ALLEGRO_KEY_LEFT || k == ALLEGRO_KEY_RIGHT) selected_enemy = !selected_enemy;
                    // Cancela mira
                    if (k == ALLEGRO_KEY_ESCAPE) mirando = 0; 
                    // Confirma ataque
                    if (k == ALLEGRO_KEY_ENTER) {
                        if (inimigos[selected_enemy].stats.hp_atual > 0) {
                            if (play_card(&player, selected_card, &inimigos[selected_enemy])) {
                                mirando = 0; // Sai da mira
                                if (selected_card >= player.mao.num_cartas) selected_card--;
                            }
                        }
                    }
                }
            }
        }

        // --- LÓGICA DE ESTADOS (Gerencia o fluxo do jogo) ---
        switch (current_state) {
            case GAME_STATE_START:
                player = setup_player(); // Cria jogador
                vitorias = 0;
                current_state = GAME_STATE_NEW_COMBAT;
                break;

            case GAME_STATE_NEW_COMBAT: // Prepara nova luta
                inimigos[0] = create_enemy();
                inimigos[1] = create_enemy();
                // Prepara baralho
                player.pilha_compra = player.baralho_completo;
                shuffle_pilha(&player.pilha_compra);
                player.mao.num_cartas = 0;
                player.pilha_descarte.num_cartas = 0;
                
                start_player_turn(&player);
                current_state = GAME_STATE_PLAYER_TURN;
                break;

            case GAME_STATE_PLAYER_TURN:
                // Checa se ganhou o combate
                if (inimigos[0].stats.hp_atual <= 0 && inimigos[1].stats.hp_atual <= 0) {
                    vitorias++;
                    if (vitorias >= 10) current_state = GAME_STATE_VICTORY;
                    else current_state = GAME_STATE_NEW_COMBAT;
                }
                break;

            case GAME_STATE_ENEMY_TURN:
                al_rest(0.5); // Pausa dramática
                // Zera escudos inimigos
                if(inimigos[0].stats.hp_atual > 0) inimigos[0].stats.escudo = 0;
                if(inimigos[1].stats.hp_atual > 0) inimigos[1].stats.escudo = 0;
                
                execute_enemy_turn(&player, inimigos);
                
                // Checa se perdeu o jogo
                if (player.stats.hp_atual <= 0) current_state = GAME_STATE_GAME_OVER;
                else {
                    start_player_turn(&player);
                    current_state = GAME_STATE_PLAYER_TURN;
                }
                break;
        }

        // --- DESENHO (Só desenha se a fila de eventos estiver vazia para não piscar) ---
        if (redraw && al_is_event_queue_empty(queue)) {
            Render(&renderer, current_state, player, inimigos, selected_card, selected_enemy, mirando);
            redraw = false;
        }
    }

    ClearRenderer(&renderer); // Limpa tudo ao sair
    return 0;
}