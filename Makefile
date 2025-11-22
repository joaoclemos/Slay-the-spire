# Define o nome do arquivo final
TARGET := game.out

# ADICIONE SEUS NOVOS ARQUIVOS .C AQUI
SRC    := main.c utils.c renderer.c setup.c logic.c

# --- Configuração do Allegro (copiado do devkit) ---
PKGCONF := pkg-config
ALLEGRO_PKGS := allegro-5 allegro_main-5 allegro_image-5 allegro_primitives-5 \
                allegro_font-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5

# Flags de compilação: -g (debug), -lm (matemática) + flags do Allegro
CFLAGS   := -g -lm $(shell $(PKGCONF) --cflags $(ALLEGRO_PKGS))
# Flags de biblioteca: Onde encontrar as bibliotecas do Allegro
LDLIBS   := $(shell $(PKGCONF) --libs   $(ALLEGRO_PKGS))


# --- Regras (Targets) ---

# Regra principal: Compila o jogo
# $^ significa "todos os pré-requisitos" (os arquivos em SRC)
# $@ significa "o nome do alvo" (o TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

# Regra de limpeza (a sua, que funciona no MSYS2)
# O -f ignora erros se não houver arquivos .out
clean:
	rm -f *.out