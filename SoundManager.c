//------------------------------------------------------------------------------
//
// File Name:	SoundManager.c
// Author(s):	Gabe Gramblicka (gabriel.gramblicka)
// Course:		GAM150
//
// Copyright © 2023 DigiPen (USA) Corporation.
//
//------------------------------------------------------------------------------

#include "stdafx.h"

#pragma warning( disable: 4201 )
#include "SoundManager.h"
#include "Sound.h"
#include "Trace.h"
#include "fmod.h"

#include "Stream.h"
#include "DGL.h"
#include "Entity.h"
#include "Miniboat.h"
#include "Transform.h"
#include "fishengine.h"
#include "Scene.h"
#include "Level.h"
#include "SavingSystem.h"
#include "Map.h"

#include <stdarg.h>
#include <stdio.h>

#define soundArraySize 100

typedef struct SoundManager
{
    Sound* effects[soundArraySize];

    Sound* music[soundArraySize][3];

    float volume;

}SoundManager;

SoundManager* soundList;
SoundManager* musicList;

void SoundManagerCreate()
{
    soundList = calloc(1, sizeof(SoundManager));
    musicList = calloc(1, sizeof(SoundManager));

    if (soundList)
    {
        soundList->volume = 1.0f;
    }
    if (musicList)
    {
        musicList->volume = 1.0f;
    }
}

void SoundManagerUnload()
{
    free(soundList);
    free(musicList);
}

void SoundManagerExit()
{
    for (int i = 0; i < soundArraySize; ++i)
    {
        if (soundList->effects[i])
        {
            if (strcmp(SoundGetName(soundList->effects[i]), "./Assets/Audio/swampambience.ogg"))
            {
                SoundStop(soundList->effects[i]);
                SoundSetDone(soundList->effects[i], false);
                SoundSetPlaying(soundList->effects[i], false);
                SoundSetTimer(soundList->effects[i], 0);
            }
        }
    }
    for (int i = 0; i < soundArraySize; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (musicList->music[i][j])
            {
                SoundStop(musicList->music[i][j]);
                SoundSetDone(musicList->music[i][j], false);
                SoundSetPlaying(musicList->music[i][j], false);
                SoundSetTimer(musicList->music[i][j], 0);
            }
        }
    }
}

void SoundManagerAddToList(Sound* sound, SoundType type)
{
    if (sound)
    {
        if (type == EFFECT || type == LONGEFFECT)
        {
            for (unsigned int i = 0; i < soundArraySize; ++i)
            {
                if (soundList->effects[i] == NULL)
                {
                    soundList->effects[i] = sound;
                    return;
                }
            }
        }
        if (type == MUSIC)
        {
            for (unsigned int i = 0; i < soundArraySize; ++i)
            {
                for (unsigned int j = 0; j < 3; ++j)
                {
                    if (musicList->music[i][j] == NULL)
                    {
                        musicList->music[i][j] = sound;
                        return;
                    }
                }
            }
        }
    }
}

void SoundManagerBuild(const char* filename)
{
    if (filename)
    {
        Stream stream = StreamOpen(filename);

        if (stream)
        {
            SoundManagerLoad(stream);
            StreamClose(&stream);
        }
    }
}

void SoundManagerLoad()
{   
    // song 1
    SoundLoad("./Assets/TugboatTerrorTheme0.ogg", publicSoundSystem, MUSIC);
    SoundLoad("./Assets/TugboatTerrorTheme1.ogg", publicSoundSystem, MUSIC);
    SoundLoad("./Assets/TugboatTerrorTheme2.ogg", publicSoundSystem, MUSIC);

    // song 2
    SoundLoad("./Assets/12 Bars of Terror0.ogg", publicSoundSystem, MUSIC);
    SoundLoad("./Assets/12 Bars of Terror1.ogg", publicSoundSystem, MUSIC);
    SoundLoad("./Assets/12 Bars of Terror2.ogg", publicSoundSystem, MUSIC);

    // song 3
    SoundLoad("./Assets/AllAboard0.ogg", publicSoundSystem, MUSIC);
    SoundLoad("./Assets/AllAboard1.ogg", publicSoundSystem, MUSIC);
    SoundLoad("./Assets/AllAboard2.ogg", publicSoundSystem, MUSIC);

    // song 4
    SoundLoad("./Assets/TheNightbegins0.ogg", publicSoundSystem, MUSIC);
    SoundLoad("./Assets/TheNightbegins1.ogg", publicSoundSystem, MUSIC);
    SoundLoad("./Assets/TheNightbegins2.ogg", publicSoundSystem, MUSIC);


    // water / ambience
    SoundLoad("./Assets/Audio/swampambience.ogg", publicSoundSystem, EFFECT);
}

void SoundManagerUpdate(const Scene* scene, float dt)
{
    for (int i = 0; i < soundArraySize; ++i)
    {
        if (soundList->effects[i])
        {
            SoundUpdate(soundList->effects[i], dt);
        }
        for (int j = 0; j < 3; ++j)
        {
            if (musicList->music[i][j])
            {
                SoundUpdate(musicList->music[i][j], dt);
                SoundManagerMusicControllerUpdate(scene, saveData->currentLevel);
            }
        }
    }
}

void SoundManagerUnloadSound()
{
    // Tell the compiler that the 'dt' variable is unused.
    for (int i = 0; i < soundArraySize; ++i)
    {
        if (soundList->effects[i])
        {
            SoundExit(&soundList->effects[i]);
        }
    }
    for (int i = 0; i < soundArraySize; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (musicList->music[i][j])
            {
                SoundExit(&musicList->music[i][j]);
            }
        }
    }
}

void SoundManagerStopAll()
{
    for (int i = 0; i < soundArraySize; ++i)
    {
        if (soundList->effects[i])
        {
            SoundStop(soundList->effects[i]);
        }
    }
}

void SoundManagerVolumeAll()
{
    for (int i = 0; i < soundArraySize; ++i)
    {
        if (soundList->effects[i])
        {
            SoundVolume(soundList->effects[i], soundList->volume);
        }
    }
    for (int i = 0; i < soundArraySize; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (musicList->music[i][j])
            {
                SoundVolume(musicList->music[i][j], musicList->volume);
            }
        }
    }
}


void SoundManagerSetVolumeAll(float volume)
{
    musicList->volume = volume;
    soundList->volume = volume;
}

void SoundManagerPauseAll()
{
    for (int i = 0; i < soundArraySize; ++i)
    {
        if (soundList->effects[i])
        {
            if (!SoundGetDone(soundList->effects[i]))
            {
                SoundPause(soundList->effects[i], 1);
            }
        }
        for (int j = 0; j < 3; ++j)
        {
            if (musicList->music[i][j])
            {
                if (!SoundGetDone(musicList->music[i][j]))
                {
                    SoundPause(musicList->music[i][j], 1);
                }
            }
        }
    }
}

void SoundManagerUnpauseAll()
{
    for (int i = 0; i < soundArraySize; ++i)
    {
        if (soundList->effects[i])
        {
            if (!SoundGetDone(soundList->effects[i]))
            {
                SoundPause(soundList->effects[i], 0);
            }
        }
        for (int j = 0; j < 3; ++j)
        {
            if (musicList->music[i][j])
            {
                if (!SoundGetDone(musicList->music[i][j]))
                {
                    SoundPause(musicList->music[i][j], 0);
                }
            }
        }
    }
}

Sound* SoundManagerGetSound(const char* entityName)
{
    if (soundList)
    {
        for (unsigned int i = 0; i < soundArraySize; ++i)
        {
            if (SoundIsNamed(soundList->effects[i], entityName))
            {
                return soundList->effects[i];
            }
            for (unsigned int j = 0; j < 3; ++j)
            {
                if (SoundIsNamed(musicList->music[i][j], entityName))
                {
                    return musicList->music[i][j];
                }
            }
        }
    }
    return NULL;
}

// NOTE: level starts at 0
void SoundManagerMusicControllerUpdate(const Scene* scene, unsigned level)
{
    if (level > 3)
    {
        level = 3;
    }

    if (scene == LevelGetInstance())
    {
        if (saveData->currentLevel >= 0)
        {
            SoundPlay(musicList->music[level][0], 0, 0.3f);
            if (SoundGetDone(musicList->music[level][0]))
            {
                SoundPause(musicList->music[level][0], 1);
                SoundSetDone(musicList->music[level][0], TRUE);
                SoundPlay(musicList->music[level][1], 1, 0.3f);
            }
            Entity* miniboat = MiniboatGetEntity();
            Entity* goal = GoalGetEntity();
            if (miniboat)
            {
                Transform* miniboatTransform = EntityGetTransform(miniboat);
                Vector2D miniboatPos = *TransformGetTranslation(miniboatTransform);

                Transform* goalTransform = EntityGetTransform(goal);
                Vector2D goalPos = *TransformGetTranslation(goalTransform);

                if (miniboatPos.y > goalPos.y - 1200)
                {
                    if (SoundGetDone(musicList->music[level][1]))
                    {
                        SoundPause(musicList->music[level][1], 1);
                        SoundSetDone(musicList->music[level][1], TRUE);
                        SoundPlay(musicList->music[level][2], 0, 0.3f);
                    }
                }
            }
        }
    }
}

bool SoundManagerIntercept(Sound* sound, int amount, ...)
{
    va_list ptr;
    va_start(ptr, amount);

    if (SoundGetPlaying(sound))
    {
        for (int i = 0; i < amount; i++)
        {
            const char* arg = va_arg(ptr, const char*);
            if (arg && !strcmp(SoundGetName(sound), arg))
            {
                va_end(ptr);
                return true;
            }
        }
    }

    va_end(ptr);
    return false;
}