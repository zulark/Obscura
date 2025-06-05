#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <raylib.h>

#define MAX_PROJECTILES 50

typedef struct Projectile {
    Vector2 position;
    Vector2 velocity;
    Texture2D texture;
    float radius;
    float speed;
    int damage;
    bool active;
    Color color;
} Projectile;

void ShootProjectile(Projectile projectiles[], int projectileIndex, Vector2 startPosition, Vector2 targetPosition);

void UpdateProjectile(Projectile *projectile);

void DrawProjectile(Projectile projectile);

void InitProjectiles(Projectile projectiles[], int maxProjectiles);

#endif