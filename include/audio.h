#ifndef AUDIO_H
#define AUDIO_H
#include "raylib.h"

typedef enum {
    SOUND_SHOOT,
    SOUND_PLAYER_HIT,
    SOUND_LEVELUP,
    SOUND_ENEMY_DEATH,
    SOUND_MAGIC_CAST,
    SOUND_MAGIC_PICKUP,
    SOUND_COUNT
} SoundID;

void AudioInit();
void AudioPlaySound(SoundID id);
void AudioPlayMusic(Music *music);
void AudioSetSoundVolume(SoundID id, float volume);
void AudioSetSoundPitch(SoundID id, float pitch);
void AudioUpdate();
void AudioUnload();

#endif // AUDIO_H
