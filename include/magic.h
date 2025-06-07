#ifndef MAGIC_H
#define MAGIC_H
#include "player.h"
#include "particle.h"

#define MAGIC_SLOT_COUNT 3

typedef enum {
    MAGIC_BARRIER,
    MAGIC_AREA_ATTACK,
    MAGIC_DASH,
    MAGIC_COUNT
} MagicID;

typedef struct {
    MagicID id;
    const char *name;
    float cooldown;
    float currentCooldown;
    int unlocked;
} Magic;

void MagicInit(Player *player);
void MagicUpdate(Player *player, Particle *particles, float *cooldowns);
void MagicCast(Player *player, int slot, Particle *particles, Vector2 targetPos, Enemy *enemies, int maxEnemies);
int MagicIsUnlocked(int magicId);
void MagicUnlock(int magicId);

#endif // MAGIC_H
