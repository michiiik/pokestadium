#include "credits.h"
#include "src/geo_node.h"
#include "src/geo_render.h"
#include "src/geo_layout.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_loop_point.h"
#include "src/audio_category_fade.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/dp_intro.h"
#include "src/fragments/particle_math/particle_math.h"
#include "src/memory.h"
#include "src/stage_loader.h"

typedef struct StageFadePalette {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ Color_RGB8 unk_03[4];
} StageFadePalette; // size = 0x10

static FogRect gStageFragmentState;
static struct GraphNode gStageBackgroundNode;
static struct GraphNode gStageOverlayNode;
static struct GraphNode gStageForegroundNode;
static s8 gCurrentStageId;
static u8 gCurrentSubStageId;
static BinArchive* gStageArchive;
static Color_RGBA8_u32 gGlobalBackgroundColor;
static GraphNode* sSceneGraphRoot;
static CameraNode* sSceneCamera;
static s16 sCameraRotationX;
static s16 sCameraRotationY;
static s16 sCameraDistance;
static s32 pad_D_86A061F8[2];
static FontContext* gStageTextObject;
static s32 pad_D_86A06208[2];
u8 gPhaseDelta;
u8 gUnusedTimer1;
u8 gUnusedTimer2;
CreditsRolePage* gStageLayoutPtr;
u32 gPrevRspCounter;

StagePhaseInfo gStagePhases[9] = {
    {
        0x00,
        0x00,
        0x0528,
    },
    {
        0x00,
        0x01,
        0x0438,
    },
    {
        0x00,
        0x02,
        0x0438,
    },
    {
        0x00,
        0x03,
        0x0438,
    },
    {
        0x00,
        0x04,
        0x0438,
    },
    {
        0x00,
        0x05,
        0x0438,
    },
    {
        0x00,
        0x06,
        0x0528,
    },
    {
        0x00,
        0x07,
        0x0438,
    },
    {
        0xFF,
        0x00,
        0x0000,
    },
};
u8 gCurrentPhaseIndex = 0;
u32 gScreenFadeAlpha = 0;
u16 gCreditsShotTimer = 0;
u32 gCreditsGeoLayout[] = {
    0x0C00FFFF, 0x05000000, 0x0B00001E,    0x00000000, 0x014000F0,  0x0000000F, 0x00000000,  0x00000000,
    0x05000000, 0x0D000000, 0x05000000,    0x07000000, &gStageFragmentState, 0x14000000, 0x002D0019,  0xFFFFFF28,
    0x16646464, 0x0F000002, 0x05000000,    0x1F00FFFF, 0x00000000,  0x00000000, 0x00000000,  0x00640064,
    0x00640000, 0x08000000, Credits_BackgroundNodePostCallback, 0x00000000, 0x05000000,  0x07000000, &gStageBackgroundNode, 0x06000000,
    0x06000000, 0x0F000003, 0x05000000,    0x1F00FFFF, 0x00000000,  0x00000000, 0x00000000,  0x00640064,
    0x00640000, 0x08000000, Credits_BackgroundNodePostCallback, 0x00000000, 0x05000000,  0x07000000, &gStageOverlayNode, 0x06000000,
    0x06000000, 0x0F000002, 0x05000000,    0x1F00FFFF, 0x00000000,  0x00000000, 0x00000000,  0x00640064,
    0x00640000, 0x08000000, Credits_BackgroundNodePostCallback, 0x00000000, 0x05000000,  0x07000000, &gStageForegroundNode, 0x06000000,
    0x06000000, 0x0F000003, 0x05000000,    0x0A000000, &gGraphRoot0, 0x06000000, 0x06000000,  0x06000000,
    0x06000000, 0x01000000,
};

s32 Credits_BackgroundNodePostCallback(s32 arg0, GraphNode* arg1) {
    DisplayObject* ptr = (DisplayObject*)arg1;

    if (arg0 == 2) {
        ptr->textureTint.rgba = gGlobalBackgroundColor.rgba;
    }
    return 0;
}

void Credits_SetBackgroundTint(u8 r, u8 g, u8 b) {
    u8 v = r & g & b;

    gGlobalBackgroundColor.r = r;
    gGlobalBackgroundColor.g = g;
    gGlobalBackgroundColor.b = b;

    if (v == 0xFF) {
        if (gStageFragmentState.node.callbackArg == 1) {
            gStageFragmentState.node.flags |= 1;
        }
    } else {
        gStageFragmentState.node.flags &= ~1;
    }
}

void Credits_LoadBackgroundScene(s8 arg0, u8 arg1) {
    MemoryBlock* sp2C;
    FragmentEntry sp28;
    u32* temp_v0;
    CreditsSceneData* temp_v0_4;

    while (Display_IsFrameReady() == 0) {}

    GeoNode_CreateContainer(NULL, &gStageBackgroundNode);
    GeoNode_CreateContainer(NULL, &gStageOverlayNode);
    GeoNode_CreateContainer(NULL, &gStageForegroundNode);

    main_pool_pop_state('BACK');

    main_pool_push_state('BACK');

    sp28 = BinArchive_GetFile(gStageArchive, arg0);
    sp2C = MainPool_AllocState(main_pool_get_available(), 0);

    temp_v0 = sp28(0, 0);
    if (temp_v0 != NULL) {
        GraphNode_AppendChild(&gStageBackgroundNode, process_geo_layout(sp2C, temp_v0));
    }

    temp_v0 = sp28(1, 0);
    if (temp_v0 != NULL) {
        GraphNode_AppendChild(&gStageOverlayNode, process_geo_layout(sp2C, temp_v0));
    }

    temp_v0 = sp28(3, 0);
    if (temp_v0 != NULL) {
        GraphNode_AppendChild(&gStageForegroundNode, process_geo_layout(sp2C, temp_v0));
    }

    gScreenFadeAlpha = sp28(2, 0);
    MainPool_FinalizeAllocation(sp2C);

    temp_v0_4 = sp28(4, 0);
    if (temp_v0_4 == NULL) {
        gStageFragmentState.node.callbackArg = 0;
        gStageFragmentState.node.flags &= ~1;
    } else {
        gStageFragmentState.unk_18.unk_00 = temp_v0_4->unk_00;
        gStageFragmentState.unk_18.unk_02 = temp_v0_4->unk_02;
        gStageFragmentState.unk_18.unk_04.rgba = temp_v0_4->unk_04.rgba;
        gStageFragmentState.node.callbackArg = 1;
    }

    Credits_SetBackgroundTint(0xFF, 0xFF, 0xFF);

    if (arg1 < 6) {
        Camera_SetupForPhase(&sCameraRotationX, &sCameraRotationY, &sCameraDistance, sSceneCamera, gStagePhases[gCurrentPhaseIndex].unk_02, arg0, arg1);
    } else if (arg1 < 9) {
        Camera_SetupForPhase(&sCameraRotationX, &sCameraRotationY, &sCameraDistance, sSceneCamera, gStagePhases[gCurrentPhaseIndex].unk_02, arg0, arg1);
    } else if (arg1 < 0xF) {
        if (gCurrentPhaseIndex == 7) {
            Credits_InitCameraKeyframeBlend(sSceneCamera, gStagePhases[gCurrentPhaseIndex].unk_02 - 0x28, arg0, arg1);
        } else {
            Credits_InitCameraKeyframeBlend(sSceneCamera, gStagePhases[gCurrentPhaseIndex].unk_02, arg0, arg1);
        }
    } else {
        sSceneCamera->view.targetPosition.y = gStageCameraPositions[arg0][gCurrentSubStageId].unk_10;
        sCameraDistance = gStageCameraPositions[arg0][gCurrentSubStageId].unk_08;
    }
}

void Credits_PollInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void Credits_UpdateCamera(void) {
    if (gCurrentSubStageId < 6) {
        Camera_ComputeEyeFromAngles(&sSceneCamera->view.targetPosition, &sSceneCamera->view.eyePosition, sCameraDistance, sCameraRotationX, sCameraRotationY);
    } else if (gCurrentSubStageId < 9) {
        Camera_ComputeEyeFromAngles(&gCreditsCameraPivot, &sSceneCamera->view.eyePosition, sCameraDistance, sCameraRotationX, sCameraRotationY);
        Camera_ComputeEyeFromAngles(&gCreditsCameraPivot, &sSceneCamera->view.targetPosition, sCameraDistance + 500.0f, sCameraRotationX, sCameraRotationY);
    }
}

void Credits_DrawFadeOverlay(void) {
    u8 temp_v1_2;

    if (gStagePhases[gCurrentPhaseIndex].unk_00 != -1) {
        if (gCreditsShotTimer < 8) {
            temp_v1_2 = (s8)ROUND_MAX(((-(f32)gCreditsShotTimer / 8) + 1.0f) * 255.0f);
            if (temp_v1_2 != 0) {
                Gfx_FillRectWithDisplayState(0, 0, 0x140, 0xF0, 0, 0, 0, temp_v1_2);
            }
        } else {
            if ((gStagePhases[gCurrentPhaseIndex].unk_02 - 0x28) < gCreditsShotTimer) {
                s32 tmp = gStagePhases[gCurrentPhaseIndex].unk_02;

                temp_v1_2 = (s32)ROUND_MAX((((gCreditsShotTimer - tmp) + 0x28) * 255.0f) / 40.0f);
                if (temp_v1_2 != 0) {
                    Gfx_FillRectWithDisplayState(0, 0, 0x140, 0xF0, 0, 0, 0, temp_v1_2);
                }
            }
        }
    }
}

void Credits_UpdateSceneTransition(u8 arg0) {
    if ((arg0 == 1) || (arg0 == 2)) {
        Credits_DrawFadeOverlay();
        if ((gCurrentPhaseIndex != 0) || (gCreditsShotTimer >= 0xF0)) {
            Credits_DrawRoleList();
        }
    }
}

void Credits_DrawFrame(u8 arg0) {
    BgStage_DrawFrame();

    if ((gScreenFadeAlpha == -1) || (gScreenFadeAlpha == 0)) {
        GfxImage_FillCurrent(&gDisplayListHead, 1);
    } else if (gScreenFadeAlpha < 0x10000) {
        GfxImage_FillCurrent(&gDisplayListHead, gScreenFadeAlpha);
    } else {
        GfxImage_FillCurrent(&gDisplayListHead, 0xA6BF);
    }

    if (arg0 == 0) {
        Credits_DrawContinuePrompt(0);
    } else {
        Geo_RenderRootNode(sSceneGraphRoot);
        Credits_UpdateSceneTransition(arg0);
    }

    BgStage_AdvanceFrame();
}

void Credits_IntroLoop(void) {
    u8 i;
    u8 j;
    u8 sp35;
    s32 var_s1;

    Audio_FadeOutAll(0x1E);

    for (i = 0; i < 3; i++) {
        var_s1 = sp35;

        switch (i) {
            case 0:
                sp35 = 0x1E;
                StageFade_StartFromOpaque(sp35);
                var_s1 = 0x1E;
                break;

            case 1:
                sp35 = 0x5A;
                var_s1 = sp35;
                break;

            case 2:
                sp35 = 0x78;
                StageFade_StartFromTransparent(sp35);
                var_s1 = sp35;
                break;

            default:
                var_s1 = sp35;
                break;
        }

        for (j = 0; j < var_s1; j++) {
            Credits_PollInput();
            Credits_DrawFrame(0);
        }
    }
}

void Credits_MainLoop(s32 arg0) {
    static u8 gCreditsSceneSubIndex = 0;

    s32 var_s6 = 1;

    StageLoader_RunFrames(0x3C);
    StageFade_StartFromOpaque(0xB4);

    if (arg0 != 0) {
        Audio_StartMusicTrack(0x42);
    } else {
        Audio_StartMusicTrack(0x43);
    }

    Credits_SelectNextScene(&gCurrentStageId, &gCurrentSubStageId);
    Credits_LoadBackgroundScene(gCurrentStageId, gCurrentSubStageId);

    while (var_s6 != 0) {
        Stage_UpdateLogic();
        Particle_UpdateFrameCounters();

        if (gStagePhases[gCurrentPhaseIndex].unk_02 != 0) {
            if (gStagePhases[gCurrentPhaseIndex].unk_00 != -1) {
                if (gCurrentSubStageId < 9) {
                    Credits_StepCameraYawBlend();
                } else {
                    Credits_StepCameraKeyframeBlend();
                }
            }

            if (gCreditsShotTimer >= gStagePhases[gCurrentPhaseIndex].unk_02) {
                gCurrentPhaseIndex++;
                if (gStagePhases[gCurrentPhaseIndex].unk_02 != 0) {
                    gStagePhases[gCurrentPhaseIndex].unk_02 =
                        gStagePhases[gCurrentPhaseIndex - 1].unk_02 + (gStagePhases[gCurrentPhaseIndex].unk_02 - gCreditsShotTimer);
                }

                gCreditsShotTimer = 0;

                if (gStagePhases[gCurrentPhaseIndex].unk_00 != -1) {
                    Credits_SelectNextScene(&gCurrentStageId, &gCurrentSubStageId);
                    Credits_LoadBackgroundScene(gCurrentStageId, gCurrentSubStageId);
                } else {
                    gCurrentStageId = -1;
                    gScreenFadeAlpha = 0;

                    GeoNode_CreateContainer(NULL, &gStageBackgroundNode);
                    GeoNode_CreateContainer(NULL, &gStageOverlayNode);
                    GeoNode_CreateContainer(NULL, &gStageForegroundNode);

                    gStageFragmentState.node.flags &= ~1;
                    if (gStagePhases[gCurrentPhaseIndex].unk_02 == 0) {
                        main_pool_pop_state('BACK');
                    }
                }
            } else if ((gCurrentPhaseIndex == 3) && (gCreditsSceneSubIndex < 2) &&
                       (gCreditsShotTimer >= (((gStagePhases[gCurrentPhaseIndex].unk_02 * (gCreditsSceneSubIndex + 1)) / 3) - 1))) {
                Credits_SelectNextScene(&gCurrentStageId, &gCurrentSubStageId);
                Credits_LoadBackgroundScene(gCurrentStageId, 0xFF);
                gCreditsSceneSubIndex++;
            }
        }

        Credits_PollInput();
        Credits_UpdateCamera();

        if (gBorderFadeCycleCount >= 0x2D1) {
            var_s6 = 0;
        } else if ((gBorderFadeCycleCount >= 0x169) &&
                   ((gPlayer1Controller->buttonPressed & 0x1000) || (gPlayer1Controller->buttonPressed & 0x8000))) {
            var_s6 = 0;
        }

        Credits_DrawFrame(1);

        if (gStagePhases[gCurrentPhaseIndex].unk_02 != 0) {
            gCreditsShotTimer += gPhaseDelta;
        }
    }
}

static s32 pad_D_86A026FC = 0;
StageFadePalette gStageFadePalettes[] = {
    {
        0,
        0,
        {
            { 0xFF, 0, 0xE6 },
            { 0xFF, 0xC8, 0xFF },
            { 0, 0x46, 0xFF },
            { 0x64, 0xFF, 0xFF },
        },
    },
    {
        8,
        0xFF,
        {
            { 0xFF, 0xC8, 0xFF },
            { 0xFF, 0xFF, 0xFF },
            { 0x64, 0xFF, 0xFF },
            { 0xFF, 0xFF, 0xFF },
        },
    },
    {
        0x1E,
        0xFF,
        {
            { 0xFF, 0, 0xFF },
            { 0xFF, 0xC8, 0xFF },
            { 0, 0x46, 0xFF },
            { 0x64, 0xFF, 0xFF },
        },
    },
    {
        0x153,
        0xFF,
        {
            { 0xFF, 0, 0xFF },
            { 0xFF, 0xC8, 0xFF },
            { 0, 0x46, 0xFF },
            { 0x64, 0xFF, 0xFF },
        },
    },
    {
        0x167,
        0,
        {
            { 0xFF, 0, 0xE6 },
            { 0xFF, 0xC8, 0xFF },
            { 0, 0x46, 0xFF },
            { 0x64, 0xFF, 0xFF },
        },
    },
};

static CreditRoleEntry gCreditsRoleLines[] = {
    {
        0,
        { &gCreditsString00, &gCreditsMessageTable },
    },
    {
        1,
        { &gCreditsString01, &gCreditsString01Len },
    },
    {
        0,
        { &D_86A05B70, &D_86A05B74 },
    },
    {
        1,
        { &D_86A05B78, &D_86A05B7C },
    },
    {
        0,
        { &D_86A05B80, &D_86A05B84 },
    },
    {
        1,
        { &D_86A05B88, &D_86A05B8C },
    },
    {
        0,
        { &D_86A05B90, &D_86A05B94 },
    },
    {
        1,
        { &D_86A05B98, &D_86A05B9C },
    },
    {
        0,
        { &D_86A05BA0, &D_86A05BA4 },
    },
    {
        1,
        { &D_86A05BA8, &D_86A05BAC },
    },
    {
        0,
        { &D_86A05BB0, &D_86A05BB4 },
    },
    {
        1,
        { &D_86A05BB8, &D_86A05BBC },
    },
    {
        0,
        { &D_86A05BC0, &D_86A05BC4 },
    },
    {
        1,
        { &D_86A05BC8, &D_86A05BCC },
    },
    {
        0,
        { &D_86A05BD0, &D_86A05BD4 },
    },
    {
        1,
        { &D_86A05BD8, &D_86A05BDC },
    },
    {
        0,
        { &D_86A05BE0, &D_86A05BE4 },
    },
    {
        1,
        { &D_86A05BE8, &D_86A05BEC },
    },
    {
        1,
        { &D_86A05BF0, &D_86A05BF4 },
    },
    {
        1,
        { &D_86A05BF8, &D_86A05BFC },
    },
    {
        0,
        { &D_86A05C00, &D_86A05C04 },
    },
    {
        1,
        { &D_86A05C08, &D_86A05C0C },
    },
    {
        0,
        { &D_86A05C10, &D_86A05C14 },
    },
    {
        1,
        { &D_86A05C18, &D_86A05C1C },
    },
    {
        0,
        { &D_86A05C20, &D_86A05C24 },
    },
    {
        1,
        { &D_86A05C28, &D_86A05C2C },
    },
    {
        0,
        { &D_86A05C30, &D_86A05C34 },
    },
    {
        1,
        { &D_86A05C38, &D_86A05C3C },
    },
    {
        1,
        { &D_86A05C40, &D_86A05C44 },
    },
    {
        1,
        { &D_86A05C48, &D_86A05C4C },
    },
    {
        1,
        { &D_86A05C50, &D_86A05C54 },
    },
    {
        1,
        { &D_86A05C58, &D_86A05C5C },
    },
    {
        0,
        { &D_86A05C60, &D_86A05C64 },
    },
    {
        1,
        { &D_86A05C68, &D_86A05C6C },
    },
    {
        1,
        { &D_86A05C70, &D_86A05C74 },
    },
    {
        1,
        { &D_86A05C78, &D_86A05C7C },
    },
    {
        1,
        { &D_86A05C80, &D_86A05C84 },
    },
    {
        0,
        { &D_86A05C88, &D_86A05C8C },
    },
    {
        1,
        { &D_86A05C90, &D_86A05C94 },
    },
    {
        1,
        { &D_86A05C98, &D_86A05C9C },
    },
    {
        0,
        { &D_86A05CA0, &D_86A05CA4 },
    },
    {
        1,
        { &D_86A05CA8, &D_86A05CAC },
    },
    {
        1,
        { &D_86A05CB0, &D_86A05CB4 },
    },
    {
        1,
        { &D_86A05CB8, &D_86A05CBC },
    },
    {
        1,
        { &D_86A05CC0, &D_86A05CC4 },
    },
    {
        1,
        { &D_86A05CC8, &D_86A05CCC },
    },
    {
        1,
        { &D_86A05CD0, &D_86A05CD4 },
    },
    {
        1,
        { &D_86A05CD8, &D_86A05CDC },
    },
    {
        1,
        { &D_86A05CE0, &D_86A05CE4 },
    },
    {
        0,
        { &D_86A05CE8, &D_86A05CEC },
    },
    {
        1,
        { &D_86A05CF0, &D_86A05CF4 },
    },
    {
        1,
        { &D_86A05CF8, &D_86A05CFC },
    },
    {
        1,
        { &D_86A05D00, &D_86A05D04 },
    },
    {
        1,
        { &D_86A05D08, &D_86A05D0C },
    },
    {
        1,
        { &D_86A05D10, &D_86A05D14 },
    },
    {
        1,
        { &D_86A05D18, &D_86A05D1C },
    },
    {
        1,
        { &D_86A05D20, &D_86A05D24 },
    },
    {
        1,
        { &D_86A05D28, &D_86A05D2C },
    },
    {
        0,
        { &D_86A05D30, &D_86A05D34 },
    },
    {
        1,
        { &D_86A05D38, &D_86A05D3C },
    },
    {
        1,
        { &D_86A05D40, &D_86A05D44 },
    },
    {
        1,
        { &D_86A05D48, &D_86A05D4C },
    },
    {
        1,
        { &D_86A05D50, &D_86A05D54 },
    },
    {
        1,
        { &D_86A05D58, &D_86A05D5C },
    },
    {
        1,
        { &D_86A05D60, &D_86A05D64 },
    },
    {
        1,
        { &D_86A05D68, &D_86A05D6C },
    },
    {
        1,
        { &D_86A05D70, &D_86A05D74 },
    },
    {
        0,
        { &D_86A05D78, &D_86A05D7C },
    },
    {
        1,
        { &D_86A05D80, &D_86A05D84 },
    },
    {
        1,
        { &D_86A05D88, &D_86A05D8C },
    },
    {
        1,
        { &D_86A05D90, &D_86A05D94 },
    },
    {
        1,
        { &D_86A05D98, &D_86A05D9C },
    },
    {
        1,
        { &D_86A05DA0, &D_86A05DA4 },
    },
    {
        1,
        { &D_86A05DA8, &D_86A05DAC },
    },
    {
        1,
        { &D_86A05DB0, &D_86A05DB4 },
    },
    {
        1,
        { &D_86A05DB8, &D_86A05DBC },
    },
    {
        0,
        { &D_86A05DC0, &D_86A05DC4 },
    },
    {
        1,
        { &D_86A05DC8, &D_86A05DCC },
    },
    {
        1,
        { &D_86A05DD0, &D_86A05DD4 },
    },
    {
        1,
        { &D_86A05DD8, &D_86A05DDC },
    },
    {
        1,
        { &D_86A05DE0, &D_86A05DE4 },
    },
    {
        1,
        { &D_86A05DE8, &D_86A05DEC },
    },
    {
        1,
        { &D_86A05DF0, &D_86A05DF4 },
    },
    {
        1,
        { &D_86A05DF8, &D_86A05DFC },
    },
    {
        1,
        { &D_86A05E00, &D_86A05E04 },
    },
    {
        0,
        { &D_86A05E08, &D_86A05E0C },
    },
    {
        1,
        { &D_86A05E10, &D_86A05E14 },
    },
    {
        1,
        { &D_86A05E18, &D_86A05E1C },
    },
    {
        1,
        { &D_86A05E20, &D_86A05E24 },
    },
    {
        0,
        { &D_86A05E28, &D_86A05E2C },
    },
    {
        1,
        { &D_86A05E30, &D_86A05E34 },
    },
    {
        0,
        { &D_86A05E38, &D_86A05E3C },
    },
    {
        1,
        { &D_86A05E40, &D_86A05E44 },
    },
    {
        1,
        { &D_86A05E48, &D_86A05E4C },
    },
    {
        0,
        { &D_86A05E50, &D_86A05E54 },
    },
    {
        1,
        { &D_86A05E58, &D_86A05E5C },
    },
    {
        0,
        { &D_86A05E60, &D_86A05E64 },
    },
    {
        1,
        { &D_86A05E68, &D_86A05E6C },
    },
    {
        1,
        { &D_86A05E70, &D_86A05E74 },
    },
    {
        0,
        { &D_86A05E78, &D_86A05E7C },
    },
    {
        1,
        { &D_86A05E80, &D_86A05E84 },
    },
    {
        0,
        { &D_86A05E88, &D_86A05E8C },
    },
    {
        1,
        { &D_86A05E90, &D_86A05E94 },
    },
    {
        1,
        { &D_86A05E98, &D_86A05E9C },
    },
    {
        0,
        { &D_86A05EA0, &D_86A05EA4 },
    },
    {
        1,
        { &D_86A05EA8, &D_86A05EAC },
    },
    {
        1,
        { &D_86A05EB0, &D_86A05EB4 },
    },
    {
        1,
        { &D_86A05EB8, &D_86A05EBC },
    },
    {
        0,
        { &D_86A05EC0, &D_86A05EC4 },
    },
    {
        1,
        { &D_86A05EC8, &D_86A05ECC },
    },
    {
        0,
        { &D_86A05ED0, &D_86A05ED4 },
    },
    {
        1,
        { &D_86A05ED8, &D_86A05EDC },
    },
    {
        0,
        { &D_86A05EE0, &D_86A05EE4 },
    },
    {
        1,
        { &D_86A05EE8, &D_86A05EEC },
    },
    {
        1,
        { &D_86A05EF0, &D_86A05EF4 },
    },
    {
        0,
        { &D_86A05EF8, &D_86A05EFC },
    },
    {
        1,
        { &D_86A05F00, &D_86A05F04 },
    },
    {
        1,
        { &D_86A05F08, &D_86A05F0C },
    },
    {
        1,
        { &D_86A05F10, &D_86A05F14 },
    },
    {
        1,
        { &D_86A05F18, &D_86A05F1C },
    },
    {
        1,
        { &D_86A05F20, &D_86A05F24 },
    },
    {
        0,
        { &D_86A05F28, &D_86A05F2C },
    },
    {
        1,
        { &D_86A05F30, &D_86A05F34 },
    },
    {
        1,
        { &D_86A05F38, &D_86A05F3C },
    },
    {
        1,
        { &D_86A05F40, &D_86A05F44 },
    },
    {
        1,
        { &D_86A05F48, &D_86A05F4C },
    },
    {
        2,
        { &D_86A05F50, &D_86A05F54 },
    },
    {
        1,
        { &D_86A05F58, &D_86A05F5C },
    },
    {
        1,
        { &D_86A05F60, &D_86A05F64 },
    },
    {
        1,
        { &D_86A05F68, &D_86A05F6C },
    },
    {
        0,
        { &D_86A05F70, &D_86A05F74 },
    },
    {
        1,
        { &D_86A05F78, &D_86A05F7C },
    },
    {
        1,
        { &D_86A05F80, &D_86A05F84 },
    },
    {
        1,
        { &D_86A05F88, &D_86A05F8C },
    },
    {
        1,
        { &D_86A05F90, &D_86A05F94 },
    },
    {
        1,
        { &D_86A05F98, &D_86A05F9C },
    },
    {
        1,
        { &D_86A05FA0, &D_86A05FA4 },
    },
    {
        0,
        { &D_86A05FA8, &D_86A05FAC },
    },
    {
        1,
        { &D_86A05FB0, &D_86A05FB4 },
    },
    {
        0,
        { &D_86A05FB8, &D_86A05FBC },
    },
    {
        1,
        { &D_86A05FC0, &D_86A05FC4 },
    },
    {
        0,
        { &D_86A05FC8, &D_86A05FCC },
    },
    {
        1,
        { &D_86A05FD0, &D_86A05FD4 },
    },
    {
        1,
        { &D_86A05FD8, &D_86A05FDC },
    },
    {
        1,
        { &D_86A05FE0, &D_86A05FE4 },
    },
    {
        1,
        { &D_86A05FE8, &D_86A05FEC },
    },
    {
        0,
        { &D_86A05FF0, &D_86A05FF4 },
    },
    {
        1,
        { &D_86A05FF8, &D_86A05FFC },
    },
    {
        1,
        { &D_86A06000, &D_86A06004 },
    },
    {
        1,
        { &D_86A06008, &D_86A0600C },
    },
    {
        1,
        { &D_86A06010, &D_86A06014 },
    },
    {
        0,
        { &D_86A06018, &D_86A0601C },
    },
    {
        1,
        { &D_86A06020, &D_86A06024 },
    },
    {
        1,
        { &D_86A06028, &D_86A0602C },
    },
    {
        1,
        { &D_86A06030, &D_86A06034 },
    },
    {
        2,
        { &D_86A06038, &D_86A0603C },
    },
    {
        1,
        { &D_86A06040, &D_86A06044 },
    },
    {
        1,
        { &D_86A06048, &D_86A0604C },
    },
    {
        1,
        { &D_86A06050, &D_86A06054 },
    },
    {
        1,
        { &D_86A06058, &D_86A0605C },
    },
    {
        1,
        { &D_86A06060, &D_86A06064 },
    },
    {
        1,
        { &D_86A06068, &D_86A0606C },
    },
    {
        0,
        { &D_86A06070, &D_86A06074 },
    },
    {
        1,
        { &D_86A06078, &D_86A0607C },
    },
    {
        1,
        { &D_86A06080, &D_86A06084 },
    },
    {
        1,
        { &D_86A06088, &D_86A0608C },
    },
    {
        1,
        { &D_86A06090, &D_86A06094 },
    },
    {
        1,
        { &D_86A06098, &D_86A0609C },
    },
    {
        0,
        { &D_86A060A0, &D_86A060A4 },
    },
    {
        1,
        { &D_86A060A8, &D_86A060AC },
    },
    {
        1,
        { &D_86A060B0, &D_86A060B4 },
    },
    {
        0,
        { &D_86A060B8, &D_86A060BC },
    },
    {
        1,
        { &D_86A060C0, &D_86A060C4 },
    },
    {
        0,
        { &D_86A060C8, &D_86A060CC },
    },
    {
        1,
        { &D_86A060D0, &D_86A060D4 },
    },
    {
        1,
        { &D_86A060D8, &D_86A060DC },
    },
    {
        1,
        { &D_86A060E0, &D_86A060E4 },
    },
    {
        0,
        { &D_86A060E8, &D_86A060EC },
    },
    {
        1,
        { &D_86A060F0, &D_86A060F4 },
    },
    {
        1,
        { &D_86A060F8, &D_86A060FC },
    },
    {
        0,
        { &D_86A06100, &D_86A06104 },
    },
    {
        1,
        { &D_86A06108, &D_86A0610C },
    },
    {
        1,
        { &D_86A06110, &D_86A06114 },
    },
    {
        1,
        { &D_86A06118, &D_86A0611C },
    },
    {
        1,
        { &D_86A06120, &D_86A06124 },
    },
    {
        0,
        { &D_86A06128, &D_86A0612C },
    },
    {
        1,
        { &D_86A06130, &D_86A06134 },
    },
};

CreditsRolePage gCreditsRolePages[29] = {
    {
        0,
        360,
        40,
        6,
        gCreditsRoleLines,
    },
    {
        0,
        360,
        40,
        6,
        NULL,
    },
    {
        0,
        368,
        48,
        8,
        NULL,
    },
    {
        0,
        352,
        40,
        6,
        NULL,
    },
    {
        0,
        360,
        40,
        6,
        NULL,
    },
    {
        0,
        368,
        48,
        5,
        NULL,
    },
    {
        0,
        352,
        40,
        3,
        NULL,
    },
    {
        0,
        360,
        40,
        9,
        NULL,
    },
    {
        0,
        368,
        48,
        9,
        NULL,
    },
    {
        0,
        352,
        40,
        9,
        NULL,
    },
    {
        0,
        360,
        40,
        9,
        NULL,
    },
    {
        0,
        368,
        48,
        9,
        NULL,
    },
    {
        0,
        232,
        40,
        4,
        NULL,
    },
    {
        0,
        240,
        40,
        5,
        NULL,
    },
    {
        0,
        240,
        40,
        5,
        NULL,
    },
    {
        0,
        368,
        48,
        9,
        NULL,
    },
    {
        0,
        352,
        40,
        7,
        NULL,
    },
    {
        0,
        360,
        40,
        6,
        NULL,
    },
    {
        0,
        368,
        48,
        9,
        NULL,
    },
    {
        0,
        352,
        40,
        9,
        NULL,
    },
    {
        0,
        240,
        40,
        2,
        NULL,
    },
    {
        0,
        360,
        40,
        10,
        NULL,
    },
    {
        0,
        368,
        48,
        11,
        NULL,
    },
    {
        0,
        352,
        40,
        9,
        NULL,
    },
    {
        0,
        360,
        40,
        9,
        NULL,
    },
    {
        0,
        360,
        40,
        5,
        NULL,
    },
    {
        0,
        240,
        40,
        2,
        NULL,
    },
    {
        0,
        0,
        40,
        0,
        NULL,
    },
    {
        0,
        0,
        0,
        0,
        NULL,
    },
};

void Credits_OutroLoop(void) {
    u8 i;

    Audio_FadeCategories(1, 0x78);
    Audio_FadeCategories(2, 0x78);
    StageFade_StartFromTransparent(0x78);

    for (i = 0; i < 120; i++) {
        Credits_PollInput();
        Credits_DrawFrame(2);
    }

    StageLoader_RunFrames(0x3C);
}

void Credits_InitCamera(void) {
    sSceneCamera = sSceneGraphRoot->children;
    sCameraRotationX = 0x100;
    sCameraRotationY = 0;
    sCameraDistance = 0x78;

    sSceneCamera->perspective.nearPlane = 50.0f;
    sSceneCamera->perspective.farPlane = 6400.0f;
    Camera_ComputeEyeFromAngles(&sSceneCamera->view.targetPosition, &sSceneCamera->view.eyePosition, sCameraDistance, sCameraRotationX, sCameraRotationY);
}

void Credits_InitScene(s32 arg0) {
    MemoryBlock* sp2C;
    MemoryBlock* temp_v0;

    gStageArchive = ASSET_LOAD2(stadium_models, 1, 1);
    GeoNode_CreateFog(0, &gStageFragmentState, 0x3C0, 0x3E8, 0xFF, 0xFF, 0xFF, 0xFF);
    GeoNode_CreateContainer(NULL, &gStageBackgroundNode);
    GeoNode_CreateContainer(NULL, &gStageOverlayNode);
    GeoNode_CreateContainer(NULL, &gStageForegroundNode);
    Credits_SetBackgroundTint(0xFF, 0xFF, 0xFF);
    sp2C = MainPool_AllocState(main_pool_get_available(), 0);
    sSceneGraphRoot = process_geo_layout(sp2C, &gCreditsGeoLayout);
    MainPool_FinalizeAllocation(sp2C);
    ModelRenderer_InitDisplayRoots();
    Credits_InitCamera();
    MessageSequence_Init(1, arg0, gCreditsRolePages);
    gCurrentStageId = -1;

    main_pool_push_state('BACK');
}

s32 Credits_Main(s32 arg0, s32 arg1) {
    StageContext* sp24;

    main_pool_push_state('ROLE');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(0, 1, 3, 1, 1, 1);
    gStageTextObject = Font_Init(4, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_3000000, kids_club_game_ui, 0);
    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    Credits_InitScene(arg1);
    StageContext_Activate(sp24);
    Credits_IntroLoop();
    Credits_MainLoop(arg1);
    Credits_OutroLoop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('ROLE');

    return 0;
}

s32 Credits_InterpolateRoleColor(Color_RGBA8_u32* arg0, u16 arg1, u16 arg2, u8 arg3) {
    u8 i;
    u8 j;
    u8 temp_fv1;

    if (arg1 >= (arg2 - arg3)) {
        return 0;
    }

    gStageFadePalettes[3].unk_00 = (arg2 - arg3) - 0x15;
    gStageFadePalettes[4].unk_00 = (arg2 - arg3) - 1;

    for (i = 0; i < 5; i++) {
        if (arg1 == gStageFadePalettes[i].unk_00) {
            for (j = 0; j < 4; j++) {
                Color_SetRGBA(&arg0[j], gStageFadePalettes[i].unk_03[j].r, gStageFadePalettes[i].unk_03[j].g, gStageFadePalettes[i].unk_03[j].b,
                              gStageFadePalettes[i].unk_02);
            }
            break;
        }

        if ((gStageFadePalettes[i].unk_00 < arg1) && (arg1 < gStageFadePalettes[i + 1].unk_00)) {
            temp_fv1 =
                (s32)ROUND_MAX((((gStageFadePalettes[i + 1].unk_02 - gStageFadePalettes[i].unk_02) * (arg1 - gStageFadePalettes[i].unk_00)) /
                                (f32)(gStageFadePalettes[i + 1].unk_00 - gStageFadePalettes[i].unk_00)) +
                               gStageFadePalettes[i].unk_02);

            for (j = 0; j < 4; j++) {
                arg0[j].r = (s32)ROUND_MAX(
                    (((gStageFadePalettes[i + 1].unk_03[j].r - gStageFadePalettes[i].unk_03[j].r) * (arg1 - gStageFadePalettes[i].unk_00)) /
                     (f32)(gStageFadePalettes[i + 1].unk_00 - gStageFadePalettes[i].unk_00)) +
                    gStageFadePalettes[i].unk_03[j].r);
                arg0[j].g = (s32)ROUND_MAX(
                    (((gStageFadePalettes[i + 1].unk_03[j].g - gStageFadePalettes[i].unk_03[j].g) * (arg1 - gStageFadePalettes[i].unk_00)) /
                     (f32)(gStageFadePalettes[i + 1].unk_00 - gStageFadePalettes[i].unk_00)) +
                    gStageFadePalettes[i].unk_03[j].g);
                arg0[j].b = (s32)ROUND_MAX(
                    (((gStageFadePalettes[i + 1].unk_03[j].b - gStageFadePalettes[i].unk_03[j].b) * (arg1 - gStageFadePalettes[i].unk_00)) /
                     (f32)(gStageFadePalettes[i + 1].unk_00 - gStageFadePalettes[i].unk_00)) +
                    gStageFadePalettes[i].unk_03[j].b);
                arg0[j].a = temp_fv1;
            }

            break;
        }
    }
    return 1;
}

/**
 * The storage at 0x86A06218 is this function's block-scope `static char** sCreditsMessageBank`:
 * only the block-scope spelling produces the target codegen (a file-scope definition,
 * static or not, costs an extra callee-saved register that caches the symbol's address
 * across the loop).
 *
 * IDO emits a block-scope static's `.lcomm` in its internal symbol-table walk order, which
 * for this translation unit interleaves the file's statics and globals with the externs
 * referenced by the gStageFadePalettes/gCreditsRolePages tables. On its own, this static's `.lcomm` would
 * be emitted just ahead of gPhaseDelta and the bss globals would slide four bytes. The six
 * never-defined `lcomm_walk_pad_*` prototypes below exist only to grow that symbol table
 * so the walk places this static between gStageLayoutPtr and gPrevRspCounter; they emit no code,
 * data, or bss. Deleting any one of them, or adding another, shifts the bss layout.
 */
static void lcomm_walk_pad_0(void);
static void lcomm_walk_pad_1(void);
static void lcomm_walk_pad_2(void);
static void lcomm_walk_pad_3(void);
static void lcomm_walk_pad_4(void);
static void lcomm_walk_pad_5(void);

void MessageSequence_Init(u8 arg0, u8 arg1, CreditsRolePage* arg2) {
    static char** sCreditsMessageBank;

    u16 i;
    u16 sp2C;
    CreditRoleEntry* ptr;

    sp2C = Credits_GetRoleCount();
    Text_InitStringTables();
    sCreditsMessageBank = Text_GetStringTable(0x2D);

    for (i = 0, ptr = gCreditsRoleLines; i < sp2C; i++, ptr++) {
        ptr->unk_04[0] = Text_GetString(NULL, 0, sCreditsMessageBank, i);
    }

    gPhaseDelta = arg0;
    gUnusedTimer1 = 0;
    gUnusedTimer2 = 0;
    gStageLayoutPtr = arg2;
}

/**
 * The wrap-around arm widens the counter to 64 bits before subtracting, which is what makes
 * IDO stage the value through the low half of a register pair (addiu t5, v0, 0) and only then
 * do the 32-bit subtract (subu t9, t5, v1). The two chained 0x100000000 subtractions cancel
 * out of the low word (the result is truncated to 8 bits anyway), but they are what puts the
 * pair at t4:t5 instead of t2:t3.
 */
void Stage_UpdateLogic(void) {
    s32 temp_v0 = gRspTaskManager.unk_A34;

    if (gUnusedTimer2 == 0) {
        gUnusedTimer2 = 1;
    } else if ((u32) gPrevRspCounter < (u32) temp_v0) {
        gPhaseDelta = temp_v0 - gPrevRspCounter;
    } else {
        gPhaseDelta = ((u64) (u32) temp_v0 - 0x100000000ULL - 0x100000000ULL) - gPrevRspCounter;
    }
    gPrevRspCounter = temp_v0;
}

void Credits_DrawRoleList(void) {
    u8 var_a1;
    u8 var_s0;
    u8 var_s1;
    u8 i;
    s32 var_s5;
    Color_RGBA8_u32 sp60[4];
    u16 var_s11;
    CreditRoleEntry* ptr;
    CreditsRolePage sp4C;

    if (gStageLayoutPtr == NULL) {
        Credits_DrawContinuePrompt(1);
        return;
    }

    var_s0 = 0;
    var_s1 = 0;

    if (Credits_InterpolateRoleColor(sp60, gStageLayoutPtr->unk_00, gStageLayoutPtr->unk_02, gStageLayoutPtr->unk_04) != 0) {
        Font_BeginTranslucentTextRendering();
        Font_EnableTwoCycleTexturing();
        Font_SetActive(4, 0);

        var_s5 = 0;

        Gfx_SetEnvColor(sp60[0].r, sp60[0].g, sp60[0].b, sp60[0].a);
        Gfx_SetPrimColor(sp60[1].r, sp60[1].g, sp60[1].b, sp60[1].a);

        ptr = gStageLayoutPtr->unk_08;
        for (i = 0; i < gStageLayoutPtr->unk_05; i++, ptr++) {
            switch (ptr->unk_00) {
                case 0:
                    var_s0++;
                    break;

                case 2:
                    var_s1++;
                    break;
            }
        }

        if (var_s0 >= 2) {
            var_a1 = ((var_s0 * 4) + (var_s0 * 0xC) + (var_s1 * 0xA)) - 0xC;
        } else {
            var_a1 = (var_s0 * 4) + (var_s1 * 0xA);
        }

        var_s11 = (((-((gStageLayoutPtr->unk_05 - var_s1) * 0x11) - var_a1) + 0xF0) / 2) & 0xFFFF;

        for (i = 0, ptr = gStageLayoutPtr->unk_08; i < gStageLayoutPtr->unk_05; i++, ptr++) {
            if (ptr->unk_00 == 2) {
                var_s11 += 0xA;
                continue;
            }

            if (var_s5 != ptr->unk_00) {
                switch (ptr->unk_00) {
                    case 0:
                        Gfx_SetEnvColor(sp60[0].r, sp60[0].g, sp60[0].b, sp60[0].a);
                        Gfx_SetPrimColor(sp60[1].r, sp60[1].g, sp60[1].b, sp60[1].a);
                        var_s5 = 0;
                        var_s11 += 0xC;
                        break;

                    case 1:
                        Gfx_SetEnvColor(sp60[2].r, sp60[2].g, sp60[2].b, sp60[2].a);
                        Gfx_SetPrimColor(sp60[3].r, sp60[3].g, sp60[3].b, sp60[3].a);
                        var_s5 = 1;
                        var_s11 += 4;
                        break;
                }
            }

            Font_Printf(0xA0 - (Font_MeasureTextExtent(0, 0, ptr->unk_04[gUnusedTimer1]) / 2), (var_s11 * 4) >> 2,
                          ptr->unk_04[gUnusedTimer1]);
            var_s11 += 0x11;
        }

        Font_DisableTwoCycleTexturing();
        Font_EndTexturedTextRendering();
    }

    gStageLayoutPtr->unk_00 = gStageLayoutPtr->unk_00 + gPhaseDelta;
    if (gStageLayoutPtr->unk_00 >= gStageLayoutPtr->unk_02) {
        sp4C = *gStageLayoutPtr;

        gStageLayoutPtr++;

        gStageLayoutPtr->unk_08 = &sp4C.unk_08[sp4C.unk_05];
        if (gStageLayoutPtr->unk_02 == 0) {
            gStageLayoutPtr = NULL;
        } else {
            gStageLayoutPtr->unk_02 = (gStageLayoutPtr->unk_02 - sp4C.unk_00) + sp4C.unk_02;
            gStageLayoutPtr->unk_00 = 0;
        }
    }
}
