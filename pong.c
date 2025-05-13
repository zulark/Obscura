#include "raylib.h"
#include "raymath.h"
#define RECTANGLE_COLOR RAYWHITE

int main(){
    const int screenWidth = 1600;
    const int screenHeight = 1000;

    int leftScore = 0;
    int rightScore = 0;

    float paddleWidth = 20;
    float paddleHeight = 100;
    float paddleSpeed = 500.0f;

    Rectangle leftPaddle;
    leftPaddle.x = 50;
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
    
    ballPos.x = (float)screenWidth/2;
    ballPos.y = (float)screenHeight/2;
    ballVel.x = 500.0f; 
    ballVel.y = 500.0f; 


    InitWindow(screenWidth, screenHeight, "Ping Pong");
    SetTargetFPS(60);

    while(!WindowShouldClose()){
        float dt = GetFrameTime();
        // getframetime retorna o tempo em segundos desde o ultimo frame renderizado, usar isso como multiplicador 
        // na velocidade do movimento garante uma movimentação especifica independente de quantos fps o jogo ta rodando

        BeginDrawing();
        ClearBackground(BLACK);
        
        DrawRectangleRec(leftPaddle, RECTANGLE_COLOR);
        DrawRectangleRec(rightPaddle, RECTANGLE_COLOR);
        DrawCircleV(ballPos, ballRadius, YELLOW);
        
        DrawText("Mova a plataforma esquerda W/S", 10, 10, 20, RED);
        DrawText("Mova a plataforma direita com os direcionais ", screenWidth - 500, 10, 20, RED);
        DrawText(TextFormat("J1: %d pontos", leftScore), 10, 30, 20, RED);
        DrawText(TextFormat("J2: %d pontos", rightScore), screenWidth - 500, 30, 20, RED);

        if(IsKeyDown(KEY_W)) leftPaddle.y -= paddleSpeed * dt;
        if(IsKeyDown(KEY_S)) leftPaddle.y += paddleSpeed * dt;

        if(IsKeyDown(KEY_UP)) rightPaddle.y -= paddleSpeed * dt;
        if(IsKeyDown(KEY_DOWN)) rightPaddle.y += paddleSpeed * dt;


        if (leftPaddle.y < 0) leftPaddle.y = 0;
        if (leftPaddle.y + leftPaddle.height > screenHeight) leftPaddle.y = screenHeight - leftPaddle.height;

        if (rightPaddle.y < 0) rightPaddle.y = 0;
        if (rightPaddle.y + rightPaddle.height > screenHeight) rightPaddle.y = screenHeight - rightPaddle.height;
        //isso aqui calcula a altura das plataformas comparando ao tamanho da tela, se ele ultrapassar tanto acima ou abaixo da tela ele limita a movimentação

        ballPos.x += ballVel.x * dt;
        ballPos.y += ballVel.y * dt;
        
        if((ballPos.y - ballRadius) <= 0 || (ballPos.y + ballRadius) >= screenHeight) ballVel.y *= -1;
        //Faz a bola quicar caso bate no topo ou no chao

        if((ballPos.x + ballRadius) >= screenWidth){
            ballPos.x = (float)screenWidth/2;
            ballPos.y = (float)screenHeight/2;
            ballVel.x = 500.0f;
            ballVel.y = 500.0f; 
            leftScore++;
            if (GetRandomValue(0, 1) == 0) ballVel.x *= -1;
        }

         if((ballPos.x - ballRadius) <= 0){ 
            ballPos.x = (float)screenWidth/2;
            ballPos.y = (float)screenHeight/2;
            ballVel.x = 500.0f;
            ballVel.y = 500.0f; 
            rightScore++;
            if (GetRandomValue(0, 1) == 0) ballVel.x *= -1;
         }


        if(CheckCollisionCircleRec(ballPos, ballRadius, leftPaddle)){
            if(ballVel.x < 0) ballVel.x *= -1.05f;
        }

        if(CheckCollisionCircleRec(ballPos, ballRadius, rightPaddle)){
            if(ballVel.x > 0) ballVel.x *= -1.05f;
        }


        EndDrawing();
    }
    CloseWindow();
    return 0;
}