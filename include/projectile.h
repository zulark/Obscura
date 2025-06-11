#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <raylib.h>

#define MAX_PROJECTILES 100

typedef struct Projectile {
    Vector2 position;
    Vector2 velocity;
    Texture2D texture;
    float radius;
    float speed;
    int damage;
    bool active;
    Color color;
    float maxDistance; // range máximo
    float traveledDistance; // distância já percorrida
} Projectile;

void ShootProjectile(Projectile projectiles[], int projectileIndex, Vector2 startPosition, Vector2 targetPosition, int attackDamage);

void UpdateProjectile(Projectile *projectile);

void DrawProjectile(Projectile projectile, Texture2D *attackFrames, int attackFrameCount, Color tint);

void InitProjectiles(Projectile projectiles[], int maxProjectiles);

#endif