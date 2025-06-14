#include "boss_magic.h"
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <math.h>

#define MAX_BOSS_MAGICS 32

BossMagic bossMagics[MAX_BOSS_MAGICS];

void InitBossMagics()
{
    for (int i = 0; i < MAX_BOSS_MAGICS; i++)
    {
        bossMagics[i].active = false;
    }
}

// OBS: Use apenas SpawnBossMagicAnimTelegraph para magias do boss (com ou sem delay, basta passar delay=0 para instantâneo)
void SpawnBossMagicAnimTelegraph(Vector2 pos, Vector2 dir, float speed, float radius, int damage, Texture2D *frames, int frameCount, float animSpeed, float duration, float delay)
{
    for (int i = 0; i < MAX_BOSS_MAGICS; i++)
    {
        if (!bossMagics[i].active)
        {
            bossMagics[i].active = true;
            bossMagics[i].position = pos;
            bossMagics[i].direction = (Vector2){0, 0}; // parado até acabar o delay
            bossMagics[i].finalDirection = Vector2Normalize(dir);
            bossMagics[i].speed = speed;
            bossMagics[i].radius = radius;
            bossMagics[i].damage = damage;
            bossMagics[i].frames = frames;
            bossMagics[i].frameCount = frameCount;
            bossMagics[i].currentFrame = 0;
            bossMagics[i].frameTime = 0.0f;
            bossMagics[i].frameSpeed = animSpeed;
            bossMagics[i].timer = duration;
            bossMagics[i].charging = true;
            bossMagics[i].delayTimer = delay;
            printf("[DEBUG] BossMagic spawned at (%.1f, %.1f) with delay %.2f\n", pos.x, pos.y, delay);
            break;
        }
    }
}

void UpdateBossMagics(float deltaTime)
{
    for (int i = 0; i < MAX_BOSS_MAGICS; i++)
    {
        if (bossMagics[i].active)
        {
            if (bossMagics[i].charging)
            {
                bossMagics[i].delayTimer -= deltaTime;
                if (bossMagics[i].delayTimer <= 0)
                {
                    bossMagics[i].charging = false;
                    bossMagics[i].direction = bossMagics[i].finalDirection;
                }
            }
            else
            {
                bossMagics[i].position = Vector2Add(bossMagics[i].position, Vector2Scale(bossMagics[i].direction, bossMagics[i].speed * deltaTime));
            }
            // Animação
            bossMagics[i].frameTime += deltaTime;
            if (bossMagics[i].frameTime >= bossMagics[i].frameSpeed && bossMagics[i].frameCount > 0)
            {
                bossMagics[i].frameTime = 0.0f;
                bossMagics[i].currentFrame = (bossMagics[i].currentFrame + 1) % bossMagics[i].frameCount;
            }
            bossMagics[i].timer -= deltaTime;
            if (bossMagics[i].timer <= 0)
            {
                bossMagics[i].active = false;
            }
        }
    }
}

void DrawBossMagics()
{
    for (int i = 0; i < MAX_BOSS_MAGICS; i++)
    {
        if (bossMagics[i].active)
        {
            if (bossMagics[i].charging && bossMagics[i].frames == NULL)
            {
                // Telegraph: campo vermelho de aviso
                DrawCircleV(bossMagics[i].position, bossMagics[i].radius, Fade(RED, 0.45f));
            }
            else if (bossMagics[i].frames && bossMagics[i].frameCount > 0)
            {
                // Apenas sprite animada, sem círculo vermelho
                Texture2D tex = bossMagics[i].frames[bossMagics[i].currentFrame];
                float maxDim = (tex.width > tex.height) ? tex.width : tex.height;
                float scale = (bossMagics[i].radius * 2) / (float)maxDim;
                Vector2 drawPos = {
                    bossMagics[i].position.x - tex.width * scale / 2,
                    bossMagics[i].position.y - tex.height * scale / 2};
                DrawTextureEx(tex, drawPos, 0.0f, scale, WHITE);
            }
            else
            {
                // Fallback: círculo roxo
                DrawCircleV(bossMagics[i].position, bossMagics[i].radius, PURPLE);
            }
        }
    }
}

void ClearBossMagics()
{
    for (int i = 0; i < MAX_BOSS_MAGICS; i++)
    {
        bossMagics[i].active = false;
    }
}
