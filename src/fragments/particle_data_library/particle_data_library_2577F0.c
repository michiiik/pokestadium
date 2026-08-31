#include "particle_data_library.h"
#include "battle_hud.h"
#include "geo_render.h"
#include "gfx_buffer.h"

// .data
static Gfx D_81004170[] = {
    gsDPPipeSync(),
    gsDPSetCombineLERP(TEXEL1, TEXEL0, ENV_ALPHA, TEXEL0, TEXEL1, TEXEL0, ENVIRONMENT, TEXEL0, COMBINED, 0, SHADE, 0, COMBINED, 0, PRIM_LOD_FRAC, 0),
    gsDPSetEnvColor(0xFF, 0xFF, 0xFF, 0x64),
    gsSPEndDisplayList(),
};

// .bss
static s16 D_81004B60;

#ifdef NON_MATCHING
void func_81002530(Gfx* gfx, DisplayListAddresses* addresses) {
    s16 temp_t3;
    s16 temp_t4;
    s16 temp_t2;
    u8 temp_v0;

    temp_v0 = ModelRenderer_GetActiveMode();
    if ((D_8006F09C->unk_01C & 0xFF) == 0) {
        temp_t2 = (D_81004B60 >> 4);
        temp_t3 = 0x4000 - (D_81004B60 >> 4);
        temp_t4 = 0x4000 - (D_81004B60 >> 3);

        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xDE000000;    _gfx->words.w1 = D_81004170;    }
        
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFD10001F;    _gfx->words.w1 = addresses->segments[0];    }  \
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5101000;    _gfx->words.w1 = 0x07014050;    }              \
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE6000000;    _gfx->words.w1 = 0;    }                       \
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF4000000;    _gfx->words.w1 = 0x0707C07C;    }              \
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }                       \
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5101000;    _gfx->words.w1 = 0x14050;    }                 \
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF2000000;    _gfx->words.w1 = 0x7C07C;    }
        
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = (((temp_t2 & 0xFFF) << 0xC) | 0xF2000000 | (temp_t3 & 0xFFF));    _gfx->words.w1 = ((((temp_t2 + 0x1F) & 0xFFF) << 0xC) | ((temp_t3 + 0x1F) & 0xFFF));    }
        
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFD10001F;    _gfx->words.w1 = addresses->segments[1];    }  \
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5101100;    _gfx->words.w1 = 0x07014050;    }              \
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE6000000;    _gfx->words.w1 = 0;    }                       \
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF4000000;    _gfx->words.w1 = 0x0707C07C;    }              \
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xE7000000;    _gfx->words.w1 = 0;    }                       \
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF5101100;    _gfx->words.w1 = 0x01014050;    }              \
        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xF2000000;    _gfx->words.w1 = 0x0107C07C;    }

        {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = (((temp_t3 & 0xFFF) << 0xC) | 0xF2000000 | (temp_t4 & 0xFFF));    _gfx->words.w1 = (((temp_t3 & 0xFFF) << 0xC) | 0x01000000 | ((temp_t4 + 0x1F) & 0xFFF));    }

        if (D_800AF7B0[temp_v0 & 1] == 0) {
            if (func_800325AC() == 0) {
                D_81004B60--;
            }
        }
    }
    gSPEndDisplayList(gfx++);
}
#else
void func_81002530(Gfx*, DisplayListAddresses*);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/31/fragment31_2577F0/func_81002530.s")
#endif

void DisplayList_InitScrollingDualPanel(s32 arg0, DisplayListState* state) {
    Gfx* gfx;
    DisplayListAddresses* addresses;

    if (arg0 == 2) {
        addresses = state->addresses;
        gfx = Gfx_AllocDisplayList(0xF0);
        state->gfx = gfx;
        func_81002530(gfx, addresses);
    }
}
