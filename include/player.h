#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include "projectile.h"

typedef struct Player {
    Vector2 position;
    Vector2 size;
    int health;
    int maxHealth;
    int mana;
    int maxMana;
    int experience;
    int level;
    int skillPoints; // Pontos de habilidade para skill tree
    bool alive;
    Color color;
    float speed;
    float shootCooldown;
    float shootTimer;
    float invencibilityTimer;
    // struct SkillTree *skills; // Para integração futura
} Player;

Player InitPlayer(float startX, float startY); 

void HandlePlayerInput(Player *player);

void UpdatePlayer(Player *player);

void DrawPlayer(Player player);

void TakeDamagePlayer(Player *player, int damageDealt);

void PlayerGainXP(Player *player, int xp);

void PlayerLevelUp(Player *player);

void PlayerTryShoot(Player *player, Projectile projectiles[]);

#endif


