#include <config.h>
#define MAX_BOSS_MAGICS 32
#define MAX_BOSS_PROJECTILES 64

GameState gameState = GAME_MENU;

// --- VARIÁVEIS DE WAVE ---
int currentWave = 0;
int enemiesToSpawn = 0;
int enemiesAlive = 0;
bool inWave = false;
bool inPreparation = false;
float waveCooldownTimer = 0.0f;
// segundos de preparação entre waves
const float wavePrepTime = 15.0f;
int enemiesSpawnedThisWave = 0;
float enemyWaveSpawnTimer = 0.0f;
// tepo de spawn entre inimigos
const float enemyWaveSpawnInterval = 0.20f; // tempo entre cada spawn (era 0.7f)

bool bossSpawned = false;
bool bossActive = false;

void ResetGame(Player *player, Enemy enemies[], Projectile projectiles[], Particle particles[])
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

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].active = false;
    }

    currentWave = 0;
    enemiesToSpawn = 0;
    enemiesAlive = 0;
    inWave = false;
    inPreparation = false;
    waveCooldownTimer = 0.0f;
    enemiesSpawnedThisWave = 0;
    enemyWaveSpawnTimer = 0.0f;
    bossSpawned = false;
    bossActive = false;
}

Texture2D bossGravityPushFrames[20];
int bossGravityPushFrameCount = 20;
Texture2D bossFloorDamageFrames[16];
int bossFloorDamageFrameCount = 16;
Texture2D bossBulletPhase2Frames[5];
int bossBulletPhase2FrameCount = 5;
Texture2D bossBulletFrames[5];
int bossBulletFrameCount = 5;

int main()
{
    // efeito de fade in/fade out do HUD
    float hudAlpha = 0.0f;
    float fadeSpeed = 2.0f;
    bool transitioningToGame = false;
    float transitionAlpha = 0.0f;
    // áudio
    InitAudioDevice();
    SetMasterVolume(1.0f);
    AudioInit();
    Music menuMusic = LoadMusicStream("assets/sounds/main_theme.mp3");
    Music gameMusic = LoadMusicStream("assets/sounds/game_theme.mp3");
    Music bossMusic = LoadMusicStream("assets/sounds/boss_theme.mp3");
    Music gameOverMusic = LoadMusicStream("assets/sounds/game_over.mp3");
    PlayMusicStream(menuMusic);

    // renderiza a janela e cursor
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, gameName);
    // ToggleFullscreen();
    Image icon = LoadImage("assets/icon.png");
    SetWindowIcon(icon);
    SetTargetFPS(60);
    CursorInit();

    // incializa o jogador, boss, e projeteis
    Player player = InitPlayer(WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f);
    Boss boss = InitBoss(WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f);

    Projectile projectiles[MAX_PROJECTILES];
    InitProjectiles(projectiles, MAX_PROJECTILES);

    InitBossProjectiles();

    Enemy enemies[MAX_ENEMIES];
    InitEnemies(enemies, MAX_ENEMIES);

    Particle particles[MAX_PARTICLES];
    InitParticles(particles, MAX_PARTICLES);

    BossMagic bossMagic[MAX_BOSS_MAGICS];
    InitBossMagics(bossMagic, MAX_BOSS_MAGICS);

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
    Texture2D gemSprite = LoadTexture("assets/sprites/gem.png");

    static UpgradeMenuState upgradeMenuState = {0, 0, 0};
    static bool upgradeMenuWasShown = false;

    // Carregar frames dos personagens e afins
    Texture2D demonIdleFrames[4];
    for (int i = 0; i < 4; i++)
    {
        char filename[100];
        sprintf(filename, "assets/sprites/demon/idle/IDLE-%d.png", i + 1);
        demonIdleFrames[i] = LoadTexture(filename);
    }

    Texture2D minionFrames[4];
    for (int i = 0; i < 4; i++)
    {
        char filename[100];
        sprintf(filename, "assets/sprites/minion/minion-45x66-%d.png", i + 1);
        minionFrames[i] = LoadTexture(filename);
    }

    Texture2D playerFrames[8];
    for (int i = 0; i < 8; i++)
    {
        char filename[100];
        sprintf(filename, "assets/sprites/player/player-%d.png", i + 1);
        playerFrames[i] = LoadTexture(filename);
    }

    Texture2D flameballFrames[4];
    for (int i = 0; i < 4; i++)
    {
        char filename[100];
        sprintf(filename, "assets/sprites/player/flameball/flameball-32x32-%d.png", i + 1);
        flameballFrames[i] = LoadTexture(filename);
    }

    Texture2D bossFrames[8];
    for (int i = 0; i < 8; i++)
    {
        char filename[100];
        sprintf(filename, "assets/sprites/boss/boss-%d.png", i + 1);
        bossFrames[i] = LoadTexture(filename);
    }

    Texture2D bossAttackFrames[8];
    for (int i = 0; i < 8; i++)
    {
        char filename[100];
        sprintf(filename, "assets/sprites/boss/boss-attack/mage-2-122x110-%d.png", i + 1);
        bossAttackFrames[i] = LoadTexture(filename);
    }

    Texture2D bossPhase2Frames[8];
    for (int i = 0; i < 8; i++)
    {
        char filename[100];
        sprintf(filename, "assets/sprites/boss/boss-phase-2/mage-3-87x110-%d.png", i + 1);
        bossPhase2Frames[i] = LoadTexture(filename);
    }

    // ataque de gravidade do boss
    for (int i = 0; i < 20; i++)
    {
        char filename[100];
        sprintf(filename, "assets/sprites/boss/boss-attack/gravity-push/Gravity-Sheet-%d.png", i + 1);
        bossGravityPushFrames[i] = LoadTexture(filename);
    }
    // dano no chão do boss
    for (int i = 0; i < 12; i++)
    {
        char filename[100];
        sprintf(filename, "assets/sprites/boss/boss-attack/floor-damage/floor-damage-%d.png", i + 1);
        bossFloorDamageFrames[i] = LoadTexture(filename);
    }
    // projéteis do boss fase 1
    for (int i = 0; i < 5; i++)
    {
        char filename[100];
        sprintf(filename, "assets/sprites/boss/boss-attack/bullet-phase-2/mage-bullet-13x13-%d.png", i + 1);
        bossBulletFrames[i] = LoadTexture(filename);
    }
    // projéteis do boss fase 2

    for (int i = 0; i < 5; i++)
    {
        char filename[100];
        sprintf(filename, "assets/sprites/boss/boss-attack/bullet-phase-2/mage-bullet-13x13-%d.png", i + 1);
        bossBulletPhase2Frames[i] = LoadTexture(filename);
    }

    int demonIdleFrameCount = 4;
    int minionFrameCount = 4;
    int bossFrameCount = 8;
    int bossPhase2FrameCount = 8;
    int playerFrameCount = 8;
    int flameballFrameCount = 4;

    while (!WindowShouldClose())
    {
        UpdateMusicStream(menuMusic);
        SetMusicVolume(menuMusic, 0.6f);
        // SetMusicPitch(menuMusic, 0.8f);
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
                stars[i].y += 1.0f;
                if (stars[i].y > screenHeight)
                    stars[i].y = 0;
            }
            // bounce no titulo
            float titleOffset = sin(GetTime() * 2) * 5;

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

                Color color = (i == menuSelected) ? PURPLE : GRAY;
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
            if (bossActive)
            {
                UpdateMusicStream(bossMusic);
            }
            else
            {
                UpdateMusicStream(gameMusic);
            }
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
            // Efeito
            DrawRectangle(0, 0, WORLD_WIDTH, WORLD_HEIGHT, Fade(BLACK, 0.70f));
            // determina qual sprite do boss usar
            Texture2D *currentBossFrames = bossFrames;
            int currentBossFrameCount = bossFrameCount;
            Color projectileTint = WHITE;

            // Exibe animação de ataque SÓ quando o boss está atirando projéteis OU attackAnimTimer > 0
            bool bossIsAttacking = false;
            if (boss.attackAnimTimer > 0.0f)
            {
                bossIsAttacking = true;
            }
            else if (boss.phase == 1)
            {
                if (boss.aiState_p1 == BOSS_IA_P1_SHOOTING_BASIC || boss.aiState_p1 == BOSS_IA_P1_SHOOTING_BURST)
                {
                    bossIsAttacking = true;
                }
            }
            // Ajuste: na fase 2, nunca troca para bossAttackFrames
            if (boss.phase == 2)
            {
                currentBossFrames = bossPhase2Frames;
                currentBossFrameCount = bossFrameCount;
            }
            else if (bossIsAttacking)
            {
                currentBossFrames = bossAttackFrames;
                currentBossFrameCount = bossFrameCount;
            }
            else
            {
                currentBossFrames = bossFrames;
                currentBossFrameCount = bossFrameCount;
            }

            // --- BOSS STATE MACHINE VISIBILITY ---
            if (bossActive)
            {
                UpdateBoss(&boss, &player, projectiles, MAX_PROJECTILES, particles, MAX_PARTICLES);
                if (boss.visible)
                {
                    DrawBoss(boss, currentBossFrames, currentBossFrameCount, WHITE);
                }

                if (boss.phase == 2 && boss.health <= 0 && !boss.isActiveInLogic)
                {
                    bossActive = false;
                    inWave = false;
                    inPreparation = true;
                    waveCooldownTimer = wavePrepTime;
                    player.skillPoints += 5;
                }
            }

            DrawPlayer(player, playerFrames, playerFrameCount);
            DrawPlayerLevelUpEffects(&player);
            UpdatePlayer(&player);
            PlayerTryShoot(&player, projectiles, camera);
            // --- MAGIAS ---
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
                if (MagicCast(&player, 0, particles, mouseWorld, enemies, MAX_ENEMIES, &boss))
                    slotCooldowns[0] = slotCooldownBase[0];
            }
            // MAGIA DE DEFESA
            if (IsKeyPressed(KEY_E) && slotCooldowns[1] <= 0.0f)
            {
                Vector2 mouseScreen = GetMousePosition();
                Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, camera);
                if (MagicCast(&player, 1, particles, mouseWorld, enemies, MAX_ENEMIES, &boss))
                    slotCooldowns[1] = slotCooldownBase[1];
            }
            // DASH
            if (IsKeyPressed(KEY_SPACE) && slotCooldowns[2] <= 0.0f)
            {
                Vector2 mouseScreen = GetMousePosition();
                Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, camera);
                if (MagicCast(&player, 2, particles, mouseWorld, enemies, MAX_ENEMIES, &boss))
                    slotCooldowns[2] = slotCooldownBase[2];
            }
            // Atualiza magicCooldowns para refletir slotCooldowns (garante sincronismo com hotkeybar)
            for (int i = 0; i < MAGIC_COUNT; i++)
            {
                magicCooldowns[i] = slotCooldowns[i];
            }
            for (int i = 0; i < MAX_PROJECTILES; i++)
            {
                if (projectiles[i].active)
                {
                    UpdateProjectile(&projectiles[i]);
                    projectileTint = projectiles[i].color;
                    DrawProjectile(projectiles[i], flameballFrames, flameballFrameCount, projectileTint);
                }
            }

            UpdateBossProjectiles(dt);
            DrawBossProjectiles();
            UpdateBossMagics(dt);
            DrawBossMagics();

            // --- COLISÃO PLAYER x BOSS PROJECTILES ---
            for (int i = 0; i < MAX_BOSS_PROJECTILES; i++)
            {
                if (bossProjectiles[i].active && !bossProjectiles[i].charging)
                {
                    float dist = Vector2Distance(
                        (Vector2){player.position.x + player.size.x / 2, player.position.y + player.size.y / 2},
                        bossProjectiles[i].position);
                    if (dist <= bossProjectiles[i].radius + player.size.x / 2 && player.invencibilityTimer <= 0)
                    {
                        TakeDamagePlayer(&player, bossProjectiles[i].damage, -1);
                        bossProjectiles[i].active = false;
                    }
                }
            }

            // --- SISTEMA DE WAVES ---

            // --- SISTEMA DE WAVES ---
            bool isBossWave = currentWave % 5 == 0;

            if (!inWave && !inPreparation && currentWave == 0)
            {
                inPreparation = true;
                waveCooldownTimer = wavePrepTime;
            }

            if (inPreparation){
                waveCooldownTimer -= GetFrameTime();

                if (waveCooldownTimer <= 0.0f)
                {
                    // Começa nova wave
                    currentWave++;
                    inWave = true;
                    inPreparation = false;
                    isBossWave = currentWave % 5 == 0;

                    if (isBossWave)
                    {
                        StopMusicStream(gameMusic);
                        PlayMusicStream(bossMusic);
                        SetMusicVolume(bossMusic, 1.0f);
                        UpdateMusicStream(bossMusic);
                        SpawnBoss(&boss, (Vector2){WORLD_WIDTH / 2.0f - boss.size.x / 2, WORLD_HEIGHT / 4.0f});
                        bossActive = true;
                        bossSpawned = true;
                        enemiesToSpawn = 0;
                        enemiesAlive = 0;
                        enemiesSpawnedThisWave = 0;
                    }
                    else
                    {
                        enemiesToSpawn = 10 + currentWave * 5;
                        enemiesAlive = 0;
                        enemiesSpawnedThisWave = 0;
                        bossSpawned = false;
                    }

                    enemyWaveSpawnTimer = 0.0f;
                    upgradeMenuWasShown = false;
                }
                else
                {
                    if (!upgradeMenuWasShown && player.skillPoints > 0)
                    {
                        upgradeMenuState.visible = 1;
                        upgradeMenuWasShown = true;
                    }
                    if (upgradeMenuState.visible)
                    {
                        UIHandleUpgradeMenuInput(&player, &upgradeMenuState);
                    }
                }
            }

            if (inWave)
            {
                isBossWave = currentWave % 5 == 0;

                // Bloqueia o spawn de inimigos se for wave de boss
                if (!isBossWave && !bossActive)
                {
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
                            if (chance > 70 - currentWave * 3)
                                type = ENEMY_TYPE_STRONG;
                            else if (chance > 45 - currentWave * 4)
                                type = ENEMY_TYPE_FAST;

                            SpawnEnemy(enemies, spawnPos, type);
                            enemiesSpawnedThisWave++;
                            enemyWaveSpawnTimer = enemyWaveSpawnInterval;
                        }
                    }
                }

                // Conta inimigos vivos
                enemiesAlive = 0;
                for (int i = 0; i < MAX_ENEMIES; i++)
                    if (enemies[i].active)
                        enemiesAlive++;

                bool waveComplete = (enemiesAlive == 0 && enemiesSpawnedThisWave == enemiesToSpawn);
                bool bossDefeated = isBossWave && !bossActive;

                if ((isBossWave && bossDefeated) || (!isBossWave && waveComplete))
                {
                    inWave = false;
                    inPreparation = true;
                    waveCooldownTimer = wavePrepTime;
                    bossSpawned = false;
                }
            }

            // --- FIM SISTEMA DE WAVES ---

            // --- ATUALIZAÇÃO E DESENHO DOS INIMIGOS ---
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
            // --- COLISAO MAGIA DE DANO EM AREA DO BOSS ---
            for (int i = 0; i < MAX_BOSS_MAGICS; i++)
            {
                if (bossMagics[i].active && !bossMagics[i].charging && bossMagics[i].frames != NULL)
                {
                    float dist = Vector2Distance((Vector2){player.position.x + player.size.x / 2, player.position.y + player.size.y / 2}, bossMagics[i].position);
                    if (dist <= bossMagics[i].radius + 8.0f && player.invencibilityTimer <= 0)
                    {
                        TakeDamagePlayer(&player, bossMagics[i].damage, -1); // -1: dano especial
                    }
                }
            }

            // --- COLISÃO JOGADOR-PARTÍCULAS ---
            UpdateParticles(particles, MAX_PARTICLES);
            PlayerCollectXPOrbs(&player, particles, MAX_PARTICLES, enemies, MAX_ENEMIES);
            DrawParticles(particles, MAX_PARTICLES, gemSprite); // Passa a sprite da gema para DrawParticles
            for (int i = 0; i < MAX_PROJECTILES; i++)
                DrawProjectile(projectiles[i], flameballFrames, flameballFrameCount, WHITE);
            // --- DESENHO DOS INIMIGOS ---
            for (int i = 0; i < MAX_ENEMIES; i++)
                if (enemies[i].active)
                {
                    if (enemies[i].type == ENEMY_TYPE_NORMAL)
                    {
                        DrawEnemy(enemies[i], minionFrames, minionFrameCount, WHITE);
                    }
                    else if (enemies[i].type == ENEMY_TYPE_STRONG)
                    {
                        DrawEnemy(enemies[i], minionFrames, minionFrameCount, Fade(RED, 0.5f)); // Tanque minion com efeito vermelho
                    }
                    else if (enemies[i].type == ENEMY_TYPE_FAST)
                    {
                        DrawEnemy(enemies[i], demonIdleFrames, demonIdleFrameCount, YELLOW);
                    }
                    else // Fallback para outros tipos
                    {
                        DrawEnemy(enemies[i], NULL, 0, WHITE);
                    }
                }
            EndMode2D();
            // --- BARRA DE VIDA DE BOSS ---
            if (bossActive && (boss.visible || boss.isBlinking) && boss.health > 0)
            {
                UIDrawBossHealthBar(&boss);
            }

            // UI/HUD do jogador
            UIDrawHUD(&player, currentWave, enemiesAlive, waveCooldownTimer, inWave);
            UIUpdateFloatingMsgs();
            UIDrawFloatingMsgs();
            UIDrawHotkeyBar(screenWidth, screenHeight, magicCooldowns, MAGIC_COUNT);

            // mostra o menu de upgrades se o jogador tiver pontos de skill e estiver em preparação
            if (upgradeMenuState.visible && inPreparation)
            {
                UIDrawUpgradeMenu(&player, &upgradeMenuState);
            }

            // game over ao morrer, pause no P, e pause da musica no M
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
            UIDrawPauseScreen();
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
            UpdateMusicStream(gameOverMusic);
            SetMusicPitch(gameOverMusic, 1.0f);
            UIDrawGameOver();
            if (IsKeyPressed(KEY_R))
            {
                ResetGame(&player, enemies, projectiles, particles);
                gameState = GAME_IS_PLAYING;
                StopMusicStream(gameOverMusic);
                gameOverMusicPlayed = false;
                PlayMusicStream(gameMusic);
            }
            break;
        }
        // Desenha o cursor customizado por cima de tudo
        CursorDraw();
        EndDrawing();
    }

    // descarrega os recursos para liberar memória
    UnloadMusicStream(menuMusic);
    UnloadMusicStream(gameMusic);
    UnloadMusicStream(bossMusic);
    UnloadMusicStream(gameOverMusic);
    UnloadTexture(tile);
    UnloadTexture(obscuraIcon);
    UnloadTexture(backGround);
    UnloadTexture(gemSprite); // Descarrega a sprite da gema

    ClearBossProjectiles();
    ClearBossMagics();

    for (int i = 0; i < demonIdleFrameCount; i++)
        UnloadTexture(demonIdleFrames[i]);
    for (int i = 0; i < minionFrameCount; i++)
        UnloadTexture(minionFrames[i]);
    for (int i = 0; i < playerFrameCount; i++)
        UnloadTexture(playerFrames[i]);
    for (int i = 0; i < flameballFrameCount; i++)
        UnloadTexture(flameballFrames[i]);
    for (int i = 0; i < bossFrameCount; i++)
        UnloadTexture(bossFrames[i]);
    for (int i = 0; i < 8; i++)
        UnloadTexture(bossAttackFrames[i]);
    for (int i = 0; i < 8; i++)
        UnloadTexture(bossPhase2Frames[i]);

    for (int i = 0; i < bossBulletFrameCount; i++)
        UnloadTexture(bossBulletFrames[i]);
    for (int i = 0; i < bossFloorDamageFrameCount; i++)
        UnloadTexture(bossFloorDamageFrames[i]);
    for (int i = 0; i < bossGravityPushFrameCount; i++)
        UnloadTexture(bossGravityPushFrames[i]);
    for (int i = 0; i < bossBulletPhase2FrameCount; i++)
        UnloadTexture(bossBulletPhase2Frames[i]);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
