#include "boss.h"
#include "ui.h" // Para UIShowFloatingMsg
#include "audio.h" // Para GetSound, PlaySound e IDs de som
#include "particle.h"
#include "player.h"
#include "config.h"
#include "projectile.h" // Para SpawnProjectile
#include "boss_projectile.h" // Added for boss projectiles
#include "boss_magic.h"      // Added for boss magic
#include <stdio.h> // Para debugging temporário

static void UpdateBossAI_Phase1(Boss *boss, Player *player, Projectile projectiles[], int maxProjectiles, Particle particles[], int maxParticles);
static void UpdateBossAI_Phase2(Boss *boss, Player *player, Projectile projectiles[], int maxProjectiles, Particle particles[], int maxParticles);

Boss InitBoss(float startX, float startY)
{
    Boss boss;
    boss.position = (Vector2){startX, startY};
    boss.size = (Vector2){256.0f, 256.0f}; // Boss size (normalized for all phases/animations)
    boss.phase = 1; // Inicializa a fase

    // Inicializa campos de estado visual
    boss.alpha = 0.0f;
    boss.isFadingIn = false;
    boss.fadeTimer = 0.0f;
    boss.isBlinking = false;
    boss.blinkTimer = 0.0f;
    boss.blinkToggleTimer = 0.0f;
    boss.visible = false; // Começa invisível até ser invocado
    boss.phaseTransitionTimer = 0.0f;
    boss.isActiveInLogic = false; // Não ativo até terminar o fade-in
    boss.color = WHITE; // Cor padrão

    // Inicializa campos de IA - Fase 1
    boss.aiState_p1 = BOSS_IA_P1_IDLE;
    boss.actionTimer = 0.0f;
    boss.shootCooldown = 0.0f;
    boss.burstCooldown = 0.0f;
    boss.dashCooldown = 0.0f;
    boss.targetPosition = (Vector2){0.0f, 0.0f};
    boss.burstShotsFired = 0;
    boss.attackAnimTimer = 0.0f;

    return boss;
}

void SpawnBoss(Boss *boss, Vector2 spawnPosition)
{
    if (!boss)
        return;
    boss->position = spawnPosition;
    boss->phase = 1;
    boss->maxHealth = 8000.0f; // Vida para Fase 1
    boss->health = boss->maxHealth;
    boss->speed = 80.0f; // Velocidade para Fase 1
    boss->attackDamage = 30.0f; // Dano de contato/ataques básicos Fase 1
    boss->specialAttackTimer = 0.0f; // Pode ser reutilizado ou removido se os cooldowns específicos forem suficientes
    boss->isChargingAttack = false;
    boss->isPerformingSpecialAttack = false;
    boss->chargeProgress = 0.0f;
    boss->currentFrame = 0;
    boss->frameTime = 0.0f;
    boss->frameSpeed = 1.0f / 10.0f; // 10 FPS para animação
    boss->maxFrames = 8; // Ajustar conforme o sprite sheet do boss Fase 1

    // Estado visual para invocação (Fase 1)
    boss->alpha = 0.0f;
    boss->isFadingIn = true;
    boss->fadeTimer = BOSS_FADE_IN_DURATION;
    boss->isBlinking = false;
    boss->blinkTimer = 0.0f;
    boss->blinkToggleTimer = 0.0f;
    boss->visible = true; // Torna visível para começar o fade-in
    boss->phaseTransitionTimer = 0.0f;
    boss->isActiveInLogic = false; // Não ativo durante o fade-in
    boss->color = WHITE;

    // Configurações de IA para Fase 1 ao spawnar
    boss->aiState_p1 = BOSS_IA_P1_IDLE; 
    boss->actionTimer = 1.5f; // Pequeno delay em IDLE antes da primeira decisão
    boss->shootCooldown = 0.0f; // Pronto para atirar se a IA decidir
    boss->burstCooldown = 5.0f; // Cooldown inicial para o burst attack
    boss->dashCooldown = 8.0f;  // Cooldown inicial para o dash attack
    boss->targetPosition = boss->position; // Posição alvo inicial é a própria posição
    boss->burstShotsFired = 0;
    boss->attackAnimTimer = 0.0f;
}

void DrawBoss(Boss boss, Texture2D *frames, int frameCount, Color tint)
{
    // if (boss.health <= 0 && !boss.isBlinking && boss.phaseTransitionTimer <= 0) // Não desenha se estiver realmente morto e não em transição visual
    //     return;
    if (!boss.visible) // Se piscando, visibilidade é controlada pela lógica de piscar
        return;

    if (!frames || frameCount == 0)
    {
        DrawRectangleV(boss.position, boss.size, Fade(boss.color, boss.alpha));
        return;
    }

    Texture2D currentFrameTexture = frames[boss.currentFrame % frameCount];
    float frameW = (float)currentFrameTexture.width;
    float frameH = (float)currentFrameTexture.height;

    float destW = boss.size.x;
    float destH = boss.size.y;
    float drawX = boss.position.x;
    float drawY = boss.position.y;
    Rectangle sourceRec = {0.0f, 0.0f, frameW, frameH};
    Rectangle destRec = {drawX, drawY, boss.size.x, boss.size.y};
    Vector2 origin = {0.0f, 0.0f};
    if (!boss.isFacingRight)
    {
        sourceRec.width = -frameW;
    }
    DrawTexturePro(currentFrameTexture, sourceRec, destRec, origin, 0.0f, Fade(tint, boss.alpha));
}

void UpdateBoss(Boss *boss, Player *player, Projectile projectiles[], int maxProjectiles, Particle particles[], int maxParticles)
{
    // Lida com Fade-in
    if (boss->isFadingIn)
    {
        boss->fadeTimer -= dt;
        boss->alpha = 1.0f - (boss->fadeTimer / BOSS_FADE_IN_DURATION);
        if (boss->alpha > 1.0f) boss->alpha = 1.0f;

        if (boss->fadeTimer <= 0.0f)
        {
            boss->isFadingIn = false;
            boss->alpha = 1.0f;
            boss->isActiveInLogic = true; // Boss fica totalmente ativo após o fade-in
        }
        // Durante o fade-in, normalmente nenhuma outra lógica é executada
        // Animação pode ser atualizada se desejado
        boss->frameTime += dt;
        if (boss->frameTime >= boss->frameSpeed)
        {
            boss->frameTime = 0.0f;
            boss->currentFrame = (boss->currentFrame + 1) % boss->maxFrames;
        }
        return; // Não processa outra lógica durante o fade-in
    }

    // Lida com Piscar (ex: na morte da Fase 1)
    if (boss->isBlinking)
    {
        boss->blinkTimer -= dt;
        boss->blinkToggleTimer -= dt;

        if (boss->blinkToggleTimer <= 0.0f)
        {
            boss->visible = !boss->visible;
            boss->blinkToggleTimer = BOSS_BLINK_INTERVAL;
        }

        if (boss->blinkTimer <= 0.0f)
        {
            boss->isBlinking = false;
            boss->visible = false; // Garante que fique invisível após terminar de piscar
            boss->isActiveInLogic = false;
            // Inicia o atraso de transição de fase
            boss->phaseTransitionTimer = BOSS_PHASE_TRANSITION_DELAY;
        }
        // Animação pode ser atualizada se desejado
        boss->frameTime += dt;
        if (boss->frameTime >= boss->frameSpeed)
        {
            boss->frameTime = 0.0f;
            boss->currentFrame = (boss->currentFrame + 1) % boss->maxFrames;
        }
        return; // Não processa outra lógica durante o piscar
    }

    // Lida com atraso de transição de fase (após piscar, antes de invocar Fase 2)
    if (boss->phaseTransitionTimer > 0.0f && boss->phase == 1 && boss->health <=0) // Checa fase e vida para garantir que é P1->P2
    {
        boss->phaseTransitionTimer -= dt;
        if (boss->phaseTransitionTimer <= 0.0f)
        {
            // Transição para fase 2: reseta status e inicia fade-in para fase 2
            boss->phase = 2;
            boss->maxHealth = 16000.0f; // Vida para Fase 2 (exemplo)
            boss->health = boss->maxHealth;
            boss->speed = 120.0f; // Velocidade para Fase 2 (exemplo)
            boss->attackDamage = 50; // Dano para Fase 2 (exemplo)
            boss->position = (Vector2){WORLD_WIDTH / 2.0f - boss->size.x / 2, WORLD_HEIGHT / 4.0f};
            AudioPlaySound(SOUND_BOSS_PHASE_CHANGE);
            // UIShowFloatingMsg("BOSS PHASE 2!", (Vector2){boss->position.x, boss->position.y - 30}, ORANGE, 2.0f);

            // Configura fade-in para fase 2
            boss->alpha = 0.0f;
            boss->isFadingIn = true;
            boss->fadeTimer = BOSS_FADE_IN_DURATION;
            boss->visible = true; // Torna visível para começar o fade-in
            boss->isActiveInLogic = false; // Não ativo durante o fade-in

            boss->size = (Vector2){256.0f, 256.0f}; // Ensure normalized size on phase 2 transition
        }
        return; // Não processa outra lógica durante esse atraso
    }
    
    // Se não está ativo na lógica (ex: morto, ou em estado não coberto acima), não faz nada.
    if (!boss->isActiveInLogic) {
        // Ainda atualiza animação se visível, para evitar sprite congelado durante transições
        if(boss->visible) {
            boss->frameTime += dt;
            if (boss->frameTime >= boss->frameSpeed)
            {
                boss->frameTime = 0.0f;
                boss->currentFrame = (boss->currentFrame + 1) % boss->maxFrames;
            }
        }
        return;
    }

    // --- Lógica principal do Boss (apenas se isActiveInLogic for true) ---
    if (boss->health <= 0) {
        if (boss->phase == 1) {
            // Vida <= 0, e não está piscando/transicionando. Inicia piscar para morte da Fase 1.
            boss->isBlinking = true;
            boss->blinkTimer = BOSS_BLINK_DURATION;
            boss->blinkToggleTimer = BOSS_BLINK_INTERVAL;
            boss->visible = true; // Começa visível no primeiro ciclo de piscar
            boss->isActiveInLogic = false; // Para lógica principal
            // PlaySound(GetSound(SOUND_BOSS_DEATH_PHASE1_EFFECT)); // Placeholder para som específico
            return; 
        } else if (boss->phase == 2) {
            // Boss derrotado (vida da fase 2 esgotada)
            // PlaySound(GetSound(SOUND_BOSS_DEATH));
            // TODO: Spawn de partículas finais de morte, talvez um efeito de fade out?
            boss->isActiveInLogic = false; // Permanentemente inativo
            boss->visible = false; // Garante que não seja desenhado
            return; 
        }
    }

    // Atualiza timer do sprite de ataque
    if (boss->attackAnimTimer > 0.0f) {
        boss->attackAnimTimer -= dt;
        if (boss->attackAnimTimer < 0.0f) boss->attackAnimTimer = 0.0f;
    }

    // Processar IA específica da fase:
    if (boss->phase == 1) {
        UpdateBossAI_Phase1(boss, player, projectiles, maxProjectiles, particles, maxParticles);
    } else if (boss->phase == 2) {
        UpdateBossAI_Phase2(boss, player, projectiles, maxProjectiles, particles, maxParticles);
    }

    // Lógica de colisão com projéteis do jogador
    for (int i = 0; i < maxProjectiles; i++)
    {
        if (projectiles[i].active)
        {
            if (CheckCollisionBossProjectile(*boss, projectiles[i]))
            {
                if (boss->isActiveInLogic && !boss->isBlinking && !boss->isFadingIn) // Só recebe dano se estiver ativo e não em transição visual
                {
                    BossTakeDamage(boss, projectiles[i].damage);
                }
                projectiles[i].active = false; // Desativa o projétil
                // Adiciona partículas de hit no boss
                for(int p = 0; p < 5; p++) {
                    //  SpawnParticle(particles, PARTICLE_TYPE_HIT, projectiles[i].position, RED, 2.0f, GetRandomValue(0,360), GetRandomValue(50,100), 0.5f, 0);
                }
            }
        }
    }

    // Lógica de colisão com o jogador (dano ao jogador)
    Rectangle bossRec = { boss->position.x, boss->position.y, boss->size.x, boss->size.y };
    Rectangle playerRec = { player->position.x, player->position.y, player->size.x, player->size.y };

    if (CheckCollisionRecs(bossRec, playerRec) && player->invencibilityTimer <= 0)
    {
        TakeDamagePlayer(player, boss->attackDamage, -1); // -1 para indicar que não é um tipo de inimigo padrão
        // Empurrar o jogador para trás ou outra lógica de knockback pode ser adicionada aqui
    }

    // Atualiza animação do boss
    boss->frameTime += dt;
    if (boss->frameTime >= boss->frameSpeed)
    {
        boss->frameTime = 0.0f;
        boss->currentFrame = (boss->currentFrame + 1) % boss->maxFrames;
    }

    // Lógica de fases e ataques especiais (a ser implementada)
    // boss->specialAttackTimer += GetFrameTime();
    // if (boss->specialAttackTimer > 10.0f) { // Exemplo: ataque especial a cada 10s
    //     BossSpecialAttack(boss, player, particles, projectiles, maxProjectiles);
    //     boss->specialAttackTimer = 0.0f;
    // }
}

// Implementação da IA da Fase 1 do Boss
static void UpdateBossAI_Phase1(Boss *boss, Player *player, Projectile projectiles[], int maxProjectiles, Particle particles[], int maxParticles) {
    // Decrementa cooldowns dos ataques
    if (boss->shootCooldown > 0) boss->shootCooldown -= dt;
    if (boss->burstCooldown > 0) boss->burstCooldown -= dt;
    if (boss->dashCooldown > 0) boss->dashCooldown -= dt;

    // Decrementa o timer da ação atual, se ativo
    if (boss->actionTimer > 0) boss->actionTimer -= dt;

    // Referências para os arrays de frames (assumindo que são carregados em main.c e acessíveis globalmente ou passados)
    extern Texture2D bossBulletFrames[];
    extern int bossBulletFrameCount;

    // Fase 1 IA
    switch (boss->aiState_p1) {
        case BOSS_IA_P1_IDLE:
            if (boss->actionTimer <= 0) {
                int choice = GetRandomValue(0, 4); // Mais opções de ataque
                if (choice == 0 && boss->shootCooldown <= 0) {
                    boss->aiState_p1 = BOSS_IA_P1_SHOOTING_BASIC;
                    boss->actionTimer = 0.15f; // Menor delay para atirar
                } else if (choice == 1 && boss->burstCooldown <= 0) {
                    boss->aiState_p1 = BOSS_IA_P1_CHARGING_BURST;
                    boss->actionTimer = 0.3f; // Menor delay para burst
                } else if (choice == 2 && boss->dashCooldown <= 0) {
                    boss->aiState_p1 = BOSS_IA_P1_PREPARING_DASH;
                    boss->actionTimer = 0.3f; // Menor delay para dash
                } else {
                    // Se nenhuma ação especial estiver pronta, decide mover ou perseguir
                    if (GetRandomValue(0,1)) {
                        boss->aiState_p1 = BOSS_IA_P1_CHASING;
                        boss->actionTimer = GetRandomValue(8, 16) / 10.0f; // Persegue por 0.8-1.6s
                    } else {
                        boss->aiState_p1 = BOSS_IA_P1_MOVING;
                        boss->targetPosition = (Vector2){
                            boss->position.x + GetRandomValue(-150, 150),
                            boss->position.y + GetRandomValue(-100, 100)
                        };
                        if(boss->targetPosition.x < 0) boss->targetPosition.x = 0;
                        if(boss->targetPosition.y < 0) boss->targetPosition.y = 0;
                        if(boss->targetPosition.x > WORLD_WIDTH - boss->size.x) boss->targetPosition.x = WORLD_WIDTH - boss->size.x;
                        if(boss->targetPosition.y > WORLD_HEIGHT - boss->size.y) boss->targetPosition.y = WORLD_HEIGHT - boss->size.y;
                        boss->actionTimer = GetRandomValue(8, 16) / 10.0f; // Move por 0.8-1.6s
                    }
                }
            }
            break;

        case BOSS_IA_P1_MOVING:
            {
                Vector2 direction = Vector2Subtract(boss->targetPosition, boss->position);
                float distance = Vector2Length(direction);
                if (distance > 5.0f && boss->actionTimer > 0) { // Continua movendo se não chegou e tem tempo
                    direction = Vector2Normalize(direction);
                    boss->position = Vector2Add(boss->position, Vector2Scale(direction, boss->speed * 0.7f * dt)); // Move um pouco mais devagar que chasing
                    boss->isFacingRight = (boss->targetPosition.x > boss->position.x);
                } else {
                    boss->aiState_p1 = BOSS_IA_P1_IDLE;
                    boss->actionTimer = GetRandomValue(5, 15) / 10.0f; // Fica IDLE por 0.5-1.5s
                }
            }
            break;

        case BOSS_IA_P1_CHASING:
            if (player->alive) {
                Vector2 direction = Vector2Subtract(player->position, boss->position);
                float distance = Vector2Length(direction);
                // Tenta manter uma certa distância, não muito perto nem muito longe
                if (distance > boss->size.x * 0.8f) { 
                    direction = Vector2Normalize(direction);
                    boss->position = Vector2Add(boss->position, Vector2Scale(direction, boss->speed * dt));
                    boss->isFacingRight = (player->position.x > boss->position.x);
                }
            }
            if (boss->actionTimer <= 0) {
                boss->aiState_p1 = BOSS_IA_P1_IDLE;
                boss->actionTimer = GetRandomValue(5, 10) / 10.0f; // Fica IDLE por 0.5-1s
            }
            break;

        case BOSS_IA_P1_SHOOTING_BASIC:
            if (boss->actionTimer <= 0) {
                if (player->alive) {
                    Vector2 playerCenter = {player->position.x + player->size.x/2, player->position.y + player->size.y/2};
                    Vector2 bossCenter = {boss->position.x + boss->size.x/2, boss->position.y + boss->size.y/2};
                    Vector2 directionToPlayer = Vector2Subtract(playerCenter, bossCenter);
                    Vector2 spawnPos = bossCenter;
                    SpawnBossProjectileAnimTelegraph(spawnPos, directionToPlayer, 300.0f, 10.0f, 15, bossBulletFrames, bossBulletFrameCount, 1.0f/14.0f, 0.3f);
                    boss->attackAnimTimer = 0.28f;
                }
                boss->shootCooldown = GetRandomValue(5, 10) / 10.0f;
                boss->aiState_p1 = BOSS_IA_P1_IDLE;
                boss->actionTimer = GetRandomValue(5, 10) / 10.0f;
            }
            break;

        case BOSS_IA_P1_CHARGING_BURST:
            if (boss->actionTimer <= 0) {
                boss->aiState_p1 = BOSS_IA_P1_SHOOTING_BURST;
                boss->burstShotsFired = 0;
                boss->actionTimer = 0.08f; // Intervalo menor para o primeiro tiro
            }
            break;

        case BOSS_IA_P1_SHOOTING_BURST:
            if (boss->actionTimer <= 0 && boss->burstShotsFired < 3) {
                if (player->alive) {
                    Vector2 playerCenter = {player->position.x + player->size.x/2, player->position.y + player->size.y/2};
                    Vector2 bossCenter = {boss->position.x + boss->size.x/2, boss->position.y + boss->size.y/2};
                    Vector2 directionToPlayer = Vector2Subtract(playerCenter, bossCenter);
                    Vector2 spawnPos = bossCenter;
                    float angleOffset = (boss->burstShotsFired - 1) * 15.0f * DEG2RAD;
                    Vector2 burstDir = Vector2Rotate(directionToPlayer, angleOffset);
                    SpawnBossProjectileAnimTelegraph(spawnPos, burstDir, 250.0f, 8.0f, 10, bossBulletFrames, bossBulletFrameCount, 1.0f/14.0f, 0.18f);
                    boss->attackAnimTimer = 0.28f;
                }
                boss->burstShotsFired++;
                if (boss->burstShotsFired < 3) {
                    boss->actionTimer = 0.08f; // Cooldown menor entre tiros
                } else {
                    boss->burstCooldown = GetRandomValue(10, 18) / 10.0f; // Cooldown menor para burst
                    boss->aiState_p1 = BOSS_IA_P1_IDLE;
                    boss->actionTimer = GetRandomValue(4, 8) / 10.0f;
                }
            }
            break;

        case BOSS_IA_P1_PREPARING_DASH:
            // TODO: Efeito visual/sonoro de preparação
            // boss->color = RED; // Exemplo
            if (player->alive) {
                boss->targetPosition = player->position; // Mira na posição atual do jogador
            }
            if (boss->actionTimer <= 0) {
                boss->aiState_p1 = BOSS_IA_P1_DASHING;
                boss->actionTimer = 5.0f; // Duração do dash
            }
            break;

        case BOSS_IA_P1_DASHING:
            {
                // TODO: Efeito visual/sonoro de dash
                // boss->color = WHITE; // Reseta cor
                Vector2 direction = Vector2Subtract(boss->targetPosition, boss->position);
                float distance = Vector2Length(direction);
                float dashSpeed = boss->speed * 3.0f; // Dash é mais rápido

                if (distance > dashSpeed * dt && boss->actionTimer > 0) {
                    direction = Vector2Normalize(direction);
                    boss->position = Vector2Add(boss->position, Vector2Scale(direction, dashSpeed * dt));
                } else {
                    // Chegou ao destino ou tempo acabou
                    // printf("Boss: Dash Attack Finished!\n");
                    boss->dashCooldown = GetRandomValue(70, 100) / 10.0f; // Cooldown de 7-10s para dash
                    boss->aiState_p1 = BOSS_IA_P1_IDLE;
                    boss->actionTimer = GetRandomValue(10, 15) / 10.0f; // Próxima decisão em 1-1.5s
                }
                // Colisão durante o dash é tratada pela colisão geral Boss-Player em UpdateBoss
            }
            break;

        default:
            boss->aiState_p1 = BOSS_IA_P1_IDLE;
            boss->actionTimer = 1.0f; // Default para voltar a um estado conhecido
            break;
    }
}

// --- Nova função para IA da Fase 2 (mais agressiva) ---
static void UpdateBossAI_Phase2(Boss *boss, Player *player, Projectile projectiles[], int maxProjectiles, Particle particles[], int maxParticles) {
    // Cooldowns e timers
    if (boss->shootCooldown > 0) boss->shootCooldown -= dt;
    if (boss->burstCooldown > 0) boss->burstCooldown -= dt; // Usado para projéteis normais
    if (boss->dashCooldown > 0) boss->dashCooldown -= dt;   // Reutilizado para um tipo de magia
    if (boss->actionTimer > 0) boss->actionTimer -= dt;
    // Cooldowns específicos para magias da Fase 2
    if (boss->magicFloorCooldown > 0) boss->magicFloorCooldown -= dt;
    if (boss->magicPushCooldown > 0) boss->magicPushCooldown -= dt;


    // Referências para os arrays de frames (assumindo que são carregados em main.c e acessíveis globalmente ou passados)
    extern Texture2D bossBulletFrames[]; // Projéteis normais
    extern int bossBulletFrameCount;
    extern Texture2D bossBulletPhase2Frames[]; // Projéteis mais rápidos/diferentes para Fase 2
    extern int bossBulletPhase2FrameCount;
    extern Texture2D bossFloorDamageFrames[];
    extern int bossFloorDamageFrameCount;
    extern Texture2D bossGravityPushFrames[];
    extern int bossGravityPushFrameCount;

    // Fase 2 IA
    switch (boss->aiState_p1) {
        case BOSS_IA_P1_IDLE:
            if (boss->actionTimer <= 0) {
                int choice = GetRandomValue(0, 4); // Mais opções de ataque
                if (choice == 0 && boss->shootCooldown <= 0) {
                    boss->aiState_p1 = BOSS_IA_P1_SHOOTING_BASIC;
                    boss->actionTimer = 0.12f; // Menor delay para atirar
                } else if (choice == 1 && boss->burstCooldown <= 0) {
                    boss->aiState_p1 = BOSS_IA_P1_CHARGING_BURST;
                    boss->actionTimer = 0.22f; // Menor delay para burst
                } else if (choice == 2 && boss->magicFloorCooldown <= 0) {
                    boss->aiState_p1 = BOSS_IA_P2_MAGIC_FLOOR_CHARGE;
                    boss->actionTimer = 0.5f; // Menor tempo de carga para magia de chão
                } else if (choice == 3 && boss->magicPushCooldown <= 0) {
                    boss->aiState_p1 = BOSS_IA_P2_MAGIC_PUSH_CHARGE;
                    boss->actionTimer = 0.7f; // Menor tempo de carga para magia de empurrão
                } else if (choice == 4 && boss->dashCooldown <= 0) {
                    boss->aiState_p1 = BOSS_IA_P1_PREPARING_DASH;
                    boss->actionTimer = 0.2f;
                } else {
                    if (GetRandomValue(0,1)) {
                        boss->aiState_p1 = BOSS_IA_P1_CHASING;
                        boss->actionTimer = GetRandomValue(5, 10) / 10.0f;
                    } else {
                        boss->aiState_p1 = BOSS_IA_P1_MOVING;
                        boss->targetPosition = (Vector2){ boss->position.x + GetRandomValue(-200, 200), boss->position.y + GetRandomValue(-150, 150) };
                        if(boss->targetPosition.x < 0) boss->targetPosition.x = 0;
                        if(boss->targetPosition.y < 0) boss->targetPosition.y = 0;
                        if(boss->targetPosition.x > WORLD_WIDTH - boss->size.x) boss->targetPosition.x = WORLD_WIDTH - boss->size.x;
                        if(boss->targetPosition.y > WORLD_HEIGHT - boss->size.y) boss->targetPosition.y = WORLD_HEIGHT - boss->size.y;
                        boss->actionTimer = GetRandomValue(5, 10) / 10.0f;
                    }
                }
            }
            break;

        case BOSS_IA_P1_SHOOTING_BASIC:
            if (boss->actionTimer <= 0) {
                if (player->alive) {
                    Vector2 playerCenter = {player->position.x + player->size.x/2, player->position.y + player->size.y/2};
                    Vector2 bossCenter = {boss->position.x + boss->size.x/2, boss->position.y + boss->size.y/2};
                    Vector2 directionToPlayer = Vector2Subtract(playerCenter, bossCenter);
                    Vector2 spawnPos = bossCenter;
                    SpawnBossProjectileAnimTelegraph(spawnPos, directionToPlayer, 300.0f, 10.0f, 15, bossBulletFrames, bossBulletFrameCount, 1.0f/14.0f, 0.3f);
                    boss->attackAnimTimer = 0.28f;
                }
                boss->shootCooldown = GetRandomValue(5, 10) / 10.0f;
                boss->aiState_p1 = BOSS_IA_P1_IDLE;
                boss->actionTimer = GetRandomValue(5, 10) / 10.0f;
            }
            break;

        case BOSS_IA_P1_CHARGING_BURST:
            if (boss->actionTimer <= 0) {
                boss->aiState_p1 = BOSS_IA_P1_SHOOTING_BURST;
                boss->burstShotsFired = 0;
                boss->actionTimer = 0.06f;
            }
            break;

        case BOSS_IA_P1_SHOOTING_BURST:
            if (boss->actionTimer <= 0 && boss->burstShotsFired < 5) {
                if (player->alive) {
                    Vector2 playerCenter = {player->position.x + player->size.x/2, player->position.y + player->size.y/2};
                    Vector2 bossCenter = {boss->position.x + boss->size.x/2, boss->position.y + boss->size.y/2};
                    Vector2 directionToPlayer = Vector2Subtract(playerCenter, bossCenter);
                    Vector2 spawnPos = bossCenter;
                    float angleOffset = (boss->burstShotsFired - 2) * 10.0f * DEG2RAD;
                    Vector2 burstDir = Vector2Rotate(directionToPlayer, angleOffset);
                    SpawnBossProjectileAnimTelegraph(spawnPos, burstDir, 350.0f, 10.0f, 15, bossBulletPhase2Frames, bossBulletPhase2FrameCount, 1.0f/18.0f, 0.12f);
                    boss->attackAnimTimer = 0.28f;
                }
                boss->burstShotsFired++;
                if (boss->burstShotsFired < 5) {
                    boss->actionTimer = 0.06f;
                } else {
                    boss->burstCooldown = GetRandomValue(7, 13) / 10.0f;
                    boss->aiState_p1 = BOSS_IA_P1_IDLE;
                    boss->actionTimer = GetRandomValue(3, 7) / 10.0f;
                }
                boss->attackAnimTimer = 0.25f; // Define o tempo da animação de ataque
            }
            break;

        case BOSS_IA_P2_MAGIC_FLOOR_CHARGE:
            if (boss->actionTimer <= 0) {
                if (player->alive) {
                    // Spawn do telegraph (campo vermelho de aviso)
                    Vector2 playerCenter = {player->position.x + player->size.x/2, player->position.y + player->size.y/2};
                    boss->floorMagicTargetPos = playerCenter; // Salva a posição do telegraph
                    float warningRadius = 96.0f; // Raio maior para o campo de aviso
                    float warningDelay = 0.7f;   // Delay reduzido para 0.7s
                    // Usa frames NULL para desenhar só o círculo vermelho
                    SpawnBossMagicAnimTelegraph(playerCenter, (Vector2){0,0}, 0, warningRadius, 0, NULL, 0, 0, warningDelay, warningDelay);
                }
                boss->aiState_p1 = BOSS_IA_P2_MAGIC_FLOOR_CAST;
                boss->actionTimer = 0.7f; // Mesmo delay do telegraph
            }
            break;

        case BOSS_IA_P2_MAGIC_FLOOR_CAST:
            if (boss->actionTimer <= 0) {
                if (player->alive) {
                    // Spawn da magia real após o telegraph
                    Vector2 targetPos = boss->floorMagicTargetPos; // Usa a posição do telegraph
                    float magicRadius = 72.0f; // Raio maior para a área de dano
                    Vector2 magicDir = {0, 0};
                    float animDuration = 0.7f; // Duração da animação
                    SpawnBossMagicAnimTelegraph(targetPos, magicDir, 0, magicRadius, 25, bossFloorDamageFrames, bossFloorDamageFrameCount, 1.0f/14.0f, animDuration, 0);
                }
                boss->magicFloorCooldown = GetRandomValue(7, 12) / 10.0f;
                boss->aiState_p1 = BOSS_IA_P1_IDLE;
                boss->actionTimer = GetRandomValue(3, 7) / 10.0f;
            }
            break;

        case BOSS_IA_P2_MAGIC_PUSH_CHARGE:
            if (boss->actionTimer <= 0) {
                boss->aiState_p1 = BOSS_IA_P2_MAGIC_PUSH_CAST;
                boss->actionTimer = 0.18f;
            }
            break;

        case BOSS_IA_P2_MAGIC_PUSH_CAST:
            if (boss->actionTimer <= 0) {
                Vector2 spawnPos = { boss->position.x + boss->size.x/2 - bossGravityPushFrames[0].width/2, boss->position.y + boss->size.y/2 - bossGravityPushFrames[0].height/2 };
                Vector2 magicDir = {0,0};
                SpawnBossMagicAnimTelegraph(spawnPos, magicDir, 0, 0, 5, bossGravityPushFrames, bossGravityPushFrameCount, 1.0f/14.0f, 0.5f, 0.8f);
                boss->magicPushCooldown = GetRandomValue(10, 16) / 10.0f;
                boss->aiState_p1 = BOSS_IA_P1_IDLE;
                boss->actionTimer = GetRandomValue(3, 7) / 10.0f;
            }
            break;

        // Casos de MOVING, CHASING, PREPARING_DASH, DASHING podem ser reutilizados da Fase 1, com ajustes se necessário
        case BOSS_IA_P1_MOVING: // Reutilizado
            {
                Vector2 direction = Vector2Subtract(boss->targetPosition, boss->position);
                float distance = Vector2Length(direction);
                if (distance > 5.0f && boss->actionTimer > 0) { 
                    direction = Vector2Normalize(direction);
                    boss->position = Vector2Add(boss->position, Vector2Scale(direction, boss->speed * 0.8f * dt)); // Fase 2 pode mover mais rápido
                    boss->isFacingRight = (boss->targetPosition.x > boss->position.x);
                } else {
                    boss->aiState_p1 = BOSS_IA_P1_IDLE;
                    boss->actionTimer = GetRandomValue(3, 8) / 10.0f; // IDLE mais curto
                }
            }
            break;

        case BOSS_IA_P1_CHASING: // Reutilizado
            if (player->alive) {
                Vector2 direction = Vector2Subtract(player->position, boss->position);
                float distance = Vector2Length(direction);
                if (distance > boss->size.x * 0.7f) { 
                    direction = Vector2Normalize(direction);
                    boss->position = Vector2Add(boss->position, Vector2Scale(direction, boss->speed * 1.1f * dt)); // Persegue mais rápido
                    boss->isFacingRight = (player->position.x > boss->position.x);
                }
            }
            if (boss->actionTimer <= 0) {
                boss->aiState_p1 = BOSS_IA_P1_IDLE;
                boss->actionTimer = GetRandomValue(3, 7) / 10.0f; // IDLE mais curto
            }
            break;

        case BOSS_IA_P1_PREPARING_DASH: // Reutilizado
            if (player->alive) {
                boss->targetPosition = player->position; 
            }
            if (boss->actionTimer <= 0) {
                boss->aiState_p1 = BOSS_IA_P1_DASHING;
                boss->actionTimer = 0.6f; // Duração do dash (pode ser menor/mais rápido)
            }
            break;

        case BOSS_IA_P1_DASHING: // Reutilizado
            {
                Vector2 direction = Vector2Subtract(boss->targetPosition, boss->position);
                float distance = Vector2Length(direction);
                float dashSpeed = boss->speed * 3.5f; // Dash mais rápido

                if (distance > dashSpeed * dt && boss->actionTimer > 0) {
                    direction = Vector2Normalize(direction);
                    boss->position = Vector2Add(boss->position, Vector2Scale(direction, dashSpeed * dt));
                } else {
                    boss->dashCooldown = GetRandomValue(50, 80) / 10.0f; // Cooldown menor para dash
                    boss->aiState_p1 = BOSS_IA_P1_IDLE;
                    boss->actionTimer = GetRandomValue(5, 10) / 10.0f; 
                }
            }
            break;

        default:
            boss->aiState_p1 = BOSS_IA_P1_IDLE;
            boss->actionTimer = 0.5f; // Volta para IDLE mais rápido
            break;
    }
}

void BossTakeDamage(Boss *boss, int damage)
{
    if (!boss->isActiveInLogic || boss->isBlinking || boss->isFadingIn) return; // Não toma dano se não estiver ativo ou em transição

    boss->health -= damage;
    AudioPlaySound(SOUND_BOSS_HIT); // Placeholder para som de dano
    if (boss->health <= 0)boss->health = 0; 
}

bool CheckCollisionBossProjectile(Boss boss, Projectile projectile)
{
    // Colisão simples AABB (Axis-Aligned Bounding Box)
    Rectangle bossRec = { boss.position.x, boss.position.y, boss.size.x, boss.size.y };
    Rectangle projectileRec = { 
        projectile.position.x - projectile.radius, 
        projectile.position.y - projectile.radius, 
        projectile.radius * 2, 
        projectile.radius * 2 
    };
    return CheckCollisionRecs(bossRec, projectileRec);
}
