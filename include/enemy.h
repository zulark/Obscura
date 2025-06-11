#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"
#include "particle.h"

#define MAX_ENEMIES 80

typedef enum EnemyType {
    ENEMY_TYPE_NORMAL,
    ENEMY_TYPE_FAST,
    ENEMY_TYPE_STRONG,
    ENEMY_TYPE_BOSS,
    ENEMY_TYPE_COUNT 
} EnemyType;

typedef struct Enemy {
    Vector2 position;
    Vector2 previousPosition; // Adicionado para rastrear a direção do movimento
    Vector2 size;
    float speed;
    Color color;
    bool active;
    int health;
    int maxHealth;
    int attackDamage;
    float attackCooldown;
    float attackTimer;
    float attackRange;
    float attackSpeed;
    EnemyType type;
    int xpReward;
    bool facingRight; // Adicionado para controlar a direção do sprite
    int currentFrame;
    float frameTime;
    float frameSpeed;
    int maxFrames;
    // Campos para boss
    float specialAttackTimer; // Timer para ataques especiais
    int bossPhase; // Fase do boss (pode mudar padrão de ataque)
    bool isChargingAttack; // Se está carregando ataque especial
    float chargeProgress; // Progresso do carregamento
} Enemy;

void InitEnemies(Enemy enemies[], int maxEnemies);

void SpawnEnemy(Enemy enemies[], Vector2 spawnPosition, EnemyType type); 

void UpdateEnemy(Enemy *enemy, Vector2 targetPosition);

void TakeDamageEnemy(Enemy *enemy, int damageDealt, Particle *particles);

void DrawEnemy(Enemy enemy, Texture2D *idleFrames, int idleFrameCount, Color tint);

#endif