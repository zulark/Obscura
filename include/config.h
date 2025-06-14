#ifndef CONFIG_H
#define CONFIG_H
#define screenWidth 1920
#define screenHeight 1080
#define dt GetFrameTime()
#define gameName "Obscura"
#define gameVersion "0.1"
#define gameAuthor "Felipe"
#define gameDescription "A game made with raylib"

#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "audio.h"
#include "boss.h"
#include "cursor.h"
#include "enemy.h"
#include "magic.h"
#include "particle.h"
#include "player.h"
#include "projectile.h"
#include "ui.h"
#include "boss_magic.h"
#include "boss_projectile.h"


#define WORLD_WIDTH 3200
#define WORLD_HEIGHT 2048

typedef enum GameState
{
    GAME_MENU,
    GAME_IS_PLAYING,
    GAME_PAUSED,
    GAME_OVER
} GameState;

#endif