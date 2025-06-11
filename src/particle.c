#include "particle.h"
#include <math.h>

// Inicializa partículas
void InitParticles(Particle particles[], int maxParticles) {
    // Inicializa todas as partículas como inativas e com um tipo padrão
    for (int i = 0; i < maxParticles; i++) {
        particles[i].active = false;
        particles[i].type = PARTICLE_TYPE_GENERIC_EFFECT; // Tipo padrão
        particles[i].xpValue = 0; // Valor XP padrão
        particles[i].maxLife = 0; // maxLife padrão
    }
}

// Cria uma nova partícula
void SpawnParticle(Particle particles[], ParticleType type, Vector2 pos, Color col, float rad, float ang, float spd, float life, int xpValue) {
    // Encontra um slot de partícula inativa e a configura
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].active = true;
            particles[i].type = type;
            particles[i].position = pos;
            particles[i].color = col;
            particles[i].radius = rad;
            particles[i].angle = ang; // Ângulo em radianos
            particles[i].speed = spd;
            particles[i].life = life;
            particles[i].maxLife = life; // Armazena a vida inicial
            particles[i].xpValue = (type == PARTICLE_TYPE_XP_ORB) ? xpValue : 0; // Somente orbs de XP têm valor de XP
            break;
        }
    }
    // TraceLog(LOG_WARNING, "PARTICLE: No inactive particles to spawn!"); // Log se não houver partículas disponíveis
}

// Atualiza partículas
void UpdateParticles(Particle particles[], int maxParticles) {
    for (int i = 0; i < maxParticles; i++) {
        if (particles[i].active) {
            // Calcula componentes de velocidade baseados no ângulo e velocidade
            float vx = cosf(particles[i].angle) * particles[i].speed;
            float vy = sinf(particles[i].angle) * particles[i].speed;

            particles[i].position.x += vx * GetFrameTime();
            particles[i].position.y += vy * GetFrameTime();
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
            // Usa maxLife para o cálculo do alfa, se maxLife for maior que zero
            float alpha = (particles[i].maxLife > 0) ? (particles[i].life / particles[i].maxLife) : 1.0f;
            Color c = Fade(particles[i].color, alpha);
            DrawCircleV(particles[i].position, particles[i].radius, c);
        }
    }
}
