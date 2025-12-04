# 🃏 Slay the Spire - Clone (PDS1)

Um jogo de cartas *roguelike* desenvolvido em **C** utilizando a biblioteca gráfica **Allegro 5** para a disciplina de Programação e Desenvolvimento de Software 1 (PDS1) da UFMG.



---

## 📋 Visão Geral do Jogo

O projeto simula a fase de combate do jogo "Slay the Spire". O herói deve gerenciar seu baralho e energia para derrotar 10 inimigos em combates por turnos. A sobrevivência exige a gestão eficaz de recursos e a compreensão da **Intenção** dos inimigos.

### ✨ Funcionalidades Principais

* **Combate por Turnos:** Jogador e Inimigos alternam ações.
* **Gerenciamento de Deck:** Implementação das pilhas de Compra, Mão e Descarte com reembaralhamento automático.
* **Tipos de Cartas:** Suporte para Ataque, Defesa, Especial (Troca de mão) e cartas de Status (Buffs/Debuffs).
* **Inteligência Artificial (IA):** Inimigos com ciclos de comportamento fixo e exibição da próxima ação (Intenção).
* **Níveis de Inimigo:** Diferenciação entre inimigos Fracos e Fortes (Boss).
* **Interface Gráfica:** Uso de sprites e barras de vida renderizadas pelo Allegro 5.

---

## 🛠 Tech Stack e Estrutura

### Tecnologias
| Categoria | Tecnologia | Função |
| :--- | :--- | :--- |
| **Linguagem Principal** | C | Lógica e motor do jogo |
| **Gráficos/Input** | Allegro 5 | Renderização, Janelas e Leitura de Teclado |
| **Compilador** | GCC / MinGW | Necessário para compilação |
| **Build System** | Makefile | Automação do processo de compilação |

### Estrutura do Projeto

O código é modularizado para separar a lógica das regras (`logic.c`) do desenho (`renderer.c`).

* **`main.c`**: Contém o **Loop Principal** e a **Máquina de Estados** (Game Flow).
* **`logic.c`**: Regras de jogo, cálculo de dano e aplicação de efeitos.
* **`renderer.c`**: Desenho de todos os elementos visuais (cartas, sprites, barras de vida).
* **`setup.c`**: Funções de inicialização e criação aleatória de baralhos e inimigos.
* **`game_structs.h`**: Definições das estruturas de dados (`Player`, `Card`, `Enemy`).

---

## 🚀 Instalação e Execução

### Pré-requisitos
* GCC (Compilador C)
* Make (Para o `Makefile`)
* Biblioteca Allegro 5 e seus addons (image, font, ttf, primitives) instalados no ambiente (ex: via MSYS2).

### Comandos de Execução (Windows/MSYS2)

1.  **Navegue até a pasta do projeto:**
    ```bash
    cd "caminho/do/projeto"
    ```

2.  **Compile o executável:**
    ```bash
    make
    # ou use o alvo específico:
    # make game
    ```

3.  **Execute o jogo:**
    ```bash
    ./game.out
    ```
    *Para apagar os arquivos compilados:* `make clean`

---

## 🎮 Guia de Controles

| Tecla | Ação |
| :--- | :--- |
| **⬅️ / ➡️ Setas** | Navegar entre cartas ou selecionar alvo |
| **ENTER** | Jogar carta / Confirmar alvo |
| **ESC** | Cancelar mira / **Encerrar Turno** |
| **Q** | Sair do jogo |

---

## 👤 Autor e Contato

**Desenvolvedor:** João Lemos

**Função:** Projeto & Full-Stack Developer

**Suporte:** [joaoclemoss@gmail.com](mailto:joaoclemoss@gmail.com)
