#ifndef ENEMY_H
#define ENEMY_H
#define MAX_ENEMIES 150
#include "raylib.h"
#include "particle.h"

typedef enum EnemyType {
    ENEMY_TYPE_NORMAL,
    ENEMY_TYPE_FAST,
    ENEMY_TYPE_STRONG,
    ENEMY_TYPE_COUNT
} EnemyType;

typedef struct Enemy {
    Vector2 position;
    Vector2 previousPosition;
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
    bool facingRight;
    int currentFrame;
    float frameTime;
    float frameSpeed;
    int maxFrames;
    // Removidos campos de boss
} Enemy;

void InitEnemies(Enemy enemies[], int maxEnemies);
void SpawnEnemy(Enemy enemies[], Vector2 spawnPosition, EnemyType type);
void UpdateEnemy(Enemy *enemy, Vector2 targetPosition);
void TakeDamageEnemy(Enemy *enemy, int damageDealt, Particle *particles);
void DrawEnemy(Enemy enemy, Texture2D *idleFrames, int idleFrameCount, Color tint);

#endif