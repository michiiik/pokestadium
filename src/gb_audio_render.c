#include "gb_audio_render.h"
#include "src/rom_device.h"
#include "src/audio_commands.h"

typedef struct unk_D_800FD008 {
    /* 0x00 */ u16 unk_00[20];
    /* 0x28 */ u8 unk_28;
    /* 0x29 */ u8 unk_29;
    /* 0x2A */ u8 unk_2A;
    /* 0x2B */ char unk2B[1];
    /* 0x2C */ u8 unk_2C;
    /* 0x2D */ char unk2D[0x13];
} unk_D_800FD008; // size = 0x40

// Each GB sound register is mirrored as a {value, dirty} byte pair.
#define GB_REG(n) (((u8*)&gGbSoundRegs.unk_00[n])[0])
#define GB_REG_DIRTY(n) (((u8*)&gGbSoundRegs.unk_00[n])[1])

typedef struct unk_D_800FD068 {
    /* 0x00 */ u8* unk_00;
    /* 0x04 */ u8 unk_04;
    /* 0x06 */ u16 unk_06;
} unk_D_800FD068; // size = 0x8

typedef struct unk_D_800FCFD8 {
    /* 0x00 */ u32 unk_00;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 pad05[0x3];
    /* 0x04 */ u32 unk_08;
    /* 0x04 */ f32 unk_0C;
    /* 0x04 */ f32 unk_10;
    /* 0x04 */ f32 unk_14;
    /* 0x04 */ f32 unk_18;
    /* 0x04 */ u32 unk_1C;
    /* 0x04 */ u32 unk_20;
    /* 0x04 */ u32 unk_24;
    /* 0x04 */ u8 unk_28;
} unk_D_800FCFD8; // size = 0x2C

typedef struct unk_D_800FCF90 {
    /* 0x00 */ u32 unk_00;
    /* 0x00 */ s16 unk_04;
    /* 0x00 */ s16 unk_06;
    /* 0x00 */ u32 unk_08;
    /* 0x00 */ u32 unk_0C;
    /* 0x10 */ u16 unk_10[2];
    /* 0x14 */ s16 unk_14;
    /* 0x16 */ u8 unk_16;
    /* 0x00 */ u32 unk_18;
    /* 0x00 */ u32 unk_1C;
    /* 0x00 */ u32 unk_20;
    /* 0x24 */ u8 unk_24;
} unk_D_800FCF90; // size = 0x28

typedef struct unk_D_800FCFB8 {
    /* 0x00 */ u32 unk_00;
    /* 0x04 */ u8 unk04[0x18];
    /* 0x1C */ u8 unk_1C;
} unk_D_800FCFB8; // size = 0x20

typedef struct unk_D_800FCF60 {
    /* 0x00 */ u32 unk_00;
    /* 0x04 */ u16 unk_04;
    /* 0x06 */ char pad06[0x2];
    /* 0x08 */ u32 unk_08;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ u32 unk_10;
    /* 0x14 */ u32 unk_14;
    /* 0x18 */ u16 unk_18[2];
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ u8 unk_1E;
    /* 0x1F */ char pad1F[0x1];
    /* 0x20 */ u32 unk_20;
    /* 0x24 */ u32 unk_24;
    /* 0x28 */ u32 unk_28;
    /* 0x2C */ u8 unk_2C;
} unk_D_800FCF60; // size = 0x30

// .bss
unk_D_800FCF60 gSquare1State;
unk_D_800FCF90 gSquare2State;
unk_D_800FCFB8 gNoiseState;
unk_D_800FCFD8 gNoiseChannelState;
u32 D_800FD004;
unk_D_800FD008 gGbSoundRegs;
u8 D_800FD048[32];
unk_D_800FD068 gQueuedRegisterWrites[199];
u8 gGbRegCache[8];
u32 gQueuedWriteIndex;
u32 gQueuedWriteAppendIndex;
static u8 D_800FD6B0[0x30]; // pad
u8 D_800FD6E0;
u8 gGbMasterVolumeLeft;
f32 D_800FD6E4;
s32 D_800FD6E8;
static u8 D_800FD6EC[0x4]; // pad
u32 gMusicFadePos;
u32 gOutputBufferPos;
s16 gInterpolationHistory[0x1140];
u32 gInterpolationPos;
u8 gMusicEchoLevel;
s32 D_800FF980;

// Noise channel sample table: 8 rows (NR43 ratio) x 16 columns (NR43 shift clock).
typedef struct GbNoiseEntry {
    /* 0x0 */ u8 sampleId;
    /* 0x4 */ f32 pitchStep;
} GbNoiseEntry; // size = 0x8

#define GB_NOISE_TABLE ((GbNoiseEntry(*)[16])gGbNoiseSampleTable)

void GbAudio_RenderSamples(u32);
void GbAudio_ClearInterpolationHistory();
void GbAudio_ClearOutputBuffer();

// Looks like {u32 index, f32} pairs; only GbAudio_RenderNoise references it, so the
// element type is unconfirmed. Kept as raw words to stay byte-exact.
u32 gGbNoiseSampleTable[0x100] = {
    0x00000000, 0x3F800000, 0x01000000, 0x3F800000,
    0x02000000, 0x3F800000, 0x03000000, 0x3F800000,
    0x04000000, 0x3F800000, 0x05000000, 0x3F800000,
    0x06000000, 0x3F800000, 0x07000000, 0x3F800000,
    0x08000000, 0x3F800000, 0x09000000, 0x3F800000,
    0x0A000000, 0x3F800000, 0x0B000000, 0x3F800000,
    0x0C000000, 0x3F800000, 0x0D000000, 0x3F800000,
    0xFF000000, 0x3F800000, 0xFF000000, 0x3F800000,
    0x01000000, 0x3F800000, 0x02000000, 0x3F800000,
    0x03000000, 0x3F800000, 0x04000000, 0x3F800000,
    0x05000000, 0x3F800000, 0x06000000, 0x3F800000,
    0x07000000, 0x3F800000, 0x08000000, 0x3F800000,
    0x09000000, 0x3F800000, 0x0A000000, 0x3F800000,
    0x0B000000, 0x3F800000, 0x0C000000, 0x3F800000,
    0x0D000000, 0x3F800000, 0x0D000000, 0x40000000,
    0xFF000000, 0x3F800000, 0xFF000000, 0x3F800000,
    0x02000000, 0x3F800000, 0x03000000, 0x3F800000,
    0x04000000, 0x3F800000, 0x05000000, 0x3F800000,
    0x06000000, 0x3F800000, 0x07000000, 0x3F800000,
    0x08000000, 0x3F800000, 0x09000000, 0x3F800000,
    0x0A000000, 0x3F800000, 0x0B000000, 0x3F800000,
    0x0C000000, 0x3F800000, 0x0D000000, 0x3F800000,
    0x0D000000, 0x40000000, 0x0D000000, 0x40800000,
    0xFF000000, 0x3F800000, 0xFF000000, 0x3F800000,
    0x02000000, 0x3FC00000, 0x03000000, 0x3FC00000,
    0x04000000, 0x3FC00000, 0x05000000, 0x3FC00000,
    0x06000000, 0x3FC00000, 0x07000000, 0x3FC00000,
    0x08000000, 0x3FC00000, 0x09000000, 0x3FC00000,
    0x0A000000, 0x3FC00000, 0x0B000000, 0x3FC00000,
    0x0C000000, 0x3FC00000, 0x0D000000, 0x3FC00000,
    0x0D000000, 0x40400000, 0x0D000000, 0x40C00000,
    0xFF000000, 0x3F800000, 0xFF000000, 0x3F800000,
    0x03000000, 0x3F800000, 0x04000000, 0x3F800000,
    0x05000000, 0x3F800000, 0x06000000, 0x3F800000,
    0x07000000, 0x3F800000, 0x08000000, 0x3F800000,
    0x09000000, 0x3F800000, 0x0A000000, 0x3F800000,
    0x0B000000, 0x3F800000, 0x0C000000, 0x3F800000,
    0x0D000000, 0x3F800000, 0x0D000000, 0x40000000,
    0x0D000000, 0x40800000, 0x0D000000, 0x41000000,
    0xFF000000, 0x3F800000, 0xFF000000, 0x3F800000,
    0x03000000, 0x3FA00000, 0x04000000, 0x3FA00000,
    0x05000000, 0x3FA00000, 0x06000000, 0x3FA00000,
    0x07000000, 0x3FA00000, 0x08000000, 0x3FA00000,
    0x09000000, 0x3FA00000, 0x0A000000, 0x3FA00000,
    0x0B000000, 0x3FA00000, 0x0C000000, 0x3FA00000,
    0x0D000000, 0x3FA00000, 0x0D000000, 0x40200000,
    0x0D000000, 0x40A00000, 0x0D000000, 0x41200000,
    0xFF000000, 0x3F800000, 0xFF000000, 0x3F800000,
    0x03000000, 0x3FC00000, 0x04000000, 0x3FC00000,
    0x05000000, 0x3FC00000, 0x06000000, 0x3FC00000,
    0x07000000, 0x3FC00000, 0x08000000, 0x3FC00000,
    0x09000000, 0x3FC00000, 0x0A000000, 0x3FC00000,
    0x0B000000, 0x3FC00000, 0x0C000000, 0x3FC00000,
    0x0D000000, 0x3FC00000, 0x0D000000, 0x40400000,
    0x0D000000, 0x40C00000, 0x0D000000, 0x41400000,
    0xFF000000, 0x3F800000, 0xFF000000, 0x3F800000,
    0x03000000, 0x3FE00000, 0x04000000, 0x3FE00000,
    0x05000000, 0x3FE00000, 0x06000000, 0x3FE00000,
    0x07000000, 0x3FE00000, 0x08000000, 0x3FE00000,
    0x09000000, 0x3FE00000, 0x0A000000, 0x3FE00000,
    0x0B000000, 0x3FE00000, 0x0C000000, 0x3FE00000,
    0x0D000000, 0x3FE00000, 0x0D000000, 0x40600000,
    0x0D000000, 0x40E00000, 0x0D000000, 0x41600000,
    0xFF000000, 0x3F800000, 0xFF000000, 0x3F800000,
};
s32 gMusicChannel2MasterVolume = 0;
s32 gMusicChannel1MasterVolume = 0;
s32 gMusicTransitionSfxId = 0;
s32 D_80078E6C = 0; // unreferenced
s32 gMusicHandle = 0;
s32 gDefaultMusicVolume = 112;
u8 gMusicStateFlag1 = 0x00;
u8 gMusicStateFlag2 = 0x00;

// gStadiumModeActive and D_80078E83/8B/93/9B/A3/AB/B3 are interior aliases
// (undefined_syms.ld); variables.h already declares gStadiumModeActive as a u8.
u8 sLoopPointTable[0x3C] = {
    0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x04, 0x05,
    0x23, 0x23, 0x23, 0x25, 0x25, 0x25, 0x25, 0x0E, 0x23, 0x23, 0x23, 0x25,
    0x25, 0x25, 0x0E, 0x0D, 0x06, 0x06, 0x06, 0x37, 0x37, 0x37, 0x3B, 0x39,
    0x34, 0x34, 0x34, 0x2F, 0x2F, 0x2F, 0x2E, 0x35, 0x0B, 0x0B, 0x0B, 0x0C,
    0x0C, 0x0C, 0x0C, 0x12, 0x0B, 0x0B, 0x0B, 0x0C, 0x0C, 0x0C, 0x12, 0x36,
};
// D_80078EBF is an interior alias.
u8 gBgmCategory12TrackIds[0xC] = {
    0x22, 0x22, 0x22, 0x24, 0x24, 0x24, 0x24, 0x24, 0x26, 0x00, 0x00, 0x00,
};
s32 gMusicFadeDuration = -1;
s32 gCurrentBgmHandle = 0;
u8 gBgmFadeState = 0x00;
s32 gSpecialSequenceState = 0;
s32 gSequenceMusicHandle1 = 0;
s32 gSequenceMusicHandle2 = 0;

void GbAudio_GenerateSamples(s16* arg0, s32 arg1, u32 arg2, f32 arg3) {
    GbAudio_RenderSamples(arg2);
}

void GbAudio_QueueRegisterWrite(u16 arg0, u8 arg1, u16 arg2) {
    gQueuedRegisterWrites[gQueuedWriteAppendIndex].unk_00 = (u8*)&gGbSoundRegs.unk_00[(arg0 & 0xFF) - 0x10];
    gQueuedRegisterWrites[gQueuedWriteAppendIndex].unk_04 = arg1;
    gQueuedRegisterWrites[gQueuedWriteAppendIndex].unk_06 = arg2 + 1;
    gGbRegCache[arg0 & 0xFF] = arg1;
    gQueuedWriteAppendIndex++;
    gQueuedWriteAppendIndex %= 200;
    gQueuedRegisterWrites[gQueuedWriteAppendIndex].unk_06 = 0;
}

u8 GbAudio_GetRegisterValue(u16 arg0) {
    return gGbRegCache[arg0 & 0xFF];
}

void GbAudio_InitMusicPlayback(u32 arg0, u8 arg1, u32 arg2) {
    u32 temp_t3;
    u8* p;
    s32 i;
    D_800FD004 = arg0 >> 1;
    D_800FD6E4 = 1048576.0f / (f32) D_800FD004;

    p = (u8*) &gGbSoundRegs;
    for (i = 0; i < 48; i++) {
       p[i * 2 + 1] = 1;
    }
    p = (u8*) &gGbSoundRegs;
    p[0x00] = 8;
    p[0x02] = 0;
    p[0x04] = 0;
    p[0x06] = 0;
    p[0x08] = 0x40;
    p[0x0C] = 0;
    p[0x0E] = 0;
    p[0x10] = 0;
    p[0x12] = 0x40;
    p[0x14] = 0;
    p[0x16] = 0;
    p[0x18] = 0;
    p[0x1A] = 0;
    p[0x1C] = 0x40;
    p[0x20] = 0;
    p[0x22] = 0;
    p[0x24] = 0;
    p[0x26] = 0x40;
    p[0x28] = 0;
    p[0x2A] = 0;
    p[0x2C] = 0;

    p = D_800FD048;
    for (i = 0; i < 16; i++) {
        p[i * 2] = 0;
    }
    gQueuedWriteAppendIndex = 0;
    gQueuedRegisterWrites[0].unk_06 = 0;
    gQueuedWriteIndex = 0;
    gSquare1State.unk_00 = 0;
    gSquare1State.unk_2C = 0;

    gSquare2State.unk_00 = 0;
    gSquare2State.unk_24 = 0;

    gNoiseState.unk_00 = 0;
    gNoiseState.unk_1C = 0;

    gNoiseChannelState.unk_00 = 0;
    gNoiseChannelState.unk_28 = 0;
    gNoiseChannelState.unk_04 = 0xFF;

    GbAudio_ClearOutputBuffer();
    GbAudio_ClearInterpolationHistory();

    gMusicEchoLevel = arg1;
    D_800FF980 = arg2;
    D_800FF980 *= 2;
    if (D_800FF980 >= 0x1141U) {
        D_800FF980 = 0x1140;
    }
    D_800FD6E8 = 2;
}

void GbAudio_ApplyQueuedWrites(u16);
s16 GbAudio_RenderSquare1(void);
s16 GbAudio_RenderSquare2(void);
s16 GbAudio_RenderNoise(void);

void GbAudio_RenderSamples(u32 arg0) {
    s32 pad[3];
    s16 sp4C[4];
    u32 i;
    f32 var_fs0;
    s16 var_a0;
    s32 var_s4;
    s16 tmp;
    u8 var_t;

    var_fs0 = 0.0f;
    var_s4 = 1;
    arg0 >>= 1;

    var_t = gGbMasterVolumeLeft;
    var_a0 = ((var_t & 1) ? -1 : 0) + ((var_t & 2) ? -1 : 0) + ((var_t & 8) ? -1 : 0);

    if (gOutputBufferPos < gMusicFadePos) {
        if ((0xB80 - (arg0 * 4)) < (gMusicFadePos - gOutputBufferPos)) {
            arg0++;
        }
    } else if ((gOutputBufferPos - gMusicFadePos) < (arg0 * 4)) {
        arg0++;
    }

    for (i = 0; i < arg0; i++) {
        if (var_fs0 <= i) {
            GbAudio_ApplyQueuedWrites(var_s4);
        }

        if (gGbSoundRegs.unk_2C & 0x80) {
            if (gGbSoundRegs.unk_29 != 0) {
                D_800FD6E0 = gGbSoundRegs.unk_28 & 7;
                gGbMasterVolumeLeft = (gGbSoundRegs.unk_28 & 0x70) >> 4;
                gGbSoundRegs.unk_29 = 0;
            }

            sp4C[0] = GbAudio_RenderSquare1();
            sp4C[1] = GbAudio_RenderSquare2();
            sp4C[3] = GbAudio_RenderNoise();

            var_t = gGbSoundRegs.unk_2A;
            var_a0 =
                ((u32)((sp4C[0] & ((var_t & 1) ? -1 : 0)) + (sp4C[1] & ((var_t & 2) ? -1 : 0)) +
                       (sp4C[3] & ((var_t & 8) ? -1 : 0))) >>
                 5) *
                (D_800FD6E0 + 1);
        } else {
            var_a0 = 0;
        }

        if (gMusicEchoLevel != 0) {
            if (1) {}
            if (1) {}
            if (1) {}
            if (1) {}
            tmp = (f32)gInterpolationHistory[((gInterpolationPos - D_800FF980) + 0x1140) % 4416];

            var_a0 += (s16)(((tmp - var_a0) * gMusicEchoLevel) >> 8);
            gInterpolationHistory[gInterpolationPos++] = var_a0;
            if (gInterpolationPos >= 0x1140) {
                gInterpolationPos -= 0x1140;
            }
        }

        gMusicFadeBuffer[gOutputBufferPos + 0] = var_a0;
        gMusicFadeBuffer[gOutputBufferPos + 1] = var_a0;

        gOutputBufferPos += 2;
        if (gOutputBufferPos >= 0xB80) {
            gOutputBufferPos -= 0xB80;
        }

        var_s4++;
        var_fs0 += D_800FD6E4;
    }
}

void GbAudio_ApplyQueuedWrites(u16 arg0) {
    while (gQueuedRegisterWrites[gQueuedWriteIndex].unk_06 >= arg0) {
        gQueuedRegisterWrites[gQueuedWriteIndex].unk_00[0] = gQueuedRegisterWrites[gQueuedWriteIndex].unk_04;
        gQueuedRegisterWrites[gQueuedWriteIndex].unk_00[1] = 1;
        gQueuedWriteIndex++;
        if (gQueuedWriteIndex >= 0xC8) {
            gQueuedWriteIndex -= 0xC8;
        }
    }
}

s16 GbAudio_RenderSquare1(void) {
    s32 i;
    s32 changed;
    s32 nr14;
    u8 phase;
    u16 freq;
    s16 out;

    changed = 0;
    for (i = 0; i < 5; i++) {
        if (GB_REG_DIRTY(i) != 0) {
            changed = 1;
            GB_REG_DIRTY(i) = 0;
        }
    }

    if (changed) {
        nr14 = GB_REG(4);
        gSquare1State.unk_04 = GB_REG(3) | (((u8)nr14 & 7) << 8);
        gSquare1State.unk_24 = ((0x800 - gSquare1State.unk_04) * D_800FD004) >> 11;
        switch ((GB_REG(1) & 0xC0) >> 6) {
            case 0:
                gSquare1State.unk_18[1] = gSquare1State.unk_24 >> 3;
                if (gSquare1State.unk_18[1] < 0x40) {
                    gSquare1State.unk_18[1] = 0x40;
                }
                phase = 1;
                gSquare1State.unk_18[0] = gSquare1State.unk_24 - gSquare1State.unk_18[1];
                goto duty_done;
            case 1:
                gSquare1State.unk_18[1] = gSquare1State.unk_24 >> 2;
                if (gSquare1State.unk_18[1] < 0x40) {
                    gSquare1State.unk_18[1] = 0x40;
                }
                phase = 0;
                gSquare1State.unk_18[0] = gSquare1State.unk_24 - gSquare1State.unk_18[1];
                goto duty_done;
            case 2:
                gSquare1State.unk_18[1] = gSquare1State.unk_24 >> 1;
                if (gSquare1State.unk_18[1] < 0x40) {
                    gSquare1State.unk_18[1] = 0x40;
                }
                phase = 0;
                gSquare1State.unk_18[0] = gSquare1State.unk_24 - gSquare1State.unk_18[1];
                goto duty_done;
            case 3:
                gSquare1State.unk_18[0] = gSquare1State.unk_24 >> 2;
                if (gSquare1State.unk_18[0] < 0x40) {
                    gSquare1State.unk_18[0] = 0x40;
                }
                phase = 1;
                gSquare1State.unk_18[1] = gSquare1State.unk_24 - gSquare1State.unk_18[0];
                goto duty_done;
        }
        gSquare1State.unk_18[1] = 0;
        phase = 1;
        gSquare1State.unk_18[0] = gSquare1State.unk_24;
    duty_done:

        if (!(GB_REG(2) & 8) && !(GB_REG(2) & 0xF0)) {
            gSquare1State.unk_2C = 0;
            gGbSoundRegs.unk_2C &= 0xFFFE;
            return 0;
        }
        if ((GB_REG(2) & 8) && !(GB_REG(2) & 0xF0) && !(GB_REG(2) & 7)) {
            gSquare1State.unk_2C = 0;
            gGbSoundRegs.unk_2C &= 0xFFFE;
            return 0;
        }

        if (nr14 & 0x80) {
            gSquare1State.unk_00 = 0;
            gSquare1State.unk_2C = 1;
            gSquare1State.unk_1E = phase;
            gSquare1State.unk_1C = 0;
            gSquare1State.unk_20 = 0;
            gSquare1State.unk_0C = (u8)(GB_REG(2) & 0xF0) << 7;
            if (GB_REG(2) & 7) {
                if (GB_REG(2) & 8) {
                    gSquare1State.unk_0E = 0x800;
                } else {
                    gSquare1State.unk_0E = -0x800;
                }
                gSquare1State.unk_14 = gSquare1State.unk_10 = (GB_REG(2) & 7) * D_800FD004;
            } else {
                gSquare1State.unk_0E = 0;
                gSquare1State.unk_10 = -1;
                gSquare1State.unk_14 = -1;
            }
            gSquare1State.unk_08 = (((GB_REG(0) & 0x70) >> 4) * D_800FD004) >> 1;
            if (gSquare1State.unk_08 == 0) {
                gSquare1State.unk_08 = -1;
            }
            if (nr14 & 0x40) {
                gSquare1State.unk_28 = ((0x40 - (GB_REG(1) & 0x3F)) * D_800FD004) >> 2;
            } else {
                gSquare1State.unk_28 = -1;
            }
            GB_REG(4) = nr14 & ~0x80;
        }
        if (gSquare1State.unk_2C == 0) {
            return 0;
        }
    } else if (gSquare1State.unk_2C == 0) {
        return 0;
    }

    if (gSquare1State.unk_28 < gSquare1State.unk_00) {
        gSquare1State.unk_2C = 0;
        gGbSoundRegs.unk_2C &= 0xFFFE;
        return 0;
    }

    if ((gSquare1State.unk_08 - (gSquare1State.unk_00 % gSquare1State.unk_08)) <= 0x40) {
        if (!(GB_REG(0) & 8)) {
            freq = (gSquare1State.unk_04 + (gSquare1State.unk_04 >> (GB_REG(0) & 7)));
            if (freq >= 0x800) {
                gSquare1State.unk_2C = 0;
                gGbSoundRegs.unk_2C &= 0xFFFE;
                return 0;
            }
            gSquare1State.unk_04 = freq;
        } else {
            freq = (gSquare1State.unk_04 - (gSquare1State.unk_04 >> (GB_REG(0) & 7)));
            if ((freq >= 0x800) || (freq < 0)) {
                goto skip;
            }
            gSquare1State.unk_04 = freq;
        }
        gSquare1State.unk_24 = ((0x800 - gSquare1State.unk_04) * D_800FD004) >> 11;
        switch ((GB_REG(1) & 0xC0) >> 6) {
            case 0:
                gSquare1State.unk_18[1] = gSquare1State.unk_24 >> 3;
                if (gSquare1State.unk_18[1] < 0x40) {
                    gSquare1State.unk_18[1] = 0x40;
                }
                gSquare1State.unk_18[0] = gSquare1State.unk_24 - gSquare1State.unk_18[1];
                break;
            case 1:
                gSquare1State.unk_18[1] = gSquare1State.unk_24 >> 2;
                if (gSquare1State.unk_18[1] < 0x40) {
                    gSquare1State.unk_18[1] = 0x40;
                }
                gSquare1State.unk_18[0] = gSquare1State.unk_24 - gSquare1State.unk_18[1];
                break;
            case 2:
                gSquare1State.unk_18[1] = gSquare1State.unk_24 >> 1;
                if (gSquare1State.unk_18[1] < 0x40) {
                    gSquare1State.unk_18[1] = 0x40;
                }
                gSquare1State.unk_18[0] = gSquare1State.unk_24 - gSquare1State.unk_18[1];
                break;
            case 3:
                gSquare1State.unk_18[0] = gSquare1State.unk_24 >> 2;
                if (gSquare1State.unk_18[0] < 0x40) {
                    gSquare1State.unk_18[0] = 0x40;
                }
                gSquare1State.unk_18[1] = gSquare1State.unk_24 - gSquare1State.unk_18[0];
                break;
            default:
                gSquare1State.unk_18[1] = 0;
                gSquare1State.unk_18[0] = gSquare1State.unk_24;
        }
    }
skip:

    if (gSquare1State.unk_14 < gSquare1State.unk_00) {
        gSquare1State.unk_0C += gSquare1State.unk_0E;
        gSquare1State.unk_14 += gSquare1State.unk_10;
    }
    while (gSquare1State.unk_20 < gSquare1State.unk_00) {
        gSquare1State.unk_1E ^= 1;
        gSquare1State.unk_20 += gSquare1State.unk_18[gSquare1State.unk_1E];
    }

    if (!(GB_REG(2) & 8) && (gSquare1State.unk_0C < 0)) {
        gSquare1State.unk_2C = 0;
        return 0;
    }
    if ((u16)gSquare1State.unk_0C > 0x7800) {
        gSquare1State.unk_0C = 0x7800;
        gSquare1State.unk_0E = 0;
        gSquare1State.unk_14 = -1;
    }
    out = gSquare1State.unk_0C;
    if (gSquare1State.unk_1E == 0) {
        out = -out;
    }
    gSquare1State.unk_00 += 0x40;
    return out;
}

s16 GbAudio_RenderSquare2(void) {
    s32 i;
    s32 changed;
    s32 nr24;
    u8 phase;
    s16 out;

    changed = 0;
    for (i = 6; i < 10; i++) {
        if (GB_REG_DIRTY(i) != 0) {
            changed = 1;
            GB_REG_DIRTY(i) = 0;
        }
    }

    if (changed) {
        nr24 = GB_REG(9);
        gSquare2State.unk_1C = ((0x800 - (GB_REG(8) | (((u8)nr24 & 7) << 8))) * D_800FD004) >> 11;
        switch ((GB_REG(6) & 0xC0) >> 6) {
            case 0:
                gSquare2State.unk_10[1] = gSquare2State.unk_1C >> 3;
                if (gSquare2State.unk_10[1] < 0x40) {
                    gSquare2State.unk_10[1] = 0x40;
                }
                phase = 1;
                gSquare2State.unk_10[0] = gSquare2State.unk_1C - gSquare2State.unk_10[1];
                goto duty_done;
            case 1:
                gSquare2State.unk_10[1] = gSquare2State.unk_1C >> 2;
                if (gSquare2State.unk_10[1] < 0x40) {
                    gSquare2State.unk_10[1] = 0x40;
                }
                phase = 0;
                gSquare2State.unk_10[0] = gSquare2State.unk_1C - gSquare2State.unk_10[1];
                goto duty_done;
            case 2:
                gSquare2State.unk_10[1] = gSquare2State.unk_1C >> 1;
                if (gSquare2State.unk_10[1] < 0x40) {
                    gSquare2State.unk_10[1] = 0x40;
                }
                phase = 0;
                gSquare2State.unk_10[0] = gSquare2State.unk_1C - gSquare2State.unk_10[1];
                goto duty_done;
            case 3:
                gSquare2State.unk_10[0] = gSquare2State.unk_1C >> 2;
                if (gSquare2State.unk_10[0] < 0x40) {
                    gSquare2State.unk_10[0] = 0x40;
                }
                phase = 1;
                gSquare2State.unk_10[1] = gSquare2State.unk_1C - gSquare2State.unk_10[0];
                goto duty_done;
        }
        gSquare2State.unk_10[1] = 0;
        phase = 1;
        gSquare2State.unk_10[0] = gSquare2State.unk_1C;
    duty_done:

        if (!(GB_REG(7) & 8) && !(GB_REG(7) & 0xF0)) {
            gSquare2State.unk_24 = 0;
            gGbSoundRegs.unk_2C &= 0xFFFD;
            return 0;
        }
        if ((GB_REG(7) & 8) && !(GB_REG(7) & 0xF0) && !(GB_REG(7) & 7)) {
            gSquare2State.unk_24 = 0;
            gGbSoundRegs.unk_2C &= 0xFFFD;
            return 0;
        }

        if (nr24 & 0x80) {
            gSquare2State.unk_00 = 0;
            gSquare2State.unk_24 = 1;
            gSquare2State.unk_16 = phase;
            gSquare2State.unk_14 = 0;
            gSquare2State.unk_18 = 0;
            gSquare2State.unk_04 = ((GB_REG(7) & 0xF0) << 3) << 4;
            if (GB_REG(7) & 7) {
                if (GB_REG(7) & 8) {
                    gSquare2State.unk_06 = 0x800;
                } else {
                    gSquare2State.unk_06 = -0x800;
                }
                gSquare2State.unk_0C = gSquare2State.unk_08 = (GB_REG(7) & 7) * D_800FD004;
            } else {
                gSquare2State.unk_06 = 0;
                gSquare2State.unk_08 = -1;
                gSquare2State.unk_0C = -1;
            }
            if (nr24 & 0x40) {
                gSquare2State.unk_20 = ((0x40 - (GB_REG(6) & 0x3F)) * D_800FD004) >> 2;
            } else {
                gSquare2State.unk_20 = -1;
            }
            GB_REG(9) = nr24 & ~0x80;
        }
        if (gSquare2State.unk_24 == 0) {
            return 0;
        }
    } else if (gSquare2State.unk_24 == 0) {
        return 0;
    }

    if (gSquare2State.unk_20 < gSquare2State.unk_00) {
        gSquare2State.unk_24 = 0;
        gGbSoundRegs.unk_2C &= 0xFFFD;
        return 0;
    }

    if (gSquare2State.unk_0C < gSquare2State.unk_00) {
        gSquare2State.unk_04 += gSquare2State.unk_06;
        gSquare2State.unk_0C += gSquare2State.unk_08;
    }
    while (gSquare2State.unk_18 <= gSquare2State.unk_00) {
        gSquare2State.unk_16 ^= 1;
        gSquare2State.unk_18 += gSquare2State.unk_10[gSquare2State.unk_16];
    }

    if (!(GB_REG(7) & 8) && (gSquare2State.unk_04 < 0)) {
        gSquare2State.unk_24 = 0;
        return 0;
    }
    if ((u16)gSquare2State.unk_04 > 0x7800) {
        gSquare2State.unk_04 = 0x7800;
        gSquare2State.unk_06 = 0;
        gSquare2State.unk_0C = -1;
    }
    out = gSquare2State.unk_04;
    if (gSquare2State.unk_16 == 0) {
        out = -out;
    }
    gSquare2State.unk_00 += 0x40;
    return out;
}

s16 GbAudio_RenderNoise(void) {
    s32 i;
    s32 nr43;
    s32 row;
    s32 changed;
    u32 nr44;
    u16 col;
    GbNoiseEntry* entry;
    u8 sampleId;
    s16 sample;
    unk_D_800FC6CC* ent;

    changed = 0;
    for (i = 0x10; i < 0x14; i++) {
        if (GB_REG_DIRTY(i) != 0) {
            changed = 1;
            GB_REG_DIRTY(i) = 0;
        }
    }

    if (changed) {
        nr43 = GB_REG(0x12);
        row = nr43 & 7;
        col = (GB_REG(0x12) & 0xF0) >> 4;
        entry = &GB_NOISE_TABLE[row][col];
        sampleId = entry->sampleId;
        if (sampleId == 0xFF) {
            gNoiseChannelState.unk_28 = 0;
            return 0;
        }
        if (GB_REG(0x12) & 8) {
            sampleId += 0x10;
        }
        gNoiseChannelState.unk_0C = GB_NOISE_TABLE[nr43 & 7][col].pitchStep;
        if (sampleId != gNoiseChannelState.unk_04) {
            gNoiseChannelState.unk_04 = sampleId;
            ent = (unk_D_800FC6CC*)((u32*)gAudioSeqTOC + (gNoiseChannelState.unk_04 << 1));
            Rom_DmaRead(ent->unk_00, (u32)gAudioSfxDataBuffer, ent->unk_04);
        }

        gNoiseChannelState.unk_08 = 0;
        gNoiseChannelState.unk_10 = 0;
        if (!(GB_REG(0x11) & 8) && !(GB_REG(0x11) & 0xF0)) {
            gNoiseChannelState.unk_28 = 0;
            gGbSoundRegs.unk_2C &= 0xFFF7;
            return 0;
        }
        if ((GB_REG(0x11) & 8) && !(GB_REG(0x11) & 0xF0) && !(GB_REG(0x11) & 7)) {
            gNoiseChannelState.unk_28 = 0;
            gGbSoundRegs.unk_2C &= 0xFFF7;
            return 0;
        }

        nr44 = GB_REG(0x13);
        if (nr44 & 0x80) {
            gNoiseChannelState.unk_00 = 0;
            gNoiseChannelState.unk_28 = 1;
            gNoiseChannelState.unk_14 = (u8)(GB_REG(0x11) & 0xF0) * 0.05f;
            if (GB_REG(0x11) & 7) {
                if (GB_REG(0x11) & 8) {
                    gNoiseChannelState.unk_18 = 0.05f;
                } else {
                    gNoiseChannelState.unk_18 = -0.05f;
                }
                gNoiseChannelState.unk_20 = gNoiseChannelState.unk_1C = (GB_REG(0x11) & 7) * D_800FD004 * 4;
            } else {
                gNoiseChannelState.unk_1C = -1;
                gNoiseChannelState.unk_20 = -1;
                gNoiseChannelState.unk_18 = 0.0f;
            }
            if (nr44 & 0x40) {
                gNoiseChannelState.unk_24 = (0x40 - (GB_REG(0x10) & 0x3F)) * D_800FD004;
            } else {
                gNoiseChannelState.unk_24 = -1;
            }
            GB_REG(0x13) = nr44 & 0xFF7F;
        }
        if (gNoiseChannelState.unk_28 == 0) {
            return 0;
        }
    } else if (gNoiseChannelState.unk_28 == 0) {
        return 0;
    }

    if (gNoiseChannelState.unk_24 < gNoiseChannelState.unk_00) {
        gNoiseChannelState.unk_28 = 0;
        gGbSoundRegs.unk_2C &= 0xFFF7;
        return 0;
    }

    if (gNoiseChannelState.unk_20 < gNoiseChannelState.unk_00) {
        gNoiseChannelState.unk_20 += gNoiseChannelState.unk_1C;
        gNoiseChannelState.unk_14 += gNoiseChannelState.unk_18;
    }

    ent = (unk_D_800FC6CC*)((u32*)gAudioSeqTOC + (gNoiseChannelState.unk_04 << 1));
    if (1) {}
    sample = ((s16*)gAudioSfxDataBuffer)[gNoiseChannelState.unk_08];
    gNoiseChannelState.unk_10 += 1.0f;
    if (gNoiseChannelState.unk_0C <= gNoiseChannelState.unk_10) {
        gNoiseChannelState.unk_08++;
        gNoiseChannelState.unk_10 -= gNoiseChannelState.unk_0C;
        if (gNoiseChannelState.unk_08 >= (ent->unk_04 >> 1)) {
            gNoiseChannelState.unk_08 = 0;
        }
    }

    if (!(GB_REG(0x11) & 8) && (gNoiseChannelState.unk_14 < 0.0f)) {
        gNoiseChannelState.unk_28 = 0;
        return 0;
    }
    if (gNoiseChannelState.unk_14 > 0.75f) {
        gNoiseChannelState.unk_14 = 0.75f;
        gNoiseChannelState.unk_18 = 0.0f;
        gNoiseChannelState.unk_20 = -1;
    }
    sample = sample * gNoiseChannelState.unk_14;
    gNoiseChannelState.unk_00 += 0x100;
    return sample;
}

void GbAudio_ClearInterpolationHistory(void) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gInterpolationHistory); i++) {
        gInterpolationHistory[i] = 0;
    }
    gInterpolationPos = 0;
}

void GbAudio_ClearOutputBuffer(void) {
    s32 i;

    for (i = 0; i < 0xB80; i++) {
        gMusicFadeBuffer[i] = 0;
    }
    gMusicFadePos = 0;
    gOutputBufferPos = 0;
}

void func_8004AD2C(void) {

}
