#include <config.h>

GameState gameState = GAME_MENU;

// --- VARIÁVEIS DE WAVE ---
int currentWave = 0; // começa em 0, só incrementa ao iniciar a primeira wave
int enemiesToSpawn = 0;
int enemiesAlive = 0;
bool inWave = false;
bool inPreparation = false;
float waveCooldownTimer = 0.0f;
const float wavePrepTime = 15.0f; // segundos de preparação entre waves
int enemiesSpawnedThisWave = 0;
float enemyWaveSpawnTimer = 0.0f;
// Ajustado para um intervalo menor para mais inimigos
const float enemyWaveSpawnInterval = 0.35f; // tempo entre cada spawn (era 0.7f)

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
    currentWave = 0;
    enemiesToSpawn = 0;
    enemiesAlive = 0;
    inWave = false;
    inPreparation = false;
    waveCooldownTimer = 0.0f;
    enemiesSpawnedThisWave = 0;
    enemyWaveSpawnTimer = 0.0f;
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

    InitWindow(screenWidth, screenHeight, gameName);
    Image icon = LoadImage("assets/icon.png");
    SetWindowIcon(icon);
    SetTargetFPS(60);
    CursorInit();

    Player player = InitPlayer(WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f);

    Projectile projectiles[MAX_PROJECTILES];
    InitProjectiles(projectiles, MAX_PROJECTILES);

    Enemy enemies[MAX_ENEMIES];
    InitEnemies(enemies, MAX_ENEMIES);

    Particle particles[MAX_PARTICLES];
    InitParticles(particles, MAX_PARTICLES);

    float magicCooldowns[MAGIC_COUNT] = {0};
    float slotCooldowns[MAGIC_COUNT] = {0};
    const float slotCooldownBase[MAGIC_COUNT] = {5.0f, 5.0f, 5.0f};

    // desenha estrelas caindo
    Vector2 stars[100];
    for (int i = 0; i < 100; i++)
    {
        stars[i] = (Vector2){GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight)};
    }
    // inicializa a câmera
    Camera2D camera = {0};
    camera.target = player.position;
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // deadzone para suavizar o movimento da câmera
    // X e Y da zona morta (bem pequena)
    const float deadzoneWidth = 64.0f;
    const float deadzoneHeight = 32.0f;

    Texture2D tile = LoadTexture("assets/sprites/ground_64x64.png");
    Texture2D obscuraIcon = LoadTexture("assets/sprites/obscura.png");
    Texture2D backGround = LoadTexture("assets/sprites/menu/Space Background(1).png");

    static UpgradeMenuState upgradeMenuState = {0, 0, 0};
    static bool upgradeMenuWasShown = false;

    // Carregar frames do demônio animado (IDLE)
    Texture2D demonIdleFrames[4];
    demonIdleFrames[0] = LoadTexture("assets/sprites/demon/idle/IDLE-1.png");
    demonIdleFrames[1] = LoadTexture("assets/sprites/demon/idle/IDLE-2.png");
    demonIdleFrames[2] = LoadTexture("assets/sprites/demon/idle/IDLE-3.png");
    demonIdleFrames[3] = LoadTexture("assets/sprites/demon/idle/IDLE-4.png");

    Texture2D minionFrames[4];
    minionFrames[0] = LoadTexture("assets/sprites/minion/minion-45x66-1.png");
    minionFrames[1] = LoadTexture("assets/sprites/minion/minion-45x66-2.png");
    minionFrames[2] = LoadTexture("assets/sprites/minion/minion-45x66-3.png");
    minionFrames[3] = LoadTexture("assets/sprites/minion/minion-45x66-4.png");

    Texture2D flameballFrames[4];
    flameballFrames[0] = LoadTexture("assets/sprites/flameball/flameball-32x32-1.png");
    flameballFrames[1] = LoadTexture("assets/sprites/flameball/flameball-32x32-2.png");
    flameballFrames[2] = LoadTexture("assets/sprites/flameball/flameball-32x32-3.png");
    flameballFrames[3] = LoadTexture("assets/sprites/flameball/flameball-32x32-4.png");

    int flameballFrameCount = 4;
    int demonIdleFrameCount = 4;
    int minionFrameCount = 4;

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
        float playerCenterX = player.position.x + player.size.x / 2;
        float playerCenterY = player.position.y + player.size.y / 2;
        // Deadzone retângulo central
        float dzLeft = camX - deadzoneWidth / 2;
        float dzRight = camX + deadzoneWidth / 2;
        float dzTop = camY - deadzoneHeight / 2;
        float dzBottom = camY + deadzoneHeight / 2;
        // Se o player sair da deadzone, move a câmera
        if (playerCenterX < dzLeft)
            camX = playerCenterX + deadzoneWidth / 2;
        if (playerCenterX > dzRight)
            camX = playerCenterX - deadzoneWidth / 2;
        if (playerCenterY < dzTop)
            camY = playerCenterY + deadzoneHeight / 2;
        if (playerCenterY > dzBottom)
            camY = playerCenterY - deadzoneHeight / 2;
        // Limita a câmera nas bordas do mundo
        if (camX < halfScreenW)
            camX = halfScreenW;
        if (camY < halfScreenH)
            camY = halfScreenH;
        if (camX > WORLD_WIDTH - halfScreenW)
            camX = WORLD_WIDTH - halfScreenW;
        if (camY > WORLD_HEIGHT - halfScreenH)
            camY = WORLD_HEIGHT - halfScreenH;
        camera.target = (Vector2){camX, camY};

        switch (gameState)
        {
        // no menu
        case GAME_MENU:
        {
            StopMusicStream(gameOverMusic);
            gameOverMusicPlayed = false;
            ClearBackground(BLACK);
            DrawTexture(backGround, 0, 0, WHITE);

            if (hudAlpha < 1.0f)
                hudAlpha += dt * fadeSpeed;

            for (int i = 0; i < 100; i++)
            {
                DrawPixelV(stars[i], WHITE);
                stars[i].y += 0.5f;
                if (stars[i].y > screenHeight)
                    stars[i].y = 0;
            }

            float titleOffset = sin(GetTime() * 2) * 5; // efeito de bounce
                                                        // DrawText("OBSCURA", screenWidth / 2 - MeasureText("OBSCURA", 60) / 2, screenHeight / 2 - 100 + titleOffset, 60, Fade(PURPLE, hudAlpha));
                                                        // Draw obscureIcon image
            if (obscuraIcon.id == 0)
                printf("Erro ao carregar a imagem!!!");
            DrawTexture(obscuraIcon, screenWidth / 2 - obscuraIcon.width / 2, screenHeight / 2 - obscuraIcon.height / 2 - 200 - titleOffset, WHITE);
            DrawText("Desenvolvido por Felipe", screenWidth / 2 - MeasureText("Desenvolvido por Felipe", 20) / 2, screenHeight - 60, 20, Fade(DARKGRAY, hudAlpha));
            static int menuSelected = 0;
            const char *menuItems[] = {"Iniciar Jogo", "Sair"};
            int menuCount = 2;
            int menuY = screenHeight / 2 + 20;
            int menuSpacing = 40;

            // se mouse estiver sobre o item do menu, destaque ele
            int mouseY = GetMouseY();
            for (int i = 0; i < menuCount; i++)
            {
                int itemY = menuY + i * menuSpacing;
                int textWidth = MeasureText(menuItems[i], 30);
                Rectangle itemRect = {
                    screenWidth / 2 - textWidth / 2,
                    itemY,
                    textWidth,
                    30};
                if (CheckCollisionPointRec(GetMousePosition(), itemRect))
                    menuSelected = i;

                Color color = (i == menuSelected) ? DARKPURPLE : GRAY;
                DrawText(menuItems[i], screenWidth / 2 - textWidth / 2, itemY, 30, Fade(color, hudAlpha));
            }

            // mesma coisa mas com o teclado
            if (IsKeyPressed(KEY_DOWN))
                menuSelected = (menuSelected + 1) % menuCount;
            if (IsKeyPressed(KEY_UP))
                menuSelected = (menuSelected - 1 + menuCount) % menuCount;

            // Iniciar jogo se pressionar ENTER ou clicar no botão
            if (IsKeyPressed(KEY_ENTER) ||
                (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                 mouseY >= menuY && mouseY < menuY + menuCount * menuSpacing))
            {
                int clickedIndex = -1;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) // Check again to ensure it was a click
                {
                    clickedIndex = (mouseY - menuY) / menuSpacing;
                    if (clickedIndex >= 0 && clickedIndex < menuCount)
                    {
                        menuSelected = clickedIndex;
                    }
                    else
                    {
                        if (!IsKeyPressed(KEY_ENTER))
                            clickedIndex = -1; // Invalidate if only mouse click and it's bad
                    }
                }

                if (menuSelected == 0 && (IsKeyPressed(KEY_ENTER) || clickedIndex == 0))
                    transitioningToGame = true;
                else if (menuSelected == 1 && (IsKeyPressed(KEY_ENTER) || clickedIndex == 1))
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
        // jogando
        case GAME_IS_PLAYING:
            StopMusicStream(menuMusic);
            UpdateMusicStream(gameMusic);
            SetMusicVolume(gameMusic, 0.5f);
            SetMusicPitch(gameMusic, 0.8f);
            gameOverMusicPlayed = false;
            ClearBackground(LIGHTGRAY);
            BeginMode2D(camera);
            // Fundo tileado
            for (int x = 0; x < WORLD_WIDTH; x += tile.width)
            {
                for (int y = 0; y < WORLD_HEIGHT; y += tile.height)
                {
                    DrawTexture(tile, x, y, WHITE);
                }
            }
            DrawPlayer(player);
            DrawPlayerLevelUpEffects(&player);
            UpdatePlayer(&player);
            PlayerTryShoot(&player, projectiles, camera);
            // --- MAGIC SYSTEM ---
            MagicUpdate(&player, particles, magicCooldowns);
            // Atualiza slotCooldowns para refletir magicCooldowns (garante sincronismo com hotkeybar)
            for (int i = 0; i < MAGIC_COUNT; i++)
            {
                if (slotCooldowns[i] > 0.0f)
                    slotCooldowns[i] -= GetFrameTime();
                if (slotCooldowns[i] < magicCooldowns[i])
                    slotCooldowns[i] = magicCooldowns[i];
                if (slotCooldowns[i] < 0.0f)
                    slotCooldowns[i] = 0.0f;
            }
            // MAGIA DE ATAQUE
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && slotCooldowns[0] <= 0.0f)
            {
                Vector2 mouseScreen = GetMousePosition();
                Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, camera);
                if (MagicCast(&player, 0, particles, mouseWorld, enemies, MAX_ENEMIES))
                    slotCooldowns[0] = slotCooldownBase[0];
            }
            // MAGIA DE DEFESA
            if (IsKeyPressed(KEY_E) && slotCooldowns[1] <= 0.0f)
            {
                Vector2 mouseScreen = GetMousePosition();
                Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, camera);
                if (MagicCast(&player, 1, particles, mouseWorld, enemies, MAX_ENEMIES))
                    slotCooldowns[1] = slotCooldownBase[1];
            }
            // DASH
            if (IsKeyPressed(KEY_SPACE) && slotCooldowns[2] <= 0.0f)
            {
                Vector2 mouseScreen = GetMousePosition();
                Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, camera);
                if (MagicCast(&player, 2, particles, mouseWorld, enemies, MAX_ENEMIES))
                    slotCooldowns[2] = slotCooldownBase[2];
            }
            // Atualiza magicCooldowns para refletir slotCooldowns (garante sincronismo com hotkeybar)
            for (int i = 0; i < MAGIC_COUNT; i++)
            {
                magicCooldowns[i] = slotCooldowns[i];
            }
            for (int i = 0; i < MAX_PROJECTILES; i++)
                UpdateProjectile(&projectiles[i]);
            // --- SISTEMA DE WAVES ---
            if (!inWave && !inPreparation && currentWave == 0)
            {
                // Ao entrar no gameplay, inicia preparação
                inPreparation = true;
                waveCooldownTimer = wavePrepTime;
            }
            if (inPreparation)
            {
                waveCooldownTimer -= GetFrameTime();
                if (waveCooldownTimer <= 0.0f)
                {
                    // Começa a wave
                    currentWave++;
                    inWave = true;
                    inPreparation = false;
                    // Aumenta a base de inimigos e o multiplicador por wave
                    enemiesToSpawn = 10 + currentWave * 5; // era 5 + currentWave * 3
                    enemiesAlive = 0;
                    enemiesSpawnedThisWave = 0;
                    enemyWaveSpawnTimer = 0.0f;
                    upgradeMenuWasShown = false;
                }
                else
                {
                    // Menu de upgrades disponível durante preparação
                    if (!upgradeMenuWasShown && player.skillPoints > 0)
                    {
                        upgradeMenuState.visible = 1; // Show the upgrade menu
                        upgradeMenuWasShown = true;
                    }
                    // Handle input for the upgrade menu
                    if (upgradeMenuState.visible)
                    {
                        UIHandleUpgradeMenuInput(&player, &upgradeMenuState);
                    }
                }
            }
            if (inWave)
            {
                // Spawn progressivo dos inimigos
                if (enemiesSpawnedThisWave < enemiesToSpawn)
                {
                    enemyWaveSpawnTimer -= GetFrameTime();
                    if (enemyWaveSpawnTimer <= 0.0f)
                    {
                        int edge = GetRandomValue(0, 3);
                        Vector2 spawnPos;
                        float safeDist = 400.0f;
                        int tries = 0;
                        do
                        {
                            switch (edge)
                            {
                            case 0:
                                spawnPos = (Vector2){(float)GetRandomValue(0, WORLD_WIDTH), -60.0f};
                                break;
                            case 1:
                                spawnPos = (Vector2){(float)GetRandomValue(0, WORLD_WIDTH), WORLD_HEIGHT + 60.0f};
                                break;
                            case 2:
                                spawnPos = (Vector2){-60.0f, (float)GetRandomValue(0, WORLD_HEIGHT)};
                                break;
                            case 3:
                                spawnPos = (Vector2){WORLD_WIDTH + 60.0f, (float)GetRandomValue(0, WORLD_HEIGHT)};
                                break;
                            }
                            tries++;
                        } while (Vector2Distance(spawnPos, (Vector2){player.position.x + player.size.x / 2, player.position.y + player.size.y / 2}) < safeDist && tries < 10);
                        EnemyType type = ENEMY_TYPE_NORMAL;
                        int chance = GetRandomValue(1, 100);
                        // Ajustar chances para balancear os tipos de inimigos com mais densidade
                        if (chance > 70 - currentWave * 2) // Mais chance de STRONG em waves altas
                            type = ENEMY_TYPE_STRONG;
                        else if (chance > 45 - currentWave * 2) // Mais chance de FAST em waves altas
                            type = ENEMY_TYPE_FAST;
                        SpawnEnemy(enemies, spawnPos, type);
                        enemiesSpawnedThisWave++;
                        enemyWaveSpawnTimer = enemyWaveSpawnInterval;
                    }
                }
                // Conta inimigos vivos
                enemiesAlive = 0;
                for (int i = 0; i < MAX_ENEMIES; i++)
                    if (enemies[i].active)
                        enemiesAlive++;
                if (enemiesAlive == 0 && enemiesSpawnedThisWave == enemiesToSpawn)
                {
                    inWave = false;
                    inPreparation = true;
                    waveCooldownTimer = wavePrepTime;
                }
            }
            // --- FIM SISTEMA DE WAVES ---
            for (int i = 0; i < MAX_ENEMIES; i++)
            {
                if (enemies[i].active)
                {
                    Vector2 playerCenter = {
                        player.position.x + player.size.x / 2,
                        player.position.y + player.size.y / 2};
                    Vector2 enemyCenterTarget = {
                        playerCenter.x - enemies[i].size.x / 2,
                        playerCenter.y - enemies[i].size.y / 2};
                    UpdateEnemy(&enemies[i], enemyCenterTarget);
                }
            }
            for (int i = 0; i < MAX_PROJECTILES; i++)
            {
                if (projectiles[i].active)
                {
                    for (int j = 0; j < MAX_ENEMIES; j++)
                    {
                        if (enemies[j].active)
                        {
                            Rectangle enemyRec = {enemies[j].position.x, enemies[j].position.y, enemies[j].size.x, enemies[j].size.y};
                            if (CheckCollisionCircleRec(projectiles[i].position, projectiles[i].radius, enemyRec))
                            {
                                // Passar o array de partículas para TakeDamageEnemy
                                TakeDamageEnemy(&enemies[j], projectiles[i].damage, particles);
                                // Remover a chamada direta para PlayerGainXP.
                                // O XP agora é concedido pela coleta de orbs (PlayerCollectXPOrbs)
                                // e o orb é dropado dentro de TakeDamageEnemy.
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
                Rectangle playerRec = {
                    player.position.x + player.size.x / 2 - player.size.x / 2,
                    player.position.y + player.size.y / 2 - player.size.y / 2,
                    player.size.x,
                    player.size.y};
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    if (enemies[i].active && enemies[i].health > 0)
                    {
                        Rectangle enemyRec = {enemies[i].position.x, enemies[i].position.y, enemies[i].size.x, enemies[i].size.y};
                        if (CheckCollisionRecs(playerRec, enemyRec))
                        {
                            TakeDamagePlayer(&player, enemies[i].attackDamage, enemies[i].type);
                        }
                    }
                }
            }
            UpdateParticles(particles, MAX_PARTICLES);
            PlayerCollectXPOrbs(&player, particles, MAX_PARTICLES, enemies, MAX_ENEMIES); // Add this call
            DrawParticles(particles, MAX_PARTICLES);
            for (int i = 0; i < MAX_PROJECTILES; i++)
                DrawProjectile(projectiles[i], flameballFrames, flameballFrameCount, WHITE);
            for (int i = 0; i < MAX_ENEMIES; i++)
                if (enemies[i].active)
                {
                    if (enemies[i].type == ENEMY_TYPE_NORMAL)
                    {
                        DrawEnemy(enemies[i], minionFrames, minionFrameCount, WHITE); // Normal minion
                    }
                    else if (enemies[i].type == ENEMY_TYPE_STRONG)
                    {
                        DrawEnemy(enemies[i], minionFrames, minionFrameCount, Fade(RED, 0.7f)); // Tanque minion com efeito vermelho
                    }
                    else if (enemies[i].type == ENEMY_TYPE_FAST)
                    {
                        DrawEnemy(enemies[i], demonIdleFrames, demonIdleFrameCount, YELLOW); // Demon rápido
                    }
                    else // Fallback para outros tipos
                    {
                        DrawEnemy(enemies[i], NULL, 0, WHITE);
                    }
                }
            EndMode2D();
            UIDrawHUD(&player, currentWave, enemiesAlive, waveCooldownTimer, inWave);
            UIDrawHotkeyBar(screenWidth, screenHeight, magicCooldowns, MAGIC_COUNT);

            // --- UPGRADE MENU DRAWING & INPUT (HUD ELEMENT) ---
            if (upgradeMenuState.visible && inPreparation) // Only draw if visible and in preparation
            {
                UIDrawUpgradeMenu(&player, &upgradeMenuState);
            }

            // --- DEBUG: Exibir informações na tela ---
            // Descomente para exibir informações de debug
            // DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, RED);
            // DrawText(TextFormat("Posicao Jogador: (%.2f, %.2f)", player.position.x, player.position.y), 10, 40, 20, GREEN);
            // DrawText(TextFormat("Inimigos Vivos: %d", enemiesAlive), 10, 70, 20, BLUE);
            // DrawText(TextFormat("Onda Atual: %d", currentWave), 10, 100, 20, YELLOW);
            // DrawText(TextFormat("Skill Points: %d", player.skillPoints), 10, 130, 20, PURPLE);

            if (player.alive == false)
                gameState = GAME_OVER;
            if (IsKeyPressed(KEY_P))
                gameState = GAME_PAUSED;
            if (IsKeyPressed(KEY_M))
            {
                static bool musicMuted = false;
                musicMuted = !musicMuted;
                if (musicMuted)
                {
                    PauseMusicStream(gameMusic);
                }
                else
                {
                    ResumeMusicStream(gameMusic);
                }
            }
            break;

        case GAME_PAUSED:
            // ClearBackground(BLACK); // Not needed if UIDrawPauseScreen handles background
            // DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
            // DrawText("PAUSE", screenWidth / 2 - MeasureText("PAUSE", 40) / 2, screenHeight / 2 - 20, 40, PURPLE);
            // DrawText("Pressione P para continuar", screenWidth / 2 - MeasureText("Pressione P para continuar", 20) / 2, screenHeight / 2 + 20, 20, WHITE);
            UIDrawPauseScreen(); // Call the UI function for drawing pause screen
            if (IsKeyPressed(KEY_P))
                gameState = GAME_IS_PLAYING;
            break;
        case GAME_OVER:
            StopMusicStream(gameMusic);
            if (!gameOverMusicPlayed)
            {
                PlayMusicStream(gameOverMusic);
                gameOverMusicPlayed = true;
            }
            // Não repete a música: não chama PlayMusicStream novamente enquanto gameOverMusicPlayed for true
            UpdateMusicStream(gameOverMusic);
            SetMusicPitch(gameOverMusic, 0.9f);
            // DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
            // DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 40) / 2, screenHeight / 2 - 20, 40, RED);
            // DrawText("Pressione R para reiniciar", screenWidth / 2 - MeasureText("Pressione R para reiniciar", 20) / 2, screenHeight / 2 + 20, 20, WHITE);
            UIDrawGameOver(); // Call the UI function for drawing game over screen
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
    UnloadTexture(tile);
    UnloadTexture(obscuraIcon);
    for (int i = 0; i < 4; i++) UnloadTexture(minionFrames[i]);
    for (int i = 0; i < demonIdleFrameCount; i++) UnloadTexture(demonIdleFrames[i]);
    for (int i = 0; i < flameballFrameCount; i++) UnloadTexture(flameballFrames[i]);
    UnloadTexture(backGround);
    UnloadImage(icon);
    AudioUnload();
    CursorUnload();
    CloseWindow();
    return 0;
}
