#include "ui.h"
#include "raylib.h"
#include <string.h>

#define MAX_FLOATING_MSGS 8

typedef struct FloatingMsg {
    char text[48];
    Vector2 pos;
    Color color;
    float timer;
    float duration;
    int active;
} FloatingMsg;

static FloatingMsg floatingMsgs[MAX_FLOATING_MSGS];

void UIInit() {}

void UIDrawHUD(const Player *player) {
    // Implemente o HUD aqui (barra de vida, XP, etc)
}

void UIDrawHotkeyBar(int width, int height, const float *cooldowns, int numSlots) {
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

void UIShowFloatingMsg(const char *msg, Vector2 pos, Color color, float duration) {
    for (int i = 0; i < MAX_FLOATING_MSGS; i++) {
        if (!floatingMsgs[i].active) {
            strncpy(floatingMsgs[i].text, msg, sizeof(floatingMsgs[i].text)-1);
            floatingMsgs[i].text[sizeof(floatingMsgs[i].text)-1] = '\0';
            floatingMsgs[i].pos = pos;
            floatingMsgs[i].color = color;
            floatingMsgs[i].timer = duration;
            floatingMsgs[i].duration = duration;
            floatingMsgs[i].active = 1;
            break;
        }
    }
}

void UIUpdateFloatingMsgs(void) {
    for (int i = 0; i < MAX_FLOATING_MSGS; i++) {
        if (floatingMsgs[i].active) {
            floatingMsgs[i].timer -= GetFrameTime();
            floatingMsgs[i].pos.y -= 18 * GetFrameTime(); // Sobe
            if (floatingMsgs[i].timer <= 0) floatingMsgs[i].active = 0;
        }
    }
}

void UIDrawFloatingMsgs(void) {
    for (int i = 0; i < MAX_FLOATING_MSGS; i++) {
        if (floatingMsgs[i].active) {
            float alpha = floatingMsgs[i].timer / floatingMsgs[i].duration;
            Color c = floatingMsgs[i].color;
            c.a = (unsigned char)(255 * alpha);
            DrawText(floatingMsgs[i].text, (int)floatingMsgs[i].pos.x, (int)floatingMsgs[i].pos.y, 22, c);
        }
    }
}
