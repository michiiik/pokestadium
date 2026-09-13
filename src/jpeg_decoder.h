#ifndef _3FB0_H_
#define _3FB0_H_

#include "global.h"
#include "scheduler.h"
#include "jpegutils.h"
#include "jpeg_mcus.h"
#include "src/memmap.h"
#include "src/memory.h"

typedef struct ret_func_8000484C {
	/* 0x00 */ char unk00[0x90];
	/* 0x90 */ s32 glyphTable; // Font_LoadSet: &fontAsset->glyphTable stored as this font set's glyph-table pointer
} ret_func_8000484C; // size >= 0x94

typedef struct PERSSZP {
    /* 0x00 */ char magic[0x8]; // PERS-SZP
    /* 0x08 */ u32 header_size;
    /* 0x0C */ u32 decompressed_size1; // How do these differ?
    /* 0x10 */ u32 decompressed_size2;
    /* 0x14 */ u32 relocationCount; // Yay0_DecompressAndRelocate: (value, offset) pair count following the header
} PERSSZP; // sze = 0x18

typedef struct PRESJPEG {
    /* 0x00 */ char magic[0x8]; // PRESJPEG
    /* 0x08 */ u32 unk_08; // header size? always 0x10
    /* 0x0C */ u32 unk_0C; 
} PRESJPEG; // size = 0x10

typedef struct Yay0 {
    /* 0x00 */ char magic[0x4]; // Yay0
    /* 0x04 */ u32 decompressed_size;
    /* 0x08 */ u32 link_table_offset;
    /* 0x0C */ u32 byte_chunk_offset;
} Yay0; // size = 0x10

typedef struct BinArchive {
    union {
        struct {
            /* 0x00 */ u16 flags; // bit0=preloaded-flat, bit1=lazy/two-pass load, bit2=arg3 variant, bit7=buffered load
            /* 0x02 */ u16 fragmentModuleId; // BinArchive_Open's arg3; Fragment_Register's arg0 for FRAGMENT-tagged files
        };
        s32 raw;
    };
    /* 0x04 */ u32 romStart; // BinArchive_Open's romStart, kept for on-demand file loads
    /* 0x08 */ u32 total_size;
    /* 0x0C */ u32 num_files;
} BinArchive; // size = 0x10

typedef struct BinArchiveFile {
    /* 0x00 */ u32 offset;
    /* 0x04 */ u32 size;
    /* 0x08 */ u32 cachedPtr; // BinArchive_GetFile: cached loaded-file pointer, populated on first access
    /* 0x0C */ char unk0C[0x4];
} BinArchiveFile; // size = 0x10

typedef struct unk_func_800041C0 {
    /* 0x00 */ u16 startLba; // Storage_SetLbaRange's arg1
    /* 0x02 */ u16 lbaCount; // Storage_SetLbaRange: arg2 - arg1
    /* 0x04 */ s32 headerOffset; // Storage_LoadAssetByLbaRange: offset into the DMA'd buffer where the asset header begins
    /* 0x08 */ s32 byteSize; // LeoLBAToByte(startLba, lbaCount, ...) result
} unk_func_800041C0; // size >= 0x8

typedef struct unk_func_80003680_sp300 {
    /* 0x00 */ u8 chromaSubsamplingMode; // Jpeg_ParseStreamMarkers: 0/2 from the 2nd component's id byte; feeds Jpeg_BuildDspInput/Output
    /* 0x01 */ u8 hasRestartMarkers; // Jpeg_ParseStreamMarkers: set when a JPEG_MARKER_RESTARTn is seen
    /* 0x02 */ char unk02[0x2];
    /* 0x04 */ s16 width;
    /* 0x06 */ s16 height;
    /* 0x08 */ u8* quantization_table;
    /* 0x0C */ u8* huffman_table;
    /* 0x10 */ u8* start_of_frame;
    /* 0x14 */ u8* start_of_scan;
} unk_func_80003680_sp300; // size = 0x18

typedef struct unk_func_80003680_sp90 {
    /* 0x00 */ s32 unk_00; // Jpeg_BuildDspOutput: always 0
    /* 0x04 */ s32 unk_04; // Jpeg_BuildDspOutput: always 1
    /* 0x08 */ s32 chromaSubsamplingMode; // mirrors unk_func_80003680_sp300's chromaSubsamplingMode
    /* 0x0C */ s32 outputBufferA; // (u32)&arg1[0], segment-masked
    /* 0x10 */ s32 outputBufferB; // (u32)&arg1[1], segment-masked
    /* 0x14 */ s32 outputBufferC; // Jpeg_BuildDspOutput: also (u32)&arg1[1] - same value as outputBufferB
} unk_func_80003680_sp90; // size >= 0x18

typedef void (*ret_func_80004454)(void);

s32 Jpeg_ReadBigEndianU16(u8* arg0);
void Jpeg_ParseStreamMarkers(unk_func_80003680_sp300* arg0, u8* arg1);
void Jpeg_InitializeDspTask(UnkStruct80001380* arg0, unk_func_80003680_sp90* arg1);
void Jpeg_BuildDspInput(unk_func_80003680_sp27* arg0, JpegHuffmanTable* arg1, unk_func_80003680_sp300* arg2);
void Jpeg_BuildDspOutput(unk_func_80003680_sp90* arg0, u8(arg1)[2][0x80], unk_func_80003680_sp300* arg2);
s32 Jpeg_DecodeImage(u32 addr, s32 arg1, u8* arg2);
void Dma_InitializeCompletionQueue(void);
void Yay0_DecompressAndRelocate(u8* in_header, u8* memory);
void Dma_ReadChunks(u8* arg0, s32 arg1, s32 arg2, s32 arg3);
s32 Flash_ProgramPages(u8* arg0, s16 arg1, s16 arg2, s32 arg3);
s32 Flash_EraseSectorByIndex(s16 arg0);
void Dma_WriteChunks(u32 arg0, u32 arg1, u32 arg2, s32 arg3);
void Dma_CopyChunks(u8* arg0, u8* arg1, u8* arg2);
void* Jpeg_LoadDecodedImage(u8* addr, PRESJPEG* arg1, s32 side);
s32 Yay0_LoadDecompressedAsset(s32 arg0, PERSSZP* arg1, s32 side);
u32* Asset_CopyUncompressed(u8* arg0, s32 arg1, u32 arg2, s32 arg3);
void* Asset_LoadCompressed(u8* romStart, u8* romEnd, s32 arg2, s32 arg3);
void* Storage_LoadAssetByLbaRange(unk_func_800041C0* arg0, s32 arg1);
unk_func_800041C0* Storage_SetLbaRange(unk_func_800041C0* arg0, s32 arg1, s32 arg2);
unk_func_800041C0* Storage_SetByteRange(unk_func_800041C0* arg0, u32 arg1, u32 offset, u32 size);
void Storage_PrefetchLbaRange(s32 arg0, s32 arg1, s32 arg2);
void Memmap_ClearSegmentOnBlockFree(u32 block_addr, u32 addr);
u8* Asset_LoadToSegment(s32 id, u8* rom_start, u8* rom_end, s32 arg3);
MainPoolBlock* Asset_LoadToSegmentByLbaRange(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Fragment_FreeCallback(u32 base_addr, u32 addr);
void Fragment_Register(s32 arg0, Fragment* addr);
ret_func_80004454 Fragment_Load(s32 arg0, u8* romStart, u8* romEnd);
void* Fragment_LoadByLbaRange(s32 arg0, s32 arg1, s32 arg2);
BinArchive* BinArchive_Open(u8* romStart, u8* romEnd, s32 arg2, s32 arg3);
void* BinArchive_LoadByLbaRange(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void BinArchive_Free(void* arg0);
Fragment* BinArchive_LoadFileFromRom(BinArchive* arg0, BinArchiveFile* arg1);
void* BinArchive_LoadFileBuffered(BinArchive* arg0, BinArchiveFile* arg1);
void* BinArchive_GetFile(BinArchive* archive, s32 file_number);
s32 BinArchive_GetCachedFile(BinArchive* archive, s32 file_number);

#endif // _3FB0_H_
