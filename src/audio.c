#include "audio.h"
#include "raylib.h"

static Sound sounds[SOUND_COUNT];

void AudioInit() {
    sounds[SOUND_SHOOT] = LoadSound("assets/sounds/player/shoot.wav");
    sounds[SOUND_PLAYER_HIT] = LoadSound("assets/sounds/player/player_hit.wav");
    sounds[SOUND_LEVELUP] = LoadSound("assets/sounds/player/levelup.wav");
    sounds[SOUND_ENEMY_SHOOT] = LoadSound("assets/sounds/enemy/enemy_shoot.wav");   
    sounds[SOUND_ENEMY_DEATH] = LoadSound("assets/sounds/enemy/death.wav");
    sounds[SOUND_MAGIC_CAST] = LoadSound("assets/sounds/player/area_damage_magic.wav");
    sounds[SOUND_PLAYER_MAGIC_AREA] = LoadSound("assets/sounds/player/FireBomb.wav");
    sounds[SOUND_PLAYER_MAGIC_DASH] = LoadSound("assets/sounds/player/dash.wav");
    sounds[SOUND_MAGIC_PICKUP] = LoadSound("assets/sounds/player/magic_pickup.wav");
    sounds[SOUND_NO_MANA] = LoadSound("assets/sounds/player/no_mana.wav");
    sounds[SOUND_BOSS_PHASE_CHANGE] = LoadSound("assets/sounds/boss/transaction.wav");
    sounds[SOUND_BOSS_DEATH] = LoadSound("assets/sounds/boss/death.wav");
    sounds[SOUND_BOSS_HIT] = LoadSound("assets/sounds/boss/hit.wav");
}

void AudioPlaySound(SoundID id) {
    if (id >= 0 && id < SOUND_COUNT) PlaySound(sounds[id]);
}

void AudioPlayMusic(Music *music) {
    if (music) PlayMusicStream(*music);
}

void AudioSetSoundVolume(SoundID id, float volume) {
    if (id >= 0 && id < SOUND_COUNT) SetSoundVolume(sounds[id], volume);
}
void AudioSetSoundPitch(SoundID id, float pitch) {
    if (id >= 0 && id < SOUND_COUNT) SetSoundPitch(sounds[id], pitch);
}

void AudioUpdate() {
}

void AudioUnload() {
    for (int i = 0; i < SOUND_COUNT; i++) UnloadSound(sounds[i]);
}
