#include "gsm_decoder.h"
#include "libgsm/libgsm.h"

typedef struct unk_arg1_func_80045D60 {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u8 unk6;
} unk_arg1_func_80045D60; // size 0x7

#define ROUND_Q15 0x4000

#define STEP(x, off, k, scale) \
    ((s16) ((((s32) (((((x) + (off)) << 10) + (k)) * (scale)) + ROUND_Q15) >> 15) * 2))

void Decoding_of_the_coded_Log_Area_Ratios(gsm_state* state, s16 LARc[8], s16 drp[40], s16 signal[160]) {
    s16* LARpp;

    LARpp = &state->LARpp[state->frame_index];
    LARpp[0] = STEP(LARc[0], -32,     0, 13107);
    LARpp[1] = STEP(LARc[1], -32,     0, 13107);
    LARpp[2] = STEP(LARc[2], -16, -4096, 13107);
    LARpp[3] = STEP(LARc[3], -16,  5120, 13107);
    LARpp[4] = STEP(LARc[4],  -8,  -188, 19223);
    LARpp[5] = STEP(LARc[5],  -8,  3584, 17476);
    LARpp[6] = STEP(LARc[6],  -4,   682, 31454);
    LARpp[7] = STEP(LARc[7],  -4,  2288, 29708);

    Gsm_Short_Term_Synthesis_Filter(state, drp, signal);
    state->frame_index ^= 1;
}

void Gsm_Decoder(gsm_state* state, s16* LARc, s16* Ncr, s16* bcr, s16* Mcr, s16* xmaxcr, xMc* xMcr, s16 signal[160]) {
    s32 i;
    s16 erp[40];
    
    for (i = 0; i < 4; i++) {
        Gsm_RPE_Decoding(*xmaxcr, *Mcr, xMcr, erp);
        Gsm_Long_term_synthesis_filtering(state, *Ncr, *bcr, erp);
        xmaxcr++;
        bcr++;
        Ncr++;
        Mcr++;
        xMcr++;
    }

    Decoding_of_the_coded_Log_Area_Ratios(state, LARc, state, signal);
}

// Unused
s32 Gsm_Decode(gsm_state* state, unk_arg1_func_80045D60* arg1, s16 signal[160]) {
    s16 LARpp[8];
    s16 spC8[4];
    s16 spC0[4];
    s16 spB8[4];
    s16 spB0[4];
    xMc sp48[4];
    s32 i;

    LARpp[0]= ((arg1[0].unk0 & 0xF) << 2) | ((arg1[0].unk1 >> 6) & 3);
    LARpp[1] = arg1[0].unk1 & 0x3F;
    LARpp[2] = (arg1[0].unk2 >> 3) & 0x1F;
    LARpp[3] = ((arg1[0].unk2 & 7) << 2) | ((arg1[0].unk3 >> 6) & 3);
    LARpp[4] = (arg1[0].unk3 >> 2) & 0xF;
    LARpp[5] = ((arg1[0].unk3 & 3) << 2) | ((arg1[0].unk4 >> 6) & 3);
    LARpp[6] = (arg1[0].unk4 >> 3) & 7;
    LARpp[7] = arg1[0].unk4 & 7;
    for (i = 0; i < 4; i++)  {
        spC8[i] = (arg1[i].unk5 >> 1) & 0x7F;
        spB8[i] = ((arg1[i].unk5 & 1) << 1) | ((arg1[i].unk6 >> 7) & 1);
        spC0[i] = (arg1[i].unk6 >> 5) & 3;
        spB0[i] = ((arg1[i].unk6 & 0x1F) << 1) | ((arg1[i + 1].unk0 >> 7) & 1);

        sp48[i].xRotA = (arg1[i + 1].unk0 >> 4) & 7;
        sp48[i].xRotB = (arg1[i + 1].unk0 >> 1) & 7;
        sp48[i].flag0 = ((arg1[i + 1].unk0 & 1) << 2) | ((arg1[i + 1].unk1 >> 6) & 3);
        sp48[i].yRotA = (arg1[i + 1].unk1 >> 3) & 7;
        sp48[i].yRotB = arg1[i + 1].unk1 & 7;
        sp48[i].flag1 = (arg1[i + 1].unk2 >> 5) & 7;
        sp48[i].zRotA = (arg1[i + 1].unk2 >> 2) & 7;
        sp48[i].zRotB = ((arg1[i + 1].unk2 & 3) << 1) | ((arg1[i + 1].unk3 >> 7) & 1);
        sp48[i].flag2 = (arg1[i + 1].unk3 >> 4) & 7;
        sp48[i].unkA = (arg1[i + 1].unk3 >> 1) & 7;
        sp48[i].unkB = ((arg1[i + 1].unk3 & 1) << 2) | ((arg1[i + 1].unk4 >> 6) & 3);
        sp48[i].unkC = (arg1[i + 1].unk4 >> 3) & 7;
        sp48[i].unkD = arg1[i + 1].unk4 & 7;
    }
    Gsm_Decoder(state, &LARpp, spC8, spB8, spC0, spB0, sp48, signal);
    return 0;
}

// Gsm_Decode_Stream
#pragma GLOBAL_ASM("asm/us/nonmatchings/gsm_decoder/func_80045FF0.s")
