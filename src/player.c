#include "player.h"
#include "enemy.h"
#include "config.h"
#include "particle.h"
#include <raylib.h>
#include "audio.h"

Player InitPlayer(float startX, float startY)
{
    Player player;
    player.position = (Vector2){startX, startY};
    player.size = (Vector2){50.0f, 50.0f};
    player.color = BLACK;
    player.speed = 200.0f;
    player.maxHealth = 100;
    player.health = player.maxHealth;
    player.maxMana = 100;
    player.mana = player.maxMana;
    player.experience = 0;
    player.level = 1;
    player.skillPoints = 0;
    player.alive = true;
    player.shootCooldown = 0.35f;
    player.shootTimer = 0.0;
    player.invencibilityTimer = 0.0f;
    player.levelUpTextTimer = 0.0f;
    player.levelUpArcTimer = 0.0f;
    player.levelUpArcProgress = 0.0f;
    player.sprite = LoadTexture("assets/sprites/player.png");
    player.barrierActive = false;
    player.attackSpeed = 600.0f; // velocidade do projétil (pixels/seg)
    player.attackRange = 400.0f;
    player.attackDamage = 25;
    return player;
}

// movimentação
void HandlePlayerInput(Player *player)
{
    Vector2 direction = {0.0f, 0.0f};

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        direction.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        direction.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        direction.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        direction.x += 1.0f;

    if (direction.x != 0.0f || direction.y != 0.0f)
    {
        direction = Vector2Normalize(direction);
        player->position.x += direction.x * player->speed * dt;
        player->position.y += direction.y * player->speed * dt;
    }

    // Limita o jogador dentro do mundo maior
    if (player->position.y < 0)
        player->position.y = 0;
    if (player->position.y + player->size.y > WORLD_HEIGHT)
        player->position.y = WORLD_HEIGHT - player->size.y;
    if (player->position.x < 0)
        player->position.x = 0;
    if (player->position.x + player->size.x > WORLD_WIDTH)
        player->position.x = WORLD_WIDTH - player->size.x;
}

void PlayerTryShoot(Player *player, Projectile projectiles[], Camera2D camera)
{
    if (player->shootTimer > 0)
        player->shootTimer -= GetFrameTime();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsKeyDown(KEY_Q))
    {
        if (player->shootTimer <= 0)
        {
            player->shootTimer = player->shootCooldown;
            Vector2 mouseScreen = GetMousePosition();
            Vector2 targetPosition = GetScreenToWorld2D(mouseScreen, camera);
            Vector2 shootPosition = {player->position.x + player->size.x / 2, player->position.y + player->size.y / 2};
            for (int i = 0; i < MAX_PROJECTILES; i++)
            {
                if (!projectiles[i].active)
                {
                    ShootProjectile(projectiles, i, shootPosition, targetPosition, player->attackDamage);
                    // projectiles[i].color = SKYBLUE;
                    projectiles[i].speed = player->attackSpeed; // velocidade fixa do projétil
                    projectiles[i].maxDistance = player->attackRange;
                    projectiles[i].traveledDistance = 0.0f;
                    AudioSetSoundVolume(SOUND_SHOOT, 0.6f);
                    AudioPlaySound(SOUND_SHOOT);
                    break;
                }
            }
        }
    }
}

// Adiciona controle de dano por tipo de inimigo
static bool enemyTypeHit[ENEMY_TYPE_COUNT] = {0};

void ResetEnemyTypeHit()
{
    for (int i = 0; i < ENEMY_TYPE_COUNT; i++)
        enemyTypeHit[i] = false;
}

void TakeDamagePlayer(Player *player, int damageAmount, EnemyType type)
{
    if (player->alive == false)
        return;
    // Corrigido: barreira mágica ignora dano completamente
    if (player->barrierActive)
    {
        return;
    }
    if (player->invencibilityTimer > 0.0f && enemyTypeHit[type])
        return;
    if (player->invencibilityTimer <= 0.0f)
    {
        for (int i = 0; i < ENEMY_TYPE_COUNT; i++)
            enemyTypeHit[i] = false;
    }
    enemyTypeHit[type] = true;
    player->invencibilityTimer = 1.0f;
    if (player->health > 0)
    {
        player->health -= damageAmount;
        TraceLog(LOG_INFO, "JOGADOR tomou %d de dano, vida: %d/%d", damageAmount, player->health, player->maxHealth);
        AudioPlaySound(SOUND_PLAYER_HIT);
        // Floating text de dano
        char txt[32];
        snprintf(txt, sizeof(txt), "-%d HP", damageAmount);
        float textWidth = MeasureText(txt, 22);
        Vector2 pos = {(screenWidth / 2.0f) - (textWidth / 2.0f), screenHeight / 2.0f - 50.0f};
        UIShowFloatingMsg(txt, pos, RED, 1.0f);
        if (player->health <= 0)
        {
            player->alive = false;
            player->health = 0;
            TraceLog(LOG_INFO, "JOGADOR MORREU!");
        }
    }
}

// Atualiza lógica do jogador
void UpdatePlayer(Player *player)
{
    HandlePlayerInput(player);
    if (player->invencibilityTimer > 0.0f)
    {
        player->invencibilityTimer -= dt;
        if (player->invencibilityTimer < 0.0f)
        {
            player->invencibilityTimer = 0.0f;
            player->barrierActive = false;
        }
    }

    // Atualiza timers de level up
    if (player->levelUpTextTimer > 0.0f)
    {
        player->levelUpTextTimer -= dt;
        if (player->levelUpTextTimer < 0.0f)
            player->levelUpTextTimer = 0.0f;
    }
    if (player->levelUpArcTimer > 0.0f)
    {
        player->levelUpArcTimer -= dt;
        // Progresso de 0.0 a 1.0
        player->levelUpArcProgress = 1.0f - (player->levelUpArcTimer / 0.7f);
        if (player->levelUpArcTimer < 0.0f)
        {
            player->levelUpArcTimer = 0.0f;
            player->levelUpArcProgress = 0.0f;
        }
    }
}

// Desenha o jogador
void DrawPlayer(Player player)
{
    if (!player.alive)
        return;
    Color playerDrawColor = WHITE;

    if (player.invencibilityTimer > 0.0f)
    {
        if (((int)(GetTime() * 10.0f)) % 2 == 0)
        {
            playerDrawColor = Fade(player.color, 0.3f);
        }
    }

    if (player.sprite.id > 0)
    {
        Rectangle sourceRec = {0.0f, 0.0f, (float)player.sprite.width, (float)player.sprite.height};
        Rectangle destRec = {player.position.x, player.position.y, player.size.x, player.size.y};
        Vector2 origin = {0.0f, 0.0f};
        float rotation = 0.0f;
        DrawTexturePro(player.sprite, sourceRec, destRec, origin, rotation, playerDrawColor);
    }
    else
    {
        DrawRectangleV(player.position, player.size, playerDrawColor);
    }

    // Efeito visual da barreira: bolha translúcida
    if (player.barrierActive)
    {
        Vector2 center = {player.position.x + player.size.x / 2, player.position.y + player.size.y / 2};
        float barrierRadius = player.size.x * 1.5f;
        Color barrierColor = (Color){80, 180, 255, 90}; // Azul claro translúcido
        DrawCircleV(center, barrierRadius, barrierColor);
    }
}

// Função para adicionar XP ao jogador
// A lógica de verificar o level up e chamar PlayerLevelUp agora está em PlayerCollectXPOrbs
void PlayerGainXP(Player *player, int xp)
{
    if (!player || !player->alive)
    {
        return;
    }
    player->experience += xp;
    // Log para depuração (opcional, remover em produção)
    // TraceLog(LOG_INFO, "Player gained %d XP. Total XP: %d", xp, player->experience); 
}

void PlayerCollectXPOrbs(Player *player, Particle *particles, int maxParticles, Enemy *enemies, int maxEnemies) {
    for (int i = 0; i < maxParticles; i++) {
        // Modificado para verificar o tipo da partícula em vez da cor
        if (particles[i].active && particles[i].type == PARTICLE_TYPE_XP_ORB) { 
            Vector2 playerCenter = {player->position.x + player->size.x / 2, player->position.y + player->size.y / 2};
            float dist = Vector2Distance(particles[i].position, playerCenter);
            // O raio de coleta pode ser ajustado. Usar o raio da partícula e um pouco do tamanho do jogador.
            float collectRadius = player->size.x * 0.6f + particles[i].radius; 
            if (dist < collectRadius) {
                int xpGained = particles[i].xpValue; // Pega o XP do orb
                player->experience += xpGained; 
                particles[i].active = false;
                char xpText[16];
                snprintf(xpText, sizeof(xpText), "+%d XP", xpGained);
                UIShowFloatingMsg(xpText, particles[i].position, GOLD, 1.0f);
                AudioPlaySound(SOUND_MAGIC_PICKUP);
                
                // Checa level up
                int xpToNextLevel = 50 + (player->level - 1) * 35;
                while (player->experience >= xpToNextLevel) {
                    player->experience -= xpToNextLevel;
                    PlayerLevelUp(player, enemies, maxEnemies);
                    xpToNextLevel = 50 + (player->level - 1) * 35; 
                }
            }
        }
    }
}

// Ao upar, apenas recupera vida/mana, não aumenta atributos automaticamente
void PlayerLevelUp(Player *player, Enemy enemies[], int maxEnemies)
{
    player->level++;
    player->health = player->maxHealth;
    player->mana = player->maxMana;
    player->skillPoints++;
    player->invencibilityTimer = 2.0f; // 2 segundos de invencibilidade
    float killRadius = 350.0f;
    for (int i = 0; i < maxEnemies; i++)
    {
        if (enemies[i].active)
        {
            float dist = Vector2Distance(player->position, enemies[i].position);
            if (dist < killRadius)
            {
                enemies[i].health = 0;
                enemies[i].active = false;
                AudioPlaySound(SOUND_ENEMY_DEATH);
            }
        }
    }
    player->levelUpTextTimer = 1.5f;
    player->levelUpArcTimer = 0.7f;
    player->levelUpArcProgress = 0.0f;
    AudioPlaySound(SOUND_LEVELUP);
    AudioSetSoundVolume(SOUND_LEVELUP, 0.3f);
}

void DrawPlayerLevelUpEffects(Player *player)
{
    // Arco azul
    if (player->levelUpArcTimer > 0.0f)
    {
        float centerX = player->position.x + player->size.x / 2;
        float centerY = player->position.y + player->size.y / 2;
        float minRadius = player->size.x * 0.7f;
        float maxRadius = player->size.x * 1.7f;
        float radius = minRadius + (maxRadius - minRadius) * player->levelUpArcProgress;
        int arcCount = 3;
        for (int i = 0; i < arcCount; i++)
        {
            float startAngle = 120.0f * i + player->levelUpArcProgress * 60.0f;
            DrawRing((Vector2){centerX, centerY}, radius, radius + 6, startAngle, startAngle + 60, 64, Fade(BLUE, 180));
        }
    }
    // Texto
    if (player->levelUpTextTimer > 0.0f)
    {
        float alpha = (player->levelUpTextTimer > 0.3f) ? 1.0f : player->levelUpTextTimer / 0.3f;
        int fontSize = 32;
        const char *txt = "LEVEL UP!";
        int textWidth = MeasureText(txt, fontSize);
        float centerX = player->position.x + player->size.x / 2;
        float y = player->position.y - 32 - (1.0f - alpha) * 20.0f;
        DrawText(txt, (int)(centerX - textWidth / 2), (int)y, fontSize, Fade(SKYBLUE, (unsigned char)(255 * alpha)));
    }
}

// Menu simples de upgrades para ser chamado entre waves
// void PlayerUpgradeMenu(Player *player) { // This function is now deprecated and its logic will be moved to ui.c
//     if (player->skillPoints <= 0) return;
//     int running = 1;
//     while (running && player->skillPoints > 0) {
//         // Exemplo: printa no console, pode ser adaptdado para UI futuramente
//         TraceLog(LOG_INFO, "\n--- UPGRADES DISPONÍVEIS ---");
//         TraceLog(LOG_INFO, "Skillpoints: %d", player->skillPoints);
//         TraceLog(LOG_INFO, "1) +20 Vida Máxima (atual: %d)", player->maxHealth);
//         TraceLog(LOG_INFO, "2) +10 Mana Máxima (atual: %d)", player->maxMana);
//         TraceLog(LOG_INFO, "3) -10%% no tempo entre tiros (atual: %.2fs)", player->shootCooldown);
//         TraceLog(LOG_INFO, "4) +80 Alcance do Projétil (atual: %.0f)", player->attackRange);
//         TraceLog(LOG_INFO, "5) Sair do menu");
//         TraceLog(LOG_INFO, "Escolha uma opção (1-5):");
//         int opt = 0;
//         scanf("%d", &opt); // Para protótipo, depois trocar por UI
//         switch(opt) {
//             case 1:
//                 player->maxHealth += 20;
//                 player->health = player->maxHealth;
//                 player->skillPoints--;
//                 break;
//             case 2:
//                 player->maxMana += 10;
//                 player->mana = player->maxMana;
//                 player->skillPoints--;
//                 break;
//             case 3:
//                 player->shootCooldown *= 0.9f; // Reduz 10% o tempo entre tiros
//                 player->skillPoints--;
//                 break;
//             case 4:
//                 player->attackRange += 80.0f;
//                 player->skillPoints--;
//                 break;
//             case 5:
//                 running = 0;
//                 break;
//             default:
//                 TraceLog(LOG_INFO, "Opção inválida!");
//         }
//     }
// }
