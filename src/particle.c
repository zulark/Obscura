#include "particle.h"
#include <math.h>

// Inicializa partículas
void InitParticles(Particle particles[], int maxParticles) {
    for (int i = 0; i < maxParticles; i++) {
        particles[i].active = false;
    }
}

// Cria uma nova partícula
void SpawnParticle(Particle particles[], Vector2 pos, Color color, float speed, float angle, float radius, float life) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].position = pos;
            particles[i].velocity = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };
            particles[i].radius = radius;
            particles[i].life = life;
            particles[i].maxLife = life;
            particles[i].color = color;
            particles[i].active = true;
            break;
        }
    }
}

// Atualiza partículas
void UpdateParticles(Particle particles[], int maxParticles) {
    for (int i = 0; i < maxParticles; i++) {
        if (particles[i].active) {
            particles[i].position.x += particles[i].velocity.x * GetFrameTime();
            particles[i].position.y += particles[i].velocity.y * GetFrameTime();
            particles[i].life -= GetFrameTime();
            if (particles[i].life <= 0) {
                particles[i].active = false;
            }
        }
    }
}

// Desenha partículas
void DrawParticles(Particle particles[], int maxParticles) {
    for (int i = 0; i < maxParticles; i++) {
        if (particles[i].active) {
            float alpha = particles[i].life / particles[i].maxLife;
            DrawCircleV(particles[i].position, particles[i].radius, Fade(particles[i].color, alpha));
        }
    }
}
