/*******************************************************************************************
 *
 *   OBSCURA - V0.1
 *
 *   Funcionalidades:
 *     - Primeira renderização de janela
 *     - Organização da estrutura do código e do Makefile
 *     - Colisão com o limite da janela
 *     - Movimento do jogador (WASD)
 ********************************************************************************************/
#include <config.h>

int main()
{
    InitWindow(screenWidth, screenHeight, gameName);
    SetTargetFPS(60);

    Player player = InitPlayer(screenWidth / 2.0f, screenHeight / 2.0f);

    Projectile projectiles[MAX_PROJECTILES];
    InitProjectiles(projectiles, MAX_PROJECTILES);

    while (!WindowShouldClose())
    {
        UpdatePlayer(&player); // atualiza o estado do jogador
        PlayerTryShoot(&player, projectiles);
        
        for(int i = 0; i < MAX_PROJECTILES; i++){
            UpdateProjectile(&projectiles[i]);
        }
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawPlayer(player);

        for(int i = 0; i < MAX_PROJECTILES; i++){
            DrawProjectile(projectiles[i]);
        }

        DrawText("Mova-se usando WASD", 10, 10, 20, DARKGRAY);
        DrawFPS(screenWidth - 100, 10);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}