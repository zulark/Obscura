# Makefile Completo e Organizado

# Nome do seu executável (sem a extensão .exe)
TARGET_EXEC := Obscura

# --- Compilador e Flags ---
CC := gcc

# Flags de compilação
# -Wall -Wextra -pedantic: Ativa muitos avisos úteis
# -Iinclude: Procura headers (.h) na pasta 'include/' do seu projeto
# -g: Para debug (com gdb)
# -O2: Otimizações para release
# -s: Remove símbolos do executável final (menor tamanho em release)
CFLAGS_COMMON := -Wall -Wextra -pedantic -Iinclude
CFLAGS_DEBUG := $(CFLAGS_COMMON) -g
CFLAGS_RELEASE := $(CFLAGS_COMMON) -O2 -s

# Por padrão, usamos as flags de DEBUG
# Para compilar para release, use: make CFLAGS_MODE=RELEASE
# ou use a regra 'make release' que definimos mais abaixo.
ifeq ($(CFLAGS_MODE),RELEASE)
    CFLAGS := $(CFLAGS_RELEASE)
    BUILD_MODE_MSG := Release
else
    CFLAGS := $(CFLAGS_DEBUG)
    BUILD_MODE_MSG := Debug
endif

# Flags para o linker (se houver alguma específica além das libs)
LDFLAGS :=

# Bibliotecas para linkar (Raylib e suas dependências no MinGW)
LDLIBS := -lraylib -lopengl32 -lgdi32 -lwinmm

# --- Diretórios ---
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# --- Arquivos ---
# Pega todos os arquivos .c da pasta src/
SOURCES := $(wildcard $(SRC_DIR)/*.c)
# Gera os nomes dos arquivos objeto (.o) correspondentes, na pasta obj/
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
# Define o caminho completo para o executável final
EXECUTABLE := $(BIN_DIR)/$(TARGET_EXEC).exe

# Arquivo de recursos do Windows (ícone, versão, etc)
RES_FILE := resource.res



# --- Regras ---

# Regra padrão (o que acontece quando você digita apenas 'make')
all: $(EXECUTABLE)

# Regra para criar o executável
$(EXECUTABLE): $(OBJECTS) $(RES_FILE)
	@mkdir -p $(BIN_DIR) # Cria a pasta bin/ se não existir
	@echo "Modo de Build: $(BUILD_MODE_MSG)"
	@echo "Linkando para criar $(EXECUTABLE)..."
	$(CC) $(OBJECTS) $(RES_FILE) -o $@ $(LDFLAGS) $(LDLIBS)
	@echo "Compilação concluída! Executável: $@"

# Regra para compilar arquivos .c em arquivos objeto .o
# $< é o nome do primeiro pré-requisito (o arquivo .c fonte)
# $@ é o nome do alvo (o arquivo .o de saída)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR) # Cria a pasta obj/ se não existir
	@echo "Compilando $< -> $@"
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para rodar o jogo
run: all
	@echo "Executando $(EXECUTABLE)..."
	$(EXECUTABLE)

# Regra para limpar os arquivos gerados pela compilação
clean:
	@echo "Limpando arquivos de build..."
	rm -f $(OBJ_DIR)/*.o
	rm -f $(EXECUTABLE)
	# Para ser mais completo, você pode querer remover as pastas obj e bin
	# Cuidado ao usar 'rm -rf'
	# @-rmdir $(OBJ_DIR) 2>/dev/null || true
	# @-rmdir $(BIN_DIR) 2>/dev/null || true
	@echo "Limpeza concluída."

# Regra para compilar em modo Release
release:
	$(MAKE) CFLAGS_MODE=RELEASE all

# Phony targets (alvos que não são nomes de arquivos reais)
.PHONY: all run clean release