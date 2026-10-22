#include "minigame_sample2_viewer.h"
#include "src/geo_render.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/jpeg_stream.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/controller.h"
#include "src/fragments/gb_tower_unused/gb_tower_unused.h"
#include "src/geo_layout.h"
#include "src/memory.h"
#include "src/stage_loader.h"

static ModelData* gViewerStageModel;
static GraphNode* gStageTextureNode;
static DisplayObject gViewerGraphNode;
static BinArchive* gViewerAssetArchive;
static u8* gViewerTextureData;

static u32 gViewerGeoLayout[] = {
    0x0C00FFFF, 0x05000000, 0x0B00001E,  0x00000000, 0x014000F0, 0x00000032, 0x00000000, 0x00000118,
    0x05000000, 0x0D000000, 0x05000000,  0x14000000, 0x002B0012, 0xFFFFFF32, 0x16FFFFFF, 0x0F000003,
    0x05000000, 0x0A000000, &gGraphRoot0, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000,
};

void Input_PollAndMenuInit(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void Minigame2SampleViewer_DrawBackgroundTiles(u8* arg0) {
    s32 i;
    s32 j;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGB, G_CC_DECALRGB);

    gDPPipeSync(gDisplayListHead++);

    if (1) {}

    for (i = 0; i < 15; i++) {
        for (j = 0; j < 20; j++, arg0 += 0x200) {
            Gfx_DrawTextureRgba16(j * 0x10, i * 0x10, 0x10, 0x10, arg0, 0x10, 0);
        }
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void Minigame2SampleViewer_DrawFrame(void) {
    BgStage_DrawFrame();
    Minigame2SampleViewer_DrawBackgroundTiles(gViewerTextureData);
    GfxImage_ClearDepthRectangle(&gDisplayListHead, 0, 0, 0x140, 0xF0);
    GeoRender_AdvanceFrameCounter();
    Geo_RenderRootNode(gStageTextureNode);
    BgStage_AdvanceFrame();
}

void Minigame2SampleViewer_Loop(void) {
    s32 i;

    StageFade_StartFromOpaque(0x10);

    for (i = 0; i < 16; i++) {
        Input_PollAndMenuInit();
        Minigame2SampleViewer_DrawFrame();
    }

    do {
        Input_PollAndMenuInit();
        Minigame2SampleViewer_DrawFrame();
    } while (!(BTN_IS_PRESSED(gPlayer1Controller, BTN_A)));

    StageFade_StartFromTransparent(0x10);

    for (i = 0; i < 16; i++) {
        Input_PollAndMenuInit();
        Minigame2SampleViewer_DrawFrame();
    }

    StageLoader_RunFrames(2);
}

void Minigame2SampleViewer_Init(void) {
    MemoryBlock* sp1C = MainPool_AllocState(main_pool_get_available(), 0);

    gStageTextureNode = process_geo_layout(sp1C, gViewerGeoLayout);
    MainPool_FinalizeAllocation(sp1C);
    ModelRenderer_InitDisplayRoots();
    ModelRenderer_AttachDisplayObject(&gViewerGraphNode);
    PokeIcon_OpenModelArchives();
    gViewerStageModel = Model_LoadByArchiveIndex(0x96);
    Model_InitDisplayObject(&gViewerGraphNode, 0, 0x96, gViewerStageModel->unk_08->unk_00[0]);
    ModelAnim_SetAnimation(&gViewerGraphNode, 0);
}

s32 Fragment52_Main(UNUSED s32 arg0, UNUSED s32 arg1) {
    StageContext* sp24;

    main_pool_push_state('MTWO');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(0, 1, 3, 1, 2, 1);
    Font_Init(6, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    gViewerAssetArchive = ASSET_LOAD2(backgrounds, 1, 1);
    gViewerTextureData = BinArchive_GetFile(gViewerAssetArchive, 2);

    Minigame2SampleViewer_Init();
    StageContext_Activate(sp24);
    Minigame2SampleViewer_Loop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('MTWO');

    return 0;
}
