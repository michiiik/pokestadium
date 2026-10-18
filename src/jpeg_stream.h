#ifndef _JPEG_STREAM_H_
#define _JPEG_STREAM_H_

#include "global.h"
#include "scheduler.h"
#include "jpegutils.h"
#include "jpeg_decoder.h"
#include "src/memmap.h"
#include "src/memory.h"

typedef struct ret_func_8000484C {
	/* 0x00 */ char unk00[0x90];
	/* 0x90 */ s32 unk_90;
} ret_func_8000484C; // size >= 0x94

typedef struct PERSSZP {
    /* 0x00 */ char magic[0x8]; // PERS-SZP
    /* 0x08 */ u32 header_size;
    /* 0x0C */ u32 decompressed_size1; // How do these differ?
    /* 0x10 */ u32 decompressed_size2;
    /* 0x14 */ u32 unk_14;
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
            /* 0x00 */ u16 unk_00;
            /* 0x02 */ u16 unk_02;
        };
        s32 raw;
    };
    /* 0x04 */ u32 unk_04;
    /* 0x08 */ u32 total_size;
    /* 0x0C */ u32 num_files;
} BinArchive; // size = 0x10

typedef struct BinArchiveFile {
    /* 0x00 */ u32 offset;
    /* 0x04 */ u32 size;
    /* 0x08 */ u32 unk_08;
    /* 0x0C */ char unk0C[0x4];
} BinArchiveFile; // size = 0x10

typedef struct unk_func_800041C0 {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
} unk_func_800041C0; // size >= 0x8

typedef struct unk_func_80003680_sp300 {
    /* 0x00 */ u8 unk_00; // mode
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ char unk02[0x2];
    /* 0x04 */ s16 width;
    /* 0x06 */ s16 height;
    /* 0x08 */ u8* quantization_table;
    /* 0x0C */ u8* huffman_table;
    /* 0x10 */ u8* start_of_frame;
    /* 0x14 */ u8* start_of_scan;
} unk_func_80003680_sp300; // size = 0x18

typedef struct unk_func_80003680_sp90 {
    // PORT64: the host JPEG task (src/port/jpeg_task.cpp) reads these as pointer-sized fields (JpegTaskData layout)
    /* 0x00 */ uintptr_t unk_00; // data/output address
    /* 0x04 */ uintptr_t unk_04; // macroblock count
    /* 0x08 */ uintptr_t unk_08; // mode (0 = 4:2:2, 2 = 4:2:0)
    /* 0x0C */ uintptr_t unk_0C; // Y quantization table
    /* 0x10 */ uintptr_t unk_10; // U quantization table
    /* 0x14 */ uintptr_t unk_14; // V quantization table
} unk_func_80003680_sp90; // size >= 0x18

typedef void (*ret_func_80004454)(void);

s32 Jpeg_ReadBigEndianU16(u8* arg0);
void Jpeg_ParseStreamMarkers(unk_func_80003680_sp300* arg0, u8* arg1);
void Jpeg_InitializeDspTask(UnkStruct80001380* arg0, unk_func_80003680_sp90* arg1);
void Jpeg_BuildDspInput(JpegDecoder* arg0, JpegHuffmanTable* arg1, unk_func_80003680_sp300* arg2);
void Jpeg_BuildDspOutput(unk_func_80003680_sp90* arg0, u8(arg1)[2][0x80], unk_func_80003680_sp300* arg2);
s32 Jpeg_DecodeImage(uintptr_t addr, s32 arg1, u8* arg2);
void Dma_InitializeCompletionQueue(void);
void Yay0_DecompressAndRelocate(u8* in_header, u8* memory);
void Dma_ReadChunks(u8* arg0, s32 arg1, s32 arg2, s32 arg3);
s32 Flash_ProgramPages(u8* arg0, s16 arg1, s16 arg2, s32 arg3);
s32 Flash_EraseSectorByIndex(s16 arg0);
void Dma_WriteChunks(uintptr_t arg0, uintptr_t arg1, uintptr_t arg2, s32 arg3);
void Dma_CopyChunks(u8* arg0, u8* arg1, u8* arg2);
void* Jpeg_LoadDecodedImage(u8* addr, PRESJPEG* arg1, s32 side);
void* Yay0_LoadDecompressedAsset(intptr_t arg0, PERSSZP* arg1, s32 side);
u32* Asset_CopyUncompressed(u8* arg0, s32 arg1, u32 arg2, s32 arg3);
void* Asset_LoadCompressed(u8* romStart, u8* romEnd, s32 arg2, s32 arg3);
void* Storage_LoadAssetByLbaRange(unk_func_800041C0* arg0, s32 arg1);
unk_func_800041C0* Storage_SetLbaRange(unk_func_800041C0* arg0, s32 arg1, s32 arg2);
unk_func_800041C0* Storage_SetByteRange(unk_func_800041C0* arg0, u32 arg1, u32 offset, u32 size);
void Storage_PrefetchLbaRange(s32 arg0, s32 arg1, s32 arg2);
void Memmap_ClearSegmentOnBlockFree(uintptr_t block_addr, u32 addr);
u8* Asset_LoadToSegment(s32 id, u8* rom_start, u8* rom_end, s32 arg3);
MainPoolBlock* Asset_LoadToSegmentByLbaRange(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Fragment_FreeCallback(uintptr_t base_addr, u32 addr);
void Fragment_Register(s32 arg0, Fragment* addr);
ret_func_80004454 Fragment_Load(s32 arg0, u8* romStart, u8* romEnd);
void* Fragment_LoadByLbaRange(s32 arg0, s32 arg1, s32 arg2);
BinArchive* BinArchive_Open(u8* romStart, u8* romEnd, s32 arg2, s32 arg3);
void* BinArchive_LoadByLbaRange(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void BinArchive_Free(void* arg0);
Fragment* BinArchive_LoadFileFromRom(BinArchive* arg0, BinArchiveFile* arg1);
void* BinArchive_LoadFileBuffered(BinArchive* arg0, BinArchiveFile* arg1);
void* BinArchive_GetFile(BinArchive* archive, s32 file_number);
void* Port_ResolveCode(uintptr_t addr);
uintptr_t Port_CallFragmentLeaf(const void* code, s32 a0, s32 a1);
void Port_NoteLoadedFragment(u32 slot, u8* romStart);
extern int gPortRelocateDataBlob;
void Port_FixPersHeader(PERSSZP* h);
void Port_FixPresJpegHeader(PRESJPEG* h);
void* BinArchive_GetCachedFile(BinArchive* archive, s32 file_number);
u32 Port_PtrToHandle(void* p);
void* Port_HandleToPtr(u32 h);

#endif // _JPEG_STREAM_H_
