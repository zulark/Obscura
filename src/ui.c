#include "ui.h"
#include "raylib.h"

void UIInit() {}

void UIDrawHUD(const Player *player) {
    // Implemente o HUD aqui (barra de vida, XP, etc)
}

void UIDrawHotkeyBar(int screenWidth, int screenHeight, const float *cooldowns, int numSlots) {
    // Implemente a hotkey bar aqui
}

void UIDrawMenu(int selected) {
    // Implemente o menu principal
}

void UIDrawGameOver() {
    // Implemente a tela de game over
}

void UIShowMessage(const char *msg, float duration) {
    // Implemente mensagens temporárias na tela
}
