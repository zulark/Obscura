#ifndef UI_H
#define UI_H
#include "player.h"

void UIInit();
void UIDrawHUD(const Player *player);
void UIDrawHotkeyBar(int screenWidth, int screenHeight, const float *cooldowns, int numSlots);
void UIDrawMenu(int selected);
void UIDrawGameOver();
void UIShowMessage(const char *msg, float duration);

#endif // UI_H
