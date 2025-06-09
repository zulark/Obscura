#include "enemy.h"
#include "raymath.h"
#include <stdlib.h>
#include "audio.h"

void InitEnemies(Enemy enemies[], int maxEnemies){
    for (int i = 0; i < maxEnemies; i++){
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
void SpawnEnemy(Enemy enemies[], Vector2 spawnPosition, EnemyType type){
    for (int i = 0; i < MAX_ENEMIES; i++){
        if (!enemies[i].active){
            enemies[i].position = spawnPosition;
            enemies[i].active = true;
            enemies[i].type = type;

            //props do inimigo baseado no tipo dele
            switch (type) {
                case ENEMY_TYPE_NORMAL:
                    enemies[i].size = (Vector2){25.0f, 25.0f};
                    enemies[i].speed = 100.0f + (rand() % 20 - 10);
                    enemies[i].color = (Color){ GetRandomValue(180, 220), GetRandomValue(20, 60), GetRandomValue(20, 60), 255 };
                    enemies[i].maxHealth = 20;
                    enemies[i].attackDamage = 20;
                    enemies[i].xpReward = 10;
                    break;
                case ENEMY_TYPE_FAST:
                    enemies[i].size = (Vector2){20.0f, 20.0f};
                    enemies[i].speed = 150.0f + (rand() % 30 - 15);
                    enemies[i].color = (Color){ GetRandomValue(20, 60), GetRandomValue(180, 220), GetRandomValue(20, 60), 255 };
                    enemies[i].maxHealth = 7;
                    enemies[i].attackDamage = 7;
                    enemies[i].xpReward = 15;
                    break;
                case ENEMY_TYPE_STRONG:
                    enemies[i].size = (Vector2){35.0f, 35.0f};
                    enemies[i].speed = 75.0f + (rand() % 20 - 10);
                    enemies[i].color = (Color){ GetRandomValue(20, 60), GetRandomValue(20, 60), GetRandomValue(180, 220), 255 };
                    enemies[i].maxHealth = 40;
                    enemies[i].attackDamage = 35;
                    enemies[i].xpReward = 50;
                    break;
                default:
                    enemies[i].size = (Vector2){25.0f, 25.0f};
                    enemies[i].speed = 100.0f;
                    enemies[i].color = GRAY;
                    enemies[i].maxHealth = 10;
                    enemies[i].attackDamage = 5;
                    enemies[i].xpReward = 5;
                    break;
            }
            enemies[i].health = enemies[i].maxHealth;
            break;
        }
    }
}

// Atualiza lógica do inimigo
void UpdateEnemy(Enemy *enemy, Vector2 targetPosition) {
    if (!enemy->active) return;

    //evita tremer quando colado no jogador
    if (Vector2DistanceSqr(enemy->position, targetPosition) > 1.0f) {
        Vector2 direction = Vector2Subtract(targetPosition, enemy->position);
        direction = Vector2Normalize(direction);
        enemy->position.x += direction.x * enemy->speed * GetFrameTime();
        enemy->position.y += direction.y * enemy->speed * GetFrameTime();
    }
}

// Aplica dano ao inimigo
void TakeDamageEnemy(Enemy *enemy, int damageDealt) {
   if (!enemy->active) return;

    enemy->health -= damageDealt;
    if(enemy->health <= 0){
        enemy->health = 0;
        enemy->active = false;
        AudioSetSoundVolume(SOUND_ENEMY_DEATH, 0.3f);
        AudioPlaySound(SOUND_ENEMY_DEATH);
    }
}

void DrawEnemy(Enemy enemy) {
    if (!enemy.active) return;
    DrawRectangleV(enemy.position, enemy.size, enemy.color);
    if(enemy.health < enemy.maxHealth){
        int healthBarWidth = 50;
        int healthBarHeight = 5;
        Vector2 healthBarPosition = { enemy.position.x + (enemy.size.x - healthBarWidth) / 2, enemy.position.y - 10 };
        DrawRectangleV(healthBarPosition, (Vector2){healthBarWidth, healthBarHeight}, Fade(MAROON, 0.5f));
        DrawRectangleV(healthBarPosition, (Vector2){(float)enemy.health / enemy.maxHealth * healthBarWidth, healthBarHeight}, GREEN);
        DrawRectangleLines(healthBarPosition.x, healthBarPosition.y, healthBarWidth, healthBarHeight, DARKGRAY);
    }
}
