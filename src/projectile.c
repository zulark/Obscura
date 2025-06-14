#include "projectile.h"
#include "config.h"
#include <raymath.h>
#include <stdio.h>

// Inicializa os projéteis
void InitProjectiles(Projectile projectiles[], int maxProjectiles) {
    for (int i = 0; i < maxProjectiles; i++){
        projectiles[i].active = false;
        projectiles[i].speed = 450.0f; // Padrão, pode ser sobrescrito por ShootProjectile
        projectiles[i].color = DARKBLUE; // Padrão, pode ser sobrescrito por ShootProjectile
        projectiles[i].radius = 10.0f; // Padrão, pode ser sobrescrito por ShootProjectile
        projectiles[i].maxDistance = 1000.0f; // Padrão, pode ser sobrescrito por ShootProjectile
        projectiles[i].traveledDistance = 0.0f;
        projectiles[i].duration = 0.0f; // Padrão, pode ser sobrescrito por ShootProjectile
        projectiles[i].currentFrame = 0;
        projectiles[i].frameTime = 0.0f;
        projectiles[i].frameSpeed = 0.0f; // Inicializa novo campo
        projectiles[i].frameCount = 0;   // Inicializa novo campo
    }
}

// Dispara um projétil
void ShootProjectile(Projectile projectiles[], int projectileIndex, Vector2 startPosition, Vector2 targetPosition, int attackDamage, float speed, float radius, float maxDistance, Color color, float frameSpeed, int frameCount, float duration){
    if (projectileIndex < 0 || projectileIndex >= MAX_PROJECTILES) return;

    projectiles[projectileIndex].position = startPosition;
    projectiles[projectileIndex].active = true;
    projectiles[projectileIndex].damage = attackDamage;
    projectiles[projectileIndex].speed = speed;
    projectiles[projectileIndex].radius = radius;
    projectiles[projectileIndex].maxDistance = maxDistance;
    projectiles[projectileIndex].color = color; // Armazena a cor passada
    projectiles[projectileIndex].traveledDistance = 0.0f;
    projectiles[projectileIndex].duration = duration; // Armazena a duração passada
    projectiles[projectileIndex].currentFrame = 0;
    projectiles[projectileIndex].frameTime = 0.0f;
    projectiles[projectileIndex].frameSpeed = frameSpeed;
    projectiles[projectileIndex].frameCount = frameCount;

    Vector2 direction = Vector2Subtract(targetPosition, startPosition);
    direction = Vector2Normalize(direction);

    projectiles[projectileIndex].velocity = Vector2Scale(direction, projectiles[projectileIndex].speed);
}

// Atualiza projéteis
void UpdateProjectile(Projectile  *projectile){
    if(!projectile->active) return;

    projectile->position.x += projectile->velocity.x * GetFrameTime();
    projectile->position.y += projectile->velocity.y * GetFrameTime();
    
    float frameDistance = Vector2Length(Vector2Scale(projectile->velocity, GetFrameTime()));
    projectile->traveledDistance += frameDistance;

    // Atualização da animação
    // Atualiza animação do projétil
    if (projectile->frameCount > 0 && projectile->frameSpeed > 0.0f) {
        projectile->frameTime += GetFrameTime();
        if (projectile->frameTime >= (1.0f / projectile->frameSpeed)) {
            projectile->frameTime = 0.0f;
            projectile->currentFrame++;
            if (projectile->currentFrame >= projectile->frameCount) {
                projectile->currentFrame = 0; // Loop da animação
                // Para projéteis que não fazem loop (ex: explosões), podem ser desativados aqui
                // ou serem controlados pela duração.
            }
        }
    }

    if (projectile->traveledDistance >= projectile->maxDistance && projectile->maxDistance > 0) { // maxDistance > 0 verifica projéteis que não expiram por distância
        projectile->active = false;
        return;
    }
    if (projectile->duration > 0.0f) {
        projectile->duration -= GetFrameTime();
        if (projectile->duration <= 0.0f) {
            projectile->active = false;
            return;
        }
    }
    
    if (projectile->position.x < 0 - projectile->radius || 
        projectile->position.x > WORLD_WIDTH + projectile->radius ||
        projectile->position.y < 0 - projectile->radius || 
        projectile->position.y > WORLD_HEIGHT + projectile->radius) {
        projectile->active = false;
    }
}

// Desenha projéteis
void DrawProjectile(Projectile projectile, Texture2D *attackFrames, int attackFrameCount, Color tint) {
    if (!projectile.active) return;

    // Usa projectile.frameCount para determinar se é animado e qual frame desenhar
    if (attackFrames && projectile.frameCount > 0 && projectile.currentFrame < attackFrameCount) {
        // Garante que currentFrame está dentro dos limites do array attackFrames fornecido.
        // attackFrameCount aqui é o número de frames carregados para ESTE tipo específico de ataque.
        // projectile.frameCount é o número total de frames para a sequência de animação DESTE projétil.
        // Estes devem ser consistentes se o array de texturas correto for passado.
        
        Texture2D tex = attackFrames[projectile.currentFrame];
        float frameW = (float)tex.width;
        float frameH = (float)tex.height;
        
        float angle = atan2f(projectile.velocity.y, projectile.velocity.x) * (180.0f / PI);
        
        Rectangle sourceRec = {0.0f, 0.0f, frameW, frameH};
        // Ajusta destRec para desenhar a partir do centro do projétil, não do canto superior esquerdo
        Rectangle destRec = {projectile.position.x, projectile.position.y, frameW, frameH};
        Vector2 origin = {frameW/2.0f, frameH/2.0f}; // Origem é o centro do frame

        DrawTexturePro(tex, sourceRec, destRec, origin, angle, tint);
    } else {
        // Alternativa: círculo se não houver frames ou informações de animação
        DrawCircleV(projectile.position, projectile.radius, projectile.color);
    }
}
