#include "ui.h"
#include "raylib.h"
#include <string.h>
#include "config.h"

#define MAX_FLOATING_MSGS 8

typedef struct FloatingMsg
{
    char text[48];
    Vector2 pos;
    Color color;
    float timer;
    float duration;
    int active;
} FloatingMsg;

static FloatingMsg floatingMsgs[MAX_FLOATING_MSGS];

void UIInit() {}

void UIDrawHUD(const Player *player, int currentWave, int enemiesAlive, float waveCooldownTimer, bool inWave)
{
    // --- HUD estilo MU Online ---
    // Parâmetros de layout
    int hudY = GetScreenHeight() - 80;
    int hudCenterX = GetScreenWidth() / 2;

    // --- CRISTAL DE VIDA (esquerda) ---
    Vector2 hpCrystalCenter = {hudCenterX - 220, hudY + 12}; // movido 20px para cima
    float hpCrystalSize = 38;
    float healthPercent = (float)player->health / player->maxHealth;
    DrawCrystal(hpCrystalCenter, hpCrystalSize, RED);
    if (healthPercent < 1.0f)
    {
        Color overlay = (Color){30, 0, 0, (unsigned char)(180 * (1.0f - healthPercent))};
        DrawCrystal(hpCrystalCenter, hpCrystalSize, overlay);
    }
    DrawText(TextFormat("%d/%d", player->health, player->maxHealth), hpCrystalCenter.x - 30, hpCrystalCenter.y + hpCrystalSize - 6, 18, WHITE);

    // --- CRISTAL DE MANA (direita) ---
    Vector2 mpCrystalCenter = {hudCenterX + 220, hudY + 12}; // movido 20px para cima
    float mpCrystalSize = 38;
    float manaPercent = (player->maxMana > 0) ? (float)player->mana / player->maxMana : 0.0f;
    DrawCrystal(mpCrystalCenter, mpCrystalSize, SKYBLUE);
    if (manaPercent < 1.0f)
    {
        Color overlay = (Color){0, 30, 60, (unsigned char)(180 * (1.0f - manaPercent))};
        DrawCrystal(mpCrystalCenter, mpCrystalSize, overlay);
    }
    DrawText(TextFormat("%d/%d", player->mana, player->maxMana), mpCrystalCenter.x - 30, mpCrystalCenter.y + mpCrystalSize - 6, 18, WHITE);

    // --- HOTBAR CENTRAL ---
    UIDrawHotkeyBar(GetScreenWidth(), GetScreenHeight(), NULL, 3); // Ajuste o número de slots conforme necessário

    // --- BARRA DE XP acima da hotbar ---
    int xpBarW = 320;
    int xpBarH = 14;
    int xpBarX = hudCenterX - xpBarW / 2;
    int xpBarY = hudY - 24;
    int xpToNextLevel = 100 + (player->level - 1) * 50;
    if (xpToNextLevel <= 0)
        xpToNextLevel = 100;
    float xpPercent = (player->level > 0) ? (float)player->experience / xpToNextLevel : 0.0f;
    DrawRectangle(xpBarX, xpBarY, xpBarW, xpBarH, Fade(BLACK, 0.5f));
    DrawRectangle(xpBarX, xpBarY, (int)(xpBarW * xpPercent), xpBarH, Fade(GREEN, 0.8f));
    DrawRectangleLines(xpBarX, xpBarY, xpBarW, xpBarH, Fade(WHITE, 0.4f));
    DrawText(TextFormat("XP: %d/%d", player->experience, xpToNextLevel), xpBarX + 8, xpBarY - 18, 16, YELLOW);
    DrawText(TextFormat("Nível: %d", player->level), xpBarX + xpBarW - 90, xpBarY - 18, 16, YELLOW);

    // --- SKILLPOINTS ---
    DrawText(TextFormat("Skillpoints: %d", player->skillPoints), hudCenterX - 60, hudY + 70, 18, YELLOW);

    // --- INFORMAÇÕES DA WAVE ---
    if (inWave)
    {
        int waveTextY = 50;
        int enemiesTextY = 80;
        const char *waveStr = TextFormat("Onda: %d", currentWave);
        const char *enemiesStr = TextFormat("Inimigos Vivos: %d", enemiesAlive);
        int waveTextW = MeasureText(waveStr, 22);
        int enemiesTextW = MeasureText(enemiesStr, 22);
        DrawText(waveStr, GetScreenWidth() / 2 - waveTextW / 2, waveTextY, 22, WHITE);
        DrawText(enemiesStr, GetScreenWidth() / 2 - enemiesTextW / 2, enemiesTextY, 22, WHITE);
    }
    else
    {
        int prepTextY = 50;
        int timerTextY = 80;
        int prepTextW = MeasureText("PREPARAÇÃO!", 26);
        int timerTextW = MeasureText(TextFormat("Próxima onda em: %.1fs", waveCooldownTimer), 22);
        DrawText("PREPARAÇÃO!", GetScreenWidth() / 2 - prepTextW / 2, prepTextY, 26, SKYBLUE);
        DrawText(TextFormat("Próxima onda em: %.1fs", waveCooldownTimer), GetScreenWidth() / 2 - timerTextW / 2, timerTextY, 22, WHITE);
    }

    // FPS no canto
    DrawFPS(GetScreenWidth() - 100, 10);

    // Feedback visual de dano: pisca a tela de vermelho se o player tomou dano
    static float damageFlash = 0.0f;
    static int lastHealth = -1;
    if (lastHealth == -1)
        lastHealth = player->health;
    if (player->health < lastHealth)
        damageFlash = 0.5f;
    lastHealth = player->health;
    if (damageFlash > 0.0f)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RED, damageFlash * 0.4f));
        damageFlash -= GetFrameTime();
        if (damageFlash < 0.0f)
            damageFlash = 0.0f;
    }
}

void UIDrawHotkeyBar(int width, int height, const float *cooldowns, int numSlots)
{
    int barW = 220, barH = 48;
    int x = width / 2 - barW / 2, y = height - barH - 12;
    DrawRectangleRounded((Rectangle){x, y, barW, barH}, 0.3f, 8, Fade(BLACK, 0.7f));
    // Ícones das teclas para cada slot
    const char *slotKeys[] = {"Botão Direito", "E", "Espaço"};
    const char *slotIcons[] = {"RMB", "E", "⎵"}; // ⎵ = espaço
    const char *slotNames[] = {"Área", "Barreira", "Dash"};
    for (int i = 0; i < numSlots; i++)
    {
        int slotX = x + 12 + i * 70;
        int slotY = y + 8;
        DrawRectangleRounded((Rectangle){slotX, slotY, 48, 32}, 0.4f, 6, DARKGRAY);
        // Desenha ícone da tecla
        DrawText(slotIcons[i], slotX + 18, slotY - 8, 18, YELLOW);
        // Nome do poder
        DrawText(slotNames[i], slotX + 4, slotY + 20, 14, LIGHTGRAY);
        // Cooldown overlay
        if (cooldowns && cooldowns[i] > 0.0f)
        {
            float pct = cooldowns[i] / 5.0f; // Exemplo: 5s de cooldown
            if (pct > 1.0f)
                pct = 1.0f;
            int cdH = (int)(32 * pct);
            DrawRectangle(slotX, slotY + 32 - cdH, 48, cdH, Fade(GRAY, 180));
            DrawText(TextFormat("%.1f", cooldowns[i]), slotX + 16, slotY + 8, 14, WHITE);
        }
        // Tooltip ao passar o mouse
        Vector2 mouse = GetMousePosition();
        Rectangle slotRect = {slotX, (float)slotY, 48, 32};
        if (CheckCollisionPointRec(mouse, slotRect))
        {
            DrawRectangle(slotX - 8, slotY - 32, 64, 24, Fade(BLACK, 0.8f));
            DrawText(slotKeys[i], slotX - 4, slotY - 28, 16, YELLOW);
        }
    }
}

void UIDrawMenu(Texture2D obscuraIcon, float hudAlpha, float titleOffset)
{
    // Desenha estrelas caindo (pode ser movido para uma função separada se usado em outros lugares)
    // For now, keep it here as it's specific to the main menu background in the original code
    // Note: `stars` array would need to be managed if this function is called repeatedly without `main.c` context
    // For simplicity, I'll assume this part of background is drawn in main.c before calling UIDrawMenu, or stars are static in ui.c
    // Vector2 stars[100];
    // for (int i = 0; i < 100; i++)
    // {
    //     stars[i] = (Vector2){GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight)};
    // }

    // for (int i = 0; i < 100; i++)
    // {
    //     DrawPixelV(stars[i], WHITE);
    //     stars[i].y += 0.5f;
    //     if (stars[i].y > screenHeight)
    //         stars[i].y = 0;
    // }

    // DrawTexture(obscuraIcon, GetScreenWidth() / 2 - obscuraIcon.width / 2, GetScreenHeight() / 2 - obscuraIcon.height / 2 + titleOffset, WHITE);

    // DrawText("Desenvolvido por Felipe", GetScreenWidth() / 2 - MeasureText("Desenvolvido por Felipe", 20) / 2, GetScreenHeight() - 60, 20, Fade(DARKGRAY, 0.8f));
}

void UIDrawGameOver()
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f)); // Darker overlay
    DrawText("GAME OVER", GetScreenWidth() / 2 - MeasureText("GAME OVER", 40) / 2, GetScreenHeight() / 2 - 40, 40, RED);
    DrawText("Pressione R para reiniciar", GetScreenWidth() / 2 - MeasureText("Pressione R para reiniciar", 20) / 2, GetScreenHeight() / 2 + 10, 20, WHITE);
    // You could add score, wave reached, etc. here
}

void UIShowMessage(const char *msg, float duration)
{
    (void)msg;
    (void)duration;
    // Mensagens temporárias na tela
}

void UIShowFloatingMsg(const char *msg, Vector2 pos, Color color, float duration)
{
    for (int i = 0; i < MAX_FLOATING_MSGS; i++)
    {
        if (!floatingMsgs[i].active)
        {
            strncpy(floatingMsgs[i].text, msg, sizeof(floatingMsgs[i].text) - 1);
            floatingMsgs[i].text[sizeof(floatingMsgs[i].text) - 1] = '\0';
            floatingMsgs[i].pos = pos;
            floatingMsgs[i].color = color;
            floatingMsgs[i].timer = duration;
            floatingMsgs[i].duration = duration;
            floatingMsgs[i].active = 1;
            break;
        }
    }
}

void UIUpdateFloatingMsgs(void)
{
    for (int i = 0; i < MAX_FLOATING_MSGS; i++)
    {
        if (floatingMsgs[i].active)
        {
            floatingMsgs[i].timer -= GetFrameTime();
            floatingMsgs[i].pos.y -= 18 * GetFrameTime(); // Sobe
            if (floatingMsgs[i].timer <= 0)
                floatingMsgs[i].active = 0;
        }
    }
}

void UIDrawFloatingMsgs(void)
{
    for (int i = 0; i < MAX_FLOATING_MSGS; i++)
    {
        if (floatingMsgs[i].active)
        {
            float alpha = floatingMsgs[i].timer / floatingMsgs[i].duration;
            Color c = floatingMsgs[i].color;
            c.a = (unsigned char)(255 * alpha);
            // Sombra para melhor leitura
            DrawText(floatingMsgs[i].text, (int)floatingMsgs[i].pos.x + 2, (int)floatingMsgs[i].pos.y + 2, 22, Fade(BLACK, alpha * 0.5f));
            DrawText(floatingMsgs[i].text, (int)floatingMsgs[i].pos.x, (int)floatingMsgs[i].pos.y, 22, c);
        }
    }
}

void UIDrawUpgradeMenu(const Player *player, const UpgradeMenuState *menuState)
{
    if (!menuState->visible)
        return;

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
        TextFormat("+5 Dano de Ataque (Atual: %d)", player->attackDamage),
        "Pular (Manter Pontos de Habilidade)"};
    int numOptions = sizeof(options) / sizeof(options[0]);
    int menuY = GetScreenHeight() / 2 - 30;
    int menuSpacing = 50;

    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < numOptions; ++i)
    {
        int textWidth = MeasureText(options[i], 20);
        int itemX = GetScreenWidth() / 2 - textWidth / 2;
        int itemY = menuY + i * menuSpacing;
        Rectangle itemRect = {itemX - 12, (float)itemY - 8, (float)textWidth + 24, 40.0f};
        Color color = (i == menuState->selectedOption) ? GOLD : WHITE;
        if (CheckCollisionPointRec(mouse, itemRect))
        {
            color = ORANGE;
        }
        DrawRectangleRounded(itemRect, 0.25f, 8, Fade(BLACK, 0.25f));
        DrawRectangleRoundedLines(itemRect, 0.25f, 8, Fade(color, 0.7f));
        DrawText(options[i], itemX, itemY, 20, color);
        if (i == 0)
            DrawCircle(itemX - 24, itemY + 16, 12, RED);
        if (i == 1)
            DrawCircle(itemX - 24, itemY + 16, 12, BLUE);
        if (i == 2)
            DrawRectangle(itemX - 32, itemY + 8, 16, 16, GRAY);
        if (i == 3)
            DrawRectangle(itemX - 32, itemY + 8, 16, 16, GREEN);
    }
}

int UIHandleUpgradeMenuInput(Player *player, UpgradeMenuState *menuState)
{
    if (!menuState->visible || player->skillPoints <= 0)
    {
        menuState->visible = 0;
        return 1;
    }
    int numOptions = 6;
    int menuY = GetScreenHeight() / 2 - 30;
    int menuSpacing = 50;
    const char *options[] = {
        TextFormat("+20 Vida Máxima (Atual: %d)", player->maxHealth),
        TextFormat("+10 Mana Máxima (Atual: %d)", player->maxMana),
        TextFormat("-10%% Tempo de Recarga (Atual: %.2fs)", player->shootCooldown),
        TextFormat("+80 Alcance de Ataque (Atual: %.0f)", player->attackRange),
        TextFormat("+5 Dano de Ataque (Atual: %d)", player->attackDamage),
        "Pular e manter os pontos"};
    Vector2 mouse = GetMousePosition();
    int hovered = -1;
    for (int i = 0; i < numOptions; ++i)
    {
        int textWidth = MeasureText(options[i], 20);
        int textDrawX = GetScreenWidth() / 2 - textWidth / 2;
        int textDrawY = menuY + i * menuSpacing;

        Rectangle cardRect = {(float)(textDrawX - 12), (float)(textDrawY - 8), (float)textWidth + 24, 40.0f};

        if (CheckCollisionPointRec(mouse, cardRect))
        {
            hovered = i;
        }
    }
    if (hovered != -1)
        menuState->selectedOption = hovered;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hovered != -1)
    {
        switch (hovered)
        {
        case 0:
            if (player->skillPoints > 0)
            {
                player->maxHealth += 20;
                player->health = player->maxHealth;
                player->skillPoints--;
                menuState->confirmState = 1;
            }
            break;
        case 1:
            if (player->skillPoints > 0)
            {
                player->maxMana += 10;
                player->mana = player->maxMana;
                player->skillPoints--;
                menuState->confirmState = 1;
            }
            break;
        case 2:
            if (player->skillPoints > 0)
            {
                player->shootCooldown *= 0.9f;
                player->skillPoints--;
                menuState->confirmState = 1;
            }
            break;
        case 3:
            if (player->skillPoints > 0)
            {
                player->attackRange += 80.0f;
                player->skillPoints--;
                menuState->confirmState = 1;
            }
            break;
        case 4:
            if (player->skillPoints > 0)
            {
                player->attackDamage += 5;
                player->skillPoints--;
                menuState->confirmState = 1;
            }
            break;
        default:
            menuState->visible = 0;
            break;
        }
    }
    if (player->skillPoints <= 0)
    {
        menuState->visible = 0;
        return 1;
    }
    if (IsKeyPressed(KEY_ESCAPE))
    {
        menuState->visible = 0;
        menuState->confirmState = 0;
        return 1;
    }
    return 0;
}

void UIDrawPauseScreen(void)
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
    DrawText("PAUSE", screenWidth / 2 - MeasureText("PAUSE", 40) / 2, screenHeight / 2 - 20, 40, PURPLE);
    DrawText("Pressione P para continuar", screenWidth / 2 - MeasureText("Pressione P para continuar", 20) / 2, screenHeight / 2 + 20, 20, WHITE);
}

// Função para desenhar um cristal facetado estilizado
void DrawCrystal(Vector2 center, float size, Color baseColor)
{
    Vector2 pts[6];
    pts[0] = (Vector2){center.x, center.y - size};
    pts[1] = (Vector2){center.x - size * 0.6f, center.y - size * 0.2f};
    pts[2] = (Vector2){center.x - size * 0.4f, center.y + size * 0.8f};
    pts[3] = (Vector2){center.x, center.y + size};
    pts[4] = (Vector2){center.x + size * 0.4f, center.y + size * 0.8f};
    pts[5] = (Vector2){center.x + size * 0.6f, center.y - size * 0.2f};
    DrawTriangle(pts[0], pts[1], pts[5], baseColor);
    DrawTriangle(pts[1], pts[2], pts[3], Fade(baseColor, 0.8f));
    DrawTriangle(pts[3], pts[4], pts[5], Fade(baseColor, 0.8f));
    DrawTriangle(pts[1], pts[3], pts[5], Fade(baseColor, 0.6f));
    for (int i = 0; i < 6; i++)
        DrawLineV(pts[i], pts[(i + 1) % 6], Fade(WHITE, 0.7f));
    DrawCircleV((Vector2){center.x, center.y - size * 0.2f}, size * 0.22f, Fade(WHITE, 0.25f));
    DrawCircleV((Vector2){center.x, center.y + size * 0.3f}, size * 0.12f, Fade(WHITE, 0.18f));
    DrawTriangle(
        (Vector2){center.x, center.y - size * 0.7f},
        (Vector2){center.x - size * 0.18f, center.y - size * 0.2f},
        (Vector2){center.x + size * 0.18f, center.y - size * 0.2f},
        Fade(WHITE, 0.18f));
}
