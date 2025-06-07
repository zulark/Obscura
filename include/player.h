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
    int skillPoints; // Pontos de habilidade para skill tree
    bool alive;
    Color color;
    Texture2D sprite;
    float speed;
    float shootCooldown;
    float shootTimer;
    float invencibilityTimer;
    bool barrierActive; // true se a invencibilidade for da barreira
    float levelUpTextTimer; // Tempo restante para mostrar o texto de level up
    float levelUpArcTimer;  // Tempo restante para mostrar o arco azul
    float levelUpArcProgress; // Progresso do arco (0.0 a 1.0)
    // struct SkillTree *skills; // Para integração futura
} Player;

Player InitPlayer(float startX, float startY); 

void HandlePlayerInput(Player *player);

void UpdatePlayer(Player *player);

void DrawPlayer(Player player);

void TakeDamagePlayer(Player *player, int damageDealt);

void PlayerGainXP(Player *player, int xp, Enemy enemies[], int maxEnemies);

void PlayerLevelUp(Player *player, Enemy enemies[], int maxEnemies);

void PlayerTryShoot(Player *player, Projectile projectiles[], Camera2D camera);

void DrawPlayerLevelUpEffects(Player *player);

void DrawHotkeyBar(int width, int height, float cooldowns[], int numSlots);

#endif


