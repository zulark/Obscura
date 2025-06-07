#ifndef UI_H
#define UI_H
#include "player.h"
#include <raylib.h>

void UIInit();
void UIDrawHUD(const Player *player);
void UIDrawHotkeyBar(int width, int height, const float *cooldowns, int numSlots);
void UIDrawMenu(int selected);
void UIDrawGameOver();
void UIShowMessage(const char *msg, float duration);
void UIShowFloatingMsg(const char *msg, Vector2 pos, Color color, float duration);
void UIUpdateFloatingMsgs(void);
void UIDrawFloatingMsgs(void);

#endif // UI_H
