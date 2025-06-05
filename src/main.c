/*******************************************************************************************
 *
 *   OBSCURA - V0.2
 *
 *   Funcionalidades:
 *     - Inimigos com diferentes tipos, atributos e raridade de spawn
 *     - Sistema de colisão entre projéteis e inimigos
 *     - Sistema de experiência, níveis e pontos de habilidade do jogador
 *     - HUD mostrando vida, XP, nível e pontos de habilidade
 *     - Cooldown de tiro para o jogador
 *     - Menus: principal, pausa e game over, com transições suaves
 *     - Fundo animado de estrelas
 ********************************************************************************************/
#include <config.h>

typedef enum GameState
{
    GAME_MENU,
    GAME_IS_PLAYING,
    GAME_PAUSED,
    GAME_OVER
} GameState;

// Variável global de estado
GameState gameState = GAME_MENU;

void DrawHUD(Player *player)
{
    int x = 20;
    int y = 20;

    int barWidth = 300;
    int barHeight = 24;

    float healthPercent = (float)player->health / player->maxHealth;

    DrawRectangle(x + 2, y + 2, barWidth, barHeight, Fade(BLACK, 0.4f));
    DrawRectangleRounded((Rectangle){x, y, barWidth, barHeight}, 0.3f, 10, Fade(MAROON, 0.5f));
    DrawRectangleRounded((Rectangle){x, y, barWidth * healthPercent, barHeight}, 0.3f, 10, RED);
    DrawRectangleRoundedLines((Rectangle){x, y, barWidth, barHeight}, 0.3f, 10, Fade(WHITE, 0.3f));
    const char *hpText = TextFormat("HP: %d / %d", player->health, player->maxHealth);
    int textWidth = MeasureText(hpText, 16);
    DrawText(hpText, x + (barWidth - textWidth) / 2, y + 4, 16, WHITE);

    DrawText(TextFormat("XP: %d  |  Lv. %d", player->experience, player->level), x, y + 40, 18, GREEN);
    DrawText(TextFormat("Pontos de habilidade: %d", player->skillPoints), x, y + 65, 18, YELLOW);

    // FPS no canto
    DrawFPS(screenWidth - 100, 10);
}


void ResetGame(Player *player, Enemy enemies[], Projectile projectiles[])
{
    *player = InitPlayer(screenWidth / 2.0f, screenHeight / 2.0f);

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].active = false;
        enemies[i].health = enemies[i].maxHealth;
    }

    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        projectiles[i].active = false;
    }
}

int main()
{
    float hudAlpha = 0.0f;
    float fadeSpeed = 2.0f;
    bool transitioningToGame = false;
    float transitionAlpha = 0.0f;

    InitAudioDevice();
    Music menuMusic = LoadMusicStream("assets/sounds/main_theme.mp3");
    PlayMusicStream(menuMusic);

    float enemySpawnTimer = 0.0f;
    float enemySpawnCooldown = 1.0f;

    InitWindow(screenWidth, screenHeight, gameName);
    SetTargetFPS(60);

    Player player = InitPlayer(screenWidth / 2.0f, screenHeight / 2.0f);

    Projectile projectiles[MAX_PROJECTILES];
    InitProjectiles(projectiles, MAX_PROJECTILES);

    Enemy enemies[MAX_ENEMIES];
    InitEnemies(enemies, MAX_ENEMIES);

    Vector2 stars[100];
    for (int i = 0; i < 100; i++)
        stars[i] = (Vector2){GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight)};

    while (!WindowShouldClose())
    {
        UpdateMusicStream(menuMusic);
        BeginDrawing();

        switch (gameState)
        {
        case GAME_MENU:
        {
            ClearBackground(BLACK);
            if (hudAlpha < 1.0f)
                hudAlpha += dt * fadeSpeed;

            // desenha estrelas caindo
            for (int i = 0; i < 100; i++)
            {
                DrawPixelV(stars[i], WHITE);
                stars[i].y += 0.5f;
                if (stars[i].y > screenHeight)
                    stars[i].y = 0;
            }

            float titleOffset = sin(GetTime() * 2) * 5; // efeito de bounce
            DrawText("OBSCURA", screenWidth / 2 - MeasureText("OBSCURA", 60) / 2, screenHeight / 2 - 100 + titleOffset, 60, Fade(PURPLE, hudAlpha));
            DrawText("Desenvolvido por Felipe Rudnik", screenWidth / 2 - MeasureText("Desenvolvido por Felipe Rudnik", 20) / 2, screenHeight - 60, 20, Fade(DARKGRAY, hudAlpha));

            // --- MENU COM BOTOES ---
            static int menuSelected = 0;
            const char *menuItems[] = {"Iniciar Jogo", "Sair"};
            int menuCount = 2;
            int menuY = screenHeight / 2 + 20;
            int menuSpacing = 40;

            for (int i = 0; i < menuCount; i++)
            {
                Color color = (i == menuSelected) ? PURPLE : GRAY;
                int textWidth = MeasureText(menuItems[i], 30);
                DrawText(menuItems[i], screenWidth / 2 - textWidth / 2, menuY + i * menuSpacing, 30, Fade(color, hudAlpha));
            }

            // Navegação
            if (IsKeyPressed(KEY_DOWN))
                menuSelected = (menuSelected + 1) % menuCount;
            if (IsKeyPressed(KEY_UP))
                menuSelected = (menuSelected - 1 + menuCount) % menuCount;

            if (IsKeyPressed(KEY_ENTER))
            {
                if (menuSelected == 0)
                    transitioningToGame = true;
                else if (menuSelected == 1)
                    CloseWindow();
            }

            if (transitioningToGame)
            {
                transitionAlpha += GetFrameTime();
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, transitionAlpha));

                if (transitionAlpha >= 1.0f)
                {
                    gameState = GAME_IS_PLAYING;
                    transitioningToGame = false;
                    transitionAlpha = 0.0f;
                    hudAlpha = 0.0f;
                }
            }

            break;
        }

        case GAME_IS_PLAYING:
            StopMusicStream(menuMusic);
            DrawPlayer(player);
            ClearBackground(DARKGRAY);
            UpdatePlayer(&player);
            PlayerTryShoot(&player, projectiles);
            DrawHUD(&player);

            for (int i = 0; i < MAX_PROJECTILES; i++)
                UpdateProjectile(&projectiles[i]);

            enemySpawnTimer += GetFrameTime();

            if (enemySpawnTimer >= enemySpawnCooldown)
            {
                enemySpawnTimer = 0.0f;
                Vector2 spawnPos;
                // randomiza o spawn nos 4 cantos
                int edge = GetRandomValue(0, 3);
                switch (edge) {
                    case 0: //topo
                        spawnPos = (Vector2){ (float)GetRandomValue(0, screenWidth), -50.0f };
                        break;
                    case 1: //base
                        spawnPos = (Vector2){ (float)GetRandomValue(0, screenWidth), (float)screenHeight + 50.0f };
                        break;
                    case 2: //esquerda
                        spawnPos = (Vector2){ -50.0f, (float)GetRandomValue(0, screenHeight) };
                        break;
                    case 3: //direita
                        spawnPos = (Vector2){ (float)screenWidth + 50.0f, (float)GetRandomValue(0, screenHeight) };
                        break;
                }

                int chanceToSpawn = GetRandomValue(1, 100);
                EnemyType typeToSpawn;
                if (chanceToSpawn <= 70) {
                    typeToSpawn = ENEMY_TYPE_NORMAL; // 70%
                } else if (chanceToSpawn <= 85) {
                    typeToSpawn = ENEMY_TYPE_FAST;   // 15%
                } else {
                    typeToSpawn = ENEMY_TYPE_STRONG; // 5%
                }
                SpawnEnemy(enemies, spawnPos, typeToSpawn);
            }

            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].active)
                {
                    UpdateEnemy(&enemies[i], player.position);
                }
            }

            for (int i = 0; i < MAX_PROJECTILES; i++)
            {
                if (projectiles[i].active)
                {
                    // Só checa projéteis ativos
                    // Define o retângulo de colisão do projétil (se for círculo, precisamos de um Rectangle para CheckCollisionCircleRec)
                    // Ou, se o projétil for um retângulo, use seu projectiles[i].size.
                    // Para um projétil circular, vamos criar um retângulo que o envolve para CheckCollisionCircleRec.
                    // No entanto, é mais comum ter projéteis retangulares ou usar CheckCollisionCircles se ambos forem círculos.
                    // Vamos assumir que nosso projétil (círculo) colide com o retângulo do inimigo.
                    // A função CheckCollisionCircleRec espera: center, radius, rectangle

                    for (int j = 0; j < MAX_ENEMIES; j++)
                    {
                        if (enemies[j].active)
                        {
                            Rectangle enemyRec = {enemies[j].position.x, enemies[j].position.y, enemies[j].size.x, enemies[j].size.y};
                            if (CheckCollisionCircleRec(projectiles[i].position, projectiles[i].radius, enemyRec))
                            {
                                TakeDamageEnemy(&enemies[j], projectiles[i].damage);
                                if (!enemies[j].active && enemies[j].health <= 0) {
                                    PlayerGainXP(&player, enemies[j].xpReward);
                                }
                                projectiles[i].active = false;
                                break;
                            }
                        }
                    }
                }
            }

            // --- COLISÃO JOGADOR-INIMIGO ---
            if (player.health > 0)
            {
                // Só checa colisão se o jogador estiver vivo
                Rectangle playerRec = {player.position.x, player.position.y, player.size.x, player.size.y};
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    if (enemies[i].active && enemies[i].health > 0)
                    {
                        // Inimigo ativo e vivo
                        Rectangle enemyRec = {enemies[i].position.x, enemies[i].position.y, enemies[i].size.x, enemies[i].size.y};
                        if (CheckCollisionRecs(playerRec, enemyRec))
                        {
                            TakeDamagePlayer(&player, enemies[i].attackDamage);
                        }
                    }
                }
            }

            for (int i = 0; i < MAX_PROJECTILES; i++)
                DrawProjectile(projectiles[i]);

            for (int i = 0; i < MAX_ENEMIES; i++)
                if (enemies[i].active)
                    DrawEnemy(enemies[i]);

            // alterações do gamestate no loop do jogo
            if (player.alive == false)
                gameState = GAME_OVER;
            if (IsKeyPressed(KEY_P))
                gameState = GAME_PAUSED;

            break;

        case GAME_PAUSED:
            ClearBackground(BLACK);
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
            DrawText("PAUSE", screenWidth / 2 - MeasureText("PAUSE", 40) / 2, screenHeight / 2 - 20, 40, PURPLE);
            DrawText("Pressione P para continuar", screenWidth / 2 - MeasureText("Pressione P para continuar", 20) / 2, screenHeight / 2 + 20, 20, WHITE);
            if (IsKeyPressed(KEY_P))
                gameState = GAME_IS_PLAYING;
            break;
        case GAME_OVER:
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
            DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 40) / 2, screenHeight / 2 - 20, 40, RED);
            DrawText("Pressione R para reiniciar", screenWidth / 2 - MeasureText("Pressione R para reiniciar", 20) / 2, screenHeight / 2 + 20, 20, WHITE);
            if (IsKeyPressed(KEY_R))
            {
                ResetGame(&player, enemies, projectiles);
                gameState = GAME_IS_PLAYING;
            }
            break;
        }
        EndDrawing();

    } // Fim do while(!WindowShouldClose())

    UnloadMusicStream(menuMusic);
    CloseWindow();
    return 0;
}
