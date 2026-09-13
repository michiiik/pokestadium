#include "particle_data_library.h"
#include "src/gfx_buffer.h"
#include "src/geo_render.h"
#include "src/battle_hud.h"
#include "src/model_renderer_callbacks.h"

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
        addresses = state->addresses;
        gfx = Gfx_AllocDisplayList(0x50);
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
        addresses = state->addresses;
        gfx = Gfx_AllocDisplayList(0x140);
        state->gfx = gfx;
        DisplayList_BuildAnimatedModelTexture(gfx, addresses, ((D_800AF7B0[temp_v1 & 1] * 3) + D_81003FD0) % 8);
        D_81003FD0 += 1;
    }
}
