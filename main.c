#include "raylib.h"
#include "raymath.h"

int main(void){
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "OBSCURA");
    SetTargetFPS(60);

    int circlePositionX =  screenWidth / 2;
    int circlePositionY =  screenHeight / 2;
    int circleRadius = 25;

    int velocity = 5;

    Color circleColor = BLACK;

    while( ! WindowShouldClose()){
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawCircle(circlePositionX, circlePositionY, circleRadius, circleColor);

            if(IsKeyDown(KEY_RIGHT)){
                circlePositionX += velocity;
            }
            
            if(IsKeyDown(KEY_LEFT)){
                circlePositionX -= velocity;
            }
            
            if(IsKeyDown(KEY_DOWN)){
                circlePositionY += velocity;
            }
            
            if(IsKeyDown(KEY_UP)){
                circlePositionY -= velocity;
            }


        EndDrawing();
        
    }
    
    CloseWindow();

    return 0;
}