#include "particle_data_library.h"
#include "src/gfx_buffer.h"
#include "src/geo_render.h"
#include "src/battle_hud.h"
#include "src/model_renderer_callbacks.h"
#include <stdlib.h>
#include "src/util.h"

extern int Port_IsRawData(const void* p);

static uintptr_t Port_PdlAddr(u32 v) {
    if (v < 0x10000000U) {
        return v; // segmented address (or a plain value): resolved by the renderer
    }
    return Util_ConvertAddrToVirtAddr(v);
}

uintptr_t* Port_PdlWords(void* arg) {
    static struct {
        const u8* src;
        uintptr_t* out;
    } sCache[128];
    static s32 sCount;
    const u8* p = (const u8*)Util_ConvertAddrToVirtAddr((uintptr_t)arg);
    uintptr_t* out;
    s32 i;

    if (p == NULL || !Port_IsRawData(p)) {
        return (uintptr_t*)p;
    }
    for (i = 0; i < sCount; i++) {
        if (sCache[i].src == p) {
            return sCache[i].out;
        }
    }
    out = calloc(10, sizeof(uintptr_t));
    for (i = 0; i < 10; i++) {
        out[i] = Port_PdlAddr(((u32)p[i * 4] << 24) | ((u32)p[i * 4 + 1] << 16) | ((u32)p[i * 4 + 2] << 8) | p[i * 4 + 3]);
    }
    if (sCount < 128) {
        sCache[sCount].src = p;
        sCache[sCount].out = out;
        sCount++;
    }
    return out;
}

const char D_810041C0[0xC] = "DUMMY NAME";
const char D_810041CC[0x4] = "";

const Gfx D_810041D0[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsSPLoadGeometryMode(0),
    gsSPSetGeometryMode(G_ZBUFFER | G_SHADE | G_LIGHTING | G_SHADING_SMOOTH),
    gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, TEXEL0, COMBINED, 0, PRIM_LOD_FRAC, 0, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

const char D_81004210[0x4] = "Ａ";
const char D_81004214[0x4] = "";
const char D_81004218[0x4] = "";
const char D_8100421C[0x4] = "";

Gfx* DisplayList_BuildIndexedTextureSegment(Gfx* gfx, DisplayListAddresses* addresses, s32 index) {
    gSPDisplayList(gfx++, addresses->segments[0]);
    // TODO: Convert to gDPLoadTextureBlock
    gDPSetTextureImage(gfx++, G_IM_FMT_I, G_IM_SIZ_16b, 1, addresses->textures[index]);
    gDPSetTile(gfx++, G_IM_FMT_I, G_IM_SIZ_16b, 0, 0x0000, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD);
    gDPLoadSync(gfx++);
    gDPLoadBlock(gfx++, 7, 0, 0, 0x3FF, 0x200);
    gDPPipeSync(gfx++);
    gDPSetTile(gfx++, G_IM_FMT_I, G_IM_SIZ_8b, 4, 0, 0, 0, G_TX_CLAMP, 0, 0, G_TX_CLAMP, 0, 0);
    gDPSetTileSize(gfx++, 0, 0, 0, 0x7C, 0xFC);
    gSPDisplayList(gfx++, addresses->segments[1]);
    gSPEndDisplayList(gfx++);
    return gfx;
}

void DisplayList_InitIndexedTextureSegment(s32 arg0, DisplayListState* state) {
    Gfx* gfx;
    DisplayListAddresses* addresses;

    if (arg0 == 2) {
        addresses = (DisplayListAddresses*)Port_PdlWords(state->addresses);
        gfx = Gfx_AllocDisplayList(GFX_N64_BYTES(0x50));
        state->gfx = gfx;
        DisplayList_BuildIndexedTextureSegment(gfx, addresses, D_8006F084 & 7);
    }
}

Gfx* DisplayList_BuildAnimatedModelTexture(Gfx* gfx, DisplayListAddresses* addresses, s32 index) {
    gDPLoadTextureBlock(gfx++, addresses->textures[index], G_IM_FMT_IA, G_IM_SIZ_8b, 32, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    ModelRenderer_UpdateSlotVertices();
    {
        Gfx* gfx2 = ModelRenderer_RenderActiveSlot(gfx);
        gSPSetGeometryMode(gfx2++, 0x00020000);
        gSPEndDisplayList(gfx2++);
        return gfx2;
    }
}

void DisplayList_InitAnimatedModelTexture(s32 arg0, DisplayListState* state) {
    static s32 D_81003FD0 = 0;
    Gfx* gfx;
    DisplayListAddresses* addresses;
    u8 temp_v1;

    temp_v1 = ModelRenderer_GetActiveMode();
    if (arg0 == 2) {
        addresses = (DisplayListAddresses*)Port_PdlWords(state->addresses);
        gfx = Gfx_AllocDisplayList(GFX_N64_BYTES(0x140));
        state->gfx = gfx;
        DisplayList_BuildAnimatedModelTexture(gfx, addresses, ((D_800AF7B0[temp_v1 & 1] * 3) + D_81003FD0) % 8);
        D_81003FD0 += 1;
    }
}
