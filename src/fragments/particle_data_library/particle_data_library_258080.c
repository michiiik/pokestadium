#include "particle_data_library.h"
#include "src/fragments/clefairy_game/clefairy_game.h"
#include "src/fragments/rattata_game/rattata_game.h"
#include "src/fragments/victory_palace/victory_palace.h"
#include "src/gfx_buffer.h"
#include "src/geo_render.h"
#include "src/battle_hud.h"

typedef struct {
    /* 0x00 */ u8 rgb0[3];      // start color
    /* 0x03 */ u8 frameStart;   // frame where this keyframe begins
    /* 0x04 */ u8 rgb1[3];      // end color
    /* 0x07 */ u8 frameEnd;     // frame where keyframe ends
} Keyframe;  // size = 0x08

extern Keyframe* D_8100419C[];
extern s32 D_810041D0[20];
extern char D_810047D0[3];
extern s32 D_81004B70;
extern s32 D_81004B80;
extern s32 D_81004B90;
extern s32 D_81004BA0;
extern s32 D_81004BB0;
extern char D_81004BB8[0x28];

#ifdef NON_MATCHING
void DisplayList_BuildScrollingIconSegment0(Gfx* gfx, s32 arg1, s32 arg2) {
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xD7000002;    _gfx->words.w1 = -1;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE3001001;    _gfx->words.w1 = 0x8000;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFD100000;    _gfx->words.w1 = arg1;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE8000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5000100;    _gfx->words.w1 = 0x07000000;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE6000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF0000000;    _gfx->words.w1 = 0x073FC000;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFD48001F;    _gfx->words.w1 = (((D_862082A0[0] * 4) & ~0x1F) + arg2 + ((D_81004B70 & 3) << 0xB));    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5480800;    _gfx->words.w1 = 0x07098250;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE6000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF4000000;    _gfx->words.w1 = 0x0707C0FC;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5480800;    _gfx->words.w1 = 0x98250;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF2000000;    _gfx->words.w1 = 0x7C0FC;    }
    gSPEndDisplayList(gfx++);
}
#else
void DisplayList_BuildScrollingIconSegment0(Gfx*, s32, s32);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/particle_data_library/particle_data_library_258080/DisplayList_BuildScrollingIconSegment0.s")
#endif

void DisplayList_InitScrollingIconSegment0(s32 arg0, DisplayListState* state) {
    Gfx* gfx;
    DisplayListAddresses* addresses;

    addresses = state->addresses;
    switch (arg0) {
    case 0:
        D_81004B70 = 0;
        return;
    case 2:
        gfx = Gfx_AllocDisplayList(0xA0);
        state->gfx = gfx;
        DisplayList_BuildScrollingIconSegment0(gfx, addresses->segments[0], addresses->segments[1]);
        D_81004B70 += 1;
        return;
    }
}

#ifdef NON_MATCHING
void DisplayList_BuildScrollingIconSegment1(Gfx* gfx, s32 arg1, s32 arg2) {
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xD7000002;    _gfx->words.w1 = -1;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE3001001;    _gfx->words.w1 = 0x8000;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFD100000;    _gfx->words.w1 = arg1;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE8000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5000100;    _gfx->words.w1 = 0x07000000;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE6000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF0000000;    _gfx->words.w1 = 0x073FC000;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFD48001F;    _gfx->words.w1 = (((D_862082A0[1] * 4) & ~0x1F) + arg2 + ((D_81004B80 & 3) << 11));    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5480800;    _gfx->words.w1 = 0x07098250;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE6000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF4000000;    _gfx->words.w1 = 0x0707C0FC;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5480800;    _gfx->words.w1 = 0x98250;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF2000000;    _gfx->words.w1 = 0x7C0FC;    }
    gSPEndDisplayList(gfx++);
}
#else
void DisplayList_BuildScrollingIconSegment1(Gfx*, s32, s32);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/particle_data_library/particle_data_library_258080/DisplayList_BuildScrollingIconSegment1.s")
#endif

void DisplayList_InitScrollingIconSegment1(s32 arg0, DisplayListState* state) {
    Gfx* gfx;
    DisplayListAddresses* addresses;

    addresses = state->addresses;
    switch (arg0) {                                 /* irregular */
    case 0:
        D_81004B80 = 0;
        return;
    case 2:
        gfx = Gfx_AllocDisplayList(0xA0);
        state->gfx = gfx;
        DisplayList_BuildScrollingIconSegment1(gfx, addresses->segments[0], addresses->segments[1]);
        D_81004B80 += 1;
        return;
    }
}

#ifdef NON_MATCHING
void DisplayList_BuildScrollingIconSegment2(Gfx* gfx, s32 arg1, s32 arg2) {
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xD7000002;    _gfx->words.w1 = -1;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE3001001;    _gfx->words.w1 = 0x8000;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFD100000;    _gfx->words.w1 = arg1;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE8000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5000100;    _gfx->words.w1 = 0x07000000;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE6000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF0000000;    _gfx->words.w1 = 0x073FC000;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFD48001F;    _gfx->words.w1 = ((((D_862082A0[2]) * 4) & ~0x1F) + arg2 + ((D_81004B90 & 3) << 0xB));    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5480800;    _gfx->words.w1 = 0x07098250;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE6000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF4000000;    _gfx->words.w1 = 0x0707C0FC;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5480800;    _gfx->words.w1 = 0x98250;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF2000000;    _gfx->words.w1 = 0x7C0FC;    }
    gSPEndDisplayList(gfx++);
}
#else
void DisplayList_BuildScrollingIconSegment2(Gfx*, s32, s32);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/particle_data_library/particle_data_library_258080/DisplayList_BuildScrollingIconSegment2.s")
#endif

void DisplayList_InitScrollingIconSegment2(s32 arg0, DisplayListState* state) {
    Gfx* gfx;
    DisplayListAddresses* addresses;

    addresses = state->addresses;
    switch (arg0) {                                 /* irregular */
    case 0:
        D_81004B90 = 0;
        return;
    case 2:
        gfx = Gfx_AllocDisplayList(0xA0);
        state->gfx = gfx;
        DisplayList_BuildScrollingIconSegment2(gfx, addresses->segments[0], addresses->segments[1]);
        D_81004B90 += 1;
        return;
    }
}

#ifdef NON_MATCHING
void DisplayList_BuildScrollingIconSegment3(Gfx* gfx, s32 arg1, s32 arg2) {
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xD7000002;    _gfx->words.w1 = -1;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE3001001;    _gfx->words.w1 = 0x8000;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFD100000;    _gfx->words.w1 = arg1;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE8000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5000100;    _gfx->words.w1 = 0x07000000;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE6000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF0000000;    _gfx->words.w1 = 0x073FC000;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFD48001F;    _gfx->words.w1 = ((((D_862082A0[3]) * 4) & ~0x1F) + arg2 + ((D_81004BA0 & 3) << 0xB));    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5480800;    _gfx->words.w1 = 0x07098250;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE6000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF4000000;    _gfx->words.w1 = 0x0707C0FC;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5480800;    _gfx->words.w1 = 0x98250;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF2000000;    _gfx->words.w1 = 0x7C0FC;    }
    gSPEndDisplayList(gfx++);
}
#else
void DisplayList_BuildScrollingIconSegment3(Gfx*, s32, s32);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/particle_data_library/particle_data_library_258080/DisplayList_BuildScrollingIconSegment3.s")
#endif

void DisplayList_InitScrollingIconSegment3(s32 arg0, DisplayListState* state) {
    Gfx* gfx;
    DisplayListAddresses* addresses;

    addresses = state->addresses;
    switch (arg0) {                                 /* irregular */
    case 0:
        D_81004BA0 = 0;
        return;
    case 2:
        gfx = Gfx_AllocDisplayList(0xA0);
        state->gfx = gfx;
        DisplayList_BuildScrollingIconSegment3(gfx, addresses->segments[0], addresses->segments[1]);
        D_81004BA0 += 1;
        return;
    }
}

#ifdef NON_MATCHING
// Matching, needs more data
void DisplayList_InitPulsingRectSegment(s32 arg0, DisplayListState* state) {
    static u8 D_81004190 = 0;
    s32 temp_a1;
    s32 temp_a2;
    Gfx* gfx;

    if (arg0 == 2) {
        gfx = Gfx_AllocDisplayList(0x10);
        temp_a1 = D_81004190 * 3;
        temp_a2 = D_81004190 * -6;
        state->gfx = gfx;
        {    Gfx *_gfx = (Gfx *)(gfx++); _gfx->words.w0 = (((temp_a1 & 0xFFF) << 0xC) | 0xF2000000 | (temp_a2 & 0xFFF));    _gfx->words.w1 = ((((0x27F << (temp_a1 + 2)) & 0xFFF) << 0xC) | ((0x7F << (temp_a2 + 2)) & 0xFFF));    }
        gSPEndDisplayList(gfx++);
        D_81004190++;
        D_81004190 &= 0x7F;
    }
}
#else
void DisplayList_InitPulsingRectSegment(s32, DisplayListState*);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/particle_data_library/particle_data_library_258080/DisplayList_InitPulsingRectSegment.s")
#endif

#ifdef NON_MATCHING
void DisplayList_InitColorCycleBoxSegment(s32 arg0, DisplayListState* state) {
    static u8 D_81004194 = 0;
    static u8 D_81004198 = 0;
    Keyframe* kf;
    Gfx* gfx;
    u8 rgbInterp[3];
    f32 var_ft2;
    s32 temp_a3_2;
    s32 temp_t1_2;
    s32 i;
    s32 c;
    u8 start;
    u8 span;
    u8 delta;
    u8 t;
    
    if (arg0 == 2) {
        gfx = Gfx_AllocDisplayList(0x20);
        state->gfx = gfx;
        for (i = 0; i < 5; i++) {
            kf = D_8100419C[i];
            if (D_81004198 == kf->frameStart) {
                for(c = 0; c < 3; c++) {
                    rgbInterp[c] = kf->rgb0[c];
                }
            } else if (kf->frameStart < D_81004198 && D_81004198 < kf->frameEnd) {
                span = (kf->frameEnd - kf->frameStart);
                t = D_81004198 - kf->frameStart;
                for(c = 0; c < 3; c++) {
                    start = kf->rgb0[c];
                    var_ft2 = (f32) start;
                    if ((s32) start < 0) {
                        var_ft2 += 4294967296.0f;
                    }
                    delta = (kf->rgb1[c] - start);
                    rgbInterp[c] = (var_ft2 + ((f32) (delta * t) / (f32) span) + 0.5f);
                }
            }
        }
        temp_a3_2 = D_81004194 * -2;
        temp_t1_2 = D_81004194 * 2;
        // set prim color?
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFA000080;    _gfx->words.w1 = ((rgbInterp[2] << 8) | (rgbInterp[1] << 0x18) | (rgbInterp[0] << 0x10) | 0xB4);    }
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = (((temp_a3_2 & 0xFFF) << 0xC) | 0xF2000000);    _gfx->words.w1 = ((((0x13F << (temp_a3_2 + 2)) & 0xFFF) << 0xC) | 0xFC);    }
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = (((temp_t1_2 & 0xFFF) << 0xC) | 0xF2000000);    _gfx->words.w1 = ((((0x13F << (temp_t1_2 + 2)) & 0xFFF) << 0xC) | 0x01000000 | 0xFC);    }
        gSPEndDisplayList(gfx++);
        D_81004194++;
        D_81004194 &= 0x7F;
        D_81004198++;
        if ((s32) D_81004198 >= 0x32) {
            D_81004198 = 0;
        }
    }
}
#else
void DisplayList_InitColorCycleBoxSegment(s32, DisplayListState*);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/particle_data_library/particle_data_library_258080/DisplayList_InitColorCycleBoxSegment.s")
#endif

void DisplayList_BuildDigitDisplayBackground(Gfx* gfx) {
    s32 temp_a3;

    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xDE000000;    _gfx->words.w1 = &D_810041D0;    }
    temp_a3 = ((s32) ((D_800AF770[5]) * 0x55) / 255) + 0xAA;
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = ((temp_a3 & 0xFF) | 0xFA000000);    _gfx->words.w1 = -0x3701;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFB000000;    _gfx->words.w1 = 0x501E0AFF;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFD700000;    _gfx->words.w1 = Util_ConvertAddrToVirtAddr(&D_3008000);    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5700000;    _gfx->words.w1 = 0x07094260;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE6000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF3000000;    _gfx->words.w1 = 0x070FF200;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5680800;    _gfx->words.w1 = 0x94260;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF2000000;    _gfx->words.w1 = 0x7C03C;    }
    gSPEndDisplayList(gfx++);
    D_81004BB0 = 0;
}

s32 Util_Strlen(s8* arg0) {
    s32 var_v1;
    s8 temp_t7;
    s8* var_v0;

    var_v1 = 0;
    var_v0 = arg0;
    if (*arg0 != 0) {
        do {
            var_v1 += 1;
            var_v0 += 1;
        } while (var_v0[0] != 0);
    }
    return var_v1;
}

#ifdef NON_MATCHING
void DisplayList_BuildDigitOrGlyphSegment(Gfx* gfx) {
    u8 sp4D;
    char* sp34;
    Mtx* sp30;
    s32 sp2C;
    u8 temp_t0;
    u8 var_v1;

    temp_t0 = D_800AF770[(D_8006F09C->poolIndex % 5) & 0xFF];
    switch (D_81004BB0) {
    case 0:
        var_v1 = (((s32) (temp_t0 + 1) / 100) + 0x10) & 0xFF;
        break;
    case 1:
        var_v1 = (((s32) ((s32) (temp_t0 + 1) % 100) / 10) + 0x10) & 0xFF;
        break;
    case 2:
        var_v1 = (((s32) (temp_t0 + 1) % 10) + 0x10) & 0xFF;
        break;
    default:
        sp34 = Text_GetString(0, 0, D_8267E760, temp_t0);;
        sprintf(D_81004BB8, D_810047D0, sp34);
        sp4D = Font_MapGlyphCode((u8) D_81004BB8[(D_81004BB0 - 3) & 0xFFFF]);
        sp2C = 0xA - Util_Strlen((s8*)sp34);
        sp30 = Gfx_AllocDisplayList(0x40);;
        guTranslate(sp30, (f32) sp2C * 32.0f, 0.0f, 0.0f);
        var_v1 = sp4D;
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xDA380001;    _gfx->words.w1 = sp30;    }
        break;
    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFD6800FF;    _gfx->words.w1 = Util_ConvertAddrToVirtAddr(D_3000000[((var_v1 & 0xF) * 0x10) + (((s32) (var_v1 & 0xF0) >> 4) << 0xC)]);    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5680600;    _gfx->words.w1 = 0x07094250;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE6000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF4000000;    _gfx->words.w1 = 0x07040040;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5680600;    _gfx->words.w1 = 0x94250;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF2000000;    _gfx->words.w1 = 0x40040;    }
    {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xDF000000;    _gfx->words.w1 = 0;    }
    D_81004BB0 += 1;
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/particle_data_library/particle_data_library_258080/DisplayList_BuildDigitOrGlyphSegment.s")
#endif

void DisplayList_InitDigitDisplayBackground(s32 arg0, DisplayListState* state) {
    Gfx* gfx;

    if (arg0 == 2) {
        gfx = Gfx_AllocDisplayList(0xF0);
        state->gfx = gfx;
        DisplayList_BuildDigitDisplayBackground(gfx);
    }
}

void DisplayList_InitDigitOrGlyphSegment(s32 arg0, DisplayListState* state) {
    Gfx* gfx;

    if (arg0 == 2) {
        gfx = Gfx_AllocDisplayList(0xA0);
        state->gfx = gfx;
        DisplayList_BuildDigitOrGlyphSegment(gfx);
    }
}
