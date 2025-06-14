#ifndef BOSS_PROJECTILE_H
#define BOSS_PROJECTILE_H
#include "config.h" // Added to ensure dt is defined
#include "raylib.h"
#include <stdbool.h>

typedef struct BossProjectile {
    Vector2 position;
    Vector2 direction;
    Vector2 finalDirection; // direção real após carregar
    float speed;
    float radius;
    int damage;
    bool active;
    // Animação
    Texture2D *frames;
    int frameCount;
    int currentFrame;
    float frameTime;
    float frameSpeed;
    // Telegraph
    bool charging; // true = parado, false = atirando
    float delayTimer; // tempo de "aviso" antes de disparar
} BossProjectile;

void InitBossProjectiles();
void SpawnBossProjectileAnimTelegraph(Vector2 pos, Vector2 dir, float speed, float radius, int damage, Texture2D *frames, int frameCount, float animSpeed, float delay);
void UpdateBossProjectiles(float deltaTime);
void DrawBossProjectiles();
void ClearBossProjectiles();

extern BossProjectile bossProjectiles[];

#endif // BOSS_PROJECTILE_H
