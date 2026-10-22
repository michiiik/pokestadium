#include "particle_data_library.h"
#include "src/gfx_buffer.h"
#include "src/geo_render.h"

typedef struct GraphNodeRange {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ u16 unk_04;
} GraphNodeRange; // size = 0x6

typedef struct GraphFlag {
    /* 0x00 */ char unk00[0x1];
    /* 0x01 */ u8 unk_01;
} GraphFlag; // size = 0x2

typedef struct GraphCondition {
    /* 0x00 */ char unk00[0x4];
    /* 0x04 */ GraphFlag* unk_04;
    /* 0x08 */ char unk08[0xC];
    /* 0x14 */ GraphNodeRange* unk_14;
} GraphCondition; // size = 0x18

void DisplayList_SetFlagInRange(s32 arg0, GraphCondition* arg1) {
    s32 temp_a2;
    s32 var_v1;
    GraphNodeRange* temp_v0;
    GraphFlag* temp_v0_2;

    if (arg0 == 2) {
        temp_v0 = arg1->unk_14;
        var_v1 = 0;
        if (gDefaultGraphRoot->transformAnim.animationId == temp_v0->unk_00) {
            temp_a2 = ((s32) gDefaultGraphRoot->transformAnim.frameFixed >> 0x10) & 0xFFFF;
            if ((temp_a2 >= (s32) temp_v0->unk_02) && ((s32) temp_v0->unk_04 >= temp_a2)) {
                var_v1 = 1;
            }
        }
        if (var_v1 == 1) {
            temp_v0_2 = arg1->unk_04;
            temp_v0_2->unk_01 |= 1;
            return;
        }
        temp_v0_2 = arg1->unk_04;
        temp_v0_2->unk_01 &= 0xFFFE;
    }
}

void DisplayList_SetFlagOutOfRange(s32 arg0, GraphCondition* arg1) {
    s32 temp_a2;
    s32 var_v1;
    GraphNodeRange* temp_v0;
    GraphFlag* temp_v0_2;

    if (arg0 == 2) {
        temp_v0 = arg1->unk_14;
        var_v1 = 1;
        if (gDefaultGraphRoot->transformAnim.animationId == temp_v0->unk_00) {
            temp_a2 = ((s32) gDefaultGraphRoot->transformAnim.frameFixed >> 0x10) & 0xFFFF;
            if ((temp_a2 >= (s32) temp_v0->unk_02) && ((s32) temp_v0->unk_04 >= temp_a2)) {
                var_v1 = 0;
            }
        }
        if (var_v1 == 1) {
            temp_v0_2 = arg1->unk_04;
            temp_v0_2->unk_01 |= 1;
            return;
        }
        temp_v0_2 = arg1->unk_04;
        temp_v0_2->unk_01 &= 0xFFFE;
    }
}

void DisplayList_InitDitheredPrimColorSegment(s32 arg0, DisplayListState* state) {
    Gfx* gfx;

    if (arg0 == 2) {
        gfx = Gfx_AllocDisplayList(0xA0);
        state->gfx = gfx;
        gDPSetAlphaDither(gfx++, G_AD_NOISE);
        gDPSetPrimColor(gfx++, 0, 0, 10, 10, 10, gDefaultGraphRoot->effectSlot & 0xFF);
        gDPSetEnvColor(gfx++, 0x37, 0x14, 0x14, 0xB4);
        gDPSetCombineLERP(gfx++, 0, 0, 0, 0, TEXEL0, 1, PRIMITIVE, 1, 0, 0, 0, 0, COMBINED, 0, ENVIRONMENT, 0);
        gSPEndDisplayList(gfx++);
    }
}
