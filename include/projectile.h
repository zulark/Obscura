#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <raylib.h>

#define MAX_PROJECTILES 100

typedef struct Projectile {
    Vector2 position;
    Vector2 velocity;
    float radius;
    float speed;
    float speedScalar;       // magnitude pré-calculada de velocity (evita sqrtf por frame)
    int damage;
    bool active;
    Color color;
    float maxDistance;       // range máximo
    float traveledDistance;  // distância já percorrida
    float duration;          // duração opcional para ataques especiais
    int currentFrame;
    float frameTime;
    float frameSpeed;        // velocidade da animação (frames por segundo)
    int frameCount;          // total de frames na animação
} Projectile;

void ShootProjectile(Projectile projectiles[], int projectileIndex, Vector2 startPosition, Vector2 targetPosition, 
                     int attackDamage, float speed, float radius, 
                     float maxDistance, Color color, float frameSpeed, int frameCount, float duration); // Adicionado duration aqui também

void UpdateProjectile(Projectile *projectile);

void DrawProjectile(Projectile projectile, Texture2D *attackFrames, int attackFrameCount, Color tint);

void InitProjectiles(Projectile projectiles[], int maxProjectiles);

#endif