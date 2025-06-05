#ifndef CONFIG_H
#define CONFIG_H
#define screenWidth 1536
#define screenHeight 1024
#define dt GetFrameTime()
#define gameName "Obscura"
#define gameVersion "0.1"
#define gameAuthor "Felipe Rudnik"
#define gameDescription "A game made with raylib"

#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "player.h"
#include "projectile.h"
#include "enemy.h"

#define WORLD_WIDTH 3000
#define WORLD_HEIGHT 3000

#endif