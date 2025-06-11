#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include "projectile.h"
#include "enemy.h"
#include "particle.h"

typedef struct Player {
    Vector2 position;
    Vector2 size;
    int health;
    int maxHealth;
    int mana;
    int maxMana;
    int experience;
    int level;
    int skillPoints;
    bool alive;
    Color color;
    Texture2D sprite;
    float speed;
    float shootCooldown;
    float shootTimer;
    float invencibilityTimer;
    bool barrierActive;
    float levelUpTextTimer;
    float levelUpArcTimer;
    float levelUpArcProgress;
    float attackSpeed;
    float attackRange;
    int attackDamage;
    int currentFrame;
    float frameTime;
    float frameSpeed;
    int maxFrames;
    bool facingRight; // Novo campo para direção do sprite
    // struct SkillTree *skills; // Para integração futura
} Player;

Player InitPlayer(float startX, float startY); 

void HandlePlayerInput(Player *player);

void UpdatePlayer(Player *player);

void DrawPlayer(Player player, Texture2D *frames, int frameCount);

void TakeDamagePlayer(Player *player, int damageDealt, EnemyType type);

void PlayerGainXP(Player *player, int xp);

void PlayerLevelUp(Player *player, Enemy enemies[], int maxEnemies);

void PlayerTryShoot(Player *player, Projectile projectiles[], Camera2D camera);

void DrawPlayerLevelUpEffects(Player *player);

void PlayerCollectXPOrbs(Player *player, Particle *particles, int maxParticles, Enemy *enemies, int maxEnemies);

#endif


