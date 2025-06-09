#include "cursor.h"
#include "raylib.h"

static Texture2D cursor;

void CursorInit() {
    cursor = LoadTexture("assets/sprites/cursor.png");
    HideCursor();
}

void CursorUnload() {
    if (cursor.id > 0) UnloadTexture(cursor);
}

// Desenha cursor
void CursorDraw() {
    Vector2 pos = GetMousePosition();
    float scale = 0.20f; // ajuste o valor para o tamanho desejado
    Vector2 offset = {(200/2) * scale, (200/2) * scale};
    DrawTextureEx(cursor, (Vector2){pos.x - offset.x, pos.y - offset.y}, 0.0f, scale, WHITE);
}
