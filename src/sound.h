#ifndef _MIIN_PROJECT_SOUND_H
#define _MIIN_PROJECT_SOUND_H

#ifdef __cplusplus
extern "C" {
#endif




typedef enum _SoundType_e
{
    SOUND_MIIN = 0,
    SOUND_BRIGHTONE,
    SOUND_PEAKLIGHT,
    SOUND_AUTO,
    SOUND_MANUAL,
    SOUND_READY,
    SOUND_STANDBY,
    SOUND_WARNING_HANDPIECE,
    SOUND_SHOTCOUNT_TOTAL,
    SOUND_SHOTCOUNT_CLEAR,
    SOUND_LOAD_MEMORY1,
    SOUND_LOAD_MEMORY2,
    SOUND_LOAD_MEMORY3,
    SOUND_SAVE_MEMORY1,
    SOUND_SAVE_MEMORY2,
    SOUND_SAVE_MEMORY3,
    SOUND_MEMORY_MODE,
    SOUND_EASY_MODE,
    SOUND_0SEC,
    SOUND_30SEC,
    SOUND_60SEC,
    SOUND_90SEC,
    SOUND_120SEC,
    SOUND_THIS_VOICE,
    SOUND_MAXIMUM
} _sound_typee;



void send_sound(int cmd);

void init_sound(void);
void close_sound(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
