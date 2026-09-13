#ifndef _CE80_H_
#define _CE80_H_

#include "global.h"
#include "jpegutils.h"

typedef struct unk_func_80003680_sp27 {
    /* 0x00 */ s32 unk_00; // Jpeg_DecodeMcus: cast to u8* for the bit-reader's inputBuffer
    /* 0x04 */ u8 chromaSubsamplingMode; // 0 = 2 luma blocks/MCU, else 4 luma blocks/MCU
    /* 0x05 */ u8 subsamplingVariant; // ==1 adds a 0x80-byte row stride between MCUs
    /* 0x08 */ JpegHuffmanTable* dcLumaTable;
    /* 0x0C */ JpegHuffmanTable* acLumaTable;
    /* 0x10 */ JpegHuffmanTable* dcChromaTable;
    /* 0x14 */ JpegHuffmanTable* acChromaTable;
    /* 0x18 */ s8 unk_18;
} unk_func_80003680_sp27; // size == 0x1C

typedef struct unk_func_80003680_sp60 {
    /* 0x00 */ s32 readOffset; // Jpeg_ReadBits's byte position into the input buffer
    /* 0x04 */ u8 bitCount; // bits currently available in bitAccumulator
    /* 0x05 */ u8 lastByteWasFF; // 0xFF00 byte-stuffing escape state
    /* 0x08 */ s32 bitAccumulator;
    /* 0x0C */ s16 lumaDcPred; // previous luma DC coefficient, carried across MCUs
    /* 0x0E */ s16 chromaDcPred1; // previous Cb DC coefficient
    /* 0x10 */ s16 chromaDcPred2; // previous Cr DC coefficient
} unk_func_80003680_sp60; // size = 0x14

s32 Jpeg_DecodeMcus(unk_func_80003680_sp27*, u32, u32, u8, unk_func_80003680_sp60*);
s32 Jpeg_DecodeBlock(JpegHuffmanTable*, JpegHuffmanTable*, s16*, s16*);
s32 Jpeg_DecodeHuffmanSymbol(JpegHuffmanTable*, s16*, s8*);
u16 Jpeg_ReadBits(u8);

#endif // _CE80_H_
