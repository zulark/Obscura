#ifndef BOSS_H
#define BOSS_H
#include "enemy.h"
#include "player.h"
#include "particle.h"
#include "projectile.h"

// Inicializa o boss (pode customizar stats, fase, etc)
void InitBoss(Enemy *boss, Vector2 position);

// Atualiza IA, ataques e comportamento do boss
void UpdateBoss(Enemy *boss, Player *player, Particle *particles, Projectile *projectiles, int maxProjectiles);

// Desenha efeitos especiais do boss (opcional)
void DrawBossEffects(const Enemy *boss);

// Spawna ataques especiais do boss
void BossSpecialAttack(Enemy *boss, Player *player, Particle *particles, Projectile *projectiles, int maxProjectiles);

#endif
