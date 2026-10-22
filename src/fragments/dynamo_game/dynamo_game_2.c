#include "dynamo_game.h"
#include "src/audio_sfx.h"
#include "src/gfx_buffer.h"
#include "src/fragments/widget_toolkit/widget_toolkit.h"
#include "src/fragments/minigame_shared_library/minigame_shared_library.h"

static Vtx gHealthBarVertices[] = {
    VTX(-1, 545, -834, 0, 1024, 0x00, 0x00, 0x78, 0xFF),
    VTX(219, 545, -834, 1024, 1024, 0x00, 0x00, 0x78, 0xFF),
    VTX(219, 764, -834, 1024, 0, 0x00, 0x00, 0x78, 0xFF),
    VTX(-1, 764, -834, 0, 0, 0x00, 0x00, 0x78, 0xFF),
};

static Gfx gHealthBarSetupDL[] = {
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_PASS2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

static Gfx gHealthBarQuadDL[] = {
    gsSPVertex(gHealthBarVertices, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};

s32 gHealthBarPaletteIndex = 0;
s32 gHealthBarMode = 0;
static u32* gHealthBarPalettes[] = {
    HealthBarPalette0, HealthBarPalette1, HealthBarPalette2, HealthBarPalette3, HealthBarPalette4,
};

const char gDynamoTimeFormatString[] = "%d : TIME = %d\n";

static SimpleGfxNode* gHealthBarNode;
u8 gHealthBarTimer;
u8 gHealthBarDuration;

void DynamoGame_PickNextCrankCue(void) {
    switch (gHealthBarMode) {
        case 0:
            gHealthBarPaletteIndex = MiniGame_RandomRange(2) + 3;
            break;

        case 1:
            gHealthBarPaletteIndex = 4;
            break;

        case 2:
            gHealthBarPaletteIndex = 3;
            break;
    }

    gHealthBarTimer = 0;
    gHealthBarDuration = (MiniGame_RandomRange(4) * 0xF) + 0x1E;

    if (gHealthBarMode != 0) {
        Audio_PlaySoundEffectById(0x11);
    }
}

void DynamoGame_DrawCrankIcon(void) {
    Gfx* sp1C;
    Mtx* sp18;

    sp1C = Gfx_AllocDisplayList(sizeof(Gfx) * 16);
    sp18 = Gfx_AllocDisplayList(sizeof(Mtx) * 1);

    gHealthBarNode->gfx = sp1C;
    guScale(sp18, 0.1f, 0.1f, 0.1f);

    gSPDisplayList(sp1C++, gHealthBarSetupDL);
    gDPLoadTextureBlock(sp1C++, gHealthBarPalettes[gHealthBarPaletteIndex], G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSPMatrix(sp1C++, sp18, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(sp1C++, gHealthBarQuadDL);
    gSPEndDisplayList(sp1C++);
}

void DynamoGame_InitCrankIconDisplay(void) {
    MainPoolState* sp1C;

    sp1C = MainPool_AllocState(main_pool_get_available(), 0);
    gHealthBarNode = GeoNode_CreateDisplayList(sp1C, NULL, 1, NULL);
    GraphNode_AppendChild(&gGraphRoot0, &gHealthBarNode->display_node_header);
    MainPool_FinalizeAllocation(sp1C);
}
