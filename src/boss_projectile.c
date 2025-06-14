#include "boss_projectile.h"
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <math.h>

#define MAX_BOSS_PROJECTILES 64

BossProjectile bossProjectiles[MAX_BOSS_PROJECTILES];

void InitBossProjectiles() {
    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++) {
        bossProjectiles[i].active = false;
    }
}

// OBS: Use apenas SpawnBossProjectileAnimTelegraph para projéteis do boss (com ou sem delay, basta passar delay=0 para instantâneo)

void SpawnBossProjectileAnimTelegraph(Vector2 pos, Vector2 dir, float speed, float radius, int damage, Texture2D *frames, int frameCount, float animSpeed, float delay) {
    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++) {
        if (!bossProjectiles[i].active) {
            bossProjectiles[i].active = true;
            bossProjectiles[i].position = pos;
            bossProjectiles[i].direction = (Vector2){0,0}; // parado até acabar o delay
            bossProjectiles[i].finalDirection = Vector2Normalize(dir);
            bossProjectiles[i].speed = speed;
            bossProjectiles[i].radius = radius;
            bossProjectiles[i].damage = damage;
            bossProjectiles[i].frames = frames;
            bossProjectiles[i].frameCount = frameCount;
            bossProjectiles[i].currentFrame = 0;
            bossProjectiles[i].frameTime = 0.0f;
            bossProjectiles[i].frameSpeed = animSpeed;
            bossProjectiles[i].charging = true;
            bossProjectiles[i].delayTimer = delay;
            printf("[DEBUG] BossProjectile spawned at (%.1f, %.1f) with delay %.2f\n", pos.x, pos.y, delay);
            break;
        }
    }
}

void UpdateBossProjectiles(float deltaTime) {
    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++) {
        if (bossProjectiles[i].active) {
            // Telegraph: só começa a andar após delay
            if (bossProjectiles[i].charging) {
                bossProjectiles[i].delayTimer -= deltaTime;
                if (bossProjectiles[i].delayTimer <= 0) {
                    bossProjectiles[i].charging = false;
                    bossProjectiles[i].direction = bossProjectiles[i].finalDirection;
                }
            }
            if (!bossProjectiles[i].charging) {
                bossProjectiles[i].position = Vector2Add(bossProjectiles[i].position, Vector2Scale(bossProjectiles[i].direction, bossProjectiles[i].speed * deltaTime));
            }
            // Animação
            bossProjectiles[i].frameTime += deltaTime;
            if (bossProjectiles[i].frameTime >= bossProjectiles[i].frameSpeed && bossProjectiles[i].frameCount > 0) {
                bossProjectiles[i].frameTime = 0.0f;
                bossProjectiles[i].currentFrame = (bossProjectiles[i].currentFrame + 1) % bossProjectiles[i].frameCount;
            }
            // Desativa se sair da tela (ajuste conforme limites do mundo)
            if (bossProjectiles[i].position.x < -100 || bossProjectiles[i].position.x > WORLD_WIDTH+100 || bossProjectiles[i].position.y < -100 || bossProjectiles[i].position.y > WORLD_HEIGHT+100) {
                bossProjectiles[i].active = false;
            }
        }
    }
}

void DrawBossProjectiles() {
    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++) {
        if (bossProjectiles[i].active) {
            // Desenha círculo de debug para área de colisão
            DrawCircleV(bossProjectiles[i].position, bossProjectiles[i].radius + 6, Fade(RED, 0.3f));
            if (bossProjectiles[i].frames && bossProjectiles[i].frameCount > 0) {
                // Centraliza e escala o sprite de acordo com o raio, usando o maior lado para encaixar perfeitamente
                Texture2D tex = bossProjectiles[i].frames[bossProjectiles[i].currentFrame];
                float maxDim = (tex.width > tex.height) ? tex.width : tex.height;
                float scale = (bossProjectiles[i].radius * 2) / (float)maxDim;
                Vector2 drawPos = {
                    bossProjectiles[i].position.x - tex.width * scale / 2,
                    bossProjectiles[i].position.y - tex.height * scale / 2
                };
                DrawTextureEx(tex, drawPos, 0.0f, scale, WHITE);
            } else {
                DrawCircleV(bossProjectiles[i].position, bossProjectiles[i].radius, SKYBLUE);
            }
        }
    }
}

void ClearBossProjectiles() {
    for (int i = 0; i < MAX_BOSS_PROJECTILES; i++) {
        bossProjectiles[i].active = false;
    }
}
