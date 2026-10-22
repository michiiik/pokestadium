#include "jigglypuff_doodle.h"
#include "include/math.h"
#include "src/geo_render.h"
#include "src/model_animation_events.h"
#include "src/geo_layout.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/ui_graphics.h"
#include "src/gallery.h"
#include "src/jpeg_stream.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/fragments/widget_toolkit/widget_toolkit.h"
#include "src/fragments/minigame_shared_graphics/minigame_shared_graphics.h"
#include "src/memory.h"
#include "src/stage_loader.h"

// プリンのらくがき
static char gText_DrawInstructions[] = {
    0xA5, 0xD7, 0xA5, 0xEA, 0xA5, 0xF3, 0xA4, 0xCE, 0xA4, 0xE9,
    0xA4, 0xAF, 0xA4, 0xAC, 0xA4, 0xAD, 0x00, 0x00, 0x00, 0x00,
};

// ３Ｄスティックでジャンプして
static char gText_JumpWithCStick[] = {
    0xA3, 0xB3, 0xA3, 0xC4, 0xA5, 0xB9, 0xA5, 0xC6, 0xA5, 0xA3, 0xA5, 0xC3, 0xA5, 0xAF, 0xA4, 0xC7,
    0xA5, 0xB8, 0xA5, 0xE3, 0xA5, 0xF3, 0xA5, 0xD7, 0xA4, 0xB7, 0xA4, 0xC6, 0x00, 0x00, 0x00, 0x00,
};

// Ａボタンでらくがきしよう。
static char gText_DrawWithAButton[] = {
    0xA3, 0xC1, 0xA5, 0xDC, 0xA5, 0xBF, 0xA5, 0xF3, 0xA4, 0xC7, 0xA4, 0xE9, 0xA4, 0xAF,
    0xA4, 0xAC, 0xA4, 0xAD, 0xA4, 0xB7, 0xA4, 0xE8, 0xA4, 0xA6, 0xA1, 0xA3, 0x00, 0x00,
};

static u32 gGeoLayout_StageCommands[] = {
    0x0C00FFFF, 0x05000000, 0x0B00001E, 0x00000000, 0x014000F0, 0x0000000F, 0x00000000, 0x00000000, 0x05000000,
    0x0D000000, 0x05000000, 0x0F000002, 0x14000000, 0x002B0012, 0xFFFFFF32, 0x16FFFFFF, 0x0F000003, 0x05000000,
    0x0A000000, &gGraphRoot0, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000,
};

typedef struct PlayerState {
    /* 0x000 */ s16 unk_000;
    /* 0x002 */ s16 unk_002;
    /* 0x004 */ DisplayObject unk_004;
    /* 0x16C */ Vec3f unk_16C;
    /* 0x178 */ s8 unk_178;
    /* 0x179 */ s8 unk_179;
    /* 0x17A */ s8 unk_17A;
    /* 0x17B */ s8 unk_17B;
    /* 0x17C */ u16 unk_17C;
    /* 0x17E */ char pad17E[6];
} PlayerState; // size = 0x184

static ModelData* gCurrentGeoLayout;
static ShadowContextNode* gStageRootNode;
static CameraNode* gCamera;
static s16 gCameraDist;
static s16 gCameraRotX;
static s16 gCameraRotY;
static u16 gFrameCounter;
static s32 gGamePhase;
static FontContext* gHudTextObj;
static PlayerState D_86301730[4];
static u32 gGameTimer;
static PlayerState* gCurrentPlayerNode;
static DisplayObject* D_86301D48;
static s16 gIsCountdownActive;
static s16 gCountdownTimer;
static s16 gPlayerJumpState[4];
static DisplayObject gStageCenterNode;

void JigglypuffGame_SnapshotInputs(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        D_86301730[i].unk_17A = D_86301730[i].unk_178;
        D_86301730[i].unk_17B = D_86301730[i].unk_179;
    }

    D_86301730[0].unk_17C = gPlayer1Controller->buttonPressed;
    D_86301730[0].unk_178 = gPlayer1Controller->rawStickX;
    D_86301730[0].unk_179 = gPlayer1Controller->rawStickY;

    D_86301730[1].unk_17C = gPlayer2Controller->buttonPressed;
    D_86301730[1].unk_178 = gPlayer2Controller->rawStickX;
    D_86301730[1].unk_179 = gPlayer2Controller->rawStickY;

    D_86301730[2].unk_17C = gPlayer3Controller->buttonPressed;
    D_86301730[2].unk_178 = gPlayer3Controller->rawStickX;
    D_86301730[2].unk_179 = gPlayer3Controller->rawStickY;

    D_86301730[3].unk_17C = gPlayer4Controller->buttonPressed;
    D_86301730[3].unk_178 = gPlayer4Controller->rawStickX;
    D_86301730[3].unk_179 = gPlayer4Controller->rawStickY;
}

void JigglypuffGame_ReadInputs(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
    JigglypuffGame_SnapshotInputs();
}

void JigglypuffGame_InitCamera(void) {
    gCamera = gStageRootNode->node.children;

    gCameraDist = 0x600;
    gCameraRotX = 0;
    gCameraRotY = 0x15E;

    gCamera->perspective.nearPlane = 100.0f;
    gCamera->perspective.farPlane = 12800.0f;
    gCamera->perspective.verticalFieldOfView = 30.0f;

    gCamera->view.targetPosition.x = 0.0f;
    gCamera->view.targetPosition.y = 70.0f;
    gCamera->view.targetPosition.z = 0.0f;

    Camera_ComputeEyeFromAngles(&gCamera->view.targetPosition, &gCamera->view.eyePosition, gCameraRotY, gCameraDist, gCameraRotX);
}

void JigglypuffGame_UpdateEffects(void) {
}

void JigglypuffGame_DrawHud(s32 arg0) {
    s32 i;
    u8 sp40[] = {
        0,
        0,
        30,
    };
    u8 sp3C[] = {
        0,
        0,
        150,
    };

    if (arg0 == 0) {
        Ui_DrawGradientPanel(0x3A, 0x37, 0xC8, 0x64, sp40, sp3C);
    } else {
        Ui_DrawGradientPanel(0x5A, 0xD7, 0x96, 0x12, sp40, sp3C);
    }

    Font_BeginTranslucentTextRendering();

    switch (arg0) {
        case 0:
            Font_EnableTwoCycleTexturing();
            Font_SetActive(4, -2);
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            Gfx_SetPrimColor(0xC8, 0xC8, 0x64, 0xFF);
            Font_Printf(0xA0 - (Text_MeasureStringWidth(gHudTextObj, gText_DrawInstructions) / 2), 0x46, gText_DrawInstructions);
            Font_SetActive(2, -2);
            Font_Printf(0xA0 - (Text_MeasureStringWidth(gHudTextObj, (char*)gMinigameSharedStrings) / 2), 0x5A, (char*)gMinigameSharedStrings);
            Font_Printf(0xA0 - (Text_MeasureStringWidth(gHudTextObj, gText_JumpWithCStick) / 2), 0x6E, gText_JumpWithCStick);
            Font_Printf(0xA0 - (Text_MeasureStringWidth(gHudTextObj, gText_DrawWithAButton) / 2), 0x7D, gText_DrawWithAButton);

            if (gFrameCounter & 0x10) {
                Font_SetActive(4, -2);
                Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
                Gfx_SetPrimColor(5, 5, 0xFF, 0xFF);
                Font_Printf(0xA0 - (Text_MeasureStringWidth(gHudTextObj, "PUSH START") / 2), 0xAA, "PUSH START");
            }

            Font_DisableTwoCycleTexturing();

            for (i = 0; i < 4; i++) {
                gCurrentPlayerNode = &D_86301730[i];

                Font_SetActive(2, -2);
                Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xC8);
                Font_Printf(30 + (i * 0x46), 0xDA, "LV%02d", gCurrentPlayerNode->unk_002);
            }
            break;

        default:
        case 2:
            Font_SetActive(2, -2);
            Gfx_SetEnvColor(0xFF, 0xFF, 5, 0xFA);
            Font_Printf(0x64, 0xD9, "TIME %02d:%02d.%02d", (gGameTimer * 2) / 3600, ((gGameTimer * 2) / 60) % 60,
                          (((gGameTimer * 2) % 60) * 0x64) / 60);

            if ((gIsCountdownActive == 1) && ((gCountdownTimer % 30) >= 0xF)) {
                Font_SetActive(0x20, -2);
                Gfx_SetEnvColor(0xFF, 5, 5, 0xFA);
                Font_Printf(0x96, 0x14, "%d", (gCountdownTimer / 30) + 1);
            }

            if ((gGameTimer != 0) && (gGameTimer < 0xF)) {
                Font_SetActive(0x20, -2);
                Gfx_SetEnvColor(0xFF, 0xFF, 5, 0xFA);
                Font_Printf(0x78, 0x14, "GO!");
            }
            break;
    }

    Font_EndTexturedTextRendering();
}

void JigglypuffGame_DrawFrame(s32 arg0) {
    BgStage_DrawFrame();
    GfxImage_FillCurrent(&gDisplayListHead, 0x53D);
    GfxImage_ClearDepthRectangle(&gDisplayListHead, 0, 0, 0x140, 0xF0);
    GeoRender_AdvanceFrameCounter();
    Geo_RenderRootNode(gStageRootNode);
    JigglypuffGame_DrawHud(arg0);
    Widget_PauseMenuUpdate();
    BgStage_AdvanceFrame();
    gFrameCounter++;
}

s32 JigglypuffGame_WaitForStart(void) {
    s32 i;
    s32 var_s5;

    StageFade_StartFromOpaque(0x10);

    for (i = 0; i < 4; i++) {
        gCurrentPlayerNode = &D_86301730[i];
        D_86301D48 = &gCurrentPlayerNode->unk_004;

        ModelAnim_SetAnimation(D_86301D48, 1);

        D_86301D48->transformAnim.speedFixed = 0x10000;
        D_86301D48->transformAnim.frameFixed = Rand_Range(0x14) << 0x10;

        gPlayerJumpState[i] = 0;
    }

    var_s5 = 1;
    gGamePhase = 0;

    while (var_s5 == 1) {
        JigglypuffGame_ReadInputs();

        for (i = 0; i < 4; i++) {
            gCurrentPlayerNode = &D_86301730[i];

            if (gCurrentPlayerNode->unk_17C & 1) {
                return -1;
            }

            if (gCurrentPlayerNode->unk_17C & 0x1000) {
                var_s5 = 0;
            }
        }

        for (i = 0; i < 4; i++) {
            JigglypuffGame_UpdatePlayerPhysics(i);
        }

        JigglypuffGame_DrawFrame(gGamePhase);
    }

    return 0;
}

void JigglypuffGame_ReadyCountdown(void) {
    s32 i;

    StageFade_StartFromOpaque(0x10);

    for (i = 0; i < 4; i++) {
        gCurrentPlayerNode = &D_86301730[i];
        D_86301D48 = &gCurrentPlayerNode->unk_004;

        ModelAnim_SetAnimation(D_86301D48, 1);

        D_86301D48->transformAnim.speedFixed = 0x10000;
        D_86301D48->transformAnim.frameFixed = Rand_Range(0x14) << 0x10;

        gPlayerJumpState[i] = 0;
    }

    gIsCountdownActive = 1;
    gCountdownTimer = 0x5A;
    gGamePhase = 1;

    while (gIsCountdownActive == 1) {
        if (gCountdownTimer > 0) {
            gCountdownTimer--;
        } else {
            gIsCountdownActive = 0;
        }

        JigglypuffGame_ReadInputs();

        for (i = 0; i < 4; i++) {
            JigglypuffGame_UpdatePlayerPhysics(i);
        }

        JigglypuffGame_DrawFrame(gGamePhase);
    }
}

void JigglypuffGame_ApplySeparationForce(s32 arg0) {
    s32 i;
    UNUSED s32 pad[2];
    f32 spA0;
    f32 sp9C;
    f32 sp98;
    f32 var_fs4;
    f32 var_fs5;
    UNUSED s32 pad2[11];
    f32 sp60;
    f32 temp_fv0;
    f32 temp_fv1;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 temp_fs0;
    f32 temp_fs1;
    f32 temp_fs2;

    spA0 = D_86301D48->position.x;
    sp9C = D_86301D48->position.y;
    sp98 = D_86301D48->position.z;

    var_fs4 = 0.0f;
    var_fs5 = 0.0f;
    sp60 = 0.0f;

    for (i = 0; i < 4; i++) {
        if (i != arg0) {
            temp_fs0 = D_86301730[i].unk_004.position.x;
            temp_fs1 = D_86301730[i].unk_004.position.y;
            temp_fs2 = D_86301730[i].unk_004.position.z;

            temp1 = temp_fs0 - spA0;
            temp2 = temp_fs1 - sp9C;
            temp3 = temp_fs2 - sp98;

            temp_fv0 = sqrtf(SQ(temp1) + SQ(temp2) + SQ(temp3));
            if ((temp_fv0 < 32.0f) && (temp_fv0 > 0.0f)) {
                temp_fv1 = 32.0f - temp_fv0;

                var_fs4 += (-temp1 / temp_fv0) * temp_fv1;
                var_fs5 += (-temp2 / temp_fv0) * temp_fv1;
                sp60 += (-temp3 / temp_fv0) * temp_fv1;
            }
        }
    }

    temp_fs0 = gStageCenterNode.position.x;
    temp_fs1 = gStageCenterNode.position.y;
    temp_fs2 = gStageCenterNode.position.z;

    temp1 = temp_fs0 - spA0;
    temp2 = temp_fs1 - sp9C;
    temp3 = temp_fs2 - sp98;

    temp_fv0 = sqrtf(SQ(temp1) + SQ(temp2) + SQ(temp3));
    if ((temp_fv0 < 40.0f) && (temp_fv0 > 0.0f)) {
        temp_fv1 = 40.0f - temp_fv0;

        var_fs4 += (-temp1 / temp_fv0) * temp_fv1;
        var_fs5 += (-temp2 / temp_fv0) * temp_fv1;
        sp60 += (-temp3 / temp_fv0) * temp_fv1;
    }

    D_86301D48->position.x += var_fs4;
    D_86301D48->position.y += var_fs5;
    D_86301D48->position.z += sp60;
}

void JigglypuffGame_UpdatePlayerPhysics(s32 arg0) {
    s32 temp_a0;
    s32 temp_a2;

    gCurrentPlayerNode = &D_86301730[arg0];
    D_86301D48 = &gCurrentPlayerNode->unk_004;

    switch (gCurrentPlayerNode->unk_000) {
        case 1:
            break;

        case 0:
            D_86301D48->scale.y = ((gCurrentPlayerNode->unk_179 + 256.0f) * 0.00390625f) + 0.2f;
            D_86301D48->rotation.y = gCurrentPlayerNode->unk_178 << 8;

            gCurrentPlayerNode->unk_16C.x = 0.0f;
            gCurrentPlayerNode->unk_16C.y = 0.0f;

            temp_a2 = gCurrentPlayerNode->unk_178 - gCurrentPlayerNode->unk_17A;
            temp_a0 = gCurrentPlayerNode->unk_179 - gCurrentPlayerNode->unk_17B;

            if (temp_a0 > 10) {
                gCurrentPlayerNode->unk_000 = 1;
                gCurrentPlayerNode->unk_16C.y = temp_a0 * 0.2f;

                if (gCurrentPlayerNode->unk_16C.y > 15.0f) {
                    gCurrentPlayerNode->unk_16C.y = 15.0f;
                }

                gCurrentPlayerNode->unk_16C.x = temp_a2 * 0.1f;
            } else if (Math_AbsF32(gCurrentPlayerNode->unk_178) > 50.0f) {
                gCurrentPlayerNode->unk_000 = 1;
                gCurrentPlayerNode->unk_16C.y = 2.0f;
                gCurrentPlayerNode->unk_16C.x = gCurrentPlayerNode->unk_178 * 0.05f;
            }
            break;
    }

    JigglypuffGame_ApplySeparationForce(arg0);

    D_86301D48->position.x += gCurrentPlayerNode->unk_16C.x;
    D_86301D48->position.y += gCurrentPlayerNode->unk_16C.y;

    if (D_86301D48->position.y > 0.0f) {
        gCurrentPlayerNode->unk_16C.y -= 1.0f;
    }

    if (D_86301D48->position.y < 0.0f) {
        D_86301D48->position.y = 0.0f;
        gCurrentPlayerNode->unk_000 = 0;
    }

    if (D_86301D48->position.x < -100.0f) {
        D_86301D48->position.x = -100.0f;
        gCurrentPlayerNode->unk_16C.x *= -1.0f;
    }

    if (D_86301D48->position.x > 100.0f) {
        D_86301D48->position.x = 100.0f;
        gCurrentPlayerNode->unk_16C.x *= -1.0f;
    }

    if (Rand_Range(0x64) == 0) {
        ModelAnim_ClearEventTrack(D_86301D48);
        ModelAnim_SetEventTrack(D_86301D48, 3);
    }
}

void JigglypuffGame_NoopStub1(void) {
}

void JigglypuffGame_PlayingLoop(void) {
    s32 i;
    s32 var_s6 = 1;

    for (i = 0; i < 4; i++) {
        gCurrentPlayerNode = &D_86301730[i];
        D_86301D48 = &gCurrentPlayerNode->unk_004;

        ModelAnim_SetAnimation(D_86301D48, 1);

        D_86301D48->transformAnim.speedFixed = 0x10000;

        gPlayerJumpState[i] = 0;
    }

    gGameTimer = 0;
    gGamePhase = 2;

    while (var_s6 != 0) {
        JigglypuffGame_ReadInputs();
        JigglypuffGame_UpdateEffects();

        if (gPlayer1Controller->buttonPressed & 0x1000) {
            var_s6 = 0;
        }

        for (i = 0; i < 4; i++) {
            JigglypuffGame_UpdatePlayerPhysics(i);
        }

        gGameTimer++;
        JigglypuffGame_DrawFrame(gGamePhase);
    }
}

void JigglypuffGame_ShowWinnerSequence(void) {
    s32 i;
    s32 j;
    int var = -1;

    for (i = 0; i < 4; i++) {
        D_86301D48 = &D_86301730[i].unk_004;

        if (i == var) {
            ModelAnim_SetAnimation(D_86301D48, 2);
            ModelAnim_SetEventTrack(D_86301D48, 6);
        } else {
            ModelAnim_SetAnimation(D_86301D48, 5);
            ModelAnim_SetEventTrack(D_86301D48, 7);
        }
    }

    gGamePhase = 3;

    for (j = 0; j < 0x5A; j++) {
        JigglypuffGame_ReadInputs();
        JigglypuffGame_DrawFrame(gGamePhase);
    }
}

void JigglypuffGame_FadeOutLoop(void) {
    s32 i;

    StageFade_StartFromTransparent(30);

    for (i = 0; i < 30; i++) {
        JigglypuffGame_ReadInputs();
        JigglypuffGame_DrawFrame(gGamePhase);
    }
}

void JigglypuffGame_WaitForExit(void) {
    gGamePhase = 4;
    Widget_PauseMenuTrigger(1);

    while (gResultExitPending == 0) {
        JigglypuffGame_ReadInputs();
        JigglypuffGame_DrawFrame(gGamePhase);
    }
}

void JigglypuffGame_Init(void) {
    s32 i;
    MemoryBlock* temp_v0 = MainPool_AllocState(main_pool_get_available(), 0);

    gStageRootNode = process_geo_layout(temp_v0, &gGeoLayout_StageCommands);
    MainPool_FinalizeAllocation(temp_v0);
    ModelRenderer_InitDisplayRoots();
    PokeIcon_OpenModelArchives();
    gCurrentGeoLayout = Model_LoadByArchiveIndex(0x27);

    for (i = 0; i < 4; i++) {
        gCurrentPlayerNode = &D_86301730[i];
        D_86301D48 = &gCurrentPlayerNode->unk_004;

        ModelRenderer_AttachDisplayObject(D_86301D48);
        Model_InitDisplayObject(D_86301D48, 0, 0x27, gCurrentGeoLayout->unk_08->unk_00[0]);

        D_86301D48->position.x = (i - 1.5f) * 50.0f;
        D_86301D48->position.y = 0.0f;
        D_86301D48->position.z = 0.0f;

        D_86301D48->scale.x = 1.2f;
        D_86301D48->scale.y = 1.2f;
        D_86301D48->scale.z = 1.2f;

        gCurrentPlayerNode->unk_16C.y = 0.0f;

        ModelAnim_SetAnimation(D_86301D48, 2);
        ModelAnim_ClearEventTrack(D_86301D48);

        D_86301D48->transformAnim.frameFixed = Rand_Range(5) << 0x10;
    }

    PokeIcon_OpenModelArchives();
    gCurrentGeoLayout = Model_LoadByArchiveIndex(0x9A);
    D_86301D48 = &gStageCenterNode;

    ModelRenderer_AttachDisplayObject(D_86301D48);
    Model_InitDisplayObject(D_86301D48, 0, 0x9A, gCurrentGeoLayout->unk_08->unk_00[0]);

    D_86301D48->position.x = 0.0f;
    D_86301D48->position.y = 0.0f;
    D_86301D48->position.z = 0.0f;

    D_86301D48->scale.x = 8.0f;
    D_86301D48->scale.y = 8.0f;
    D_86301D48->scale.z = 8.0f;

    D_86301D48->rotation.y = 0;

    ModelAnim_SetAnimation(D_86301D48, 0);
    ModelAnim_ClearEventTrack(D_86301D48);
    D_86301D48->transformAnim.speedFixed = 0;
    JigglypuffGame_InitCamera();
}

void Minigame_JigglypuffDoodle(UNUSED s32 arg0, UNUSED s32 arg1) {
    StageContext* sp24;

    main_pool_push_state('MINI');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(0, 1, 3, 1, 2, 1);
    gHudTextObj = Font_Init(0x26, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    Fragment_Load((((u32)&D_8D000000 & 0x0FF00000) >> 0x14) - 0x10, _5C7A70_ROM_START, _5C7A70_ROM_END);

    JigglypuffGame_Init();
    Widget_PauseMenuInit();
    StageContext_Activate(sp24);

    if (JigglypuffGame_WaitForStart() != -1) {
        JigglypuffGame_ReadyCountdown();
        JigglypuffGame_PlayingLoop();
        JigglypuffGame_ShowWinnerSequence();
        JigglypuffGame_WaitForExit();
    }

    JigglypuffGame_FadeOutLoop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('MINI');

    Widget_PauseMenuGetResult();
}

void JigglypuffGame_NoopStub2(void) {
}
