#include "boss.h"
#include "raymath.h"
#include <raylib.h>
#include <stdio.h>

void InitBoss(Enemy *boss, Vector2 position) {
    boss->active = true;
    boss->type = ENEMY_TYPE_BOSS;
    boss->position = position;
    boss->size = (Vector2){256, 256};
    boss->color = PURPLE;
    boss->speed = 60.0f;
    boss->maxHealth = 2000;
    boss->health = boss->maxHealth;
    boss->attackDamage = 50;
    boss->xpReward = 1000;
    boss->currentFrame = 0;
    boss->frameTime = 0.0f;
    boss->frameSpeed = 1.0f / 6.0f;
    boss->maxFrames = 8;
    boss->specialAttackTimer = 0.0f;
    boss->bossPhase = 1;
    boss->isChargingAttack = false;
    boss->chargeProgress = 0.0f;
}

void UpdateBoss(Enemy *boss, Player *player, Particle *particles, Projectile *projectiles, int maxProjectiles) {
    //ia básica: segue o player
    Vector2 direction = Vector2Subtract(player->position, boss->position);
    float distance = Vector2Length(direction);
    if (distance > 0) {
        direction = Vector2Normalize(direction);
        boss->position.x += direction.x * boss->speed * GetFrameTime();
        boss->position.y += direction.y * boss->speed * GetFrameTime();
    }
    // ataques especiais
    boss->specialAttackTimer += GetFrameTime();
    if (!boss->isChargingAttack && boss->specialAttackTimer > 3.0f) {
        boss->isChargingAttack = true;
        boss->chargeProgress = 0.0f;
    }
    if (boss->isChargingAttack) {
        boss->chargeProgress += GetFrameTime();
        if (boss->chargeProgress > 1.5f) {
            BossSpecialAttack(boss, player, particles, projectiles, maxProjectiles);
            boss->isChargingAttack = false;
            boss->specialAttackTimer = 0.0f;
        }
    }
    // fases: pode mudar padrão de ataque conforme vida
    if (boss->health < boss->maxHealth * 0.5f) boss->bossPhase = 2;
    if (boss->health < boss->maxHealth * 0.2f) boss->bossPhase = 3;
}

void DrawBossEffects(const Enemy *boss) {
    // xemplo: círculo de energia ao carregar ataque
    if (boss->isChargingAttack) {
        DrawCircleLines(boss->position.x + boss->size.x/2, boss->position.y + boss->size.y/2, 120 + 40 * boss->chargeProgress, Fade(RED, 0.5f));
    }
}

void BossSpecialAttack(Enemy *boss, Player *player, Particle *particles, Projectile *projectiles, int maxProjectiles) {
    // Exemplo: ataque em área radial
    Vector2 center = {boss->position.x + boss->size.x/2, boss->position.y + boss->size.y/2};
    for (int i = 0; i < 16; i++) {
        float angle = (2 * PI * i) / 16.0f;
        Vector2 pos = {center.x + cosf(angle) * 100, center.y + sinf(angle) * 100};
        SpawnParticle(particles, PARTICLE_TYPE_GENERIC_EFFECT, pos, ORANGE, 24.0f, angle, 80.0f, 1.0f, 0);
    }
    // Exemplo: chuva de projéteis
    for (int i = 0; i < maxProjectiles; i++) {
        if (!projectiles[i].active) {
            float angle = GetRandomValue(0, 360) * (PI/180.0f);
            Vector2 dir = {cosf(angle), sinf(angle)};
            projectiles[i].position = center;
            projectiles[i].velocity = Vector2Scale(dir, 300.0f);
            projectiles[i].active = true;
            projectiles[i].damage = boss->attackDamage / 2;
            projectiles[i].radius = 12.0f;
            projectiles[i].speed = 300.0f;
            projectiles[i].traveledDistance = 0.0f;
            break; // só um projétil por chamada, para não lotar
        }
    }
    // TODO: adicionar mais ataques mirabolantes
}
