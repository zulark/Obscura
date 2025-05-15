#include "player.h"
#include "config.h"
#include <raylib.h>

Player InitPlayer(float startX, float startY)
{
    Player player;
    player.position = (Vector2){startX, startY};
    player.size = (Vector2){30.0f, 30.0f};
    player.speed = 200.0f;
    player.color = MAROON;
    return player;
}

void HandlePlayerInput(Player *player)
{
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    player->position.y -= player->speed * dt;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  player->position.y += player->speed * dt;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  player->position.x -= player->speed * dt;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) player->position.x += player->speed * dt;
    
    //se a posição do personagem for menor que o limite da tela o movimento é travado
    if (player->position.y < 0) {
        player->position.y = 0;
    }
    //caso seja maior que a tela ele pega o tamanho do player e o limite da tela e trava o movimento (sim é meio confuso mas é assim que faz)
    if (player->position.y + player->size.y > screenHeight) {
        player->position.y = screenHeight - player->size.y;
    }
    if (player->position.x < 0) {
        player->position.x = 0;
    }
    if (player->position.x + player->size.x > screenWidth) {
        player->position.x = screenWidth - player->size.x;
    }
}

void UpdatePlayer(Player *player){
    HandlePlayerInput(player);
}

void DrawPlayer(Player player){
    DrawRectangleV(player.position, player.size, player.color);
}