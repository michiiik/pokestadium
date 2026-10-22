#include "audio_channel.h"
#include "src/audio_sfx.h"
#include "src/libnumus/player.h"
#include "src/3D140.h"

void Audio_SetChannelVolume(s32 arg0, u32 arg1) {
    s32 var_a1;

    switch (arg1) {
        case 0:
            var_a1 = 0;
            break;

        case 1:
            var_a1 = 0x3FFF;
            break;

        case 2:
            var_a1 = 0x4FFF;
            break;

        case 3:
            var_a1 = 0x5FFF;
            break;

        case 4:
            var_a1 = 0x6FFF;
            break;

        case 5:
            var_a1 = 0x7FFF;
            break;

        default:
            var_a1 = 0x6FFF;
            break;
    }

    if (arg0 & 1) {
        gMusicChannel1MasterVolume = var_a1;
    }

    if (arg0 & 2) {
        gMusicChannel2MasterVolume = var_a1;
    }

    Audio_QueueSetVolumeCommand(arg0, var_a1);
}

void Audio_SetMonoOutput(s32 arg0) {
    Audio_QueueSetMonoOutputCommand(arg0);
}

void Audio_SetPauseMixMode(u32 arg0) {
    switch (arg0) {
        case 1:
            Audio_QueueFadeSoundCommand(gMusicTransitionSfxId, 3);
            Audio_QueueSetVolumeCommand(2, gMusicChannel2MasterVolume >> 1);
            Audio_QueueSetVolumeCommand(1, gMusicChannel1MasterVolume / 3);
            gMusicTransitionSfxId = Audio_PlaySoundEffectById(SFX_PAUSE);
            Audio_QueueVoiceVolumeScale(gMusicTransitionSfxId, 0xFF);
            break;

        case 0:
            Audio_QueueFadeSoundCommand(gMusicTransitionSfxId, 3);
            Audio_QueueSetVolumeCommand(2, gMusicChannel2MasterVolume);
            Audio_QueueSetVolumeCommand(1, gMusicChannel1MasterVolume);
            gMusicTransitionSfxId = Audio_PlaySoundEffectById(SFX_RESUME);
            break;

        case 2:
            Audio_QueueVoiceVolumeScale(Audio_PlaySoundEffectById(SFX_MENU_SCROLL), 0xFF);
            break;

        case 3:
            Audio_QueueVoiceVolumeScale(Audio_PlaySoundEffectById(0x21), 0xFF);
            break;

        case 4:
            Audio_QueueFadeSoundCommand(gMusicTransitionSfxId, 3);
            Audio_QueueSetVolumeCommand(2, gMusicChannel2MasterVolume);
            Audio_QueueSetVolumeCommand(1, gMusicChannel1MasterVolume);
            break;
    }
}

void Audio_BeginVolumeTransition(u32 arg0) {
    u32 temp_v0;

    if (arg0 == 0) {
        arg0 = 1;
    }

    temp_v0 = osSetIntMask(1);

    gMusicChannel2Volume = gMusicChannel2MasterVolume;
    gMusicChannel1Volume = gMusicChannel1MasterVolume;
    gMusicChannel2Step = gMusicChannel2Volume / arg0;
    gMusicChannel1Step = gMusicChannel1Volume / arg0;
    mus_sound_effect_blocked = 1;

    osSetIntMask(temp_v0);
}

void Audio_ResetVolumeTransition(void) {
    u32 temp_v0 = osSetIntMask(1);

    mus_sound_effect_blocked = 0;
    gMusicChannel2Volume = 0.0f;
    gMusicChannel1Volume = 0.0f;

    osSetIntMask(temp_v0);

    Audio_QueueSetVolumeCommand(2, gMusicChannel2MasterVolume);
    Audio_QueueSetVolumeCommand(1, gMusicChannel1MasterVolume);
}
