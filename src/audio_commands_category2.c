#include "audio_commands_category2.h"
#include "src/libnumus/player.h"
#include "src/audio_sfx.h"
#include "src/rom_device.h"
#include "src/audio_loop_point.h"
#include "src/audio_cache.h"
#include "src/audio_commands.h"
#include "src/audio_category_fade.h"

static s32 gPlayerVolumeDelta[4] = {
    0xFFFFFF9C,
    0xFFFFFFD8,
    0x00000028,
    0x00000064,
};
static s32 gPlayerPitchDelta[4] = {
    0xFFFFFFFD,
    0xFFFFFFFE,
    0xFFFFFFFE,
    0xFFFFFFFE,
};
static s32 gPlayerSoundHandles[8] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

s32 Audio_PlayCategory2SoundCommand(s32 arg0, s32 arg1, s32 arg2) {
    s32 sp34;
    UNUSED s32 pad[3];
    f32 var_fv0;
    s32 var_v0;

    var_fv0 = 0.0f;
    var_v0 = 0x80;

    if (arg0 == 0x2000A) {
        arg0 = 0x20001;
    }

    switch (arg0) {
        case 0x20003:
        case 0x20004:
        case 0x20005:
        case 0x20006:
        case 0x20007:
        case 0x20008:
            if ((arg1 <= 0) || (arg1 >= 5)) {
                arg1 = 1;
            }

            switch (arg2) {
                case 1:
                    break;

                case 2:
                    var_v0 = (arg1 * 0xAF) - 0x87;
                    break;

                case 3:
                    var_v0 = (arg1 * 0x66) - 0x4D;
                    break;

                case 4:
                    if (1) {}
                    var_v0 = (arg1 * 0x4E) - 0x44;
                    break;
            }

            if ((arg0 == 0x20004) || (arg0 == 0x20005) || (arg0 == 0x20006) || (arg0 == 0x20007)) {
                var_fv0 = (4 - arg1) * 0.5f;
            }
            break;
    }

    sp34 = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, arg0 & 0xFFFF, 0x80, var_v0, -1);
    Audio_QueueVoiceDetune(sp34, var_fv0);

    return sp34;
}

void Audio_StopCategory2SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2) {
}

s32 Audio_PlayCategory3SoundCommand(s32 arg0, s32 arg1, s32 arg2) {
    s32 sp44;
    UNUSED s32 pad[2];
    u32 sp38;
    f32 var_fv0;
    s32 var_v0;

    var_v0 = 0x7F;
    sp38 = 0x80;
    var_fv0 = 0.0f;

    switch (arg0) {
        case 0x30003:
            sp38 = 0x50;
            if (arg2 >= 0) {
                if (Audio_IsSoundPlaying(gPlayerSoundHandles[arg1]) == 0) {
                    var_fv0 += gPlayerPitchDelta[arg1];
                    var_v0 += gPlayerVolumeDelta[arg1];
                    break;
                }

                if ((arg2 >= 0) && (arg2 < 0x14)) {
                    var_fv0 += gPlayerPitchDelta[arg1];
                    var_fv0 += 0.5f * arg2;

                    Audio_QueueVoiceDetune(gPlayerSoundHandles[arg1], var_fv0);
                    return 0;
                }

                if (arg2 >= 0x14) {
                    var_fv0 += gPlayerPitchDelta[arg1];
                    var_fv0 += (0, 0.5f) * 22.5;

                    Audio_QueueVoiceDetune(gPlayerSoundHandles[arg1], var_fv0);
                    return 0;
                }
            } else if (Audio_IsSoundPlaying(gPlayerSoundHandles[arg1]) == 0) {
                return 0;
            } else {
                Audio_QueueFadeSoundCommand(gPlayerSoundHandles[arg1], 0xA);
                return 0;
            }

        case 0x30004:
            Audio_QueueFadeSoundCommand(gPlayerSoundHandles[0], 0x14);
            Audio_QueueFadeSoundCommand(gPlayerSoundHandles[1], 0x14);
            Audio_QueueFadeSoundCommand(gPlayerSoundHandles[2], 0x14);
            Audio_QueueFadeSoundCommand(gPlayerSoundHandles[3], 0x14);
            var_v0 += gPlayerVolumeDelta[arg1];
            break;

        case 0x30001:
        case 0x30002:
            break;

        default:
            return 0;
    }

    sp44 = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, arg0 & 0xFFFF, sp38, var_v0, -1);
    Audio_QueueVoiceDetune(sp44, var_fv0);
    if (arg0 == 0x30003) {
        gPlayerSoundHandles[arg1] = sp44;
    }
    return sp44;
}

void Audio_StopCategory3SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2) {
}

s32 Audio_ComputeIndexedVolume(u32 arg0, u32 arg1) {
    s32 ret;

    if ((arg0 == 0) || (arg0 >= 5)) {
        arg0 = 1;
    }

    switch (arg1) {
        case 1:
            ret = 0x80;
            break;

        case 2:
            ret = (arg0 * 0xAF) - 0x87;
            break;

        case 3:
            ret = (arg0 * 0x66) - 0x4D;
            break;

        case 4:
            ret = (arg0 * 0x4E) - 0x44;
            break;

        default:
            ret = 0x80;
            break;
    }

    return ret;
}

s32 Audio_PlayCategory4SoundCommand(s32 arg0, s32 arg1, s32 arg2) {
    s32 sp3C;
    UNUSED s32 pad;
    s32 var_v1 = 0x80;
    u32 var_a3 = 0x80;
    f32 var_fv0 = 0.0f;

    switch (arg0) {
        case 0x00040004:
            if ((arg1 <= 0) || (arg1 >= 5)) {
                var_a3 = 0x80;
                arg1 = 1;
            }
            Audio_QueueFadeSoundCommand(gPlayerSoundHandles[arg1 + 3], 0x14);
            var_v1 = Audio_ComputeIndexedVolume(arg1, arg2);
            var_fv0 = (4 - arg1) * 0.5f;
            break;

        case 0x00040005:
        case 0x00040006:
        case 0x00040007:
            if ((arg1 <= 0) || (arg1 >= 5)) {
                arg1 = 1;
            }
            var_v1 = Audio_ComputeIndexedVolume(arg1, arg2);
            var_a3 = 0x80;
            var_fv0 = (4 - arg1) * 0.5f;
            break;

        case 0x00040001:
            var_a3 = 0xC0;
            break;

        case 0x00040003:
            if ((arg1 <= 0) || (arg1 >= 5)) {
                arg1 = 1;
            }
            var_v1 = Audio_ComputeIndexedVolume(arg1, arg2);
            var_a3 = 0xC0;
            var_fv0 = 0.0f;
            break;

        case 0x00040002:
            break;

        default:
            return 0;
    }

    sp3C = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, arg0 & 0xFFFF, var_a3, var_v1, -1);
    Audio_QueueVoiceDetune(sp3C, var_fv0);

    if (arg0 == 0x40004) {
        gPlayerSoundHandles[arg1 + 3] = sp3C;
    }

    return sp3C;
}

void Audio_StopCategory4SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2) {
}

s32 Audio_PlayCategory5SoundCommand(s32 arg0, s32 arg1, s32 arg2) {
    static s32 gAmbientSoundHandle = 0x00000000;

    s32 sp34;
    UNUSED s32 pad;
    u32 var_v0 = 0x80;
    s32 var_a3 = 0x80;
    f32 var_fv0 = 0.0f;

    switch (arg0) {
        case 0x50001:
        case 0x50002:
        case 0x50003:
        case 0x50004:
        case 0x50005:
        case 0x50006:
        case 0x50007:
            break;

        case 0x50008:
        case 0x50009:
        case 0x5000A:
            if ((arg1 <= 0) || (arg1 >= 5)) {
                arg1 = 1;
            }
            var_fv0 = (4 - arg1) * 0.5f;
            if (arg2 >= 0x141) {
                arg2 = 0x140;
            }
            var_v0 = arg2 * 0.796875f;
            break;

        case 0x5000B:
            Audio_QueueFadeSoundCommand(gAmbientSoundHandle, 1);
            var_v0 = 0x80;
            var_a3 = 0x80;
            var_fv0 = 0.0f;
            break;

        case 0x5000C:
            var_a3 = 0xA0;
            break;

        default:
            return 0;
    }

    sp34 = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, arg0 & 0xFFFF, var_a3, var_v0, -1);
    Audio_QueueVoiceDetune(sp34, var_fv0);
    gAmbientSoundHandle = sp34;
    return sp34;
}

void Audio_StopCategory5SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2) {
}

s32 Audio_PlayCategory6SoundCommand(s32 arg0, s32 arg1, UNUSED s32 arg2) {
    static s32 gMenuVolumeDelta[4] = {
        0xFFFFFF88,
        0xFFFFFFC4,
        0x0000003C,
        0x00000078,
    };
    static f32 gMenuPitchDelta[4] = {
        -1,
        -0.5,
        0,
        0.5,
    };
    static s32 gMenuLoopHandle = 0x00000000;
    static s32 gMenuPrevHandle = 0x00000000;

    s32 sp34;
    UNUSED s32 pad;
    u32 var_v1;
    s32 sp28;
    UNUSED s32 pad2;
    f32 sp20;

    sp20 = 0.0f;
    var_v1 = 0x7F;
    sp28 = 0x80;

    if ((arg1 >= 4) | (arg1 < 0)) {
        return 0;
    }

    switch (arg0) {
        case 0x60003:
            sp20 += gMenuPitchDelta[arg1];
            var_v1 = gMenuVolumeDelta[arg1] + 0x7F;
        case 0x60001:
        case 0x60002:
        case 0x60006:
        case 0x60008:
        case 0x60009:
        case 0x6000A:
        case 0x6000B:
            break;

        case 0x60004:
            var_v1 = gMenuVolumeDelta[arg1] + 0x7F;
            sp28 = 0x70;
            break;

        case 0x60005:
            if (Audio_IsSoundPlaying(gMenuPrevHandle) != 0) {
                return 0;
            }
            var_v1 = 0x7F;
            if (Audio_IsSoundPlaying(gMenuLoopHandle) != 0) {
                return 0;
            }
            break;

        case 0x60007:
            var_v1 = 0x7F;
            if (Audio_IsSoundPlaying(gMenuPrevHandle) != 0) {
                arg0 = 0x60008;
            }
            break;

        default:
            return 0;
    }

    sp34 = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, arg0 & 0xFFFF, sp28, var_v1, -1);
    if (arg0 == 0x60005) {
        if (Audio_IsSoundPlaying(gMenuLoopHandle) == 0) {
            gMenuLoopHandle = sp34;
        }
    }
    gMenuPrevHandle = sp34;
    Audio_QueueVoiceDetune(sp34, sp20);
    return sp34;
}

void Audio_StopCategory6SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2) {
}

static u8 gBattleCryCycle = 0;

s32 Audio_PlayCategory7SoundCommand(s32 arg0, s32 arg1, UNUSED s32 arg2) {
    s32 sp44;
    s32 var_a2;
    s32 var_a3;
    s32 var_v0;
    f32 var_fv0;
    u8 var_t0;
    u8 sp2C[] = {
        0x19,
        0x4E,
        0xB2,
        0xE6,
    };

    var_fv0 = 0.0f;
    var_t0 = 0x80;
    var_a3 = 0x80;
    var_v0 = -1;
    var_a2 = arg0 & 0xFFFF;

    switch (arg0) {
        case 0x70004:
        case 0x70005:
        case 0x70006:
            if ((arg1 <= 0) || (arg1 >= 5)) {
                arg1 = 1;
            }
            var_t0 = sp2C[arg1 - 1];
            var_fv0 = ((4 - arg1) * 0.5f) + -2.0f;
            break;

        case 0x70009:
            break;

        case 0x70003:
            if (arg1 != 0) {
                var_t0 = 0x1E;
            } else {
                var_t0 = 0xE1;
            }
            break;

        case 0x70001:
            switch (gBattleCryCycle) {
                case 0:
                    var_a2 = 1;
                    break;

                case 1:
                    var_a2 = 7;
                    break;

                case 2:
                    var_a2 = 8;
                    break;
            }
            var_a3 = 0x70;

            gBattleCryCycle++;
            if (gBattleCryCycle >= 3) {
                gBattleCryCycle = 0;
            }
            var_v0 = 0x80;
            break;

        case 0x70002:
            gBattleCryCycle = 0;
            var_a3 = 0x70;
            var_v0 = 0x80;
            break;

        default:
            return 0;
    }

    sp44 = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, var_a2, var_a3, var_t0, var_v0);
    Audio_QueueVoiceDetune(sp44, var_fv0);
    return sp44;
}

void Audio_StopCategory7SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2) {
}

static s32 gCryVolumeDelta[4] = {
    0xFFFFFF88,
    0xFFFFFFC4,
    0x0000003C,
    0x00000078,
};

static f32 gCryPitchDelta[4] = {
    0.0f,
    0.1000000015f,
    -0.1000000015f,
    0.200000003f,
};

static s32 gCryHandles[4] = {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};

static s32 gCrySpecialHandle1 = 0x00000000;

static s32 gCrySpecialHandle2 = 0x00000000;

static s32 gCrySpecialHandles[4] = {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};

static s32 gRngSeedA = 0x00003039;
static u32 gRngSeedB = 0x00001A85;

void Audio_SeedRandom(s32 arg0, u32 arg1) {
    gRngSeedA = arg0;
    gRngSeedB = arg1;
}

u32 Audio_NextRandom(void) {
    gRngSeedA = (gRngSeedA * 0x41C64E6D) + 0x3039;

    if (gRngSeedB & 1) {
        gRngSeedB ^= 0x11020;
    }

    gRngSeedB >>= 1;

    return gRngSeedA ^ gRngSeedB;
}

f32 Audio_RandomFloat(void) {
    return (f32)Audio_NextRandom() / -1u;
}

s32 Audio_PlayCategory8SoundCommand(s32 arg0, s32 arg1, s32 arg2) {
    static u32 gCryModCounter = 0;

    s32 sp44;
    UNUSED s32 pad;
    u32 sp3C;
    s32 sp38;
    UNUSED s32 pad2;
    f32 var_fv1;

    sp3C = 0x7F;
    sp38 = 0x80;
    var_fv1 = 0.0f;

    if ((arg1 >= 4) || (arg1 < 0)) {
        return 0;
    }

    if ((arg2 < 0) || (arg2 >= 6)) {
        return 0;
    }

    switch (arg0) {
        case 0x80001:
            sp3C += gCryVolumeDelta[arg1];
            sp38 = 0x40;
            var_fv1 += gCryPitchDelta[arg1];
            switch (arg2) {
                case 0:
                    break;

                case 1:
                    return 0;

                case 2:
                    var_fv1 += 0.5;
                    Audio_QueueVoiceDetune(gCryHandles[arg1], var_fv1);
                    return 0;

                case 3:
                    var_fv1 += 1.0;
                    Audio_QueueVoiceDetune(gCryHandles[arg1], var_fv1);
                    return 0;

                case 4:
                    var_fv1 += 2.0;
                    Audio_QueueVoiceDetune(gCryHandles[arg1], var_fv1);
                    Audio_QueueVoiceVolumeScale(gCryHandles[arg1], 0x58);
                    return 0;

                case 5:
                    Audio_QueueFadeSoundCommand(gCryHandles[arg1], 3);

                default:
                    return 0;
            }
            break;

        case 0x80002:
            if (Audio_IsSoundPlaying(gCrySpecialHandles[arg1]) != 0) {
                return 0;
            }

            sp3C += gCryVolumeDelta[arg1];
            var_fv1 += gCryPitchDelta[arg1];

            switch (arg2) {
                case 1:
                    return 0;

                case 2:
                    if (Audio_RandomFloat() > 0.05) {
                        return 0;
                    }
                    break;

                case 3:
                    if (Audio_RandomFloat() > 0.1) {
                        return 0;
                    }
                    break;

                case 4:
                    if (Audio_RandomFloat() > 0.17) {
                        return 0;
                    }
                    break;

                default:
                    return 0;
            }
            break;

        case 0x80003:
            if (Audio_IsSoundPlaying(gCrySpecialHandle1) != 0) {
                return 0;
            }
            Audio_FadeCategories(1, 1);
            break;

        case 0x80004:
            Audio_FadeCategories(1, 1);
            break;

        case 0x80005:
            var_fv1 += (f32)gCryModCounter * 2.0;
            if (gCryModCounter < 2) {
                gCryModCounter += 1;
            } else if (gCryModCounter >= 2) {
                gCryModCounter = 0;
            }
            break;

        case 0x80006:
            Audio_FadeCategories(1, 2);
            break;

        case 0x80007:
        case 0x80008:
            break;

        default:
            return 0;
    }

    sp44 = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, arg0 & 0xFFFF, sp38, sp3C, -1);

    switch (arg0) {
        case 0x80001:
            gCryHandles[arg1] = sp44;
            break;

        case 0x80002:
            gCrySpecialHandles[arg1] = sp44;
            break;

        case 0x80003:
            gCrySpecialHandle1 = sp44;

        case 0x80004:
            gCrySpecialHandle2 = sp44;
            break;
    }

    Audio_QueueVoiceDetune(sp44, var_fv1);
    return sp44;
}

void Audio_StopCategory8SoundCommand(s32 arg0, s32 arg1, UNUSED s32 arg2) {
    if (arg0 == 0x80001) {
        Audio_QueueFadeSoundCommand(gCryHandles[arg1], 0xA);
    }
}

s32 gBattleMusicHandle = 0;

s32 Audio_PlayCategory9SoundCommand(s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2) {
    static s32 gBattleMusicSequenceState = 0;

    s32 sp3C;
    s32 sp38;
    UNUSED s32 pad;
    s32 sp30;
    f32 var_fv1;
    UNUSED s32 pad2;

    sp30 = 0x80;
    var_fv1 = 0.0f;
    sp38 = arg0 & 0xFFFF;

    switch (arg0) {
        case 0x90001:
            if (gBattleMusicSequenceState == 0) {
                gSpecialSequenceState = 0;
                gSequenceMusicHandle1 = 0;
                gSequenceMusicHandle2 = 0;
                sp38 = 2;
                gBattleMusicSequenceState++;
                Audio_UpdateSpecialSequence(0x1E, 0);
            } else if (gBattleMusicSequenceState == 1) {
                gBattleMusicSequenceState++;
                var_fv1 -= 4.0f;
            } else if (gBattleMusicSequenceState == 2) {
                gBattleMusicSequenceState++;
                var_fv1 -= 2.0f;
            } else if (gBattleMusicSequenceState == 3) {
                gBattleMusicSequenceState = 0;
                var_fv1 -= 9.0f;
            }
            break;

        case 0x90002:
        case 0x90003:
        case 0x90004:
        case 0x90005:
        case 0x90006:
        case 0x90007:
        case 0x90008:
        case 0x90009:
            break;

        case 0x9000A:
            Audio_StartMusicTrack(0x1A);
            break;

        case 0x9000B:
            Audio_StartMusicTrack(0x1B);
            break;

        case 0x9000C:
            return 0;

        case 0x9000D:
        case 0x9000E:
        case 0x9000F:
        case 0x90010:
        case 0x90011:
            sp38 = sp38 - 1;
            sp30 = 0x60;
            break;

        default:
            return 0;
    }

    sp3C = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, sp38, sp30, 0x7F, -1);
    Audio_QueueVoiceDetune(sp3C, var_fv1);

    switch (arg0) {
        case 0x9000D:
        case 0x9000E:
        case 0x9000F:
        case 0x90010:
        case 0x90011:
            gBattleMusicHandle = sp3C;
            break;
    }

    return sp3C;
}

void Audio_StopCategory9SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2) {
}

s32 Audio_PlayCategoryASoundCommand(s32 arg0, s32 arg1, s32 arg2) {
    static s32 gCatASoundVolumeDelta[4] = {
        0xFFFFFF88,
        0xFFFFFFD8,
        0x00000028,
        0x00000078,
    };
    static s32 gCatASoundVolMod[3] = {
        0xFFFFFF8D,
        0x00000000,
        0x00000073,
    };
    static f32 gCatASoundVolScale[3] = {
        1.0f,
        0.8000000119f,
        0.6000000238f,
    };
    static f32 gCatASoundPitchDelta[4] = {
        -0.6000000238f,
        1.0f,
        -1.0f,
        0.6000000238f,
    };
    static f32 gCatASoundPitchOffsetByVariant[4] = {
        0.0f,
        0.0f,
        2.0f,
        2.0f,
    };
    static f32 gCatASoundPriorityOffsetByVariant[4] = {
        0.0f,
        4.0f,
        8.0f,
        12.0f,
    };

    s32 sp34;
    s32 sp30;
    u32 var_t0;
    u32 var_a3;
    f32 var_fv0;
    UNUSED s32 pad;

    var_t0 = 0x7F;
    var_a3 = 0x80;
    sp30 = arg0 & 0xFFFF;
    var_fv0 = 0.0f;

    switch (arg0) {
        case 0xA0003:
            if (arg1 < 0) {
                return 0;
            }

            if (arg1 >= 4) {
                return 0;
            }

            switch (arg2) {
                case 0:
                    break;

                case 1:
                    sp30 = 4;
                    break;

                case 2:
                    sp30 = 5;
                    break;

                case 3:
                    sp30 = 6;
                    break;
            }

            var_fv0 += gCatASoundPitchDelta[arg1];
            var_fv0 += gCatASoundPitchOffsetByVariant[arg2];
            var_t0 = gCatASoundVolumeDelta[arg1] + 0x7F;
            var_a3 = 240.0f;
            var_a3 += gCatASoundPriorityOffsetByVariant[arg2];

            if (var_t0 >= 0x100) {
                var_t0 = 0xFF;
            }

            if (var_t0 == 0) {
                var_t0 = 1;
            }
            break;

        case 0xA0001:
        case 0xA0002:
        case 0xA0009:
        case 0xA000C:
        case 0xA000D:
        case 0xA0010:
        case 0xA0012:
        case 0xA0013:
            break;

        case 0xA0007:
            if (arg1 < 0) {
                return 0;
            }

            if (arg1 >= 4) {
                return 0;
            }

            if (arg2 < -0x7D) {
                arg2 = -0x7D;
            } else if (arg2 >= 0x7E) {
                arg2 = 0x7D;
            }

            var_t0 = arg2 + 0x7F;
            var_fv0 += gCatASoundPitchDelta[arg1];
            break;

        case 0xA0008:
            if (arg1 == 1) {
                sp30 = 9;
            } else if (arg1 != 0) {
                return 0;
            }

            if ((arg2 < 0) || (arg2 >= 0xB) || (arg2 == 3) || (arg2 == 7)) {
                return 0;
            }

            var_t0 = (s32)(gCatASoundVolScale[(arg2 & 0xC) >> 2] * (f32)gCatASoundVolMod[arg2 & 3]) + 0x7F;
            break;

        case 0xA000A:
            var_t0 = (s32)((0.8 * (f32)gCatASoundVolMod[arg2 & 3]) * gCatASoundVolScale[(arg2 & 0xC) >> 2]) + (0, 0x7F);
            break;

        case 0xA000B:
            if (arg1 == 1) {
                sp30 = 0xC;
            } else if (arg1 != 0) {
                return 0;
            }
            var_t0 = (s32)(0.8 * (f32)gCatASoundVolMod[arg2 & 3] * gCatASoundVolScale[(arg2 & 0xC) >> 2]) + (0, 0x7F);
            break;

        case 0xA000E:
            Audio_FadeCategories(1, 5);
            var_a3 = 0x80;
            var_t0 = 0x7F;
            var_fv0 = 0.0f;
            break;

        case 0xA000F:
            return 0;

        case 0xA0011:
            if (arg1 == 1) {
                sp30 = 0x10;
            } else if (arg1 != 0) {
                return 0;
            }
            var_t0 = (s32)(0.8 * (f32)gCatASoundVolMod[arg2 & 3] * gCatASoundVolScale[(arg2 & 0xC) >> 2]) + (0, 0x7F);
            break;

        default:
            return 0;
    }

    if (var_t0 == 0) {
        var_t0 = 1;
    } else if (var_t0 >= 0x100) {
        var_t0 = 0xFF;
    }

    sp34 = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, sp30, var_a3, var_t0, -1);
    Audio_QueueVoiceDetune(sp34, var_fv0);
    return sp34;
}

void Audio_StopCategoryASoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2) {
}

s32 Audio_PlayPersistentSoundCommand(s32 arg0, u32 arg1) {
    static s32 gPersistentSfxHandle1 = 0x00000000;
    static s32 gPersistentSfxHandle2 = 0x00000000;
    static s32 gPersistentSfxHandle3 = 0x00000000;

    s32 temp_v0;

    switch (arg0) {
        case 19:
            Audio_QueueFadeSoundCommand(gPersistentSfxHandle1, 1);
            break;

        case 18:
        case 20:
            break;

        case 21:
            switch (arg1) {
                case 0:
                    if (Audio_IsSoundPlaying(gPersistentSfxHandle3) != 0) {
                        return 0;
                    }
                    break;

                case 1:
                    Audio_QueueFadeSoundCommand(gPersistentSfxHandle2, 0x19);
                    break;
            }
            break;

        default:
            return 0;
    }

    if (arg0 == 0x15) {
        Audio_QueueFadeSoundCommand(gPersistentSfxHandle3, 0xA);
    }

    temp_v0 = Audio_PlaySoundEffect(gSfxBankStandard, gSfxTable, arg0, 0x80, 0x80, -1);
    switch (arg0) {
        case 18:
            gPersistentSfxHandle1 = temp_v0;
            break;

        case 20:
            gPersistentSfxHandle2 = temp_v0;
            break;

        case 21:
            gPersistentSfxHandle3 = temp_v0;
            break;
    }

    return temp_v0;
}

u32 gCurrentSfxBankId = 0xFFFFFFFF;
static s32 gLastPlayedSfxHandle = 0x00000000;

s32 Audio_DispatchSoundBankCommand(s32 arg0, s32 arg1, s32 arg2) {
    UNUSED s32 pad[5];
    s32 var_v0;
    u32 sp24;
    u32 temp_v1;

    temp_v1 = arg0 & 0xFFFF0000;

    switch (temp_v1) {
        case 0x20000:
        case 0x30000:
        case 0x40000:
        case 0x50000:
        case 0x60000:
        case 0x70000:
        case 0x80000:
        case 0x90000:
        case 0xA0000:
            break;

        default:
            return 0;
    }

    if (temp_v1 != gCurrentSfxBankId) {
        sp24 = gAudioSeqHeaderNested->seqArray[(temp_v1 >> 0x10) - 2].offset;

        Rom_DmaRead(sp24, (u32)gAudioHeaderTemp, 0xC);
        Audio_RelocateOffsets((u32)gAudioHeaderTemp, sp24, 3);
        Rom_DmaRead(gAudioHeaderTemp[0], gMoveSoundTempBuffer, gAudioHeaderTemp[1] - gAudioHeaderTemp[0]);
        Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gAudioSfxInstBank, 0xBB8);
        Audio_RelocateSoundBankTable(gAudioSfxInstBank);
        Rom_DmaRead(gAudioHeaderTemp[1], gMoveSoundTempBuffer, gAudioHeaderTemp[2] - gAudioHeaderTemp[1]);
        Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gAudioSfxBank, 0x1388);
        Audio_RelocateSoundBankWrapper(gAudioSfxBank, gAudioHeaderTemp[2]);
    }

    gCurrentSfxBankId = temp_v1;

    switch (temp_v1) {
        case 0x20000:
            var_v0 = Audio_PlayCategory2SoundCommand(arg0, arg1, arg2);
            break;

        case 0x30000:
            var_v0 = Audio_PlayCategory3SoundCommand(arg0, arg1, arg2);
            break;

        case 0x40000:
            var_v0 = Audio_PlayCategory4SoundCommand(arg0, arg1, arg2);
            break;

        case 0x50000:
            var_v0 = Audio_PlayCategory5SoundCommand(arg0, arg1, arg2);
            break;

        case 0x60000:
            var_v0 = Audio_PlayCategory6SoundCommand(arg0, arg1, arg2);
            break;

        case 0x70000:
            var_v0 = Audio_PlayCategory7SoundCommand(arg0, arg1, arg2);
            break;

        case 0x80000:
            var_v0 = Audio_PlayCategory8SoundCommand(arg0, arg1, arg2);
            break;

        case 0x90000:
            var_v0 = Audio_PlayCategory9SoundCommand(arg0, arg1, arg2);
            break;

        case 0xA0000:
            var_v0 = Audio_PlayCategoryASoundCommand(arg0, arg1, arg2);
            break;

        default:
            return 0;
    }

    gLastPlayedSfxHandle = var_v0;
}

void Audio_DispatchSoundBankStopCommand(s32 arg0, s32 arg1, s32 arg2) {
    switch (arg0 & 0xFFFF0000) {
        case 0x20000:
            Audio_StopCategory2SoundCommand(arg0, arg1, arg2);
            break;

        case 0x30000:
            Audio_StopCategory3SoundCommand(arg0, arg1, arg2);
            break;

        case 0x40000:
            Audio_StopCategory4SoundCommand(arg0, arg1, arg2);
            break;

        case 0x50000:
            Audio_StopCategory5SoundCommand(arg0, arg1, arg2);
            break;

        case 0x60000:
            Audio_StopCategory6SoundCommand(arg0, arg1, arg2);
            break;

        case 0x70000:
            Audio_StopCategory7SoundCommand(arg0, arg1, arg2);
            break;

        case 0x80000:
            Audio_StopCategory8SoundCommand(arg0, arg1, arg2);
            break;

        case 0x90000:
            Audio_StopCategory9SoundCommand(arg0, arg1, arg2);
            break;

        case 0xA0000:
            Audio_StopCategoryASoundCommand(arg0, arg1, arg2);
            break;
    }
}

s32 Audio_PlayDirectSoundCommand(s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2) {
    UNUSED s32 pad[4];
    s32 var_v0;
    s32 sp2C;
    u32 sp34;
    u32 sp28;

    sp28 = arg0 & 0xFFFF0000;
    sp2C = arg0 & 0xFFFF;

    if (sp28 != gCurrentSfxBankId) {
        sp34 = gAudioSfxBankSeqHeader->seqArray[2].offset;

        Rom_DmaRead(sp34, &gAudioHeaderTemp[0], 0xC);
        Audio_RelocateOffsets(gAudioHeaderTemp, sp34, 3);
        sp34 = gAudioHeaderTemp[1] - gAudioHeaderTemp[0];
        Rom_DmaRead(gAudioHeaderTemp[0], gMoveSoundTempBuffer, sp34);
        Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gAudioSfxInstBank, 0xBB8);
        Audio_RelocateSoundBankTable(gAudioSfxInstBank);
        sp34 = gAudioHeaderTemp[2] - gAudioHeaderTemp[1];
        Rom_DmaRead(gAudioHeaderTemp[1], gMoveSoundTempBuffer, sp34);
        Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gAudioSfxBank, 0x1388);
        Audio_RelocateSoundBankWrapper(gAudioSfxBank, gAudioHeaderTemp[2]);
    }

    gCurrentSfxBankId = sp28;

    switch (arg0) {
        case 0x1000001:
            var_v0 = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, sp2C, 0x80, 0x80, -1);
            break;

        case 0x1000002:
            var_v0 = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, sp2C, 0xA0, 0x80, -1);
            break;

        case 0x1000003:
            var_v0 = Audio_PlaySoundEffect(gSfxWaveTable, gAudioSfxInstBank, sp2C, 0xA0, 0x80, -1);
            break;

        default:
            return 0;
    }

    gLastPlayedSfxHandle = var_v0;
}

s32 Audio_PlayCategory11SoundCommand(s32 arg0, s32 arg1, s32 arg2) {
    static s32 gCat11SfxHandle1 = 0x00000000;
    static s32 gCat11SfxHandle2 = 0x00000000;
    static s32 gCat11SfxHandle3 = 0x00000000;
    static s32 gCat11SfxHandle4 = 0;
    static u8 gSfxPitchTableSingleFaint[] = {
        0x60, 0xA0, 0xE0, 0x20, 0x60, 0xA0,
    };
    static u8 gSfxPitchTableDoubleFaint[] = {
        0x60, 0xA0, 0xE0, 0x60, 0xA0, 0xE0, 0x20, 0x60, 0xA0, 0x20, 0x60, 0xA0,
    };
    static u8 gSfxPitchTableBadge[] = {
        0x20, 0x60, 0xA0, 0xE0, 0x20, 0x60, 0xA0, 0xE0, 0x20, 0x60, 0xA0, 0xE0,
    };

    UNUSED s32 pad;
    s32 sp40;
    s32 temp_v0_2;
    u32 sp38;
    s32 sp34;
    UNUSED s32 pad2;
    u32 sp2C;
    u32 sp20;

    sp20 = arg0 & 0xFFFF0000;
    sp38 = 0x80;
    sp34 = 0x80;

    if (sp20 != gCurrentSfxBankId) {
        sp2C = gAudioSfxBankSeqHeader->seqArray[3].offset;

        Rom_DmaRead(sp2C, (u32)gAudioHeaderTemp, 0xC);
        Audio_RelocateOffsets((u32)gAudioHeaderTemp, sp2C, 3);
        Rom_DmaRead(gAudioHeaderTemp[0], gMoveSoundTempBuffer, gAudioHeaderTemp[1] - gAudioHeaderTemp[0]);
        Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gAudioSfxInstBank, 0xBB8);
        Audio_RelocateSoundBankTable(gAudioSfxInstBank);
        Rom_DmaRead(gAudioHeaderTemp[1], gMoveSoundTempBuffer, gAudioHeaderTemp[2] - gAudioHeaderTemp[1]);
        Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gAudioSfxBank, 0x1388);
        Audio_RelocateSoundBankWrapper(gAudioSfxBank, gAudioHeaderTemp[2]);
    }

    gCurrentSfxBankId = sp20;

    sp40 = arg0 & 0xFFFF;
    switch (arg0) {
        case 0x1100007:
            return 0;

        case 0x1100015:
        case 0x1100016:
        case 0x1100017:
            sp40--;
            break;

        case 0x1100001:
        case 0x1100002:
        case 0x1100003:
        case 0x1100005:
        case 0x1100006:
        case 0x1100009:
        case 0x110000A:
        case 0x110000C:
        case 0x110000D:
        case 0x110000F:
        case 0x1100010:
        case 0x1100011:
            break;

        case 0x1100013:
            sp34 = 0xF0;
            break;

        case 0x1100012:
            Audio_PlaySoundEffectById(0x1B);
            break;

        case 0x1100004:
            Audio_QueueFadeSoundCommand(gCat11SfxHandle1, 5);
            break;

        case 0x1100008:
            Audio_QueueFadeSoundCommand(gCat11SfxHandle2, 5);
            Audio_QueueFadeSoundCommand(gCat11SfxHandle3, 5);
            break;

        case 0x110000B:
            if (arg2 == 0) {
                if ((arg1 <= 0) || (arg1 >= 7)) {
                    return 0;
                }
                sp38 = gSfxPitchTableSingleFaint[arg1 - 1];
            } else {
                if ((arg1 <= 0) || (arg1 >= 0xD)) {
                    return 0;
                }
                sp38 = gSfxPitchTableDoubleFaint[arg1 - 1];
            }
            Audio_QueueFadeSoundCommand(gCat11SfxHandle4, 0xA);
            break;

        case 0x1100014:
            sp40 = 0xB;
            if (arg2 == 1) {
                sp38 = 0x7F;
            } else {
                sp38 = gSfxPitchTableBadge[(arg1 - 1) % 4];
            }
            break;

        case 0x110000E:
            if ((arg1 < 0) || (arg1 >= 9)) {
                return 0;
            }
            sp38 = gSfxPitchTableBadge[3 + arg1];
            break;

        default:
            return 0;
    }

    temp_v0_2 = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, sp40, sp34, sp38, -1);
    switch (arg0) {
        case 0x1100003:
            gCat11SfxHandle1 = temp_v0_2;
            break;

        case 0x1100006:
            gCat11SfxHandle2 = temp_v0_2;
            break;

        case 0x1100007:
            gCat11SfxHandle3 = temp_v0_2;
            break;

        case 0x110000B:
            gCat11SfxHandle4 = temp_v0_2;
            break;
    }
    gLastPlayedSfxHandle = temp_v0_2;
}

s32 Audio_PlayCategory12SoundCommand(s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2) {
    static s32 gCat12SfxHandle1 = 0x00000000;
    static s32 gCat12SfxHandle2 = 0x00000000;
    static s32 gCat12SfxHandle3 = 0x00000000;
    static s32 gCat12SfxHandle4 = 0x00000000;
    static s32 gCat12SfxHandle5 = 0x00000000;
    static u8 gCat12SfxFirstPlayFlag = 0;
    static u8 gCat12MusicStoppedFlag = 0;

    UNUSED s32 pad[2];
    s32 sp4C;
    UNUSED s32 pad2[2];
    f32 var_fv1;
    u32 sp3C;
    u32 sp24;

    sp24 = arg0 & 0xFFFF0000;
    if (sp24 != gCurrentSfxBankId) {
        sp3C = gAudioSfxBankSeqHeader->seqArray[4].offset;

        Rom_DmaRead(sp3C, (u32)gAudioHeaderTemp, 0xC);
        Audio_RelocateOffsets((u32)gAudioHeaderTemp, sp3C, 3);
        Rom_DmaRead(gAudioHeaderTemp[0], gMoveSoundTempBuffer, gAudioHeaderTemp[1] - gAudioHeaderTemp[0]);
        Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gAudioSfxInstBank, 0xBB8);
        Audio_RelocateSoundBankTable(gAudioSfxInstBank);
        Rom_DmaRead(gAudioHeaderTemp[1], gMoveSoundTempBuffer, gAudioHeaderTemp[2] - gAudioHeaderTemp[1]);
        Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gAudioSfxBank, 0x1388);
        Audio_RelocateSoundBankWrapper(gAudioSfxBank, gAudioHeaderTemp[2]);
    }

    gCurrentSfxBankId = sp24;

    switch (arg0) {
        case SFX_TRADE_TEXT:
            Audio_QueueFadeSoundCommand(gCat12SfxHandle1, 1);
            var_fv1 = __ull_to_f(__ull_rem(osGetTime(), 8)) - 4.0f;
            sp4C = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 7, 0x80, 0x80, -1);
            Audio_QueueVoiceDetune(sp4C, var_fv1);
            gCat12SfxHandle1 = sp4C;
            break;

        case SFX_TRADE_BEGIN:
            sp4C = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 1, 0x80, 0x80, -1);
            gCat12SfxHandle2 = sp4C;
            gCat12MusicStoppedFlag = 0;
            break;

        case SFX_TRADE_END:
            Audio_QueueFadeSoundCommand(gCat12SfxHandle2, 0x3C);
            Audio_QueueFadeSoundCommand(gCat12SfxHandle3, 0x3C);
            gCat12SfxFirstPlayFlag = 0;
            sp4C = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 3, 0x80, 0x80, -1);
            break;

        case SFX_TRADE_LIGHT_ON:
            if (gCat12SfxFirstPlayFlag == 0) {
                sp4C = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 2, 0x80, 0x80, -1);
                gCat12SfxHandle3 = sp4C;
                var_fv1 = 2.0f;
                gCat12SfxFirstPlayFlag = 1;
            } else {
                var_fv1 = 4.0f;
            }
            Audio_QueueVoiceDetune(gCat12SfxHandle3, var_fv1);
            break;

        case SFX_TRADE_BALL_APPEAR:
            sp4C = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 4, 0x80, 0x80, -1);
            gCat12SfxHandle4 = sp4C;
            break;

        case SFX_TRADE_BALL_READY:
            Audio_QueueFadeSoundCommand(gCat12SfxHandle3, 0x5A);
            Audio_QueueFadeSoundCommand(gCat12SfxHandle4, 1);
            sp4C = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 5, 0x80, 0x80, -1);
            break;

        case SFX_EVOLUTION_START:
            gCat12SfxHandle5 = sp4C = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 6, 0x80, 0x80, -1);
            Audio_FadeOutAll(0x78);
            gCat12MusicStoppedFlag = 1;
            break;

        case 0x1200008:
            Audio_QueueFadeSoundCommand(gCat12SfxHandle5, 6);
            Audio_StartMusicTrack(0x13);
            gCat12MusicStoppedFlag = 1;
            break;

        case 0x1200009:
            if (gCat12MusicStoppedFlag != 0) {
                Audio_StartMusicTrack(0x10);
            }
            Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
            gCat12MusicStoppedFlag = 0;
            break;

        case 0x120000C:
            sp4C = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 8, 0x80, 0x80, -1);
            break;

        case 0x120000D:
            sp4C = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 9, 0x80, 0x80, -1);
            break;

        case 0x120000E:
            Audio_QueueFadeSoundCommand(gCat12SfxHandle1, 1);
            sp4C = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 7, 0x80, 0x80, -1);
            Audio_QueueVoiceDetune(sp4C, -5.0f);
            gCat12SfxHandle1 = sp4C;
            break;

        default:
            return 0;
    }

    gLastPlayedSfxHandle = sp4C;
}
