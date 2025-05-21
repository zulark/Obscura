#include "projectile.h"
#include "config.h"   // Para SCREEN_WIDTH, SCREEN_HEIGHT se precisar para desativar projéteis fora da tela
#include <raymath.h> // Para Vector2Normalize, Vector2Scale, Vector2Subtract, etc.
#include <stdio.h>   // Para printf (debug, se necessário)

void InitProjectiles(Projectile projectiles[], int maxProjectiles) {
    for (int i = 0; i < maxProjectiles; i++){
        projectiles[i].active = false;
        projectiles[i].radius = 5.0f;
        projectiles[i].speed = 450.0f;
        projectiles[i].color = DARKBLUE;
    }
}

void ShootProjectile(Projectile projectiles[], int projectileIndex, Vector2 startPosition, Vector2 targetPosition){
    if (projectileIndex < 0 || projectileIndex >= MAX_PROJECTILES) return;

    projectiles[projectileIndex].position = startPosition;
    projectiles[projectileIndex].active = true;

    Vector2 direction = Vector2Subtract(targetPosition, startPosition);
    direction = Vector2Normalize(direction);

    projectiles[projectileIndex].velocity = Vector2Scale(direction, projectiles[projectileIndex].speed);
    printf("Disparado projétil %d para (%.2f, %.2f) com velocidade (%.2f, %.2f)\n", 
    projectileIndex, targetPosition.x, targetPosition.y, 
    projectiles[projectileIndex].velocity.x, projectiles[projectileIndex].velocity.y);
}

void UpdateProjectile(Projectile  *projectile){
    if(!projectile->active) return;

    projectile->position.x += projectile->velocity.x * GetFrameTime();
    projectile->position.y += projectile->velocity.y * GetFrameTime();
    
    if (projectile->position.x < 0 - projectile->radius || 
        projectile->position.x > screenWidth + projectile->radius ||
        projectile->position.y < 0 - projectile->radius || 
        projectile->position.y > screenHeight + projectile->radius) {
        projectile->active = false;
    }
}

void DrawProjectile(Projectile projectile){
    if(!projectile.active) return;
    DrawCircleV(projectile.position, projectile.radius, projectile.color);
}
