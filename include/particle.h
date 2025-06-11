#ifndef PARTICLE_H
#define PARTICLE_H
#define MAX_PARTICLES 512

#include "raylib.h"

// Adicionado ParticleType enum
typedef enum {
    PARTICLE_TYPE_GENERIC_EFFECT,
    PARTICLE_TYPE_XP_ORB,
    PARTICLE_TYPE_PROJECTILE_TRAIL,
    PARTICLE_TYPE_HEAL_EFFECT ,
    PARTICLE_TYPE_BARRIER_EFFECT, 
    // Adicionar mais tipos conforme necessário
} ParticleType;

typedef struct Particle {
    Vector2 position;
    Color color;
    float radius;
    float speed;
    float angle; 
    float life;  
    bool active;
    ParticleType type; // Novo campo
    int xpValue; // Novo campo para armazenar o valor de XP do orb
    float maxLife; // Novo campo para armazenar a vida inicial para cálculo de alfa
    // Texture2D texture; // Opcional: para futuras partículas com textura
    // float rotation;    // Opcional: para futuras partículas com textura
    // float frame;       // Opcional: para animação de sprite
} Particle;

void InitParticles(Particle particles[], int maxParticles);
void SpawnParticle(Particle particles[], ParticleType type, Vector2 pos, Color col, float rad, float ang, float spd, float life, int xpValue);
void UpdateParticles(Particle particles[], int maxParticles);
void DrawParticles(Particle particles[], int maxParticles);

#endif
