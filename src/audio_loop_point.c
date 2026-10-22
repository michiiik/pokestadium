#include "audio_loop_point.h"
#include "src/libnumus/player.h"
#include "src/audio_channel.h"
#include "src/audio_commands_category2.h"
#include "src/rom_device.h"
#include "src/audio_stream.h"
#include "src/audio_cache.h"
#include "src/3D140.h"
#include "src/audio_commands.h"

static s32 gLastMusicHandle;
static s32 gCurrentAnimationId;

extern u8 D_80078E83;
extern u8 D_80078E8B;
extern u8 D_80078E93;
extern u8 D_80078E9B;
extern u8 D_80078EA3;
extern u8 D_80078EAB;
extern u8 D_80078EB3;
extern u8 gBgmCategory12TrackIds;
extern u8 D_80078EBF;

void Audio_SetMusicLoopPoint(s32 arg0) {
    gMusicFadeDuration = arg0;
}

s32 Audio_StartMusicTrack(s32 track_id) {
    u32 file_index;
    s32 file_size;
    UNUSED s32 pad;
    s32 volume_scale;
    s32 voice_volume;

    volume_scale = track_id;
    voice_volume = 0x80;
    AudioStream_SetEnabled(1);
    Audio_SetVoiceStartTimingMode(1);
    gCurrentAnimationId = track_id;
    if (track_id == 0x50) {
        gMusicStateFlag1 = 0;
        gMusicStateFlag2 = 0;
        if (gBgmFadeState == 1) {
            Audio_StartVolumeTransition(gCurrentBgmHandle, 0, 0x80, 0x50);
            gBgmFadeState = 2;
            return 0;
        }
        Audio_StopAllSoundEffects();
        gMusicHandle = 0;
        Audio_QueueFadeSoundCommand(gCurrentBgmHandle, 1);
        gBgmFadeState = 0;
        gCurrentBgmHandle = Audio_PlayDirectSoundCommand(0x01000003, 0, 0);
        track_id = 0x28;
        gBgmFadeState = 2;
    } else if (track_id == 0x28) {
        gMusicStateFlag1 = 0;
        if (gBgmFadeState == 2) {
            Audio_StartVolumeTransition(gCurrentBgmHandle, 0x80, 0, 0x50);
            gBgmFadeState = 1;
            return 0;
        }
        Audio_StopAllSoundEffects();
        gMusicHandle = 0;
        Audio_QueueFadeSoundCommand(gCurrentBgmHandle, 1);
        gBgmFadeState = 0;
        gCurrentBgmHandle = Audio_PlayDirectSoundCommand(0x01000003, 0, 0);
        Audio_QueueVoiceVolumeScale(gCurrentBgmHandle, 0);
        gBgmFadeState = 1;
    } else {
        Audio_StopAllSoundEffects();
        gMusicHandle = 0;
        Audio_QueueFadeSoundCommand(gCurrentBgmHandle, 1);
        gBgmFadeState = 0;
    }

    if (track_id == 0x32) {
        if ((gCurrentStadiumPhase < 0xC) && (gPlayerRemainingPokemon[0] > 0) && (gPlayerRemainingPokemon[1] == 0) && (gStadiumPhaseSubstate == 8)) {
            track_id = 0x3C;
        } else if ((gCurrentStadiumPhase == 0xD) && (gPlayerRemainingPokemon[0] > 0) && (gPlayerRemainingPokemon[1] == 0) && (gStadiumPhaseSubstate == 5)) {
            track_id = 0x3E;
            gCurrentAnimationId = 0x3E;
        } else if ((gCurrentStadiumPhase == 0xF) && (gPlayerRemainingPokemon[0] > 0) && (gPlayerRemainingPokemon[1] == 0)) {
            track_id = 0x40;
        }
    }

    if ((track_id == 0x3F) && (gCurrentStadiumPhase == 0xD) && (gStadiumPhaseSubstate == 5)) {
        track_id = 0x41;
    }

    if ((track_id == 0x31) && (gMusicStateFlag1 != 0)) {
        track_id = 0x46;
    }

    if ((track_id == 0x3A) && (gMusicStateFlag2 != 0)) {
        track_id = 0x48;
    }

    file_index = track_id;
    switch (track_id) {
        case 0x0:
            gMusicStateFlag1 = 0;
            gMusicStateFlag2 = 0;
            gStadiumModeActive = 1;
            volume_scale = 0x98;
            break;

        case 0x1:
            volume_scale = 0x68;
            gStadiumModeActive = 1;
            Audio_SelectModeMusic(0, 0xF);
            break;

        case 0x9:
            volume_scale = 0x50;
            break;

        case 0x7:
            volume_scale = 0x78;
            break;

        case 0x8:
            volume_scale = 0xA0;
            break;

        case 0xF:
            volume_scale = 0x60;
            break;

        case 0xA:
            volume_scale = 0x60;
            break;

        case 0x11:
            volume_scale = 0x60;
            break;

        case 0x17:
            volume_scale = 0x60;
            break;

        case 0x18:
            volume_scale = 0x60;
            break;

        case 0x19:
            volume_scale = 0x58;
            break;

        case 0x1C:
            volume_scale = 0x60;
            break;

        case 0x1D:
            volume_scale = gDefaultMusicVolume;
            break;

        case 0x1E:
            volume_scale = 0x65;
            break;

        case 0x1F:
            volume_scale = 0x70;
            break;

        case 0x20:
            volume_scale = 0x70;
            break;

        case 0x21:
            volume_scale = 0x50;
            break;

        case 0x1A:
            volume_scale = 0x60;
            Audio_ResetVolumeTransition();
            break;

        case 0x1B:
            volume_scale = 0x58;
            Audio_ResetVolumeTransition();
            break;

        case 0x2C:
            volume_scale = 0x70;
            break;

        case 0x13:
            volume_scale = 0x68;
            break;

        case 0x38:
            volume_scale = 0x60;
            break;

        case 0x15:
            volume_scale = 0x60;
            break;

        case 0x2D:
            volume_scale = 0x60;
            break;

        case 0x16:
            volume_scale = 0x60;
            break;

        case 0x10:
            volume_scale = 0x68;
            break;

        case 0x2A:
            volume_scale = 0x60;
            break;

        case 0x27:
            volume_scale = 0x58;
            break;

        case 0x2B:
            volume_scale = 0x60;
            break;

        case 0x30:
            gMusicStateFlag1 = 1;
            volume_scale = 0x64;
            break;

        case 0x28:
            gMusicStateFlag1 = 0;
            gMusicStateFlag2 = 0;
            volume_scale = 0x48;
            break;

        case 0x29:
            gMusicStateFlag1 = 0;
            gMusicStateFlag2 = 0;
            volume_scale = 0x78;
            voice_volume = 0x87;
            break;

        case 0x31:
            volume_scale = 0x60;
            break;

        case 0x33:
            volume_scale = 0x60;
            break;

        case 0x46:
            volume_scale = 0x60;
            break;

        case 0x32:
            volume_scale = 0x60;
            break;

        case 0x3C:
            volume_scale = 0x60;
            break;

        case 0x3D:
            volume_scale = 0x88;
            break;

        case 0x3E:
            volume_scale = 0x60;
            break;

        case 0x3F:
            volume_scale = 0x60;
            break;

        case 0x40:
            volume_scale = 0x50;
            break;

        case 0x41:
            volume_scale = 0x60;
            break;

        case 0x45:
            volume_scale = 0x60;
            break;

        case 0x44:
            volume_scale = 0x90;
            break;

        case 0x47:
            volume_scale = 0x88;
            gMusicStateFlag2 = 1;
            break;

        case 0x5B:
            volume_scale = 0x78;
            file_index = 1;
            break;

        case 0x3A:
            volume_scale = 0x68;
            Audio_SelectModeMusic(0, 0xF);
            Audio_PlayCategory11SoundCommand(0x01100016, 0, 0);
            break;

        case 0x48:
            Audio_SelectModeMusic(0, 0xF);
            Audio_PlayCategory11SoundCommand(0x01100016, 0, 0);
            volume_scale = 0x70;
            break;

        case 0x49:
            volume_scale = 0x68;
            break;

        case 0x4A:
            volume_scale = 0xA8;
            break;

        case 0x4B:
            volume_scale = 0x60;
            break;

        case 0x42:
            gMusicStateFlag1 = 0;
            gMusicStateFlag2 = 0;
            volume_scale = 0x80;
            break;

        case 0x43:
            gMusicStateFlag1 = 0;
            gMusicStateFlag2 = 0;
            volume_scale = 0x80;
            break;

        case 0x22:
            volume_scale = 0x48;
            break;

        case 0x24:
            volume_scale = 0x48;
            break;

        case 0x26:
            volume_scale = 0x40;
            break;

        case 0x23:
            volume_scale = 0x50;
            break;

        case 0x25:
            volume_scale = 0x50;
            break;

        case 0xE:
            volume_scale = 0x64;
            break;

        case 0xD:
            volume_scale = 0x48;
            break;

        case 0xB:
            volume_scale = 0x38;
            break;

        case 0xC:
            volume_scale = 0x38;
            break;

        case 0x12:
            volume_scale = 0x2E;
            break;

        case 0x36:
            volume_scale = 0x38;
            break;

        case 0x6:
            volume_scale = 0x38;
            break;

        case 0x37:
            volume_scale = 0x38;
            break;

        case 0x3B:
            volume_scale = 0x3C;
            break;

        case 0x39:
            volume_scale = 0x38;
            break;

        case 0x34:
            volume_scale = 0x60;
            break;

        case 0x2F:
            volume_scale = 0x58;
            break;

        case 0x2E:
            volume_scale = 0x54;
            break;

        case 0x35:
            volume_scale = 0x54;
            break;

        case 0x14:
            volume_scale = 0x58;
            break;

        case 0x2:
            volume_scale = 0x34;
            break;

        case 0x3:
            volume_scale = 0x44;
            break;

        case 0x4:
            volume_scale = 0x38;
            break;

        case 0x5:
            volume_scale = 0x34;
            break;

        case 0x5A:
            volume_scale = 0x50;
            gStadiumModeActive = 0;
            file_index = 2;
            break;

        default:
            return 0;
    }

    if (file_index < gSfxArchiveHeader->num_files - 1) {
        file_size = gSfxArchiveHeader->files[file_index + 1] - gSfxArchiveHeader->files[file_index];
    } else if (file_index == gSfxArchiveHeader->num_files - 1) {
        file_size = gAudioRegions[2] - gSfxArchiveHeader->files[file_index];
    } else {
        return 0;
    }

    Rom_DmaRead(gSfxArchiveHeader->files[file_index], gMoveSoundTempBuffer, file_size);
    Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gSfxWaveTableBuffer, 0x98D8);

    if (gMusicFadeDuration <= 0) {
        gMusicHandle = Audio_PlayMusicSequence(gSfxWaveTable, gSfxWaveTableBuffer);
    } else {
        gMusicHandle = Audio_PlayMusicSequenceLooped(gSfxWaveTable, gSfxWaveTableBuffer, gMusicFadeDuration ^ 0);
    }

    gMusicFadeDuration = -1;
    Audio_QueueVoiceVolumeScale(gMusicHandle, volume_scale);

    if (voice_volume != 0x80) {
        Audio_QueueVoiceVolume(gMusicHandle, voice_volume);
    }

    if (track_id == 0x1D) {
        gLastMusicHandle = gMusicHandle;
    }

    return gMusicHandle;
}

void Audio_FadeOutAll(s32 arg0) {
    s32 i;

    if (arg0 == 0) {
        arg0 = 1;
    }

    Audio_QueueFadeSoundCommand(gMusicHandle, arg0);
    gMusicFadeDuration = -1;
    Audio_QueueFadeSoundCommand(gCurrentBgmHandle, arg0);
    gBgmFadeState = 0;
    gAudioStadiumSfxEnabled = 0;
    Audio_QueueFadeSoundCommand(gBattleMusicHandle, arg0);
    AudioStream_SetEnabled(0);
    AudioFrameUpdate(0);

    if (gCurrentAnimationId != 0x50) {
        for (i = 0; i < 2; i++) {
            AudioStream_Stop(i, arg0);
        }
    }
}

s32 Audio_StartTrackForCategory(s32 arg0, s32 arg1) {
    s32 ret;
    gStadiumModeActive = 0;
    if ((arg0 == 0) || (arg0 == 0xFF)) {
        arg0 = 1;
        if ((arg1 == 0xE)) {    
            return Audio_StartMusicTrack(2U);
        }
    }
    switch (arg1) {
    case 10:
    case 11:
        if (arg0 >= 9U) {
            return 0;
        }
        ret = Audio_StartMusicTrack(*(&D_80078E83 + arg0));
        break;
    case 6:
    case 7:
    case 8:
        if (arg0 >= 9U) {
            return 0;
        }
        ret = Audio_StartMusicTrack(*(&D_80078E8B + arg0));
        break;
    case 9:
        if (arg0 >= 9U) {
            return 0;
        }
        ret = Audio_StartMusicTrack(*(&D_80078E93 + arg0));
        break;
    case 5:
        if (arg0 >= 9U) {
            return 0;
        }
        ret = Audio_StartMusicTrack(*(&D_80078E9B + arg0));
        break;
    case 4:
        if (arg0 >= 9U) {
            return 0;
        }
        ret = Audio_StartMusicTrack(*(&D_80078EA3 + arg0));
        break;
    case 0:
    case 1:
    case 2:
        if (arg0 >= 9U) {
            return 0;
        }
        ret = Audio_StartMusicTrack(*(&D_80078EAB + arg0));
        break;
    case 3:
        if (arg0 >= 9U) {
            return 0;
        }
        ret = Audio_StartMusicTrack(*(&D_80078EB3 + arg0));
        break;
    case 12:
        if (arg0 >= 0x21U) {
            return 0;
        }
        ret = Audio_StartMusicTrack(*(&gBgmCategory12TrackIds + ((arg0 - 1) & 3)));
        break;
    case 13:
        if (arg0 >= 6U) {
            return 0;
        }
        ret = Audio_StartMusicTrack(*(&D_80078EBF + arg0));
        break;
    case 15:
        ret = Audio_StartMusicTrack(0x14U);
        break;
    default:
        ret = Audio_StartMusicTrack(2U);
    }
    return ret;
}

s32 Audio_UpdateSpecialSequence(s32 arg0, u32 arg1) {
    s32 var_a3;

    switch (arg0) {
        case 30:
            switch (gSpecialSequenceState) {
                case 0:
                    gSpecialSequenceState = 1;
                    return Audio_StartMusicTrack(0x1E);

                case 1:
                    return 0;

                case 2:
                    return 0;

                case 3:
                    if (Audio_IsSoundPlaying(gSequenceMusicHandle2) == 0) {
                        gSpecialSequenceState = 1;
                        if (arg1 == 0) {
                            return Audio_StartMusicTrack(0x1E);
                        }
                        return 0;
                    }
                    return 0;
            }
            return 0;

        case 31:
            gSpecialSequenceState = 2;
            switch (arg1) {
                case 0:
                    var_a3 = (arg1 * 0x48) + 0xD6;
                    break;

                case 1:
                    var_a3 = (arg1 * 0x48) + 0xD6;
                    break;

                case 2:
                    var_a3 = (arg1 * 0x48) + 0xD6;
                    break;

                case 3:
                    var_a3 = (arg1 * 0x48) + 0xD6;
                    break;

                case 4:
                    var_a3 = (arg1 * 0x48) + 0xD6;
                    break;

                default:
                    var_a3 = 0xD6;
                    break;
            }

            switch (arg1) {
                case 0:
                    Audio_DispatchSoundBankCommand(0x9000D, 0, 0);
                    break;

                case 1:
                    Audio_DispatchSoundBankCommand(0x9000E, 0, 0);
                    break;

                case 2:
                    Audio_DispatchSoundBankCommand(0x9000F, 0, 0);
                    break;

                case 3:
                    Audio_DispatchSoundBankCommand(0x90010, 0, 0);
                    break;

                case 4:
                    Audio_DispatchSoundBankCommand(0x90011, 0, 0);
                    break;
            }

            Audio_SetMusicLoopPoint(var_a3);
            gSequenceMusicHandle1 = Audio_StartMusicTrack(0x1F);
            return gSequenceMusicHandle1;

        case 32:
            switch (gSpecialSequenceState) {
                case 0:
                    return 0;

                case 1:
                    return 0;

                case 2:
                    if (Audio_IsSoundPlaying(gSequenceMusicHandle1) == 0) {
                        if (arg1 == 0) {
                            gSpecialSequenceState = 3;
                            gSequenceMusicHandle2 = 0;
                        } else {
                            Audio_SetMusicLoopPoint(0x46);
                            gSpecialSequenceState = 3;
                            gSequenceMusicHandle2 = Audio_StartMusicTrack(0x20);
                            return gSequenceMusicHandle2;
                        }
                    }
                    return 0;

                case 3:
                    return 0;
            }
            return 0;
    }
}

u32 Audio_GetSpecialSequenceState(void) {
    return gSpecialSequenceState;
}
