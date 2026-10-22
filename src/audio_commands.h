#ifndef _373A0_H_
#define _373A0_H_

#include "global.h"

#ifdef M2C
#define ENTRY_SIZE 1
#else
#define ENTRY_SIZE
#endif

typedef struct File1Header {
    /* 0x00 */ u32 offset1;
    /* 0x04 */ u32 wave_tables_offset;
    /* 0x08 */ u32 num_files;
    /* 0x0C */ u32 files[1];
} File1Header; // size >= 0x10

typedef struct File2SubHeader1 {
    /* 0x00 */ u32 offset1;
    /* 0x04 */ u32 offset2;
    /* 0x08 */ u32 wave_tables_offset;
} File2SubHeader1; // size = 0xC

typedef struct File2SubHeader2 {
    /* 0x00 */ u32 num_files;
    /* 0x04 */ u32 offset1;
    /* 0x08 */ u32 wave_tables_offset;
} File2SubHeader2; // size = 0xC

typedef union {
    File1Header file1;
    ALSeqFile seq;
    File2SubHeader2 file2sub2;
} FileHeaders;

typedef struct unk_D_800FC6CC {
    /* 0x00 */ u32 unk_00;
    /* 0x04 */ u32 unk_04;
} unk_D_800FC6CC; // size = 0x8

typedef struct unk_D_800FC6D4_entry {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
} unk_D_800FC6D4_entry; // size = 0x4

typedef struct unk_D_800FC6D4_008 {
    /* 0x00 */ u32 unk_00;
    /* 0x04 */ u8 unk_04;
} unk_D_800FC6D4_008; // size = 0x5

typedef struct GbTrackHeader {
    /* 0x00 */ u16 flags;
    /* 0x02 */ u16 num_entries;
    /* 0x04 */ u16 unk_04;
    /* 0x06 */ u16 unk_06;
    /* 0x08 */ unk_D_800FC6D4_008* offset;
    /* 0x0C */ char unk_0C[0x4];
    /* 0x10 */ unk_D_800FC6D4_entry entries[ENTRY_SIZE];
} GbTrackHeader; // size = 0x10

typedef struct MoveSoundEntry { u8 unk0[0x6]; } MoveSoundEntry;
typedef struct MoveSoundTable { u8 header[0x10]; MoveSoundEntry entries[1]; } MoveSoundTable;

extern u8 gAudioHeap[0x493E0];
extern u32* gSfxWaveTableBuffer;
extern u8* gSfxWaveTable;
extern u8* gSfxTable;
extern u8* gSfxBankStandard;
extern u8* gSfxBankSecondary;
extern u8* gSfxBankScratch[3];
extern u8* gAudioSfxBank;
extern u8* gAudioSfxInstBank;
extern u8* gMoveSoundBankBase;
extern u8* gMoveSoundBankData[3];
extern MoveSoundTable* gMoveSoundTables[3];
extern unk_D_800FC6CC* gAudioSeqTOC;
extern u8* gAudioSfxDataBuffer;
extern GbTrackHeader* gAudioSfxBankExtra;
extern s16* gMusicFadeBuffer;
extern u8* gMoveSoundTempBuffer;
extern u32* gAudioRegions;
extern File1Header* gSfxArchiveHeader;
extern ALSeqFile* gMoveSoundSeqInfo;
extern File2SubHeader1* gSfxStandardSubHeader;
extern File2SubHeader2* gSfxSecondarySubHeader;
extern ALSeqFile* gAudioSfxBankSeqHeader;
extern ALSeqFile* gAudioSeqHeaderNested;
extern File2SubHeader2* gMoveSoundArchive;
extern ALSeqFile* gMoveSoundSeqTable;
extern ALSeqFile* gAudioSeqBank;
extern ALSeqFile* gAudioSeqBuffer;
extern ALSeqFile* gAudioSeqData;
extern u32* gAudioHeaderTemp;

s32 Audio_PlayCommand(u32 command, s32 parameter, UNUSED s32 unused);
s32 Audio_FadeCommand(s32 arg0, u32 arg1);
void Audio_NullTask(void);
void Audio_CreateFrameTaskWrapper(void* arg0);
void Audio_RelocateOffsets(u32* arg0, u32 arg1, s32 arg2);
s32 Audio_InitBanks(void);

#endif // _373A0_H_
