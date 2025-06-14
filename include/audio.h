#ifndef AUDIO_H
#define AUDIO_H
#include "raylib.h"

typedef enum {
    SOUND_SHOOT,
    SOUND_PLAYER_HIT,
    SOUND_LEVELUP,
    SOUND_ENEMY_DEATH,
    SOUND_ENEMY_SHOOT,
    SOUND_MAGIC_CAST,
    SOUND_PLAYER_MAGIC_AREA,
    SOUND_PLAYER_MAGIC_DASH,
    SOUND_MAGIC_PICKUP,
    SOUND_NO_MANA,
    SOUND_BOSS_DEATH,
    SOUND_BOSS_HIT,
    SOUND_BOSS_PHASE_CHANGE,
    SOUND_COUNT
} SoundID;

void AudioInit();
void AudioPlaySound(SoundID id);
void AudioPlayMusic(Music *music);
void AudioSetSoundVolume(SoundID id, float volume);
void AudioSetSoundPitch(SoundID id, float pitch);
void AudioUpdate();
void AudioUnload();

#endif