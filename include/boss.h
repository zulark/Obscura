#ifndef BOSS_H
#define BOSS_H
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include "audio.h"
#include "player.h"
#include "projectile.h" // Para interações com projéteis
#include "particle.h"   // Para efeitos de partículas

#define BOSS_FADE_IN_DURATION 1.5f
#define BOSS_BLINK_DURATION 2.0f
#define BOSS_BLINK_INTERVAL 0.15f
#define BOSS_PHASE_TRANSITION_DELAY 1.0f

// Estados da IA para a Fase 1 do Boss
typedef enum {
    BOSS_IA_P1_IDLE,
    BOSS_IA_P1_MOVING,
    BOSS_IA_P1_CHASING,
    BOSS_IA_P1_SHOOTING_BASIC,
    BOSS_IA_P1_CHARGING_BURST,
    BOSS_IA_P1_SHOOTING_BURST,
    BOSS_IA_P1_PREPARING_DASH,
    BOSS_IA_P1_DASHING,
    // Fase 2 - Estados de magia
    BOSS_IA_P2_MAGIC_FLOOR_CHARGE,
    BOSS_IA_P2_MAGIC_FLOOR_CAST,
    BOSS_IA_P2_MAGIC_PUSH_CHARGE,
    BOSS_IA_P2_MAGIC_PUSH_CAST
} BossAIState_Phase1;

typedef struct Boss
{
    Vector2 position;
    Vector2 size;
    float health;
    float maxHealth;
    float speed;
    int attackDamage; // Adicionado novamente
    int phase; // 1 ou 2
    bool active;
    bool isPerformingSpecialAttack; // Flag para indicar animação de ataque especial
    float specialAttackTimer;       // Temporizador para duração do ataque especial
    bool isChargingAttack; // Adicionado novamente
    float chargeProgress; // Adicionado novamente
    int currentFrame; // Adicionado novamente
    float frameTime; // Adicionado novamente
    float frameSpeed; // Adicionado novamente
    int maxFrames; // Adicionado novamente
    Color color; // Adicionado novamente
    bool isFacingRight; // Adicionado novamente

    // Novos campos para transições visuais
    float alpha;                // Alpha atual para fade (0.0 a 1.0)
    bool isFadingIn;            // Verdadeiro se estiver fazendo fade in
    float fadeTimer;            // Temporizador para efeito de fade
    bool isBlinking;            // Verdadeiro se estiver piscando (ex: ao morrer na fase 1)
    float blinkTimer;           // Duração total do efeito de piscar
    float blinkToggleTimer;     // Temporizador para alternar visibilidade durante o piscar
    bool visible;               // Visibilidade principal, usada por DrawBoss e para piscar
    float phaseTransitionTimer; // Temporizador de atraso entre morte da fase 1 e início da fase 2
    bool isActiveInLogic;       // Indica se o boss deve ser processado em UpdateBoss (para atividade geral)
    
    // Campos para IA e Padrões de Ataque - Fase 1
    BossAIState_Phase1 aiState_p1;
    float actionTimer;          // Temporizador para a ação atual (ex: duração de um movimento, carga de ataque)
    float shootCooldown;        // Cooldown para o ataque básico de projétil
    float burstCooldown;        // Cooldown para o ataque de rajada de projéteis
    float dashCooldown;         // Cooldown para o ataque de investida
    float magicFloorCooldown;   // Cooldown para magia de chão (fase 2)
    float magicPushCooldown;    // Cooldown para magia de empurrão gravitacional (fase 2)
    Vector2 targetPosition;     // Posição alvo para movimento ou ataques
    int burstShotsFired;        // Contador para disparos na rajada
    float attackAnimTimer; // Tempo restante para exibir o sprite de ataque
    // Adicione outros campos específicos do boss aqui
    Vector2 floorMagicTargetPos; // Posição do telegraph do ataque de chão
} Boss;

Boss InitBoss(float startX, float startY); 
// Inicializa o boss (pode customizar stats, fase, etc)
void DrawBoss(Boss boss, Texture2D *frames, int frameCount, Color tint);
void SpawnBoss(Boss *boss, Vector2 spawnPosition);
// Atualiza IA, ataques e comportamento do boss
void UpdateBoss(Boss *boss, Player *player, Projectile projectiles[], int maxProjectiles, Particle particles[], int maxParticles);
void BossTakeDamage(Boss *boss, int damage);
bool CheckCollisionBossProjectile(Boss boss, Projectile projectile);

// // Desenha efeitos especiais do boss (opcional)
// void DrawBossEffects(const Boss *boss);

// // Spawna ataques especiais do boss
// void BossSpecialAttack(Boss *boss, Player *player, Particle *particles, Projectile *projectiles, int maxProjectiles);
// void BossAttack_RadialBurst(Boss *boss, Player *player, Particle *particles, Projectile *projectiles, int maxProjectiles);
// void BossAttack_Charge(Boss *boss, Player *player, Particle *particles);

#endif
