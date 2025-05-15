#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"

typedef struct Player {
    Vector2 position;
    Vector2 size;
    float speed;
    Color color;
} Player;

Player InitPlayer(float startX, float startY); 

void HandlePlayerInput(Player *player);

void UpdatePlayer(Player *player);

void DrawPlayer(Player player);

#endif 


