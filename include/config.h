#ifndef CONFIG_H
#define CONFIG_H
#define screenWidth 1600
#define screenHeight 1024
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
#include "player.h"
#include "projectile.h"
#include "enemy.h"

#define WORLD_WIDTH 3200
#define WORLD_HEIGHT 2048

#endif