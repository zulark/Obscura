#ifndef UI_H
#define UI_H
#include "player.h"
#include <raylib.h>

// Define the structure for the Upgrade Menu
typedef struct UpgradeMenuState {
    int visible;
    int selectedOption;
    int confirmState; // Or other relevant fields
} UpgradeMenuState;

void UIInit();
void UIDrawHUD(const Player *player, int currentWave, int enemiesAlive, float waveCooldownTimer, bool inWave);
void UIDrawHotkeyBar(int width, int height, const float *cooldowns, int numSlots);
// Changed UIDrawMenu to accept parameters needed for drawing the main menu
void UIDrawMenu(int selectedOption, float hudAlpha, float titleOffset);
void UIDrawGameOver();
void UIShowMessage(const char *msg, float duration);
void UIShowFloatingMsg(const char *msg, Vector2 pos, Color color, float duration);
void UIUpdateFloatingMsgs(void);
void UIDrawFloatingMsgs(void);

// Declarations for the Upgrade Menu functions
void UIDrawUpgradeMenu(const Player *player, const UpgradeMenuState *menuState);
int UIHandleUpgradeMenuInput(Player *player, UpgradeMenuState *menuState); 
void UIDrawPauseScreen(void);

#endif // UI_H
