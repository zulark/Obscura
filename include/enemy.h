#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

#define MAX_ENEMIES 30

typedef enum EnemyType {
    ENEMY_TYPE_NORMAL,
    ENEMY_TYPE_FAST,
    ENEMY_TYPE_STRONG,
    ENEMY_TYPE_COUNT 
} EnemyType;

typedef struct Enemy {
    Vector2 position;
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
    int xpReward; // XP concedido ao ser derrotado

} Enemy;

void InitEnemies(Enemy enemies[], int maxEnemies);

void SpawnEnemy(Enemy enemies[], Vector2 spawnPosition, EnemyType type); 

void UpdateEnemy(Enemy *enemy, Vector2 targetPosition);

void TakeDamageEnemy(Enemy *enemy, int damageDealt);

void DrawEnemy(Enemy enemy);

#endif