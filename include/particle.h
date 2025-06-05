#ifndef PARTICLE_H
#define PARTICLE_H
#define MAX_PARTICLES 256

#include "raylib.h"

typedef struct Particle {
    Vector2 position;
    Vector2 velocity;
    float radius;
    float life;
    float maxLife;
    Color color;
    bool active;
} Particle;

void InitParticles(Particle particles[], int maxParticles);
void SpawnParticle(Particle particles[], Vector2 pos, Color color, float speed, float angle, float radius, float life);
void UpdateParticles(Particle particles[], int maxParticles);
void DrawParticles(Particle particles[], int maxParticles);

#endif
