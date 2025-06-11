#include <stdlib.h>
#include "enemy.h"
#include "raymath.h"
#include "audio.h"
#include "particle.h"

void InitEnemies(Enemy enemies[], int maxEnemies)
{
    for (int i = 0; i < maxEnemies; i++)
    {
        enemies[i].active = false;
        enemies[i].size = (Vector2){25.0f, 25.0f};
        enemies[i].speed = 100.0f;
        enemies[i].color = RED;
        enemies[i].maxHealth = 10;
        enemies[i].health = enemies[i].maxHealth;
        enemies[i].attackDamage = 5;
    }
}

// Spawna inimigo
void SpawnEnemy(Enemy enemies[], Vector2 position, EnemyType type)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
        {
            Enemy enemy = {0}; // Inicializa para zerar todos os campos
            enemy.active = true;
            enemy.position = position;
            enemy.previousPosition = position; 
            enemy.type = type;
            enemy.facingRight = (GetRandomValue(0, 1) == 1);

            switch (type)
            {
            case ENEMY_TYPE_NORMAL:                    // Demônio (Normal)
                enemy.size = (Vector2){67.0f, 100.0f}; // Tamanho do sprite do demônio
                enemy.color = RED;
                enemy.speed = 100.0f;
                enemy.maxHealth = 100;
                enemy.health = enemy.maxHealth;
                enemy.attackDamage = 25;
                enemy.xpReward = 50;
                enemy.currentFrame = 0;
                enemy.frameTime = 0.0f;
                enemy.frameSpeed = 1.0f / 8.0f; // Animação a 8 FPS
                enemy.maxFrames = 4;            // 4 quadros de animação
                break;
            case ENEMY_TYPE_FAST:                     // Demônio (Rápido, Menor)
                enemy.size = (Vector2){64.0f, 64.0f}; // Sprite de demônio menor
                enemy.color = YELLOW;
                enemy.speed = 150.0f; // Movimento mais rápido
                enemy.maxHealth = 70;
                enemy.health = enemy.maxHealth;
                enemy.attackDamage = 15; // Dano menor
                enemy.xpReward = 30;     // Menos XP
                enemy.currentFrame = 0;
                enemy.frameTime = 0.0f;
                enemy.frameSpeed = 1.0f / 12.0f; // Animação a 12 FPS
                enemy.maxFrames = 4;             // 4 quadros de animação
                break;
            case ENEMY_TYPE_STRONG:                    // Slime (Forte)
                enemy.size = (Vector2){90.0f, 132.0f}; // Tamanho do sprite do slime
                enemy.color = DARKGREEN;               // Cor de slime mais forte
                enemy.speed = 50.0f;                   // Movimento mais lento
                enemy.maxHealth = 200;
                enemy.health = enemy.maxHealth;
                enemy.attackDamage = 30; // Dano maior
                enemy.xpReward = 40;     // Mais XP
                enemy.currentFrame = 0;
                enemy.frameTime = 0.0f;
                enemy.frameSpeed = 1.0f / 8.0f; // Animação a 8 FPS
                enemy.maxFrames = 4;            // 4 quadros de animação
                break;
            case ENEMY_TYPE_BOSS:
                enemy.size = (Vector2){256.0f, 256.0f}; // Ajuste conforme sprite
                enemy.color = PURPLE;
                enemy.speed = 60.0f;
                enemy.maxHealth = 2000;
                enemy.health = enemy.maxHealth;
                enemy.attackDamage = 50;
                enemy.xpReward = 1000;
                enemy.currentFrame = 0;
                enemy.frameTime = 0.0f;
                enemy.frameSpeed = 1.0f / 6.0f;
                enemy.maxFrames = 8; // Supondo 8 sprites
                // Inicializa campos de boss
                enemy.specialAttackTimer = 0.0f;
                enemy.bossPhase = 1;
                enemy.isChargingAttack = false;
                enemy.chargeProgress = 0.0f;
                break;
            default:
                enemies[i].size = (Vector2){25.0f, 25.0f};
                enemies[i].speed = 100.0f;
                enemies[i].color = GRAY;
                enemies[i].maxHealth = 10;
                enemies[i].health = enemies[i].maxHealth;
                enemies[i].attackDamage = 5;
                enemies[i].xpReward = 5;
                // Animação
                enemies[i].currentFrame = 0;
                enemies[i].frameTime = 0.0f;
                enemies[i].frameSpeed = 0.15f;
                enemies[i].maxFrames = 1;
                break;
            }
            enemies[i] = enemy;
            break;
        }
    }
}

// Atualiza lógica do inimigo
void UpdateEnemy(Enemy *enemy, Vector2 targetPosition)
{
    if (!enemy->active)
        return;

    enemy->previousPosition = enemy->position; // Guarda a posição antes de mover

    // Movimento em direção ao jogador
    Vector2 direction = Vector2Subtract(targetPosition, enemy->position);
    float distance = Vector2Length(direction);
    if (distance > 0)
    { // Evita divisão por zero se já estiver no destino
        direction = Vector2Normalize(direction);
        enemy->position.x += direction.x * enemy->speed * GetFrameTime();
        enemy->position.y += direction.y * enemy->speed * GetFrameTime();
    }

    // Atualiza a animação para inimigos que a possuem (maxFrames > 0)
    if (enemy->maxFrames > 0)
    {
        enemy->frameTime += GetFrameTime();
        if (enemy->frameTime >= enemy->frameSpeed)
        {
            enemy->frameTime = 0.0f;
            enemy->currentFrame++;
            if (enemy->currentFrame >= enemy->maxFrames)
            {
                enemy->currentFrame = 0;
            }
        }
    }

    // Atualiza a direção do inimigo com base no movimento
    if (enemy->position.x > enemy->previousPosition.x)
    {
        enemy->facingRight = true;
    }
    else if (enemy->position.x < enemy->previousPosition.x)
    {
        enemy->facingRight = false;
    }
    // Se não houve movimento horizontal, mantém a direção anterior

    // Lógica especial para boss
    if (enemy->type == ENEMY_TYPE_BOSS)
    {
        // Exemplo: timer para ataque especial
        enemy->specialAttackTimer += GetFrameTime();
        if (!enemy->isChargingAttack && enemy->specialAttackTimer > 3.0f)
        {
            enemy->isChargingAttack = true;
            enemy->chargeProgress = 0.0f;
            // Aqui pode iniciar animação de carregamento
        }
        if (enemy->isChargingAttack)
        {
            enemy->chargeProgress += GetFrameTime();
            if (enemy->chargeProgress > 1.5f)
            {
                // Dispara ataque especial (ex: área, chuva de projéteis, etc)
                // TODO: Implementar Spawn de partículas/projéteis/minions
                enemy->isChargingAttack = false;
                enemy->specialAttackTimer = 0.0f;
            }
        }
        // TODO: Implementar mudança de fase, ataques diferentes, etc
    }
}

// Aplica dano ao inimigo
void TakeDamageEnemy(Enemy *enemy, int damageDealt, Particle *particles)
{
    if (!enemy->active)
        return;

    enemy->health -= damageDealt;
    if (enemy->health <= 0)
    {
        enemy->health = 0;
        enemy->active = false;
        AudioSetSoundVolume(SOUND_ENEMY_DEATH, 0.3f);
        AudioPlaySound(SOUND_ENEMY_DEATH);
        Vector2 pos = {enemy->position.x + enemy->size.x / 2, enemy->position.y + enemy->size.y / 2};
        // Passa o xpReward do inimigo para o orb de XP
        SpawnParticle(particles, PARTICLE_TYPE_XP_ORB, pos, GOLD, 12.0f, 0.0f, 0.0f, 20.0f, enemy->xpReward);
    }
}

void DrawEnemy(Enemy enemy, Texture2D *idleFrames, int idleFrameCount, Color tint)
{
    if (!idleFrames || idleFrameCount == 0)
    {
        // Fallback para desenhar um retângulo se não houver sprites
        DrawRectangleV(enemy.position, enemy.size, enemy.color);
        return;
    }

    Texture2D currentFrameTexture = idleFrames[enemy.currentFrame % idleFrameCount];
    float frameW = (float)currentFrameTexture.width;
    float frameH = (float)currentFrameTexture.height;

    // Calcula a posição e escala para desenhar o sprite centralizado no hitbox, usando o size do inimigo
    float destW = enemy.size.x;
    float destH = enemy.size.y;
    float drawX = enemy.position.x;
    float drawY = enemy.position.y;
    Rectangle sourceRec = {0.0f, 0.0f, frameW, frameH};
    Rectangle destRec = {drawX, drawY, destW, destH};
    Vector2 origin = {0.0f, 0.0f};
    if (!enemy.facingRight)
    {
        sourceRec.width = -frameW;
    }
    DrawTexturePro(currentFrameTexture, sourceRec, destRec, origin, 0.0f, tint);

    // Desenhar hitbox para depuração (opcional)
    // DrawRectangleLines(enemy.position.x, enemy.position.y, enemy.size.x, enemy.size.y, LIME);
    // Barra de vida
    if (enemy.health < enemy.maxHealth && enemy.type != ENEMY_TYPE_BOSS)
    {
        int healthBarWidth = 50;
        int healthBarHeight = 5;
        Vector2 healthBarPosition = {enemy.position.x + (enemy.size.x - healthBarWidth) / 2, enemy.position.y - 10};
        DrawRectangleV(healthBarPosition, (Vector2){healthBarWidth, healthBarHeight}, Fade(MAROON, 0.5f));
        DrawRectangleV(healthBarPosition, (Vector2){(float)enemy.health / enemy.maxHealth * healthBarWidth, (float)healthBarHeight}, GREEN);
        DrawRectangleLines((int)healthBarPosition.x, (int)healthBarPosition.y, healthBarWidth, healthBarHeight, DARKGRAY);
    }
}
