# 🃏 Slay the Spire - Clone (PDS1)

Trabalho Prático desenvolvido para a disciplina de **Programação e Desenvolvimento de Software 1 (PDS1)** da UFMG.

Este projeto é uma implementação de um jogo de cartas *roguelike* inspirado em "Slay the Spire", desenvolvido inteiramente em **C** utilizando a biblioteca gráfica **Allegro 5**.

---

## 📋 Sobre o Jogo

O jogador controla um herói que deve enfrentar uma série de 10 combates consecutivos contra inimigos variados. O objetivo é gerenciar seu baralho, energia e vida para sobreviver até o final.

### Funcionalidades Implementadas
* **Sistema de Combate por Turnos:** Jogador e Inimigos alternam ações.
* **Gerenciamento de Baralho:** Pilhas de Compra, Mão e Descarte (com reembaralhamento automático).
* **Tipos de Cartas:** Ataque, Defesa e Especial (efeitos variados).
* **Inteligência Artificial:** Inimigos com ciclos de comportamento (Ataque/Defesa) e níveis de dificuldade (Fraco/Forte).
* **Interface Gráfica:** Renderização de sprites, barras de vida, energia e intenção dos inimigos.
* **Visual Personalizado:** Suporte a carregamento de imagens `.png` para personagens e cartas.

---

## 🎮 Como Jogar

O objetivo é vencer **10 combates**. Se a vida do jogador chegar a 0, é **Game Over**.

### Controles

| Tecla | Ação |
| :--- | :--- |
| **⬅️ / ➡️ Setas** | Navegar entre as cartas da mão |
| **ENTER** | Jogar a carta selecionada / Confirmar alvo |
| **ESC** | Cancelar seleção de alvo / **Encerrar Turno** |
| **Q** | Sair do jogo |

### Regras Básicas
1.  Você começa o turno com **3 de Energia** e **5 Cartas**.
2.  Cartas de **Ataque** causam dano (reduzem escudo primeiro, depois vida).
3.  Cartas de **Defesa** dão escudo (o escudo zera no início do seu próximo turno).
4.  Os inimigos mostram a **Intenção** (o que vão fazer no turno deles) acima da cabeça.
5.  Ao vencer um combate, sua vida **não** é regenerada totalmente para o próximo (gerenciamento de recursos é crucial!).

---

## 🛠️ Instalação e Compilação

### Pré-requisitos
* Compilador GCC (MinGW)
* Make
* Biblioteca **Allegro 5** instalada (e seus addons: image, font, ttf, primitives).

### Como Compilar (Windows/MSYS2)

1.  Navegue até a pasta do projeto:
    ```bash
    cd "caminho/do/projeto"
    ```

2.  Compile o jogo usando o `Makefile`:
    ```bash
    make
    ```

3.  Execute o jogo:
    ```bash
    ./game.out
    ```

*Para limpar os arquivos compilados:* `make clean`

---

## 📂 Estrutura do Projeto

* **`main.c`**: Loop principal, gerenciamento de eventos e máquina de estados.
* **`renderer.c/.h`**: Responsável por toda a parte visual (desenhar cartas, personagens, textos e imagens).
* **`logic.c/.h`**: Contém as regras do jogo (dano, turnos, baralho, IA).
* **`setup.c/.h`**: Funções de inicialização (criar baralho, gerar inimigos aleatórios).
* **`game_structs.h`**: Definição das estruturas de dados (Player, Enemy, Card).
* **`constants.h`**: Constantes globais (tamanho da tela, posições).
* **`Makefile`**: Script de automação de compilação.
* **Assets**:
    * `player.png`, `enemy_weak.png`, `enemy_strong.png`: Sprites dos personagens.
    * `card.png`, `background.png`: Elementos de interface.
    * `font.ttf`: Fonte personalizada do jogo.

---

👥 Time

   João Lemos - Projeto & Full-Stack Developer
   

📞 Support
email: joaoclemoss@gmail.com
