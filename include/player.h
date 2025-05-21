#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include "projectile.h"

typedef struct Player {
    Vector2 position;
    Vector2 size;
    float speed;
    Color color;
    float shootCooldown;
    float shootTimer;
} Player;

Player InitPlayer(float startX, float startY); 

void HandlePlayerInput(Player *player);

void UpdatePlayer(Player *player);

void DrawPlayer(Player player);

void PlayerTryShoot(Player *player, Projectile projectiles[]);

#endif 


