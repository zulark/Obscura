#include <string.h>
#include <stdio.h>
#include <math.h>
#include "config.h"
static Magic magics[MAGIC_COUNT] = {
    {MAGIC_AREA_ATTACK, "Ataque em Área", 5.0f, 0.0f, 1},
    {MAGIC_BARRIER, "Barreira", 5.0f, 0.0f, 1},
    {MAGIC_DASH, "Dash", 5.0f, 0.0f, 1}
};

void MagicInit(Player *player) {
    for (int i = 0; i < MAGIC_COUNT; i++) {
        magics[i].currentCooldown = 0.0f;
    }
}

void MagicUpdate(Player *player, Particle *particles, float *cooldowns) {
    for (int i = 0; i < MAGIC_COUNT; i++) {
        if (magics[i].currentCooldown > 0.0f) magics[i].currentCooldown -= GetFrameTime();
        if (magics[i].currentCooldown < 0.0f) magics[i].currentCooldown = 0.0f;
        if (cooldowns) cooldowns[i] = magics[i].currentCooldown;
    }
    // Recupera mana gradualmente (5 em 5 por segundo)
    static float manaRegenTimer = 0.0f;
    manaRegenTimer += GetFrameTime();
    if (manaRegenTimer >= 1.0f) {
        manaRegenTimer -= 1.0f;
        if (player->mana < player->maxMana) {
            player->mana += 1;
            if (player->mana > player->maxMana) player->mana = player->maxMana;
        }
    }
}

void MagicCast(Player *player, int slot, Particle *particles, Vector2 targetPos, Enemy *enemies, int maxEnemies) {
    printf("DEBUG: MagicCast chamada (slot=%d)\n", slot);
    if (slot < 0 || slot >= MAGIC_COUNT) return;
    if (magics[slot].currentCooldown > 0.0f) {
        AudioPlaySound(SOUND_NO_MANA);
        return;
    }
    // Novo: custo de mana variável por magia (exemplo: 30 para área)
    int manaCost = 0;
    switch (magics[slot].id) {
        case MAGIC_AREA_ATTACK: manaCost = 30; break;
        case MAGIC_BARRIER: manaCost = 20; break;
        case MAGIC_DASH: manaCost = 15; break;
        default: manaCost = 20; break;
    }
    
    if (player->mana < manaCost) {
        AudioPlaySound(SOUND_NO_MANA);
        Vector2 cursor = GetMousePosition();
        UIShowFloatingMsg("Sem mana!", cursor, SKYBLUE, 1.0f);
        return;
    }
    player->mana -= manaCost;
    if (magics[slot].id == MAGIC_AREA_ATTACK) {
        printf("DEBUG: Entrou no bloco MAGIC_AREA_ATTACK\n");
        // Efeito visual: círculo de fogo animado
        float areaRadius = 80.0f; // Ajuste para combinar com o efeito visual
        for (int i = 0; i < 32; i++) {
            float angle = (2 * PI * i) / 32.0f;
            float radius = 64.0f + GetRandomValue(-8, 8);
            Vector2 pos = {
                targetPos.x + cosf(angle) * radius + GetRandomValue(-6, 6),
                targetPos.y + sinf(angle) * radius + GetRandomValue(-6, 6)
            };
            Color fireColor = (Color){255, GetRandomValue(80, 160), 0, 220};
            printf("DEBUG: SpawnParticle fogo (i=%d)\n", i);
            SpawnParticle(particles, pos, fireColor, 18.0f + GetRandomValue(-4, 4), angle, 36.0f, 0.8f + GetRandomValue(0, 10)/20.0f);
        }
        printf("DEBUG: Fim do loop de fogo\n");
        // Círculo central
        for (int i = 0; i < 16; i++) {
            float angle = (2 * PI * i) / 16.0f;
            Color fireColor = (Color){255, GetRandomValue(120, 180), 40, 200};
            printf("DEBUG: SpawnParticle centro (i=%d)\n", i);
            SpawnParticle(particles, targetPos, fireColor, 24.0f + GetRandomValue(-8, 8), angle, 48.0f, 1.0f + GetRandomValue(0, 10)/20.0f);
        }
        printf("DEBUG: Fim do loop de centro\n");
        // DEBUG: círculo vermelho visível e persistente
        for (int i = 0; i < 4; i++) {
            float angle = (2 * PI * i) / 4.0f;
            Vector2 pos = {
                targetPos.x + cosf(angle) * 64.0f,
                targetPos.y + sinf(angle) * 64.0f
            };
            printf("DEBUG: SpawnParticle vermelho (i=%d)\n", i);
            SpawnParticle(particles, pos, RED, 0.0f, 0.0f, 18.0f, 0.7f);
        }
        // Dano em área
        for (int i = 0; i < maxEnemies; i++) {
            if (enemies[i].active && enemies[i].health > 0) {
                Vector2 enemyCenter = {
                    enemies[i].position.x + enemies[i].size.x/2,
                    enemies[i].position.y + enemies[i].size.y/2
                };
                float dist = Vector2Distance(enemyCenter, targetPos);
                if (dist <= areaRadius) {
                    TakeDamageEnemy(&enemies[i], enemies[i].health); // Mata instantaneamente
                }
            }
        }
        printf("MAGIA DE FOGO criada em (%.1f, %.1f)\n", targetPos.x, targetPos.y);
        AudioSetSoundVolume(SOUND_PLAYER_MAGIC_AREA, 1.0f); // Garante volume máximo
        AudioPlaySound(SOUND_PLAYER_MAGIC_AREA);
        printf("DEBUG: AudioPlaySound(SOUND_PLAYER_MAGIC_AREA) chamado\n");
    } else if (magics[slot].id == MAGIC_BARRIER) {
        // Barreira: invulnerabilidade por 2s + efeito visual
        player->invencibilityTimer = 2.0f;
        player->barrierActive = true;
        // Efeito visual: círculo translúcido fixo ao redor do player
        Vector2 center = {player->position.x + player->size.x/2, player->position.y + player->size.y/2};
        float barrierRadius = player->size.x * 1.5f;
        // Armazena o tempo de barreira para desenhar na UI (pode ser um campo no player, mas aqui só para efeito visual imediato)
        DrawCircleV(center, barrierRadius, (Color){80, 180, 255, 100}); // Azul claro translúcido
        UIShowFloatingMsg("Barreira!", (Vector2){center.x, player->position.y - 32}, SKYBLUE, 1.2f);
        AudioPlaySound(SOUND_MAGIC_CAST);
    } else if (magics[slot].id == MAGIC_DASH) {
        // Dash: move rapidamente na direção do mouse, invulnerável por 0.5s
        Vector2 playerCenter = {player->position.x + player->size.x/2, player->position.y + player->size.y/2};
        Vector2 dashDir = Vector2Normalize(Vector2Subtract(targetPos, playerCenter));
        float dashDist = 220.0f;
        Vector2 dashTarget = Vector2Add(player->position, Vector2Scale(dashDir, dashDist));
        // Limita dentro do mundo
        dashTarget.x = fmaxf(0, fminf(WORLD_WIDTH - player->size.x, dashTarget.x));
        dashTarget.y = fmaxf(0, fminf(WORLD_HEIGHT - player->size.y, dashTarget.y));
        player->position = dashTarget;
        player->invencibilityTimer = 0.5f;
        // Efeito visual: rastro
        for (int i = 0; i < 12; i++) {
            float t = (float)i / 12.0f;
            Vector2 pos = Vector2Lerp(playerCenter, dashTarget, t);
            Color c = Fade(YELLOW, 0.5f - t*0.4f);
            SpawnParticle(particles, pos, c, 16.0f, 0, 5.0f, 0.5f);
        }
        UIShowFloatingMsg("Dash!", (Vector2){player->position.x + player->size.x/2, player->position.y - 32}, WHITE, 1.0f);
        AudioPlaySound(SOUND_MAGIC_CAST);
    }
    magics[slot].currentCooldown = magics[slot].cooldown;
}

int MagicIsUnlocked(int magicId) {
    if (magicId < 0 || magicId >= MAGIC_COUNT) return 0;
    return magics[magicId].unlocked;
}

void MagicUnlock(int magicId) {
    if (magicId < 0 || magicId >= MAGIC_COUNT) return;
    magics[magicId].unlocked = 1;
}
