#include "audio_commands.h"
#include "src/audio_commands_category2.h"
#include "src/audio_commands.h"
#include "src/3D140.h"
#include "src/libnumus/player.h"
#include "src/rom_device.h"
#include "src/audio_channel.h"
#include "src/audio_cache.h"
#include "src/cry.h"
#include "src/audio_fade.h"

typedef struct SfxTableEntry {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 unk_04;
} SfxTableEntry; // size = 0x5

static SfxTableEntry gSpeciesSfxTable[151] = {
    { 1, 1, 0, 0x52, 4 },    { 1, 1, 0, 0x58, 0x36 }, { 2, 2, 0, 0x1A, 0x3C }, { 1, 1, 0, 0, 0 },
    { 1, 1, 0, 0x54, 0x3C }, { 2, 2, 1, 0x2E, 0xA },  { 1, 1, 0, 0x5C, 0 },    { 1, 1, 0, 0x44, 0x52 },
    { 2, 2, 0, 0x22, 0x36 }, { 1, 1, 0, 0x98, 0 },    { 1, 1, 0, 0x32, 0xA4 }, { 1, 1, 0, 0, 0 },
    { 1, 1, 0, 0x32, 0x9E }, { 1, 1, 0, 0xC, 0 },     { 1, 1, 0, 0x12, 0 },    { 1, 1, 0, 0x8C, 0x24 },
    { 1, 1, 0, 0x8C, 0x24 }, { 2, 1, 0, 0x8C, 0x24 }, { 1, 1, 0, 0x54, 0 },    { 1, 1, 0, 0x50, 0x34 },
    { 0, 1, 0, 0x3C, 0 },    { 9, 1, 0, 0x28, 0x10 }, { 2, 1, 0, 0x76, 0x94 }, { 2, 2, 0, 0x72, 0x3C },
    { 1, 0, 0, 0x4E, 0x3C }, { 1, 1, 0, 0x7E, 0x72 }, { 1, 1, 0, 0, 0x7E },    { 1, 1, 0, 0x3C, 0 },
    { 1, 1, 0, 0x34, 0x16 }, { 1, 1, 0, 0x3C, 0x5A }, { 2, 2, 0, 0, 0 },       { 1, 1, 0, 8, 0xA },
    { 1, 1, 0, 0x6E, 0x64 }, { 2, 2, 0, 0x40, 0 },    { 1, 0, 0, 0x54, 0x7E }, { 1, 0, 0, 0x2C, 0x66 },
    { 1, 0, 0, 0x72, 0x98 }, { 1, 1, 0, 0x2A, 0x78 }, { 9, 0, 0, 0x92, 0x2C }, { 1, 1, 0, 0x52, 0x3C },
    { 1, 1, 0, 0x46, 0 },    { 2, 2, 0, 0x54, 0x4A }, { 0, 0, 0, 0x38, 0x1E }, { 1, 1, 0, 0, 0x38 },
    { 9, 1, 0, 0x50, 0x7E }, { 1, 1, 0, 0x6E, 0 },    { 1, 1, 0, 0x8E, 0 },    { 1, 1, 0, 0x38, 0xBA },
    { 2, 1, 0, 0x3A, 0x36 }, { 0, 0, 0, 0x3C, 0x5E }, { 0, 1, 0, 0x3C, 0x54 }, { 1, 1, 0, 0x24, 0x88 },
    { 2, 1, 0, 0x5A, 8 },    { 1, 1, 0, 0x5A, 0 },    { 2, 1, 0, 0x38, 0x64 }, { 1, 1, 0, 0x64, 0x54 },
    { 1, 1, 0, 0, 0 },       { 1, 1, 0, 0x42, 0x18 }, { 2, 1, 0, 0x6A, 0x16 }, { 1, 0, 0, 0x32, 0x60 },
    { 1, 1, 0, 0xA4, 0x42 }, { 2, 1, 0, 0x38, 0x86 }, { 1, 1, 0, -0x34, 0 },   { 1, 1, 0, 0x5A, 0x6C },
    { 2, 1, 0, 0x46, 0 },    { 1, 1, 0, 0x14, 0xAE }, { 2, 1, 0, 0x64, 0x32 }, { 2, 2, 0, 0x6C, 0xB0 },
    { 0, 1, 0, 0x28, 0x92 }, { 1, 1, 0, 0x24, 0x34 }, { 2, 1, 0, 0x16, 0 },    { 1, 1, 0, 0x30, 0x9E },
    { 2, 2, 0, 0x20, 0 },    { 2, 1, 0, 0x7A, 0x7C }, { 2, 2, 0, 0, 0xC },     { 2, 2, 0, 0x3C, 0x2E },
    { 1, 0, 0, 0, 0 },       { 2, 1, 0, 0x28, 0x10 }, { 2, 1, 0, 0, 0x98 },    { 2, 1, 0, 0, 0x80 },
    { 1, 1, 0, 0x6A, 0x1A }, { 1, 1, 0, 0, 0x1A },    { 1, 1, 0, 0x64, 0xC },  { 1, 1, 0, 0, 0 },
    { 2, 1, 0, 0x28, 0 },    { 1, 1, 0, 0x54, 0x72 }, { 2, 1, 0, 0x6A, 0 },    { 9, 2, 0, 0x44, 0 },
    { 9, 2, 0, 0x40, 0x2A }, { 1, 1, 0, 0x7E, 0 },    { 2, 2, 0, 0, 0x7C },    { 0, 1, 0, 0x3E, 0x2E },
    { 0, 2, 0, 0x32, 0x33 }, { 0, 2, 0, 0x38, 0x32 }, { 2, 2, 0, 0x30, 0x3A }, { 1, 1, 0, 0x34, 0 },
    { 2, 1, 0, 0, 0 },       { 1, 1, 0, 0x58, 0 },    { 1, 1, 0, 0x46, 0 },    { 9, 1, 0, 0, 0 },
    { 9, 1, 0, 0x28, 0x36 }, { 1, 1, 0, 0x44, 0x2A }, { 2, 1, 0, 0x60, 0 },    { 1, 1, 0, 0xB4, 0x36 },
    { 1, 1, 0, 0x14, 0 },    { 2, 1, 0, 0x10, 0x72 }, { 2, 1, 0, 0x40, 0xB6 }, { 2, 1, 0, 0x3E, 0x2C },
    { 1, 1, 0, 0x50, 0x78 }, { 2, 2, 0, 0x20, 0x3C }, { 2, 1, 0, 0x4C, 0x3C }, { 2, 2, 0, 0x36, 0xC },
    { 1, 0, 0, 0x1E, 0 },    { 1, 1, 0, 0, 0x51 },    { 2, 1, 0, 0x8C, 0x22 }, { 0, 0, 0, 0x5C, 0x6E },
    { 1, 1, 0, 0x5A, 0x6E }, { 1, 0, 0, 0x7C, 0 },    { 1, 1, 0, 0, 0 },       { 1, 1, 0, 0x28, 0 },
    { 1, 1, 0, 0x1A, 0x64 }, { 1, 1, 0, 0x62, 0 },    { 1, 1, 0, 0x14, 0x1A }, { 9, 1, 0, 0x4A, 0 },
    { 2, 1, 0, 0xA, 0x5E },  { 2, 2, 0, 0x2E, 0 },    { 1, 1, 0, 0x14, 0 },    { 2, 2, 0, 0x8C, 0x62 },
    { 0, 1, 0, 0, 0x64 },    { 2, 2, 0, 0x8C, 0x1E }, { 2, 1, 0, 0x28, 0x82 }, { 9, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 0x3C },    { 1, 0, 0, 0, 6 },       { 1, 0, 0, 0x40, 0xC },  { 1, 0, 0, 0x18, 0x8E },
    { 1, 1, 0, 0x3C, 0xE },  { 1, 1, 0, 0, 0 },       { 1, 1, 0, 0, 0x34 },    { 1, 1, 0, 0x32, 0x12 },
    { 1, 1, 0, 0x50, 0x42 }, { 9, 2, 0, 0x28, 0 },    { 2, 1, 0, 0x3C, 0x82 }, { 2, 1, 0, 0x32, 0x2A },
    { 2, 1, 0, 0x38, 0x2E }, { 2, 1, 0, 0x1E, 0x2A }, { 2, 0, 0, 0x23, 0x18 }, { 2, 1, 0, 0x28, 0 },
    { 2, 1, 0, 0x6C, 0 },    { 2, 1, 0, 0x64, 0xA },  { 0, 0, 0, 0x82, 0 },
};

static s32 gSfxHandle_MenuSelect = 0;
static s32 gSfxHandle_MenuConfirm = 0;
static s32 gSfxHandle_BackgroundMusic = 0;
static s32 gSfxHandle_Unused = 0;
static s32 gSfxHandle_MusicStartSound = 0;
static s32 gSfxHandle_MusicMidwaySound = 0;

u8 gAudioHeap[0x493E0];
u32* gSfxWaveTableBuffer;
u8* gSfxWaveTable;
u8* gSfxTable;
u8* gSfxBankStandard;
u8* gSfxBankSecondary;
u8 pad_D_800FC694[4];
u8* gSfxBankScratch[3];
u8* gAudioSfxBank;
u8* gAudioSfxInstBank;
u8* gMoveSoundBankBase;
u8* gMoveSoundBankData[3];
MoveSoundTable* gMoveSoundTables[3];
unk_D_800FC6CC* gAudioSeqTOC;
u8* gAudioSfxDataBuffer;
GbTrackHeader* gAudioSfxBankExtra;
s16* gMusicFadeBuffer;
u8* gMoveSoundTempBuffer;
u32* gAudioRegions;
File1Header* gSfxArchiveHeader;
ALSeqFile* gMoveSoundSeqInfo;
File2SubHeader1* gSfxStandardSubHeader;
File2SubHeader2* gSfxSecondarySubHeader;
ALSeqFile* gAudioSfxBankSeqHeader;
ALSeqFile* gAudioSeqHeaderNested;
File2SubHeader2* gMoveSoundArchive;
ALSeqFile* gMoveSoundSeqTable;
ALSeqFile* gAudioSeqBank;
ALSeqFile* gAudioSeqBuffer;
s32 pad_D_800FC70C;
s32 pad_D_800FC710;
ALSeqFile* gAudioSeqData;
u8 pad_D_800FC718[0x80];
u32* gAudioHeaderTemp;

s32 Audio_PlayCommand(u32 command, s32 parameter, UNUSED s32 unused) {
    static s32 gSfxHandle_MenuSfx = 0;

    UNUSED s32 pad;
    u32 bank_offset;
    u32 sweep_amount;
    s32 effect_priority;
    s32 effect_id;
    UNUSED s32 pad2;
    volatile s32 sound_handle;
    u32 command_bank;

    command_bank = command & 0xFFFF0000;
    effect_priority = 0x80;

    if (command_bank != gCurrentSfxBankId) {
        bank_offset = gAudioSfxBankSeqHeader->seqArray[0].offset;
        Rom_DmaRead(bank_offset, gAudioHeaderTemp, 0xC);
        Audio_RelocateOffsets(gAudioHeaderTemp, bank_offset, 3);
        Rom_DmaRead(gAudioHeaderTemp[0], gMoveSoundTempBuffer, gAudioHeaderTemp[1] - gAudioHeaderTemp[0]);
        Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gAudioSfxInstBank, 0xBB8);
        Audio_RelocateSoundBankTable(gAudioSfxInstBank);
        Rom_DmaRead(gAudioHeaderTemp[1], gMoveSoundTempBuffer, gAudioHeaderTemp[2] - gAudioHeaderTemp[1]);
        Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gAudioSfxBank, 0x1388);
        Audio_RelocateSoundBankWrapper(gAudioSfxBank, gAudioHeaderTemp[2]);
    }

    gCurrentSfxBankId = command_bank;

    switch (command) {
        case 0x0:
            if (Audio_IsSoundPlaying(gSfxHandle_MenuSelect) != 0) {
                return 0;
            }

            switch (parameter) {
                case 1:
                    effect_id = 1;
                    break;

                case 2:
                    effect_id = 2;
                    break;

                default:
                    return 0;
            }

            gSfxHandle_MenuSelect = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, effect_id, 0x70, 0x80, -1);
            sound_handle = gSfxHandle_MenuSelect;
            break;

        case 0x1:
            if (parameter < -0x5A) {
                parameter = -0x5A;
            } else if (parameter >= 0x5B) {
                parameter = 0x5A;
            }

            do {
                sweep_amount = (parameter + 0x5A) * 1.4111111f;
            } while (0);

            if (sweep_amount > 0xFE) {
                sweep_amount = 0xFE;
            }
            sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 3, 0x80, sweep_amount, -1);
            break;

        case 0x2:
            if (Audio_IsSoundPlaying(gSfxHandle_BackgroundMusic) != 0) {
                Audio_QueueFadeSoundCommand(gSfxHandle_BackgroundMusic, 0x14);
                gSfxHandle_BackgroundMusic = 0;
            }
            sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 4, 0x80, 0x80, -1);
            break;

        case 0x3:
            gSfxHandle_BackgroundMusic = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 5, 0x80, 0x80, -1);
            sound_handle = gSfxHandle_BackgroundMusic;
            break;

        case 0x4:
            switch (parameter) {
                case 1:
                    effect_id = 1;
                    break;
                case 2:
                    effect_id = 2;
                    break;

                default:
                    return 0;
            }
            gSfxHandle_MenuConfirm = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, effect_id, 0x80, 0x80, -1);
            sound_handle = gSfxHandle_MenuConfirm;
            break;

        case 0x6:
            sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 6, 0x80, 0x80, -1);
            break;

        case 0x7:
            sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 7, 0x80, 0x80, -1);
            break;

        case 0x8:
            return 0;

        case 0x33:
            if (parameter >= 0x98) {
                parameter = 3;
            }

            if ((parameter > 0) && (parameter < 0x98)) {
                parameter--;
            } else {
                parameter = 0;
            }

            switch (gSpeciesSfxTable[parameter].unk_00) {
                case 0:
                    return 0;

                case 1:
                    sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 7, 0x80, 0x80, -1);
                    break;

                case 2:
                    sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 6, 0x80, 0x80, -1);
                    break;

                default:
                    return 0;
            }
            break;

        case 0x32:
            if (parameter == 0xC8) {
                return Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 0x20, 0x80, 0x80, -1);
            }

            if (parameter >= 0x98) {
                parameter = 2;
            }

            if ((parameter > 0) && (parameter < 0x98)) {
                parameter--;
            } else {
                parameter = 0;
            }

            switch (gSpeciesSfxTable[parameter].unk_01) {
                case 0:
                    sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 0xB, 0x80, 0x80, -1);
                    break;

                case 1:
                    sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 0xA, 0x80, 0x80, -1);
                    break;

                case 2:
                    sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 9, 0x80, 0x80, -1);
                    break;
            }
            break;

        case 0x12:
            sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 0xC, 0x80, 0x80, -1);
            break;

        case 0x13:
            sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 0xD, 0x80, 0x80, -1);
            break;

        case 0x14:
            sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 0xE, 0x80, 0x80, -1);
            break;

        case 0x28:
            sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 0xF, 0x80, 0x80, -1);
            break;

        case 0x5A:
            switch (parameter) {
                case 1:
                    effect_id = 0x12;
                    break;

                case 2:
                    effect_id = 0x13;
                    break;

                default:
                    return 0;
            }

            gSfxHandle_MusicStartSound = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, effect_id, 0x80, 0x80, -1);
            break;

        case 0x5B:
            switch (parameter) {
                case 1:
                    break;

                case 2:
                    return 0;

                default:
                    return 0;
            }
            gSfxHandle_MusicMidwaySound = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 0x14, 0x80, 0x80, -1);
            break;

        case 0x15:
            switch (parameter) {
                case 1:
                    effect_id = 0x10;
                    break;

                case 2:
                    effect_id = 0x11;
                    break;

                default:
                    return 0;
            }
            sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, effect_id, 0x80, 0x80, 0x80);
            break;

        case 0x16:
            switch (parameter) {
                case 1:
                    effect_id = 0x16;
                    break;

                case 2:
                    effect_id = 0x17;
                    break;

                default:
                    return 0;
            }

            if (Audio_IsSoundPlaying(gSfxHandle_MenuSfx) != 0) {
                gSfxHandle_MenuSfx = 0;
                sound_handle = gSfxHandle_MenuSfx;
            } else {
                gSfxHandle_MenuSfx = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, effect_id, 0x80, 0x80, 0x80);
                sound_handle = gSfxHandle_MenuSfx;
            }
            break;

        case 0x3C:
            sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 0x18, 0x80, 0x80, -1);
            break;

        case 0x3D:
            switch (parameter) {
                case 3:
                    effect_id = 0x19;
                    break;

                case 2:
                    effect_id = 0x1A;
                    break;

                case 1:
                    effect_id = 0x1B;
                    break;

                case 0:
                    effect_id = 0x1C;
                    effect_priority = 0xFF;
                    break;

                default:
                    return 0;
            }
            sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, effect_id, effect_priority, 0x80, -1);
            break;

        case 0x3E:
            if (parameter < 11) {
                if (parameter == 1 || parameter == 2) {
                    effect_id = 0x1D;
                } else {
                    effect_id = 0x1E;
                }
            } else {
                return 0;
            }
            sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, effect_id, 0xFF, 0x80, -1);
            break;

        case 0x3F:
            sound_handle = Audio_PlaySoundEffect(gAudioSfxBank, gAudioSfxInstBank, 0x1F, 0x80, 0x80, -1);
            break;

        case 0x1E:
            sound_handle = Audio_PlayMoveSoundEffect(0xA6, 1, 0);
            break;

        case 0x1F:
            sound_handle = Audio_PlayMoveSoundEffect(0xA6, 1, 1);
            break;

        case 0x20:
            sound_handle = Audio_PlayMoveSoundEffect(0xA6, 1, 2);
            break;

        case 0x21:
            sound_handle = Audio_PlayMoveSoundEffect(0xA6, 1, 3);
            break;

        case 0x22:
            sound_handle = Audio_PlayMoveSoundEffect(0xA6, 1, 4);
            break;

        case 0x23:
            sound_handle = Audio_PlayMoveSoundEffect(0xA6, 1, 5);
            break;

        default:
            return 0;
    }

    return sound_handle;
}

s32 Audio_FadeCommand(s32 arg0, u32 arg1) {
    switch (arg0) {
        case 0x0:
            if (arg1 == 0) {
                arg1 = 1;
            }
            Audio_QueueFadeSoundCommand(gSfxHandle_MenuSelect, arg1);
            gSfxHandle_MenuSelect = 0;
            break;

        case 0x4:
            if (arg1 == 0) {
                arg1 = 1;
            }
            Audio_QueueFadeSoundCommand(gSfxHandle_MenuConfirm, arg1);
            break;

        case 0x3:
            Audio_QueueFadeSoundCommand(gSfxHandle_BackgroundMusic, 0x3C);
            gSfxHandle_BackgroundMusic = 0;
            break;

        case 0xA1:
            Audio_QueueFadeSoundCommand(gSfxHandle_Unused, 0x78);
            gSfxHandle_Unused = 0;
            break;

        case 0x5A:
            Audio_QueueFadeSoundCommand(gSfxHandle_MusicStartSound, 0x5A);
            gSfxHandle_MusicStartSound = 0;
            break;

        case 0x5B:
            Audio_QueueFadeSoundCommand(gSfxHandle_MusicMidwaySound, 0x5A);
            gSfxHandle_MusicMidwaySound = 0;
            break;

        default:
            return 0;
    }

    return 1;
}

void Audio_NullTask(void) {
}

void Audio_CreateFrameTaskWrapper(void* arg0) {
    Audio_CreateFrameTask(arg0);
}

void Audio_RelocateOffsets(u32* arg0, u32 arg1, s32 arg2) {
    u32 i;

    for (i = 0; i < arg2; i++, arg0++) {
        *arg0 += arg1;
    }
}

#define AUDIO_HEAP_SIZE 300000

s32 Audio_InitBanks(void) {
    s32 i;
    FileHeaders* sp78;
    UNUSED s32 pad[3];
    u32 num_files;
    musConfig config;

    config.channels = 0x18;
    config.vvoices = 0x34;
    config.pvoices = 0x18;
    config.heap = gAudioHeap;
    config.heap_length = AUDIO_HEAP_SIZE;
    config.syn_updates = 0x100;
    config.syn_output_rate = 32000;
    config.syn_rsp_cmds = 0x440;
    config.syn_retraceCount = 1;
    config.syn_num_dma_bufs = 0x30;
    config.syn_dma_buf_size = 0x400;
    config.command_queue_max_size = 0x400;

    Audio_InitializeVoiceManager(&config);
    Rom_InitDevice(2);

    gMoveSoundTempBuffer = alHeapAlloc(audio_heap, 1, 0x61A8);
    gAudioHeaderTemp = alHeapAlloc(audio_heap, 8, 4);
    gAudioRegions = alHeapAlloc(audio_heap, 7, 4);

    // Copy rom data from 0x15C0000 into gAudioRegions
    Rom_DmaRead(&gAudioData, gAudioRegions, sizeof(u8*) * 7);
    // Add the address 0x15C0000 to every offset (besides the first which is the total size)
    // to make absolute offsets.
    Audio_RelocateOffsets(&gAudioRegions[1], &gAudioData, 6);
    sp78 = gAudioHeaderTemp;
    // Copy data from the offset at 0x15C0004 into sp78, a sub-header for this "archive,"
    // specifying the size of the header and number of files etc. Header at 0x15C0020
    Rom_DmaRead(gAudioRegions[1], sp78, 0xC);
    num_files = sp78->file1.num_files;
    // Allocate vram, copy rom bytes, and add address to make absolute,
    // for all of the first archive's files, including header.
    gSfxArchiveHeader = alHeapAlloc(audio_heap, (0xC / sizeof(u32)) + num_files, sizeof(u32));
    Rom_DmaRead(gAudioRegions[1], gSfxArchiveHeader, 0xC + (num_files * 4));
    // Update just the first 2 pointers from the header,
    // pointer to first file, and pointer to N64 wave tables.
    Audio_RelocateOffsets(&gSfxArchiveHeader->offset1, gAudioRegions[1], 2);
    // Make absolute all the offsets after the header, for the archive files at 0x15C002C
    Audio_RelocateOffsets(gSfxArchiveHeader->files, gAudioRegions[1], num_files);
    // Copy the first file into gMoveSoundTempBuffer. What is this?
    // Yay0 compressed at 0x15C0160
    Rom_DmaRead(gSfxArchiveHeader->offset1, gMoveSoundTempBuffer, gSfxArchiveHeader->wave_tables_offset - gSfxArchiveHeader->offset1);

    // Decompressed first file memory.
    gSfxWaveTable = alHeapAlloc(audio_heap, 1, 0x4120);

    // Decompress first file Yay0 from gMoveSoundTempBuffer, into gSfxWaveTable
    Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gSfxWaveTable, 0x4120);

    // ??
    Audio_RelocateSoundBankWrapper(gSfxWaveTable, gSfxArchiveHeader->wave_tables_offset);

    gSfxWaveTableBuffer = alHeapAlloc(audio_heap, 1, 0x98D8);

    // Load second archive header at 0x16F27E0
    Rom_DmaRead(gAudioRegions[2], sp78, 4 + 4);
    num_files = sp78->seq.seqCount;
    // Memory to hold archive toc
    gMoveSoundSeqInfo = alHeapAlloc(audio_heap, 1, 4 + (num_files * sizeof(ALSeqData)));
    // Read the alSeqFile into gMoveSoundSeqInfo
    Rom_DmaRead(gAudioRegions[2], gMoveSoundSeqInfo, 4 + (num_files * sizeof(ALSeqData)));
    // Initialise the memory as an alSeqFile
    alSeqFileNew(gMoveSoundSeqInfo, gAudioRegions[2]);

    // Read the first archive sub header from 0x16F2804, into gSfxStandardSubHeader
    gSfxStandardSubHeader = alHeapAlloc(audio_heap, 3, 4);
    Rom_DmaRead(gMoveSoundSeqInfo->seqArray[0].offset, gSfxStandardSubHeader, sizeof(File2SubHeader1));
    Audio_RelocateOffsets(gSfxStandardSubHeader, gMoveSoundSeqInfo->seqArray[0].offset, sizeof(File2SubHeader1) / sizeof(u32));

    // Read the first file from the sub header into gMoveSoundTempBuffer (0x16F2814)
    Rom_DmaRead(gSfxStandardSubHeader->offset1, gMoveSoundTempBuffer, gSfxStandardSubHeader->offset2 - gSfxStandardSubHeader->offset1);
    // Decompress the yay0 file into gSfxTable
    gSfxTable = alHeapAlloc(audio_heap, 1, 0xBB8);
    Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gSfxTable, 0xBB8);
    // ???
    Audio_RelocateSoundBankTable(gSfxTable);

    // Read the second file from the sub header into gMoveSoundTempBuffer (0x16F2BD0)
    Rom_DmaRead(gSfxStandardSubHeader->offset2, gMoveSoundTempBuffer, gSfxStandardSubHeader->wave_tables_offset - gSfxStandardSubHeader->offset2);
    // Decompress the yay0 file into gSfxBankStandard
    gSfxBankStandard = alHeapAlloc(audio_heap, 1, 0x157C);
    Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gSfxBankStandard, 0x157C);
    // ???
    Audio_RelocateSoundBankWrapper(gSfxBankStandard, gSfxStandardSubHeader->wave_tables_offset);

    // Read archive 2 sub file 2 (0x1722F74)
    Rom_DmaRead(gMoveSoundSeqInfo->seqArray[1].offset, sp78, sizeof(File2SubHeader2));
    num_files = sp78->file2sub2.num_files;

    gSfxSecondarySubHeader = alHeapAlloc(audio_heap, 4, (sizeof(File2SubHeader2) / sizeof(u32)) + num_files);
    Rom_DmaRead(gMoveSoundSeqInfo->seqArray[1].offset, gSfxSecondarySubHeader, sizeof(File2SubHeader2) + (num_files * sizeof(u32)));
    Audio_RelocateOffsets(&gSfxSecondarySubHeader->offset1, gMoveSoundSeqInfo->seqArray[1].offset, num_files + 2);
    Rom_DmaRead(gSfxSecondarySubHeader->offset1, gMoveSoundTempBuffer, gSfxSecondarySubHeader->wave_tables_offset - gSfxSecondarySubHeader->offset1);
    // Decompress the yay0 file into gSfxTable
    gSfxBankSecondary = alHeapAlloc(audio_heap, 1, 0x2EE0);
    Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gSfxBankSecondary, 0x2EE0);
    // ???
    Audio_RelocateSoundBankWrapper(gSfxBankSecondary, gSfxSecondarySubHeader->wave_tables_offset);

    gSfxBankScratch[0] = alHeapAlloc(audio_heap, 1, 0x258);
    gSfxBankScratch[1] = alHeapAlloc(audio_heap, 1, 0x258);
    gSfxBankScratch[2] = alHeapAlloc(audio_heap, 1, 0x258);

    // Read the second seq array (0x17C9C24)
    Rom_DmaRead(gMoveSoundSeqInfo->seqArray[2].offset, sp78, sizeof(File2SubHeader2));
    num_files = sp78->file2sub2.num_files;

    gMoveSoundArchive = alHeapAlloc(audio_heap, 4, num_files + 3);
    Rom_DmaRead(gMoveSoundSeqInfo->seqArray[2].offset, gMoveSoundArchive, sizeof(File2SubHeader2) + (num_files * sizeof(u32)));
    Audio_RelocateOffsets(&gMoveSoundArchive->offset1, gMoveSoundSeqInfo->seqArray[2].offset, num_files + 2);
    Rom_DmaRead(gMoveSoundArchive->offset1, gMoveSoundTempBuffer, gMoveSoundArchive->wave_tables_offset - gMoveSoundArchive->offset1);
    gMoveSoundBankBase = alHeapAlloc(audio_heap, 1, 0x9C4);
    // Decompress
    Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gMoveSoundBankBase, 0x9C4);
    Audio_RelocateSoundBankWrapper(gMoveSoundBankBase, gMoveSoundArchive->wave_tables_offset);

    gMoveSoundBankData[0] = alHeapAlloc(audio_heap, 1, 0x3E8);
    gMoveSoundBankData[1] = alHeapAlloc(audio_heap, 1, 0x3E8);
    gMoveSoundBankData[2] = alHeapAlloc(audio_heap, 1, 0x3E8);

    // Read seq header from 0x17DC304
    Rom_DmaRead(gMoveSoundSeqInfo->seqArray[3].offset, sp78, 4 + 4);
    num_files = sp78->seq.seqCount;

    gAudioSfxBankSeqHeader = alHeapAlloc(audio_heap, 4, 4 + (num_files * sizeof(ALSeqData)));
    Rom_DmaRead(gMoveSoundSeqInfo->seqArray[3].offset, gAudioSfxBankSeqHeader, 4 + (num_files * sizeof(ALSeqData)));
    alSeqFileNew(gAudioSfxBankSeqHeader, gMoveSoundSeqInfo->seqArray[3].offset);

    // Load second seq header from 0x1850C20
    Rom_DmaRead(gAudioSfxBankSeqHeader->seqArray[1].offset, sp78, 4 + 4);
    num_files = sp78->seq.seqCount;

    gAudioSeqHeaderNested = alHeapAlloc(audio_heap, 4, 4 + (num_files * sizeof(ALSeqData)));
    Rom_DmaRead(gAudioSfxBankSeqHeader->seqArray[1].offset, gAudioSeqHeaderNested, 4 + (num_files * sizeof(ALSeqData)));
    alSeqFileNew(gAudioSeqHeaderNested, gAudioSfxBankSeqHeader->seqArray[1].offset);

    gAudioSfxBank = alHeapAlloc(audio_heap, 1, 0x1388);
    gAudioSfxInstBank = alHeapAlloc(audio_heap, 1, 0xBB8);

    // load third archive from the main list, archive is at 0x1978820
    Rom_DmaRead(gAudioRegions[3], sp78, 4 + 4);
    num_files = sp78->seq.seqCount;

    gMoveSoundSeqTable = alHeapAlloc(audio_heap, 1, 4 + (num_files * sizeof(ALSeqData)));
    Rom_DmaRead(gAudioRegions[3], gMoveSoundSeqTable, 4 + (num_files * sizeof(ALSeqData)));
    alSeqFileNew(gMoveSoundSeqTable, gAudioRegions[3]);

    // load fourth archive from the main list, archive is at 0x197C1E0
    Rom_DmaRead(gAudioRegions[4], sp78, 4 + 4);
    num_files = sp78->seq.seqCount;

    gAudioSeqBank = alHeapAlloc(audio_heap, 1, 4 + (num_files * sizeof(ALSeqData)));
    Rom_DmaRead(gAudioRegions[4], gAudioSeqBank, 4 + (num_files * sizeof(ALSeqData)));
    alSeqFileNew(gAudioSeqBank, gAudioRegions[4]);

    gMoveSoundTables[0] = alHeapAlloc(audio_heap, 1, 0x44C);
    gMoveSoundTables[1] = alHeapAlloc(audio_heap, 1, 0x44C);
    gMoveSoundTables[2] = alHeapAlloc(audio_heap, 1, 0x44C);

    // load the fourth archive's first seq, at 0x197C1FC
    Rom_DmaRead(gAudioSeqBank->seqArray[0].offset, sp78, 4 + 4);
    num_files = sp78->seq.seqCount;
    gAudioSeqBuffer = alHeapAlloc(audio_heap, 1, 4 + (num_files * sizeof(ALSeqData)));
    Rom_DmaRead(gAudioSeqBank->seqArray[0].offset, gAudioSeqBuffer, 4 + (num_files * sizeof(ALSeqData)));
    alSeqFileNew(gAudioSeqBuffer, gAudioSeqBank->seqArray[0].offset);

    gAudioSeqLoadPending = 0;

    // load the fourth archive's third seq at 0x1A2AD20
    Rom_DmaRead(gAudioSeqBank->seqArray[2].offset, sp78, 4 + 4);
    num_files = sp78->seq.seqCount;
    gAudioSeqData = alHeapAlloc(audio_heap, 1, 4 + (num_files * sizeof(ALSeqData)));
    Rom_DmaRead(gAudioSeqBank->seqArray[2].offset, gAudioSeqData, 4 + (num_files * sizeof(ALSeqData)));
    alSeqFileNew(gAudioSeqData, gAudioSeqBank->seqArray[2].offset);

    gMusicFadeBuffer = alHeapAlloc(audio_heap, 0xB80, 2);

    // load the 5th main archive at 0x1FBA260
    gAudioSfxBankExtra = alHeapAlloc(audio_heap, 0x1388, 1);
    Rom_DmaRead(gAudioRegions[5], gAudioSfxBankExtra, gAudioRegions[6] - gAudioRegions[5]);

    // load the 6th main archive at 0x1FBB220
    gAudioSeqTOC = alHeapAlloc(audio_heap, 0x100, 1);
    Rom_DmaRead(gAudioRegions[6], gAudioSeqTOC, 0x100);

    for (i = 0; i < 32; i++) {
        gAudioSeqTOC[i].unk_00 += gAudioRegions[6];
    }

    gAudioSfxDataBuffer = alHeapAlloc(audio_heap, 0x2328, 1);

    Audio_SetChannelVolume(2, 2);
    Audio_SetChannelVolume(1, 3);

    gAudioDMAFlags = 0;

    return audio_heap->cur - audio_heap->base;
}

static s32 pad_D_80077C80 = 0;
