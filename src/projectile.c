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
        projectiles[i].maxDistance = 1000.0f; // Distância máxima padrão
        projectiles[i].traveledDistance = 0.0f; // Distância percorrida inicializada em 0
    }
}

void ShootProjectile(Projectile projectiles[], int projectileIndex, Vector2 startPosition, Vector2 targetPosition){
    if (projectileIndex < 0 || projectileIndex >= MAX_PROJECTILES) return;

    projectiles[projectileIndex].position = startPosition;
    projectiles[projectileIndex].active = true;
    projectiles[projectileIndex].damage = 5;
    projectiles[projectileIndex].speed = 450.0f; // Garante velocidade correta
    projectiles[projectileIndex].radius = 10.0f; // Raio menor para melhor precisão
    projectiles[projectileIndex].traveledDistance = 0.0f; // Reinicia a distância percorrida ao disparar um novo projétil

    Vector2 direction = Vector2Subtract(targetPosition, startPosition);
    direction = Vector2Normalize(direction);

    projectiles[projectileIndex].velocity = Vector2Scale(direction, projectiles[projectileIndex].speed);
    printf("Disparado projétil %d para (%.2f, %.2f) com velocidade (%.2f, %.2f)\n", 
    projectileIndex, targetPosition.x, targetPosition.y, 
    projectiles[projectileIndex].velocity.x, projectiles[projectileIndex].velocity.y);
}

// Atualiza projéteis
void UpdateProjectile(Projectile  *projectile){
    if(!projectile->active) return;

    projectile->position.x += projectile->velocity.x * GetFrameTime();
    projectile->position.y += projectile->velocity.y * GetFrameTime();
    
    float frameDistance = Vector2Length(Vector2Scale(projectile->velocity, GetFrameTime()));
    projectile->traveledDistance += frameDistance;
    if (projectile->traveledDistance >= projectile->maxDistance) {
        projectile->active = false;
        return;
    }
    
    if (projectile->position.x < 0 - projectile->radius || 
        projectile->position.x > WORLD_WIDTH + projectile->radius ||
        projectile->position.y < 0 - projectile->radius || 
        projectile->position.y > WORLD_HEIGHT + projectile->radius) {
        projectile->active = false;
    }
}

// Desenha projéteis
void DrawProjectile(Projectile projectile){
    if(!projectile.active) return;
    // DrawTexture(projectile.texture, (int)projectile.position.x - projectile.radius, (int)projectile.position.y - projectile.radius, projectile.color);
    DrawCircle((int)projectile.position.x, (int)projectile.position.y, projectile.radius, Fade(projectile.color, 0.5f));
}
