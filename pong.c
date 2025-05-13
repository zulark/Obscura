#include "raylib.h"
#include "raymath.h"
#define RECTANGLE_COLOR RAYWHITE
#define BACKGROUND_COLOR (Color){64,64,64, 255}

typedef enum GameScreen
{
    MENU = 0,
    GAMEPLAY,
    GAME_OVER
} GameScreen;

typedef enum GameMode {PLAYER_VS_PLAYER, PLAYER_VS_AI} GameMode;

void ResetGame(const int screenWidth, const int screenHeight, Vector2 *ballPos, Vector2 *ballVel, Rectangle *leftPaddle, Rectangle *rightPaddle, float paddleHeight, int *leftScore, int *rightScore)
{
    ballPos->x = (float)screenWidth / 2;
    ballPos->y = (float)screenHeight / 2;
    ballVel->x = 500.0f;
    ballVel->y = 500.0f;
    if (GetRandomValue(0, 1) == 0)
        ballVel->x *= -1;

    leftPaddle->y = (float)screenHeight / 2 - paddleHeight / 2;
    rightPaddle->y = (float)screenHeight / 2 - paddleHeight / 2;

    *leftScore = 0;
    *rightScore = 0;
}

int main()
{
    const int screenWidth = 1600;
    const int screenHeight = 1000;

    InitWindow(screenWidth, screenHeight, "Ping Pong");

    GameScreen currentScreen = MENU;
    GameMode currentGameMode = PLAYER_VS_AI;

    InitAudioDevice();
    Sound fxWavBallBounce = LoadSound("sounds/Hit.wav");
    Sound fxWavScore = LoadSound("sounds/PowerUp3.wav");
    Sound fxWavHitBall = LoadSound("sounds/Pickup1.wav");

    int leftScore = 0;
    int rightScore = 0;

    float paddleWidth = 20;
    float paddleHeight = 120;
    float paddleSpeed = 500.0f;

    Rectangle leftPaddle;
    leftPaddle.x = 40;
    leftPaddle.y = (float)screenHeight / 2 - paddleHeight / 2;
    leftPaddle.width = paddleWidth;
    leftPaddle.height = paddleHeight;

    Rectangle rightPaddle;
    rightPaddle.x = (float)screenWidth - 50 - paddleWidth;
    rightPaddle.y = (float)screenHeight / 2 - paddleHeight / 2;
    rightPaddle.width = paddleWidth;
    rightPaddle.height = paddleHeight;

    Vector2 ballPos;
    Vector2 ballVel;
    float ballRadius = 10.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        // getframetime retorna o tempo em segundos desde o ultimo frame renderizado, usar isso como multiplicador
        // na velocidade do movimento garante uma movimentação especifica independente de quantos fps o jogo ta rodando

        switch (currentScreen)
        {
            case MENU:
            {
                if (IsKeyPressed(KEY_ENTER))
                {
                    currentScreen = GAMEPLAY;
                    currentGameMode = PLAYER_VS_PLAYER;
                    ResetGame(screenWidth, screenHeight, &ballPos, &ballVel, &leftPaddle, &rightPaddle, paddleHeight, &leftScore, &rightScore);
                }
                if(IsKeyPressed(KEY_SPACE))
                {
                    currentScreen = GAMEPLAY;
                    currentGameMode = PLAYER_VS_AI;
                    ResetGame(screenWidth, screenHeight, &ballPos, &ballVel, &leftPaddle, &rightPaddle, paddleHeight, &leftScore, &rightScore);
                }
            }
            break;
            case GAMEPLAY:
            {
                if (IsKeyDown(KEY_W))
                    leftPaddle.y -= paddleSpeed * dt;
                if (IsKeyDown(KEY_S))
                    leftPaddle.y += paddleSpeed * dt;

                if(currentGameMode == PLAYER_VS_PLAYER){
                    if (IsKeyDown(KEY_UP))
                    rightPaddle.y -= paddleSpeed * dt;
                    if (IsKeyDown(KEY_DOWN))
                    rightPaddle.y += paddleSpeed * dt;
                    if (IsKeyPressed(KEY_R))  ResetGame(screenWidth, screenHeight, &ballPos, &ballVel, &leftPaddle, &rightPaddle, paddleHeight, &leftScore, &rightScore);
                }
                //lógica para a IA 
                else{
                    if (IsKeyPressed(KEY_R))  ResetGame(screenWidth, screenHeight, &ballPos, &ballVel, &leftPaddle, &rightPaddle, paddleHeight, &leftScore, &rightScore);
                    float rightPaddleCenterY = rightPaddle.y + rightPaddle.height / 2;
                    float aiPaddleSpeed = paddleSpeed * 0.85f;

                    float deadZone = rightPaddle.height * 0.15f;
                    
                    if(ballPos.y < rightPaddleCenterY - deadZone){
                        rightPaddle.y -= aiPaddleSpeed * dt;
                    }
                    else if(ballPos.y > rightPaddleCenterY + deadZone){ 
                        rightPaddle.y += aiPaddleSpeed * dt;
                    }
                }
                //Fim da lógica da ia 
                if (leftPaddle.y < 0)
                    leftPaddle.y = 0;
                if (leftPaddle.y + leftPaddle.height > screenHeight)
                    leftPaddle.y = screenHeight - leftPaddle.height;

                if (rightPaddle.y < 0)
                    rightPaddle.y = 0;
                if (rightPaddle.y + rightPaddle.height > screenHeight)
                    rightPaddle.y = screenHeight - rightPaddle.height;
                // isso aqui calcula a altura das plataformas comparando ao tamanho da tela, se ele ultrapassar tanto acima ou abaixo da tela ele limita a movimentação

                ballPos.x += ballVel.x * dt;
                ballPos.y += ballVel.y * dt;

                static bool ballBounced = false;
                if ((ballPos.y - ballRadius) <= 0 || (ballPos.y + ballRadius) >= screenHeight)
                {
                    if (!ballBounced)
                    {
                        PlaySound(fxWavBallBounce);
                        ballBounced = true;
                    }
                    ballVel.y *= -1;
                }
                else
                {
                    ballBounced = false;
                }
                // Faz a bola quicar caso bate no topo ou no chao

                if ((ballPos.x + ballRadius) >= screenWidth)
                {
                    ballPos.x = (float)screenWidth / 2;
                    ballPos.y = (float)screenHeight / 2;
                    ballVel.x = 500.0f;
                    ballVel.y = 500.0f;
                    leftScore++;
                    PlaySound(fxWavScore);
                    if (GetRandomValue(0, 1) == 0)
                        ballVel.x *= -1;
                }

                if ((ballPos.x - ballRadius) <= 0)
                {
                    ballPos.x = (float)screenWidth / 2;
                    ballPos.y = (float)screenHeight / 2;
                    ballVel.x = 500.0f;
                    ballVel.y = 500.0f;
                    rightScore++;
                    PlaySound(fxWavScore);
                    if (GetRandomValue(0, 1) == 0)
                        ballVel.x *= -1;
                }

                if (CheckCollisionCircleRec(ballPos, ballRadius, leftPaddle))
                {   
                    if (ballVel.x < 0)
                        PlaySound(fxWavHitBall);
                        ballVel.x *= -1.05f;
                }

                if (CheckCollisionCircleRec(ballPos, ballRadius, rightPaddle))
                {
                    if (ballVel.x > 0)
                        PlaySound(fxWavHitBall);
                        ballVel.x *= -1.05f;
                }
            }break;
            case GAME_OVER:
            {
                // TODO
            }break;
            default: break;
        }

        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        if(currentScreen == MENU){
            DrawText("PING PONG", screenWidth / 2 - MeasureText("PING PONG", 80) / 2, screenHeight / 2 - 120, 80, RAYWHITE);
 
            DrawText("Pressione ENTER para jogar contra a máquina", screenWidth / 2 - MeasureText("Pressione ENTER para jogar contra a máquina", 30) / 2, screenHeight / 2 - 20, 30, LIGHTGRAY);
            DrawText("Pressione ESPAÇO para jogar contra outro jogador", screenWidth / 2 - MeasureText("Pressione ENTER para jogar contra outro jogador", 30) / 2, screenHeight / 2 - 50, 30, LIGHTGRAY);
 
            DrawText("Jogador 1: W (Cima) / S (Baixo)", screenWidth / 2 - MeasureText("Jogador 1: W (Cima) / S (Baixo)", 20) / 2, screenHeight / 2 + 30, 20, LIGHTGRAY);
            DrawText("Jogador 2: Seta pra CIMA / seta para BAIXO", screenWidth / 2 - MeasureText("Jogador 2: Seta pra CIMA / seta para BAIXO", 20) / 2, screenHeight / 2 + 60, 20, LIGHTGRAY);

        }else if(currentScreen == GAMEPLAY) {
            //DrawCircle(screenWidth/2, screenHeight/2, 30.0, ColorAlpha(WHITE, 0.5f));
            //DrawLine(screenWidth/2, 0, screenWidth/2, screenHeight, ColorAlpha(WHITE, 5.0f));
            //DrawRectangleLines(0, 0, 120, screenHeight, ColorAlpha(WHITE, 2.0F));  // Draw left goal line
            //DrawRectangleLines(screenWidth - 80, 0, 120, screenHeight, ColorAlpha(WHITE, 5.0f));  // Draw right goal line
            
            DrawRectangleRec(leftPaddle, RECTANGLE_COLOR);
            DrawRectangleRec(rightPaddle, RECTANGLE_COLOR);
            DrawCircleV(ballPos, ballRadius, WHITE);
            
            DrawText(TextFormat("%d", leftScore), screenWidth / 4 - MeasureText(TextFormat("%d", leftScore), 60) / 2, 30, 60, RAYWHITE);
            DrawText(TextFormat("%d", rightScore), screenWidth * 3 / 4 - MeasureText(TextFormat("%d", rightScore), 60) / 2, 30, 60, RAYWHITE);
            DrawText("Pressione R para resetar o jogo", screenWidth / 2 - MeasureText("Pressione R para resetar o jogo", 20) / 2, 10, 30, RAYWHITE);
        }

        EndDrawing();
    }
    UnloadSound(fxWavBallBounce);
    UnloadSound(fxWavScore);
    UnloadSound(fxWavHitBall);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}