#include <stdlib.h>
#include "src/util.h"
#include "particle_data_library.h"
#include "src/gfx_buffer.h"
#include "src/geo_render.h"
#include "src/battle_hud.h"

// PORT: s16 pair then texture addresses (host addresses; indexed with & 7, past the 6 the original declares)
typedef struct unk_arg1_func_81002830 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ uintptr_t unk_04[8];
} unk_arg1_func_81002830; // size = 0x1C

typedef struct unk_arg1_func_81002968 {
    PDL_NODE_WITH_DATA(u32*, unk_14);
    /* 0x18 */ Gfx* unk_18;
} unk_arg1_func_81002968; // size = 0x1C

typedef struct TextureState {
    PDL_NODE_WITH_DATA(u32*, textures);
    /* 0x18 */ Gfx* gfx;
} TextureState; // size = 0x1C

extern int Port_IsRawData(const void* p);

// the frame record from ROM: two big-endian s16, then 32-bit addresses
static unk_arg1_func_81002830* Port_PdlFrameRecord(void* arg) {
    static struct {
        const u8* src;
        unk_arg1_func_81002830* out;
    } sCache[32];
    static s32 sCount;
    const u8* p = (const u8*)Util_ConvertAddrToVirtAddr((uintptr_t)arg);
    unk_arg1_func_81002830* out;
    uintptr_t* words;
    s32 i;

    if (p == NULL || !Port_IsRawData(p)) {
        return (unk_arg1_func_81002830*)p;
    }
    for (i = 0; i < sCount; i++) {
        if (sCache[i].src == p) {
            return sCache[i].out;
        }
    }
    out = calloc(1, sizeof(*out));
    out->unk_00 = (s16)((p[0] << 8) | p[1]);
    out->unk_02 = (s16)((p[2] << 8) | p[3]);
    words = Port_PdlWords((void*)(p + 4));
    for (i = 0; i < 8; i++) {
        out->unk_04[i] = words[i];
    }
    if (sCount < 32) {
        sCache[sCount].src = p;
        sCache[sCount].out = out;
        sCount++;
    }
    return out;
}

void func_81002830(Gfx* gfx, unk_arg1_func_81002830* arg1) {
    s16 temp_v0;
    s32 var_a3;
    unk_arg1_func_81002830* p;
    p = arg1;
    if (D_8006F09C->textureMode == 0) {
        temp_v0 = 0x4000;
        // FAKE
        if ((arg1 && arg1) && arg1) {}
        var_a3 = D_800AF7AE;
        temp_v0 = temp_v0 - (arg1->unk_00 * var_a3);
        // FAKE
        arg1 = NULL;
        var_a3 = D_800AF7AE;
        gDPLoadTextureBlock(gfx++, p->unk_04[var_a3 & 0x7], G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                    G_TX_NOMIRROR | G_TX_CLAMP, 6, 5, G_TX_NOLOD, G_TX_NOLOD);
        gDPSetTileSize(gfx++, 0, temp_v0, 0, ((temp_v0 + 63) << 2), 124);
    }
    gSPEndDisplayList(gfx++);
}

void DisplayList_InitAnimatedFrameSegment(s32 arg0, unk_arg1_func_81002968* arg1) {
    intptr_t temp_v0;
    intptr_t sp18;

    if (arg0 == 2) {
        sp18 = (intptr_t)Port_PdlFrameRecord(arg1->unk_14);
        temp_v0 = Gfx_AllocDisplayList(GFX_N64_BYTES(0x50));
        arg1->unk_18 = temp_v0;
        func_81002830(temp_v0, sp18);
    }
}

Gfx* DisplayList_BuildSelectedTextureSegment(Gfx* gfx, uintptr_t* textures, s32 index) {
    gDPLoadTextureBlock(gfx++, textures[index + 1], G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD, G_TX_NOLOD);
    gSPEndDisplayList(gfx++);
    return gfx;
}

void DisplayList_InitSelectedTextureSegment(s32 arg0, TextureState* state) {
    s32 index;
    uintptr_t* textures;

    if (arg0 == 2) {
        textures = Port_PdlWords(state->textures);
        state->gfx = (Gfx*)Gfx_AllocDisplayList(GFX_N64_BYTES(0x50));
        if (D_8006F09C->objectType == 0x58) {
            index = (s32) D_8006F09C->transformAnim.frameFixed >> 0x10;
            if (index >= 0x42) {
                index = index - 0x42;
            } else {
                index = 0;
            }
            if (index >= 8) {
                index = 0;
            }
            if (D_8006F09C->transformAnim.animationId >= 2) {
                index = 0;
            }
        } else {
            index = D_8006F084 & 7;
        }
        DisplayList_BuildSelectedTextureSegment(state->gfx, textures, index);
    }
}
