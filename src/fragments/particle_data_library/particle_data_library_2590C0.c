#include "src/util.h"
#include "particle_data_library.h"
#include "src/gfx_buffer.h"
#include "src/geo_render.h"

typedef struct unk_arg1_func_81003E00_14 {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ u16 unk_04;
} unk_arg1_func_81003E00_14; // size = 0x6

typedef struct unk_arg1_func_81003E00_04 {
    /* 0x00 */ char unk00[0x1];
    /* 0x01 */ u8 unk_01;
} unk_arg1_func_81003E00_04; // size = 0x2

// PORT: the callback's node (was a flat N64 view): unk_04 was GraphNode.prevSibling (the callbacks switch the previous
// sibling's render flag, bit 0 of GraphNode.flags at byte 1), unk_14 the callback data: three u16 (animation id, first
// and last frame), big-endian in ROM models.
typedef GraphNode unk_arg1_func_81003E00;

static unk_arg1_func_81003E00_14 Port_PdlFrameRange(void* arg) {
    const u8* p = (const u8*)Util_ConvertAddrToVirtAddr((uintptr_t)arg);
    unk_arg1_func_81003E00_14 r = { 0xFFFF, 0, 0 };

    if (p != NULL) {
        r.unk_00 = (u16)((p[0] << 8) | p[1]);
        r.unk_02 = (u16)((p[2] << 8) | p[3]);
        r.unk_04 = (u16)((p[4] << 8) | p[5]);
    }
    return r;
}

void DisplayList_SetFlagInRange(s32 arg0, unk_arg1_func_81003E00* arg1) {
    s32 temp_a2;
    s32 var_v1;
    unk_arg1_func_81003E00_14 range;
    unk_arg1_func_81003E00_14* temp_v0 = &range;

    if (arg0 == 2) {
        range = Port_PdlFrameRange(arg1->callbackArg);
        var_v1 = 0;
        if (D_8006F09C->transformAnim.animationId == temp_v0->unk_00) {
            temp_a2 = ((s32) D_8006F09C->transformAnim.frameFixed >> 0x10) & 0xFFFF;
            if ((temp_a2 >= (s32) temp_v0->unk_02) && ((s32) temp_v0->unk_04 >= temp_a2)) {
                var_v1 = 1;
            }
        }
        if (var_v1 == 1) {
            arg1->prevSibling->flags |= 1;
            return;
        }
        arg1->prevSibling->flags &= 0xFFFE;
    }
}

void DisplayList_SetFlagOutOfRange(s32 arg0, unk_arg1_func_81003E00* arg1) {
    s32 temp_a2;
    s32 var_v1;
    unk_arg1_func_81003E00_14 range;
    unk_arg1_func_81003E00_14* temp_v0 = &range;

    if (arg0 == 2) {
        range = Port_PdlFrameRange(arg1->callbackArg);
        var_v1 = 1;
        if (D_8006F09C->transformAnim.animationId == temp_v0->unk_00) {
            temp_a2 = ((s32) D_8006F09C->transformAnim.frameFixed >> 0x10) & 0xFFFF;
            if ((temp_a2 >= (s32) temp_v0->unk_02) && ((s32) temp_v0->unk_04 >= temp_a2)) {
                var_v1 = 0;
            }
        }
        if (var_v1 == 1) {
            arg1->prevSibling->flags |= 1;
            return;
        }
        arg1->prevSibling->flags &= 0xFFFE;
    }
}

void DisplayList_InitDitheredPrimColorSegment(s32 arg0, DisplayListState* state) {
    Gfx* gfx;

    if (arg0 == 2) {
        gfx = Gfx_AllocDisplayList(GFX_N64_BYTES(0xA0));
        state->gfx = gfx;
        gDPSetAlphaDither(gfx++, G_AD_NOISE);
        gDPSetPrimColor(gfx++, 0, 0, 10, 10, 10, D_8006F09C->effectSlot & 0xFF);
        gDPSetEnvColor(gfx++, 0x37, 0x14, 0x14, 0xB4);
        gDPSetCombineLERP(gfx++, 0, 0, 0, 0, TEXEL0, 1, PRIMITIVE, 1, 0, 0, 0, 0, COMBINED, 0, ENVIRONMENT, 0);
        gSPEndDisplayList(gfx++);
    }
}
