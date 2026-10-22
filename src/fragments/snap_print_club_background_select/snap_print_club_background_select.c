#include "snap_print_club_background_select.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/jpeg_stream.h"
#include "src/audio_sfx.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/controller.h"
#include "src/memory.h"
#include "src/stage_loader.h"

static s32 gMenuSlideShowState;
static BinArchive* gMenuBackgroundArchive;
static u8* gCurrentBackgroundData;
static s32 gCurrentBackgroundIndex;
static DepthBuffer* gBackgroundRenderTarget;

void Snap_PrintClubInitBackgroundSelect(void) {
    gMenuSlideShowState = 0;
    gBackgroundRenderTarget = GfxImage_Allocate(0, 2, 0x280, 0x1E0, 0);
    gCurrentBackgroundIndex = -1;
}

void Snap_PrintClubDrawBackgroundImage(s32 arg0, s32 arg1) {
    u8* img = gBackgroundRenderTarget->img_p;
    s32 i;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 640, 480);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGB, G_CC_DECALRGB);

    gDPPipeSync(gDisplayListHead++);

    for (i = 0; i < 480; i++) {
        gDPLoadTextureBlock(gDisplayListHead++, img + i * 0x500, G_IM_FMT_RGBA, G_IM_SIZ_16b, 640, 1, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, arg0 << 2, (arg1 + i) << 2, (arg0 + 0x280) << 2, (arg1 + 1 + i) << 2,
                            G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);
    }
}

void Snap_PrintClubDrawBackgroundMosaic(s32 arg0, u8* arg1) {
    static s32 gMosaicOffsetX = 4;
    static s32 gMosaicOffsetY = 6;

    UNUSED s32 pad[1];
    s32 j;
    s32 i;
    s32 sp48;
    s32 sp44;
    s32 sp40;
    s32 sp3C;
    f32 divisor = 0.375f;
    s32 sp34;
    s16 temp_a0;
    s32 temp_ft0;
    s32 tmp1;
    s32 tmp2;

    sp48 = arg0 % 4;
    sp44 = arg0 / 4;

    sp40 = ROUND_MAX(320.0f * divisor);
    sp3C = ROUND_MAX(240.0f * divisor);
    temp_ft0 = ROUND_MAX(16.0f * divisor);
    sp34 = ROUND_MAX(16.0f * divisor);
    temp_a0 = ROUND_MAX(1024.0f / divisor);

    tmp2 = ((sp40 + gMosaicOffsetX) * sp48) + ((-(sp40 * 4) - (gMosaicOffsetX * 3)) + 0x280) / 2;
    tmp1 = ((sp3C + gMosaicOffsetY) * sp44) + ((-(sp3C * 4) - (gMosaicOffsetY * 3)) + 0x1E0) / 2;

    for (i = 0; i < sp3C; i += sp34) {
        for (j = 0; j < sp40; j += temp_ft0) {
            gDPLoadTextureBlock(gDisplayListHead++, arg1, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 16, 0,
                                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(gDisplayListHead++, (tmp2 + j) << 2, (tmp1 + i) << 2, ((tmp2 + j) + temp_ft0) << 2,
                                ((tmp1 + i) + sp34) << 2, G_TX_RENDERTILE, 0, 0, temp_a0, temp_a0);

            arg1 += 0x200;
        }
    }
}

void Snap_PrintClubDrawBackgroundSelectFrame(void) {
    static u8 D_83A00B58[] = {
        0x00, 0x11, 0x01, 0x02, 0x04, 0x06, 0x03, 0x0A, 0x0B, 0x07, 0x08, 0x09, 0x0C, 0x10, 0x0F, 0x14,
    };

    if (gCurrentBackgroundIndex >= 0) {
        main_pool_push_state('mksl');

        gCurrentBackgroundData = BinArchive_GetFile(gMenuBackgroundArchive, D_83A00B58[gCurrentBackgroundIndex]);
        GfxImage_SetRenderTarget(&gDisplayListHead, gBackgroundRenderTarget);

        gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
        gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);

        Snap_PrintClubDrawBackgroundMosaic(gCurrentBackgroundIndex, gCurrentBackgroundData);
    }

    BgStage_DrawFrame();
    Snap_PrintClubDrawBackgroundImage(0, 0);
    BgStage_AdvanceFrame();

    if (gCurrentBackgroundIndex >= 0) {
        main_pool_pop_state('mksl');
    }
}

s32 Snap_PrintClubCheckConfirmInput(void) {
    s32 var_v1 = 0;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A | BTN_B)) {
        var_v1 = 1;
    }
    return var_v1;
}

s32 Snap_PrintClubUpdateBackgroundSelect(void) {
    static s32 gSlideShowDelayTimer = 2;

    s32 sp1C = 1;

    switch (gMenuSlideShowState) {
        case 0:
            if (StageContext_GetFadeMode() == 0) {
                gSlideShowDelayTimer--;
                if (gSlideShowDelayTimer <= 0) {
                    gMenuSlideShowState = 1;
                    gCurrentBackgroundIndex = 0;
                }
            }
            break;

        case 1:
            if (gCurrentBackgroundIndex >= 0) {
                gCurrentBackgroundIndex++;
            }

            if (gCurrentBackgroundIndex >= 0x10) {
                gCurrentBackgroundIndex = -1;
            }

            if ((gCurrentBackgroundIndex == -1) && (Snap_PrintClubCheckConfirmInput() != 0)) {
                gMenuSlideShowState = 2;
                Audio_PlaySoundEffectById(SFX_MENU_BACK);
                StageFade_StartFromTransparent(8);
            }
            break;

        case 2:
            if (StageContext_GetFadeMode() == 1) {
                sp1C = 0;
            }
            break;
    }
    return sp1C;
}

void Snap_PrintClubReadInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void func_83A009C0(void) {
}

void Snap_PrintClubBackgroundSelectLoop(void) {
    GfxImage_SetRenderTarget(&gDisplayListHead, gBackgroundRenderTarget);
    GfxImage_FillRectangle(&gDisplayListHead, 0, 0, 0x280, 0x1E0, 0xFFFF);
    BgStage_AdvanceFrame();
    StageFade_StartFromOpaque(8);
    do {
        Snap_PrintClubReadInput();
        Snap_PrintClubDrawBackgroundSelectFrame();
    } while (Snap_PrintClubUpdateBackgroundSelect() != 0);
}

s32 Snap_PrintClubBackgroundSelect(s32 arg0, s32 arg1) {
    StageContext* sp24;

    main_pool_push_state('BNVW');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(1, 0, 2, 0, 2, 1);
    Font_Init(8, 0);
    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    gMenuBackgroundArchive = ASSET_LOAD2(backgrounds, 1, 1);
    Snap_PrintClubInitBackgroundSelect();
    StageContext_Activate(sp24);
    Snap_PrintClubBackgroundSelectLoop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('BNVW');

    return 0;
}
