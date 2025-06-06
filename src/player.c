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
    // player.sprite = LoadTexture("assets/sprites/player.png");
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

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if (player->shootTimer <= 0)
        {
            player->shootTimer = player->shootCooldown;
            // Corrige para pegar a posição do mouse no mundo, não na tela
            Vector2 mouseScreen = GetMousePosition();
            Vector2 targetPosition = GetScreenToWorld2D(mouseScreen, camera);
            Vector2 shootPosition = {player->position.x + player->size.x / 2, player->position.y + player->size.y / 2};
            for (int i = 0; i < MAX_PROJECTILES; i++)
            {
                if (!projectiles[i].active)
                {
                    ShootProjectile(projectiles, i, shootPosition, targetPosition);
                    projectiles[i].color = DARKPURPLE;
                    AudioSetSoundVolume(SOUND_SHOOT, 0.6f);
                    AudioPlaySound(SOUND_SHOOT);
                    break;
                }
            }
        }
    }
}

void TakeDamagePlayer(Player *player, int damageAmount)
{
    if (player->alive == false)
        return;

    if (player->invencibilityTimer > 0.0f)
        return;

    if (player->health > 0)
    {
        player->health -= damageAmount;
        player->invencibilityTimer = 1.0f;
        TraceLog(LOG_INFO, "JOGADOR tomou %d de dano, vida: %d/%d", damageAmount, player->health, player->maxHealth);
        AudioPlaySound(SOUND_PLAYER_HIT);
        if (player->health <= 0)
        {
            player->alive = false;
            player->health = 0;
            TraceLog(LOG_INFO, "JOGADOR MORREU!");
        }
    }
}

void UpdatePlayer(Player *player)
{
    HandlePlayerInput(player);
    if (player->invencibilityTimer > 0.0f)
    {
        player->invencibilityTimer -= dt;
        if (player->invencibilityTimer < 0.0f)
        {
            player->invencibilityTimer = 0.0f;
        }
    }

    // Atualiza timers de level up
    if (player->levelUpTextTimer > 0.0f) {
        player->levelUpTextTimer -= dt;
        if (player->levelUpTextTimer < 0.0f) player->levelUpTextTimer = 0.0f;
    }
    if (player->levelUpArcTimer > 0.0f) {
        player->levelUpArcTimer -= dt;
        // Progresso de 0.0 a 1.0
        player->levelUpArcProgress = 1.0f - (player->levelUpArcTimer / 0.7f);
        if (player->levelUpArcTimer < 0.0f) {
            player->levelUpArcTimer = 0.0f;
            player->levelUpArcProgress = 0.0f;
        }
    }
}

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

    // if (player.sprite.id > 0)
    // {
    //     Rectangle sourceRec = {0.0f, 0.0f, (float)player.sprite.width, (float)player.sprite.height};
    //     Rectangle destRec = {player.position.x, player.position.y, player.size.x, player.size.y};
    //     Vector2 origin = {0.0f, 0.0f};
    //     float rotation = 0.0f;
    //     DrawTexturePro(player.sprite, sourceRec, destRec, origin, rotation, playerDrawColor);
    // }
    // else
    // {
    DrawRectangleV(player.position, player.size, playerDrawColor);
    // }
}

void PlayerGainXP(Player *player, int xp, Enemy enemies[], int maxEnemies)
{
    player->experience += xp;
    int xpToNextLevel = 50 + (player->level - 1) * 50;
    // Ganha níveis enquanto tiver XP suficiente
    while (player->experience >= xpToNextLevel)
    {
        player->experience -= xpToNextLevel;
        PlayerLevelUp(player, enemies, maxEnemies);
        xpToNextLevel = 50 + (player->level - 1) * 50;
    }
}

void PlayerLevelUp(Player *player, Enemy enemies[], int maxEnemies){
    player->level++;
    player->maxHealth += 10 * player->level;
    player->health = player->maxHealth;
    player->skillPoints++;
    player->invencibilityTimer = 2.0f; // 2 segundos de invencibilidade
    float killRadius = 350.0f;
    for (int i = 0; i < maxEnemies; i++) {
        if (enemies[i].active) {
            float dist = Vector2Distance(player->position, enemies[i].position);
            if (dist < killRadius) {
                enemies[i].health = 0;
                enemies[i].active = false;
                AudioPlaySound(SOUND_ENEMY_DEATH);
            }
        }
    }
    player->levelUpTextTimer = 1.5f; // 1.5 segundos de texto
    player->levelUpArcTimer = 0.7f;  // 0.7 segundos de arco
    player->levelUpArcProgress = 0.0f;
    AudioPlaySound(SOUND_LEVELUP);
    AudioSetSoundVolume(SOUND_LEVELUP, 0.3f);
}

void DrawPlayerLevelUpEffects(Player *player) {
    // Arco azul
    if (player->levelUpArcTimer > 0.0f) {
        float centerX = player->position.x + player->size.x/2;
        float centerY = player->position.y + player->size.y/2;
        float minRadius = player->size.x * 0.7f;
        float maxRadius = player->size.x * 1.7f;
        float radius = minRadius + (maxRadius - minRadius) * player->levelUpArcProgress;
        int arcCount = 3;
        for (int i = 0; i < arcCount; i++) {
            float startAngle = 120.0f * i + player->levelUpArcProgress * 60.0f;
            DrawRing((Vector2){centerX, centerY}, radius, radius+6, startAngle, startAngle+60, 64, Fade(BLUE, 180));
        }
    }
    // Texto
    if (player->levelUpTextTimer > 0.0f) {
        float alpha = (player->levelUpTextTimer > 0.3f) ? 1.0f : player->levelUpTextTimer / 0.3f;
        int fontSize = 32;
        const char* txt = "LEVEL UP!";
        int textWidth = MeasureText(txt, fontSize);
        float centerX = player->position.x + player->size.x/2;
        float y = player->position.y - 32 - (1.0f - alpha) * 20.0f;
        DrawText(txt, (int)(centerX - textWidth/2), (int)y, fontSize, Fade(SKYBLUE, (unsigned char)(255*alpha)));
    }
}

void DrawHotkeyBar(int width, int height, float cooldowns[], int numSlots) {
    int barW = 220, barH = 48;
    int x = width/2 - barW/2, y = height - barH - 12;
    DrawRectangleRounded((Rectangle){x, y, barW, barH}, 0.3f, 8, Fade(BLACK, 0.7f));
    for (int i = 0; i < numSlots; i++) {
        int slotX = x + 12 + i*70;
        int slotY = y + 8;
        DrawRectangleRounded((Rectangle){slotX, slotY, 48, 32}, 0.4f, 6, DARKGRAY);
        DrawText(TextFormat("%d", i+1), slotX+4, slotY+2, 16, WHITE);
        // Cooldown overlay
        if (cooldowns[i] > 0.0f) {
            float pct = cooldowns[i] / 5.0f; // Exemplo: 5s de cooldown
            if (pct > 1.0f) pct = 1.0f;
            int cdH = (int)(32*pct);
            DrawRectangle(slotX, slotY + 32 - cdH, 48, cdH, Fade(GRAY, 180));
            DrawText(TextFormat("%.1f", cooldowns[i]), slotX+16, slotY+8, 14, WHITE);
        }
    }
}

