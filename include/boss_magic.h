#ifndef BOSS_MAGIC_H
#define BOSS_MAGIC_H
#include "config.h" // Added to ensure dt is defined
#include "raylib.h"
#include <stdbool.h>

typedef struct BossMagic {
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
    float timer;
    // Telegraph
    bool charging;
    float delayTimer;
} BossMagic;

void InitBossMagics();
void SpawnBossMagicAnimTelegraph(Vector2 pos, Vector2 dir, float speed, float radius, int damage, Texture2D *frames, int frameCount, float animSpeed, float duration, float delay);
void UpdateBossMagics(float deltaTime);
void DrawBossMagics();
void ClearBossMagics();

extern BossMagic bossMagics[];

#endif // BOSS_MAGIC_H
