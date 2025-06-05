#include "player.h"
#include "config.h"
#include <raylib.h>

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

    if (player->position.y < 0)
        player->position.y = 0;
    if (player->position.y + player->size.y > screenHeight)
        player->position.y = screenHeight - player->size.y;
    if (player->position.x < 0)
        player->position.x = 0;
    if (player->position.x + player->size.x > screenWidth)
        player->position.x = screenWidth - player->size.x;
}

void PlayerTryShoot(Player *player, Projectile projectiles[])
{
    if (player->shootTimer > 0)
        player->shootTimer -= GetFrameTime();

    if (IsKeyPressed(KEY_SPACE) || IsKeyDown(KEY_SPACE))
    {
        if (player->shootTimer <= 0)
        {
            player->shootTimer = player->shootCooldown;
            Vector2 targetPosition = GetMousePosition();
            // shoot from middle of player sprite
            Vector2 shootPosition = {player->position.x + player->size.x / 2, player->position.y + player->size.y / 2};
            for (int i = 0; i < MAX_PROJECTILES; i++)
            {
                if (!projectiles[i].active)
                {
                    ShootProjectile(projectiles, i, shootPosition, targetPosition);
                    projectiles[i].color = SKYBLUE;
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

void PlayerGainXP(Player *player, int xp)
{
    player->experience += xp;
    int xpToNextLevel = 100 + (player->level - 1) * 50;
    // Ganha níveis enquanto tiver XP suficiente
    while (player->experience >= xpToNextLevel)
    {
        player->experience -= xpToNextLevel;
        PlayerLevelUp(player);
        xpToNextLevel = 100 + (player->level - 1) * 50;
    }
}

void PlayerLevelUp(Player *player){
    player->level++;
    player->maxHealth += 10 * player->level;
    player->health = player->maxHealth;
    player->skillPoints++;
    // Aqui pode-se adicionar lógica para desbloquear habilidades
}

