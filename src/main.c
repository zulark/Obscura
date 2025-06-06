/*******************************************************************************************
 *
 *   OBSCURA - V0.4.0
 *
 *   Funcionalidades:
 *   - Mundo expandido, limites de movimentação, câmera centralizada e limitada
 *   - Deadzone (zona morta) para suavizar o movimento da câmera
 *   - Grid de debug com coordenadas
 *   - Tipos de inimigos (NORMAL, FAST, STRONG) com spawn probabilístico e spawn fora da área andável
 *   - Inimigos perseguem o centro do player
 *   - Sistema de XP, level, skill points
 *   - XP ao matar inimigos, barra de XP no HUD, level up aumenta atributos
 *   - Ataque circular e invencibilidade temporária ao subir de nível
 *   - Floating text: valor de XP ganho aparece acima do personagem
 *   - Sistema de partículas (particle.h/.c), efeito visual ao upar, pronto para magias
 *   - HUD mostra vida, XP, nível, skill points, FPS
 *   - Menus de jogo, pausa, game over
 *   - Hotkey bar de magias (slots 1,2,3), efeito visual de cooldown
 *   - Modularização do áudio: criação de audio.h/.c, centralização dos efeitos sonoros
 *   - Sons para tiro, dano no player, level up, morte de inimigo integrados e tocando nos eventos corretos
 *   - Ajuste de volume e pitch via funções do módulo de áudio
 *   - Correção da música de game over para tocar apenas uma vez
 *   - Modularização inicial: headers e arquivos fonte para áudio, magias, UI/HUD
 *   - Limpeza de código, remoção de warnings, atualização de comentários
 *   - Ajuste de colisão para alinhar visualmente player e inimigos
 *   - Makefile ajustado para incluir resource.res (ícone do .exe)
 *   - Orientações para exportar o executável junto com a pasta assets
 *   - Explicação sobre falsos positivos de antivírus e assinatura digital
 *   - Implementação de cursor customizado com Texture2D, escala ajustável e offset para centralização
 *   - Sistema simplificado para usar apenas um sprite de cursor
 *   - Orientação para alinhar o centro do cursor à posição do mouse
 *
 ********************************************************************************************/
#include <config.h>
#include "raylib.h"
#include "particle.h"
#include "audio.h"
#include "cursor.h"

typedef enum GameState
{
    GAME_MENU,
    GAME_IS_PLAYING,
    GAME_PAUSED,
    GAME_OVER
} GameState;

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

    // Barra de XP
    int xpBarY = y + 30;
    int xpBarHeight = 12;
    int xpBarWidth = barWidth;
    int xpToNextLevel = 100 + (player->level - 1) * 50;
    float xpPercent = (float)player->experience / xpToNextLevel;
    DrawRectangle(x + 2, xpBarY + 2, xpBarWidth, xpBarHeight, Fade(BLACK, 0.4f));
    DrawRectangleRounded((Rectangle){x, xpBarY, xpBarWidth * xpPercent, xpBarHeight}, 0.3f, 10, GREEN);
    DrawRectangleRoundedLines((Rectangle){x, xpBarY, xpBarWidth, xpBarHeight}, 0.3f, 10, Fade(WHITE, 0.3f));
    DrawText(TextFormat("XP: %d / %d", player->experience, xpToNextLevel), x + 8, xpBarY - 2, 12, WHITE);

    DrawText(TextFormat("Pontos de habilidade: %d", player->skillPoints), x, y + 65, 18, YELLOW);

    // FPS no canto
    DrawFPS(screenWidth - 100, 10);
}


void ResetGame(Player *player, Enemy enemies[], Projectile projectiles[])
{   
    *player = InitPlayer(WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f);

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
    SetMasterVolume(1.0f);
    AudioInit(); // Inicializa o sistema de áudio
    Music menuMusic = LoadMusicStream("assets/sounds/main_theme.mp3");
    Music gameMusic = LoadMusicStream("assets/sounds/game_theme.mp3");
    Music gameOverMusic = LoadMusicStream("assets/sounds/game_over.mp3");

    PlayMusicStream(menuMusic);

    float enemySpawnTimer = 0.0f;
    float enemySpawnCooldown = 1.0f;

    InitWindow(screenWidth, screenHeight, gameName);
    Image icon = LoadImage("assets/icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);
    SetTargetFPS(60);
    CursorInit();

    Player player = InitPlayer(WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f);

    Projectile projectiles[MAX_PROJECTILES];
    InitProjectiles(projectiles, MAX_PROJECTILES);

    Enemy enemies[MAX_ENEMIES];
    InitEnemies(enemies, MAX_ENEMIES);

    Particle particles[MAX_PARTICLES];
    InitParticles(particles, MAX_PARTICLES);

    Vector2 stars[100];
    for (int i = 0; i < 100; i++)
        stars[i] = (Vector2){GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight)};

    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth/2.0f, screenHeight/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Deadzone para suavizar o movimento da câmera
    const float deadzoneWidth = 64.0f;  // largura da zona morta (bem pequena)
    const float deadzoneHeight = 32.0f;  // altura da zona morta (bem pequena)

    while (!WindowShouldClose())
    {
        UpdateMusicStream(menuMusic);
        SetMusicVolume(menuMusic, 0.6f);
        SetMusicPitch(menuMusic, 0.8f);
        static bool gameOverMusicPlayed = false;
        // --- DEADZONE DA CÂMERA ---
        float halfScreenW = screenWidth / 2.0f;
        float halfScreenH = screenHeight / 2.0f;
        float camX = camera.target.x;
        float camY = camera.target.y;
        float playerCenterX = player.position.x + player.size.x/2;
        float playerCenterY = player.position.y + player.size.y/2;
        // Deadzone retângulo central
        float dzLeft = camX - deadzoneWidth/2;
        float dzRight = camX + deadzoneWidth/2;
        float dzTop = camY - deadzoneHeight/2;
        float dzBottom = camY + deadzoneHeight/2;
        // Se o player sair da deadzone, move a câmera
        if (playerCenterX < dzLeft) camX = playerCenterX + deadzoneWidth/2;
        if (playerCenterX > dzRight) camX = playerCenterX - deadzoneWidth/2;
        if (playerCenterY < dzTop) camY = playerCenterY + deadzoneHeight/2;
        if (playerCenterY > dzBottom) camY = playerCenterY - deadzoneHeight/2;
        // Limita a câmera nas bordas do mundo
        if (camX < halfScreenW) camX = halfScreenW;
        if (camY < halfScreenH) camY = halfScreenH;
        if (camX > WORLD_WIDTH - halfScreenW) camX = WORLD_WIDTH - halfScreenW;
        if (camY > WORLD_HEIGHT - halfScreenH) camY = WORLD_HEIGHT - halfScreenH;
        camera.target = (Vector2){camX, camY};

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
            DrawText("Desenvolvido por Felipe", screenWidth / 2 - MeasureText("Desenvolvido por Felipe", 20) / 2, screenHeight - 60, 20, Fade(DARKGRAY, hudAlpha));

            // --- MENU COM BOTOES ---
            static int menuSelected = 0;
            const char *menuItems[] = {"Iniciar Jogo", "Sair"};
            int menuCount = 2;
            int menuY = screenHeight / 2 + 20;
            int menuSpacing = 40;

            for (int i = 0; i < menuCount; i++)
            {
                Color color = (i == menuSelected) ? DARKPURPLE : GRAY;
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
                    StopMusicStream(menuMusic);
                    PlayMusicStream(gameMusic);
                }
            }

            break;
        }

        case GAME_IS_PLAYING:
            StopMusicStream(menuMusic);
            UpdateMusicStream(gameMusic);
            SetMusicVolume(gameMusic, 0.5f);
            SetMusicPitch(gameMusic, 0.8f);
            gameOverMusicPlayed = false; // Reset flag ao sair do game over
            ClearBackground(LIGHTGRAY);
            BeginMode2D(camera);
            // Desenha um grid de debug na área andável
            int gridSize = 200;
            Color gridColor = Fade(BLACK, 0.5f);
            for (int x = 0; x <= WORLD_WIDTH; x += gridSize) {
                DrawLine(x, 0, x, WORLD_HEIGHT, gridColor);
                DrawText(TextFormat("%d", x), x + 4, 4, 18, gridColor);
            }
            for (int y = 0; y <= WORLD_HEIGHT; y += gridSize) {
                DrawLine(0, y, WORLD_WIDTH, y, gridColor);
                DrawText(TextFormat("%d", y), 4, y + 4, 18, gridColor);
            }
            DrawPlayer(player);
            DrawPlayerLevelUpEffects(&player);
            UpdatePlayer(&player);
            PlayerTryShoot(&player, projectiles, camera);
            for (int i = 0; i < MAX_PROJECTILES; i++)
                UpdateProjectile(&projectiles[i]);

            enemySpawnTimer += GetFrameTime();

            if (enemySpawnTimer >= enemySpawnCooldown)
            {
                enemySpawnTimer = 0.0f;
                Vector2 spawnPos;
                // Spawna inimigos fora da área andável
                int edge = GetRandomValue(0, 3);
                switch (edge) {
                    case 0: // topo
                        spawnPos = (Vector2){ (float)GetRandomValue(0, WORLD_WIDTH), -60.0f };
                        break;
                    case 1: // base
                        spawnPos = (Vector2){ (float)GetRandomValue(0, WORLD_WIDTH), WORLD_HEIGHT + 10.0f };
                        break;
                    case 2: // esquerda
                        spawnPos = (Vector2){ -60.0f, (float)GetRandomValue(0, WORLD_HEIGHT) };
                        break;
                    case 3: // direita
                        spawnPos = (Vector2){ WORLD_WIDTH + 10.0f, (float)GetRandomValue(0, WORLD_HEIGHT) };
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
                    Vector2 playerCenter = {
                        player.position.x + player.size.x/2,
                        player.position.y + player.size.y/2
                    };
                    // Ajuste: inimigo mira o centro do player, compensando metade do seu próprio tamanho
                    Vector2 enemyCenterTarget = {
                        playerCenter.x - enemies[i].size.x/2,
                        playerCenter.y - enemies[i].size.y/2
                    };
                    UpdateEnemy(&enemies[i], enemyCenterTarget);
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
                                    PlayerGainXP(&player, enemies[j].xpReward, enemies, MAX_ENEMIES);
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
                // Ajuste: colisão pelo centro do player
                Rectangle playerRec = {
                    player.position.x + player.size.x/2 - player.size.x/2,
                    player.position.y + player.size.y/2 - player.size.y/2,
                    player.size.x,
                    player.size.y
                };
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    if (enemies[i].active && enemies[i].health > 0)
                    {
                        Rectangle enemyRec = {enemies[i].position.x, enemies[i].position.y, enemies[i].size.x, enemies[i].size.y};
                        if (CheckCollisionRecs(playerRec, enemyRec))
                        {
                            TakeDamagePlayer(&player, enemies[i].attackDamage);
                        }
                    }
                }
            }

            UpdateParticles(particles, MAX_PARTICLES);
            DrawParticles(particles, MAX_PARTICLES); // Desenha partículas dentro do mundo, acima dos inimigos

            for (int i = 0; i < MAX_PROJECTILES; i++)
                DrawProjectile(projectiles[i]);

            for (int i = 0; i < MAX_ENEMIES; i++)
                if (enemies[i].active)
                    DrawEnemy(enemies[i]);
            EndMode2D();
            DrawHUD(&player);
            // Hotkey bar de magias (apenas visual)
            static float slotCooldowns[3] = {0.0f, 0.0f, 0.0f};
            static float slotCooldownBase[3] = {5.0f, 5.0f, 5.0f}; // 5s para cada slot
            // Atualiza cooldowns
            for (int i = 0; i < 3; i++) {
                if (slotCooldowns[i] > 0.0f) slotCooldowns[i] -= GetFrameTime();
                if (slotCooldowns[i] < 0.0f) slotCooldowns[i] = 0.0f;
            }
            // Checa teclas 1,2,3
            for (int i = 0; i < 3; i++) {
                if (IsKeyPressed(KEY_ONE + i) && slotCooldowns[i] <= 0.0f) {
                    slotCooldowns[i] = slotCooldownBase[i];
                }
            }
            DrawHotkeyBar(screenWidth, screenHeight, slotCooldowns, 3);

            // alterações do gamestate no loop do jogo
            if (player.alive == false)
                gameState = GAME_OVER;
            if (IsKeyPressed(KEY_P))
                gameState = GAME_PAUSED;


            if (IsKeyPressed(KEY_M)){
                static bool musicMuted = false;
                musicMuted = !musicMuted;
                if (musicMuted) {
                    PauseMusicStream(gameMusic);
                } else {
                    ResumeMusicStream(gameMusic);
                }
            }
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
            StopMusicStream(gameMusic);
            if (!gameOverMusicPlayed) {
                PlayMusicStream(gameOverMusic);
                gameOverMusicPlayed = true;
            }
            UpdateMusicStream(gameOverMusic);
            SetMusicPitch(gameOverMusic, 0.9f);
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
            DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 40) / 2, screenHeight / 2 - 20, 40, RED);
            DrawText("Pressione R para reiniciar", screenWidth / 2 - MeasureText("Pressione R para reiniciar", 20) / 2, screenHeight / 2 + 20, 20, WHITE);
            if (IsKeyPressed(KEY_R))
            {
                ResetGame(&player, enemies, projectiles);
                gameState = GAME_IS_PLAYING;
                StopMusicStream(gameOverMusic);
                gameOverMusicPlayed = false;
                PlayMusicStream(gameMusic);
                SetMusicVolume(gameMusic, 0.4f);
            }
            break;
        }
        CursorDraw(); // Desenha o cursor customizado por cima de tudo
        EndDrawing();
    } // Fim do while(!WindowShouldClose())
    UnloadMusicStream(menuMusic);
    UnloadMusicStream(gameMusic);
    UnloadMusicStream(gameOverMusic);
    AudioUnload(); // Libera recursos de áudio
    CursorUnload();
    CloseWindow();
    return 0;
}
