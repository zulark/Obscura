#include "projectile.h"
#include "config.h"
#include <raymath.h>
#include <stdio.h>


void InitProjectiles(Projectile projectiles[], int maxProjectiles) {
    for (int i = 0; i < maxProjectiles; i++){
        projectiles[i].active = false;
        projectiles[i].speed = 450.0f;
        projectiles[i].color = DARKBLUE;
        // projectiles[i].texture = LoadTexture("assets/sprites/projectile.png");
        projectiles[i].radius = 30.0f;
    }
}

void ShootProjectile(Projectile projectiles[], int projectileIndex, Vector2 startPosition, Vector2 targetPosition){
    if (projectileIndex < 0 || projectileIndex >= MAX_PROJECTILES) return;

    projectiles[projectileIndex].position = startPosition;
    projectiles[projectileIndex].active = true;
    projectiles[projectileIndex].damage = 5;

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
    // DrawTexture(projectile.texture, (int)projectile.position.x - projectile.radius, (int)projectile.position.y - projectile.radius, projectile.color);
    DrawCircle((int)projectile.position.x, (int)projectile.position.y, projectile.radius, Fade(projectile.color, 0.5f));
}
