#ifndef UI_H
#define UI_H
#include <raylib.h>
#include "player.h"
#include "boss.h"

// Define the structure for the Upgrade Menu
typedef struct UpgradeMenuState {
    int visible;
    int selectedOption;
    int confirmState; // Or other relevant fields
} UpgradeMenuState;

void UIInit();
void UIDrawHUD(const Player *player, int currentWave, int enemiesAlive, float waveCooldownTimer, bool inWave);
void UIDrawHotkeyBar(int width, int height, const float *cooldowns, int numSlots);
void UIDrawGameOver();
void UIShowMessage(const char *msg, float duration);
void UIShowFloatingMsg(const char *msg, Vector2 pos, Color color, float duration);
void UIUpdateFloatingMsgs(void);
void UIDrawFloatingMsgs(void);
void DrawCrystal(Vector2 center, float size, Color color);
void UIDrawUpgradeMenu(const Player *player, const UpgradeMenuState *menuState);
int UIHandleUpgradeMenuInput(Player *player, UpgradeMenuState *menuState); 
void UIDrawPauseScreen(void);
void UIDrawBossHealthBar(Boss *boss); // Added declaration for boss health bar

#endif // UI_H
