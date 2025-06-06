#include "magic.h"
#include <string.h>

static Magic magics[MAGIC_COUNT] = {
    {MAGIC_BARRIER, "Barreira", 5.0f, 0.0f, 1},
    {MAGIC_AREA_ATTACK, "Ataque em Área", 5.0f, 0.0f, 1},
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
}

void MagicCast(Player *player, int slot, Particle *particles) {
    if (slot < 0 || slot >= MAGIC_COUNT) return;
    if (magics[slot].currentCooldown > 0.0f) return;
    // Aqui você implementa o efeito da magia
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
