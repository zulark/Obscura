#include "ui.h"
#include "raylib.h"
#include <string.h>
#include "config.h"

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

void UIDrawHUD(const Player *player, int currentWave, int enemiesAlive, float waveCooldownTimer, bool inWave) {
    int x = 20;
    int y = 20;

    // Health bar width scales with maxHealth (base 300 at 100 HP, +1.5px per extra HP)
    int baseBarWidth = 300;
    int baseMaxHealth = 100;
    float scalePerHP = 2.0f; // Cada HP extra aumenta mais a barra
    int barWidth = baseBarWidth + (int)((player->maxHealth - baseMaxHealth) * scalePerHP);
    if (barWidth < baseBarWidth) barWidth = baseBarWidth;
    int barHeight = 24;

    float healthPercent = (float)player->health / player->maxHealth;

    DrawRectangle(x + 2, y + 2, barWidth, barHeight, Fade(BLACK, 0.4f));
    DrawRectangleRounded((Rectangle){x, y, barWidth, barHeight}, 0.3f, 10, Fade(MAROON, 0.5f));
    DrawRectangleRounded((Rectangle){x, y, barWidth * healthPercent, barHeight}, 0.3f, 10, RED);
    DrawRectangleRoundedLines((Rectangle){x, y, barWidth, barHeight}, 0.3f, 10, Fade(WHITE, 0.3f));

    // Barra de XP
    int xpBarY = y + 30;
    int xpBarHeight = 12;
    int xpBarWidth = barWidth;
    int xpToNextLevel = 100 + (player->level - 1) * 50; 
    if (xpToNextLevel <= 0) xpToNextLevel = 100;
    float xpPercent = (player->level > 0) ? (float)player->experience / xpToNextLevel : 0.0f;

    DrawRectangle(x + 2, xpBarY + 2, xpBarWidth, xpBarHeight, Fade(BLACK, 0.4f));
    DrawRectangleRounded((Rectangle){x, xpBarY, xpBarWidth * xpPercent, xpBarHeight}, 0.3f, 10, GREEN);
    DrawRectangleRoundedLines((Rectangle){x, xpBarY, xpBarWidth, xpBarHeight}, 0.3f, 10, Fade(WHITE, 0.3f));

    // Barra de mana
    int manaBarY = xpBarY + xpBarHeight + 20;
    int manaBarHeight = 12;
    int manaBarWidth = barWidth;
    float manaPercent = (player->maxMana > 0) ? (float)player->mana / player->maxMana : 0.0f;
    DrawRectangle(x + 2, manaBarY + 2, manaBarWidth, manaBarHeight, Fade(BLACK, 0.4f));
    DrawRectangleRounded((Rectangle){x, manaBarY, manaBarWidth * manaPercent, manaBarHeight}, 0.3f, 10, BLUE);
    DrawRectangleRoundedLines((Rectangle){x, manaBarY, manaBarWidth, manaBarHeight}, 0.3f, 10, Fade(WHITE, 0.3f));

    // Pontos de habilidade
    DrawText(TextFormat("Skillpoints: %d", player->skillPoints), x + 8, manaBarY + manaBarHeight + 8, 16, YELLOW);

    // Informações da wave
    if (inWave)
    {
        DrawText(TextFormat("Onda: %d", currentWave), x + 8, manaBarY + manaBarHeight + 30, 20, WHITE);
        DrawText(TextFormat("Inimigos Vivos: %d", enemiesAlive), x + 8, manaBarY + manaBarHeight + 50, 20, WHITE);
    }
    else
    {
        DrawText("PREPARAÇÃO!", GetScreenWidth() - 300, 50, 25, SKYBLUE);
        DrawText(TextFormat("Próxima onda em: %.1fs", waveCooldownTimer), GetScreenWidth() - 300, 80, 25, WHITE);
    }

    // FPS no canto
    DrawFPS(GetScreenWidth() - 100, 10);

    // Feedback visual de dano: pisca a tela de vermelho se o player tomou dano
    static float damageFlash = 0.0f;
    static int lastHealth = -1;
    if (lastHealth == -1) lastHealth = player->health;
    if (player->health < lastHealth) damageFlash = 0.5f;
    lastHealth = player->health;
    if (damageFlash > 0.0f) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RED, damageFlash*0.4f));
        damageFlash -= GetFrameTime();
        if (damageFlash < 0.0f) damageFlash = 0.0f;
    }
}

void UIDrawHotkeyBar(int width, int height, const float *cooldowns, int numSlots) {
    int barW = 220, barH = 48;
    int x = width/2 - barW/2, y = height - barH - 12;
    DrawRectangleRounded((Rectangle){x, y, barW, barH}, 0.3f, 8, Fade(BLACK, 0.7f));
    // Ícones das teclas para cada slot
    const char *slotKeys[] = {"Botão Direito", "E", "Espaço"};
    const char *slotIcons[] = {"RMB", "E", "⎵"}; // ⎵ = espaço
    const char *slotNames[] = {"Área", "Barreira", "Dash"};
    for (int i = 0; i < numSlots; i++) {
        int slotX = x + 12 + i*70;
        int slotY = y + 8;
        DrawRectangleRounded((Rectangle){slotX, slotY, 48, 32}, 0.4f, 6, DARKGRAY);
        // Desenha ícone da tecla
        DrawText(slotIcons[i], slotX+18, slotY-8, 18, YELLOW);
        // Nome do poder
        DrawText(slotNames[i], slotX+4, slotY+20, 14, LIGHTGRAY);
        // Cooldown overlay
        if (cooldowns && cooldowns[i] > 0.0f) {
            float pct = cooldowns[i] / 5.0f; // Exemplo: 5s de cooldown
            if (pct > 1.0f) pct = 1.0f;
            int cdH = (int)(32*pct);
            DrawRectangle(slotX, slotY + 32 - cdH, 48, cdH, Fade(GRAY, 180));
            DrawText(TextFormat("%.1f", cooldowns[i]), slotX+16, slotY+8, 14, WHITE);
        }
        // Tooltip ao passar o mouse
        Vector2 mouse = GetMousePosition();
        Rectangle slotRect = {slotX, (float)slotY, 48, 32};
        if (CheckCollisionPointRec(mouse, slotRect)) {
            DrawRectangle(slotX-8, slotY-32, 64, 24, Fade(BLACK, 0.8f));
            DrawText(slotKeys[i], slotX-4, slotY-28, 16, YELLOW);
        }
    }
}

void UIDrawMenu(int selectedOption, float hudAlpha, float titleOffset) {
    // Desenha estrelas caindo (pode ser movido para uma função separada se usado em outros lugares)
    // For now, keep it here as it's specific to the main menu background in the original code
    // Note: `stars` array would need to be managed if this function is called repeatedly without `main.c` context
    // For simplicity, I'll assume this part of background is drawn in main.c before calling UIDrawMenu, or stars are static in ui.c

    DrawText("OBSCURA", GetScreenWidth() / 2 - MeasureText("OBSCURA", 60) / 2, GetScreenHeight() / 2 - 100 + titleOffset, 60, Fade(PURPLE, hudAlpha));
    DrawText("Desenvolvido por Felipe", GetScreenWidth() / 2 - MeasureText("Desenvolvido por Felipe", 20) / 2, GetScreenHeight() - 60, 20, Fade(DARKGRAY, hudAlpha));

    const char *menuItems[] = {"Iniciar Jogo", "Sair"};
    int menuCount = sizeof(menuItems) / sizeof(menuItems[0]);
    int menuY = GetScreenHeight() / 2 + 20;
    int menuSpacing = 40;

    for (int i = 0; i < menuCount; i++)
    {
        int itemY = menuY + i * menuSpacing;
        int textWidth = MeasureText(menuItems[i], 30);
        Color color = (i == selectedOption) ? DARKPURPLE : GRAY;
        DrawText(menuItems[i], GetScreenWidth() / 2 - textWidth / 2, itemY, 30, Fade(color, hudAlpha));
    }
}

void UIDrawGameOver() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f)); // Darker overlay
    DrawText("GAME OVER", GetScreenWidth() / 2 - MeasureText("GAME OVER", 40) / 2, GetScreenHeight() / 2 - 40, 40, RED);
    DrawText("Pressione R para reiniciar", GetScreenWidth() / 2 - MeasureText("Pressione R para reiniciar", 20) / 2, GetScreenHeight() / 2 + 10, 20, WHITE);
    // You could add score, wave reached, etc. here
}

void UIShowMessage(const char *msg, float duration) {
    (void)msg; (void)duration;
    // Mensagens temporárias na tela
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
            // Sombra para melhor leitura
            DrawText(floatingMsgs[i].text, (int)floatingMsgs[i].pos.x+2, (int)floatingMsgs[i].pos.y+2, 22, Fade(BLACK, alpha*0.5f));
            DrawText(floatingMsgs[i].text, (int)floatingMsgs[i].pos.x, (int)floatingMsgs[i].pos.y, 22, c);
        }
    }
}

void UIDrawUpgradeMenu(const Player *player, const UpgradeMenuState *menuState) {
    if (!menuState->visible) return;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
    const char *title = "UPGRADES";
    int titleWidth = MeasureText(title, 40);
    DrawText(title, GetScreenWidth() / 2 - titleWidth / 2, GetScreenHeight() / 2 - 150, 40, YELLOW);
    DrawText(TextFormat("Skill Points: %d", player->skillPoints), GetScreenWidth() / 2 - MeasureText(TextFormat("Skill Points: %d", player->skillPoints), 20) / 2, GetScreenHeight() / 2 - 100, 20, WHITE);

    const char *options[] = {
        TextFormat("+20 Vida Máxima (Atual: %d)", player->maxHealth),
        TextFormat("+10 Mana Máxima (Atual: %d)", player->maxMana),
        TextFormat("-10%% Tempo de Recarga (Atual: %.2fs)", player->shootCooldown),
        TextFormat("+80 Alcance de Ataque (Atual: %.0f)", player->attackRange),
        "Pular (Manter Pontos de Habilidade)"
    };
    int numOptions = sizeof(options) / sizeof(options[0]);
    int menuY = GetScreenHeight() / 2 - 30;
    int menuSpacing = 50;

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < numOptions; ++i) {
        int textWidth = MeasureText(options[i], 20);
        int itemX = GetScreenWidth() / 2 - textWidth / 2;
        int itemY = menuY + i * menuSpacing;
        Rectangle itemRect = { itemX-12, (float)itemY-8, (float)textWidth+24, 40.0f };
        Color color = (i == menuState->selectedOption) ? GOLD : WHITE;
        if (CheckCollisionPointRec(mouse, itemRect)) {
            color = ORANGE;
        }
        DrawRectangleRounded(itemRect, 0.25f, 8, Fade(BLACK, 0.25f));
        DrawRectangleRoundedLines(itemRect, 0.25f, 8, Fade(color, 0.7f));
        DrawText(options[i], itemX, itemY, 20, color);
        if (i == 0) DrawCircle(itemX-24, itemY+16, 12, RED);
        if (i == 1) DrawCircle(itemX-24, itemY+16, 12, BLUE);
        if (i == 2) DrawRectangle(itemX-32, itemY+8, 16, 16, GRAY);
        if (i == 3) DrawRectangle(itemX-32, itemY+8, 16, 16, GREEN);
    }
}

int UIHandleUpgradeMenuInput(Player *player, UpgradeMenuState *menuState) {
    if (!menuState->visible || player->skillPoints <= 0) {
        menuState->visible = 0;
        return 1;
    }
    int numOptions = 5;
    int menuY = GetScreenHeight() / 2 - 30;
    int menuSpacing = 50;
    const char *options[] = {
        TextFormat("+20 Vida Máxima (Atual: %d)", player->maxHealth),
        TextFormat("+10 Mana Máxima (Atual: %d)", player->maxMana),
        TextFormat("-10%% Tempo de Recarga (Atual: %.2fs)", player->shootCooldown),
        TextFormat("+80 Alcance de Ataque (Atual: %.0f)", player->attackRange),
        "Pular e manter os pontos"
    };
    Vector2 mouse = GetMousePosition();
    int hovered = -1;
    for (int i = 0; i < numOptions; ++i) {
        int textWidth = MeasureText(options[i], 20);
        int textDrawX = GetScreenWidth() / 2 - textWidth / 2;
        int textDrawY = menuY + i * menuSpacing;

        Rectangle cardRect = { (float)(textDrawX - 12), (float)(textDrawY - 8), (float)textWidth + 24, 40.0f };

        if (CheckCollisionPointRec(mouse, cardRect)) {
            hovered = i;
        }
    }
    if (hovered != -1) menuState->selectedOption = hovered;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovered != -1) {
        switch (hovered) {
            case 0:
                if (player->skillPoints > 0) {
                    player->maxHealth += 20;
                    player->health = player->maxHealth;
                    player->skillPoints--;
                    menuState->confirmState = 1;
                }
                break;
            case 1:
                if (player->skillPoints > 0) {
                    player->maxMana += 10;
                    player->mana = player->maxMana;
                    player->skillPoints--;
                    menuState->confirmState = 1;
                }
                break;
            case 2:
                if (player->skillPoints > 0) {
                    player->shootCooldown *= 0.9f;
                    player->skillPoints--;
                    menuState->confirmState = 1;
                }
                break;
            case 3:
                if (player->skillPoints > 0) {
                    player->attackRange += 80.0f;
                    player->skillPoints--;
                    menuState->confirmState = 1;
                }
                break;
            case 4:
                menuState->visible = 0;
                menuState->confirmState = 1;
        }
        if (player->skillPoints <= 0) {
            menuState->visible = 0;
            return 1;
        }
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        menuState->visible = 0;
        menuState->confirmState = 0;
        return 1;
    }
    return 0;
}

void UIDrawPauseScreen(void) {
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
    DrawText("PAUSE", screenWidth / 2 - MeasureText("PAUSE", 40) / 2, screenHeight / 2 - 20, 40, PURPLE);
    DrawText("Pressione P para continuar", screenWidth / 2 - MeasureText("Pressione P para continuar", 20) / 2, screenHeight / 2 + 20, 20, WHITE);
}
