# --- Configurações Gerais ---
TARGET_EXEC := Obscura
CC := gcc
CFLAGS_COMMON := -Wall -Wextra -pedantic -Iinclude
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# --- Detecção de Sistema Operacional ---
ifeq ($(OS),Windows_NT)
    # Configurações para Windows (MinGW)
    OS_TYPE := Windows
    EXECUTABLE := $(BIN_DIR)/$(TARGET_EXEC).exe
    LDLIBS := -lraylib -lopengl32 -lgdi32 -lwinmm
    RES_FILE := resource.res
    RM := rm -f
    MKDIR := mkdir -p
else
    # Configurações para Linux
    OS_TYPE := Linux
    EXECUTABLE := $(BIN_DIR)/$(TARGET_EXEC)
    LDLIBS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    RES_FILE := # Linux ignora arquivos de recurso .res
    RM := rm -rf
    MKDIR := mkdir -p
endif

# --- Seleção de Modo (Debug/Release) ---
ifeq ($(CFLAGS_MODE),RELEASE)
    CFLAGS := $(CFLAGS_COMMON) -O2 -s
    BUILD_MODE_MSG := Release
else
    CFLAGS := $(CFLAGS_COMMON) -g
    BUILD_MODE_MSG := Debug
endif

# --- Arquivos ---
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# --- Regras de Compilação ---

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@$(MKDIR) $(BIN_DIR)
	@echo "S.O. Detectado: $(OS_TYPE)"
	@echo "Modo de Build: $(BUILD_MODE_MSG)"
	$(CC) $(OBJECTS) $(RES_FILE) -o $@ $(LDLIBS)
	@echo "Build finalizado: $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(MKDIR) $(OBJ_DIR)
	@echo "Compilando: $<"
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	@echo "Executando..."
	./$(EXECUTABLE)

clean:
	@echo "Limpando arquivos de build..."
	$(RM) $(OBJ_DIR) $(BIN_DIR)
	@echo "Limpeza concluída."

release:
	$(MAKE) CFLAGS_MODE=RELEASE all

.PHONY: all run clean release