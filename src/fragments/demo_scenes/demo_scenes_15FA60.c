#include "demo_scenes.h"
#include "src/geo_render.h"
#include "src/model_animation.h"
#include "src/geo_layout.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/save_data.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_loop_point.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/fragments/particle_math/particle_math.h"
#include "src/memory.h"
#include "src/stage_loader.h"

typedef struct DioramaColorAnim {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 unk_05;
    /* 0x06 */ u8 unk_06;
    /* 0x07 */ u8 unk_07;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
} DioramaColorAnim; // size = 0xC

extern u32 gFragment17TransitionGradientTex[];
extern DioramaObjectDef* gFragment17ObjectDefs[];
extern DioramaAnimData gFragment17AnimData[];
extern DioramaPresetData gFragment17PresetData[];
extern u32 gFragment17LandStageLayout[];
extern u32 gFragment17SeaStageLayout[];

s32 Diorama_LogoRotateColorCallback(s32, GraphNode*);
s32 Diorama_ScaleRotateCallbackA(s32, GraphNode*);
s32 Diorama_ScaleRotateCallbackB(s32, GraphNode*);
s32 Diorama_GroupStateCallback(s32, GraphNode*);

static s16 D_86B0BBF0 = -0x3C6E;
static s16 D_86B0BBF4 = 0x38E;

static u32 gFragment17GeoLayoutCommands[] = {
    0x0C00FFFF, 0x05000000, 0x0B00001E, 0x00000000, 0x014000F0,  0x0000000F,    0x00000000,    0x00000000,
    0x05000000, 0x0D000000, 0x05000000, 0x1A050000, 0x08000000,  Diorama_GroupStateCallback, 0x00000000,    0x05000000,
    0x0F000002, 0x05000000, 0x06000000, 0x0F000002, 0x05000000,  0x06000000,    0x0F000002,    0x05000000,
    0x1F00FFFF, 0xC3920000, 0x00000000, 0xFC400000, 0x00640064,  0x00640000,    0x08000000,    Diorama_ScaleRotateCallbackA,
    0x00000000, 0x05000000, 0x00000000, gFragment17LandStageLayout, 0x06000000,  0x06000000,    0x0F000002,    0x05000000,
    0x1F00FFFF, 0x00000000, 0x00000000, 0x0514F79A, 0x00640064,  0x00640000,    0x08000000,    Diorama_ScaleRotateCallbackB,
    0x00000000, 0x05000000, 0x00000000, gFragment17SeaStageLayout, 0x06000000,  0x06000000,    0x0F000002,    0x05000000,
    0x06000000, 0x06000000, 0x14000000, 0xF8E40000, 0xFFC89BFF,  0x08000000,    Diorama_LogoRotateColorCallback, 0x00000000,
    0x16FFFFFF, 0x0F000003, 0x05000000, 0x0A000000, &gGraphRoot0, 0x06000000,    0x06000000,    0x06000000,
    0x06000000, 0x01000000,
};

static GraphNode* gFragment17RootNode;
static DioramaState* gFragment17State;
static DioramaColorAnim* gFragment17ColorAnim;
s32 gDioramaEndScreenActive;
#if !defined(VERSION_JP)
// String table 0x14, read by Diorama_DrawEndCaption. The Japanese build has neither, and its .bss
// places D_86B0E5E4 directly after gDioramaEndScreenActive.
char** gDemoScenesControllerWarningBank;
#endif
static BinArchive* D_86B0E5E4;
static u8* D_86B0E5E8;
static DioramaModelSlot gFragment17Objects[4];
static s32 gFragment17WindowAlpha;
static s32 gFragment17Result;
static u16 D_86B0EBB8;

s32 Diorama_LogoRotateColorCallback(s32 arg0, GraphNode* arg1) {
    if (arg0 == 2) {
        unk_D_86002F34_alt18* ptr = (unk_D_86002F34_alt18*)arg1;

        gFragment17ColorAnim->unk_00 += gFragment17ColorAnim->unk_08;
        gFragment17ColorAnim->unk_02 += gFragment17ColorAnim->unk_0A;

        ptr->unk_1C = gFragment17ColorAnim->unk_00;
        ptr->unk_1E = gFragment17ColorAnim->unk_02;
        ptr->r = gFragment17ColorAnim->unk_05;
        ptr->g = gFragment17ColorAnim->unk_06;
        ptr->b = gFragment17ColorAnim->unk_07;
        ptr->a = gFragment17ColorAnim->unk_04;
    }

    return 0;
}

s32 Diorama_ScaleRotateCallbackA(s32 arg0, GraphNode* arg1) {
    if (arg0 == 2) {
        DisplayObject* ptr = (DisplayObject*)arg1;

        ptr->scale.x = 1.5f;
        ptr->scale.y = 1.5f;
        ptr->scale.z = 1.5f;
        ptr->rotation.x = D_86B0BBF0;
    }

    return 0;
}

s32 Diorama_ScaleRotateCallbackB(s32 arg0, GraphNode* arg1) {
    if (arg0 == 2) {
        DisplayObject* ptr = (DisplayObject*)arg1;

        ptr->scale.x = 1.0f;
        ptr->scale.y = 1.0f;
        ptr->scale.z = 1.0f;

        ptr->rotation.x = D_86B0BBF4;
    }
    return 0;
}

s32 Diorama_GroupStateCallback(s32 arg0, GraphNode* arg1) {
    if (arg0 == 2) {
        unk_D_86002F34_alt21* ptr = (unk_D_86002F34_alt21*)arg1;

        ptr->unk_1A = gFragment17State->unk_04.unk_44;
    }
    return 0;
}

void Diorama_ClearModelSlots(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        gFragment17Objects[i].unk_16C = 0;
    }
}

void Diorama_FreeModelSlots(DioramaModelSlot* arg0) {
    while (arg0->unk_16C != 1) {
        ModelRenderer_ClearDisplayObject(&arg0->unk_004);
        arg0++;
    }
    main_pool_pop_state('MONS');
}

void Diorama_PushModelPool(void) {
    main_pool_push_state('MONS');
}

void Diorama_InitModelSlot(DioramaModelSlot* arg0, DioramaObjectDef* arg1) {
    ModelRenderer_AttachDisplayObject(&arg0->unk_004);

    arg0->unk_000 = Model_LoadByArchiveIndex(arg1->unk_0C);

    Model_InitDisplayObject(&arg0->unk_004, 0, arg1->unk_0C, arg0->unk_000->unk_08->unk_00[0]);
    ModelAnim_SetAnimation(&arg0->unk_004, arg1->unk_10);

    if (arg1->unk_0C == 6) {
        arg0->unk_004.effectSlot = 0;
    } else {
        arg0->unk_004.effectSlot = 0xFF;
    }

    ModelAnim_SetFrame(&arg0->unk_004, 0);
    arg0->unk_004.node.renderFlags &= ~0x40;
}

void Diorama_SetModelSlotPosition(DioramaModelSlot* arg0, DioramaObjectDef* arg1) {
    arg0->unk_004.position.x = arg1->unk_00.x;
    arg0->unk_004.position.y = arg1->unk_00.y;
    arg0->unk_004.position.z = arg1->unk_00.z;
}

void Diorama_SetGlowFromGroup(s32 arg0) {
    gFragment17ColorAnim->unk_00 = gFragment17PresetData[arg0].unk_0C;
    gFragment17ColorAnim->unk_02 = gFragment17PresetData[arg0].unk_0E;
    gFragment17ColorAnim->unk_08 = gFragment17PresetData[arg0].unk_18;
    gFragment17ColorAnim->unk_0A = gFragment17PresetData[arg0].unk_1A;
    gFragment17ColorAnim->unk_05 = gFragment17PresetData[arg0].unk_10;
    gFragment17ColorAnim->unk_06 = gFragment17PresetData[arg0].unk_12;
    gFragment17ColorAnim->unk_07 = gFragment17PresetData[arg0].unk_14;
    gFragment17ColorAnim->unk_04 = gFragment17PresetData[arg0].unk_16;
}

void Diorama_LoadModelGroup(s32 arg0) {
    DioramaObjectDef* var_s0 = gFragment17ObjectDefs[arg0];
    DioramaModelSlot* var_s1 = gFragment17Objects;

    Diorama_ClearModelSlots();
    Diorama_PushModelPool();

    while (var_s0->unk_0C != 0x98) {
        Diorama_InitModelSlot(var_s1, var_s0);
        Diorama_SetModelSlotPosition(var_s1, var_s0);
        var_s1->unk_16C = 1;

        var_s1++;
        var_s0++;
    }

    Diorama_SetGlowFromGroup(arg0);
}

void Diorama_SwitchModelGroup(s32 arg0) {
    DioramaObjectDef* sp2C = gFragment17ObjectDefs[arg0];
    DioramaModelSlot* var_s1 = gFragment17Objects;

    Diorama_FreeModelSlots(gFragment17Objects);
    Diorama_ClearModelSlots();
    Diorama_PushModelPool();

    while (sp2C->unk_0C != 0x98) {
        Diorama_InitModelSlot(var_s1, sp2C);
        Diorama_SetModelSlotPosition(var_s1, sp2C);
        var_s1->unk_16C = 1;

        var_s1++;
        sp2C++;
    }

    Diorama_SetGlowFromGroup(arg0);

    if (arg0 == 4) {
        ModelAnim_SetEventTrack(&gFragment17Objects[0].unk_004, 0);
    }
}

void Diorama_SetGroupCamera(s32 arg0) {
    if (arg0 == 4) {
        gFragment17State->unk_00->view.targetPosition.x = -0.04f;
        gFragment17State->unk_00->view.targetPosition.y = 24.2f;
        gFragment17State->unk_00->view.targetPosition.z = -4.08f;
        gFragment17State->unk_00->view.eyePosition.x = 0.04f;
        gFragment17State->unk_00->view.eyePosition.y = 24.2f;
        gFragment17State->unk_00->view.eyePosition.z = 177.41f;
        gFragment17State->unk_00->perspective.verticalFieldOfView = 25.0f;
    } else {
        gFragment17State->unk_04.unk_00.x = gFragment17PresetData[arg0].unk_00.x;
        gFragment17State->unk_04.unk_00.y = gFragment17PresetData[arg0].unk_00.y;
        gFragment17State->unk_04.unk_00.z = gFragment17PresetData[arg0].unk_00.z;

        Diorama_LoadCameraKeyframe(&gFragment17AnimData[arg0], gFragment17State);
        Diorama_SetCameraToKeyframeStart(&gFragment17State->unk_04, gFragment17State);
    }
}

void func_86B00680(void) {
}

void Diorama_LoopIdleAnimations(void) {
    s32 i = 0;

    if ((gFragment17State->unk_04.unk_4C != 5) && (gFragment17State->unk_04.unk_4C != 6) && (gFragment17State->unk_04.unk_4C != 7) &&
        (gFragment17State->unk_04.unk_4C != 8)) {
        if (gFragment17State->unk_04.unk_44 != 4) {
            while (gFragment17ObjectDefs[gFragment17State->unk_04.unk_44][i].unk_0C != 0x98) {
                if (ModelAnim_IsAnimationDone(&gFragment17Objects[i].unk_004) != 0) {
                    ModelAnim_SetFrame(&gFragment17Objects[i].unk_004, 0);
                }
                i++;
            }
        }
    }
}

void Diorama_DrawFadeOverlay(void) {
    if (gFragment17State->unk_04.unk_48 == 1) {
        Gfx_FillRectWithDisplayState(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, gFragment17WindowAlpha);
    }
}

void Diorama_DrawGroupTransitionOverlay(void) {
    if (gFragment17State->unk_04.unk_44 == 2) {
        Diorama_DrawFullscreenTexture(&gFragment17TransitionGradientTex, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, 0x80);
    }
}

s32 Diorama_SlideModelsForward(s32 arg0) {
    DioramaModelSlot* var_v0 = gFragment17Objects;
    DioramaObjectDef* var_v1 = gFragment17ObjectDefs[arg0];
    s32 var_a1 = 1;

    while (var_v1->unk_0C != 0x98) {
        var_v0->unk_004.position.z += 20.0f;
        if (var_v0->unk_004.position.z <= 650.0f) {
            var_a1 = 0;
        }
        var_v0++;
        var_v1++;
    }
    return var_a1;
}

void Diorama_BeginGroupTransition(void) {
    StageContext_SetClearColor(1);
    StageFade_StartFromTransparent(0xC);
    gFragment17State->unk_04.unk_4C = 0;
    gFragment17State->unk_04.unk_50 = 0;
}

void Diorama_FinishSequence(void) {
    gFragment17State->unk_04.unk_4C = 9;
    gFragment17State->unk_04.unk_50 = 0;
    gFragment17Objects->unk_004.node.flags &= ~1;
    gDioramaEndScreenActive = 1;
}

void Diorama_UpdateProximityHighlight(s32 arg0) {
    DioramaObjectDef* ptr1 = gFragment17ObjectDefs[arg0];
    DioramaModelSlot* ptr2 = &gFragment17Objects[0];
    s16 sp88[] = {
        // D_86B0BD20
        0x1554,
        0x1554,
        0x1554,
        0x1554,
    };
    s16 sp80[] = {
        // D_86B0BD28
        0x18E2,
        0x11C6,
        0x1554,
        0x1554,
    };
    f32 sp70[] = { 1000.0f, 1000.0f, 2000.0f, 2000.0f }; // D_86B0BD30
    f32 sp60[] = { 1.0f, 1.0f, 100.0f, 50.0f };          // D_86B0BD40

    while (ptr1->unk_0C != 0x98) {
        if (Diorama_IsModelInCameraCone(gFragment17State->unk_00, &ptr2->unk_004, sp88[arg0], sp80[arg0], sp70[arg0], sp60[arg0]) != 0) {
            ptr2->unk_004.node.flags |= 1;
        } else {
            ptr2->unk_004.node.flags &= ~1;
        }
        ptr2++;
        ptr1++;
    }
}

void Diorama_ReadControllerInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void Diorama_InitSceneState(void) {
    gFragment17State = main_pool_alloc(sizeof(DioramaState), 0);
    bzero(gFragment17State, sizeof(DioramaState));

    gFragment17ColorAnim = main_pool_alloc(sizeof(DioramaColorAnim), 0);
    bzero(gFragment17ColorAnim, sizeof(DioramaColorAnim));

    gFragment17State->unk_00 = gFragment17RootNode->children;

    gFragment17State->unk_00->perspective.nearPlane = 50.0f;
    gFragment17State->unk_00->perspective.farPlane = 8400.0f;
    gFragment17State->unk_00->perspective.verticalFieldOfView = 30.0f;
    gFragment17State->unk_00->view.targetPosition.x = 50.0f;
    gFragment17State->unk_00->view.targetPosition.y = 50.0f;
    gFragment17State->unk_00->view.targetPosition.z = 50.0f;
    gFragment17State->unk_00->view.eyePosition.x = 0.0f;
    gFragment17State->unk_00->view.eyePosition.y = 0.0f;
    gFragment17State->unk_00->view.eyePosition.z = 0.0f;
    gFragment17State->unk_04.unk_4C = 8;
    gFragment17State->unk_04.unk_50 = 0;
    gFragment17State->unk_04.unk_48 = 0;
}

s32 Diorama_UpdateStateMachine(void) {
    // The Japanese build skips the diorama with A (0x8000) or B (0x4000) only; Start (0x1000) is
    // not tested (its code has no andi 0x1000 before the A/B tests).
#if defined(VERSION_JP)
    if (((gPlayer1Controller->buttonPressed & 0x8000) || (gPlayer1Controller->buttonPressed & 0x4000)) &&
        (gFragment17State->unk_04.unk_48 == 0)) {
#else
    if (((gPlayer1Controller->buttonPressed & 0x1000) || (gPlayer1Controller->buttonPressed & 0x8000) ||
         (gPlayer1Controller->buttonPressed & 0x4000)) &&
        (gFragment17State->unk_04.unk_48 == 0)) {
#endif
        if ((gFragment17State->unk_04.unk_4C != 8) && (gFragment17State->unk_04.unk_4C != 5)) {
            gFragment17State->unk_04.unk_48 = 1;
            Audio_FadeOutAll(0xA);
            gFragment17WindowAlpha = 0;
            gFragment17Result = 2;
        }
    }

    if (gFragment17State->unk_04.unk_48 == 1) {
        gFragment17WindowAlpha = Math_StepToS32(gFragment17WindowAlpha, 0xFF, 0x1E, 0x1E);
        if (gFragment17WindowAlpha == 0xFF) {
            return 1;
        }
    }

    switch (gFragment17State->unk_04.unk_4C) {
        case 5:
            if (gFragment17State->unk_04.unk_48 == 1) {
                return 1;
            }

            if (gFragment17State->unk_04.unk_44 == 4) {
                gFragment17State->unk_04.unk_50 = 0;
                gFragment17Result = 1;
                return 1;
            }

            gFragment17State->unk_04.unk_44++;
            Diorama_SetGroupCamera(gFragment17State->unk_04.unk_44);
            Diorama_SwitchModelGroup(gFragment17State->unk_04.unk_44);
            gFragment17State->unk_04.unk_4C = 6;
            gFragment17State->unk_04.unk_50 = 0;
            break;

        case 6:
            if (gFragment17State->unk_04.unk_50++ >= 4) {
                StageContext_SetClearColor(1);
                StageFade_StartFromOpaque(0x20);
                gFragment17State->unk_04.unk_4C = 7;
                if ((gFragment17State->unk_04.unk_44 == 0) || (gFragment17State->unk_04.unk_44 == 1)) {
                    Diorama_UpdateCameraKeyframe(&gFragment17State->unk_04, gFragment17State);
                }
            }
            break;

        case 7:
            if (gFragment17State->unk_04.unk_44 == 4) {
                if (ModelAnim_IsAnimationDone(&gFragment17Objects[0].unk_004) != 0) {
                    Diorama_FinishSequence();
                }
            } else {
                if (gFragment17State->unk_04.unk_44 >= 2) {
                    if (Diorama_SlideModelsForward(gFragment17State->unk_04.unk_44) != 0) {
                        Diorama_BeginGroupTransition();
                    }
                } else if (gFragment17State->unk_04.unk_6C < gFragment17State->unk_04.unk_30) {
                    Diorama_BeginGroupTransition();
                }

                if ((gFragment17State->unk_04.unk_44 == 0) || (gFragment17State->unk_04.unk_44 == 1)) {
                    Diorama_UpdateCameraKeyframe(&gFragment17State->unk_04, gFragment17State);
                }

                Diorama_UpdateProximityHighlight(gFragment17State->unk_04.unk_44);
            }
            break;

        case 8:
            if (gFragment17State->unk_04.unk_50++ >= 0xB) {
                gFragment17State->unk_04.unk_44 = 0;
                Diorama_SetGroupCamera(gFragment17State->unk_04.unk_44);
                gFragment17State->unk_04.unk_4C = 6;
                gFragment17State->unk_04.unk_50 = 0;
                Diorama_LoadModelGroup(gFragment17State->unk_04.unk_44);
                Audio_StartMusicTrack(0x29);
            }
            break;

        case 9:
            if (gFragment17State->unk_04.unk_50++ >= 3) {
                gFragment17State->unk_04.unk_4C = 5;
                gFragment17State->unk_04.unk_50 = 0;
            }
            break;

        default:
            if (gFragment17State->unk_04.unk_44 != 4) {
                if (gFragment17State->unk_04.unk_44 >= 2) {
                    Diorama_SlideModelsForward(gFragment17State->unk_04.unk_44);
                } else {
                    Diorama_UpdateCameraKeyframe(&gFragment17State->unk_04, gFragment17State);
                }
            }

            if (gFragment17State->unk_04.unk_50++ >= 0xE) {
                gFragment17State->unk_04.unk_4C = 5;
                gFragment17State->unk_04.unk_50 = 0;
            }
            break;
    }

    return 0;
}

void func_86B01004(void) {
}

void Diorama_RenderFrame(void) {
    BgStage_DrawFrame();
    GfxImage_FillCurrent(&gDisplayListHead, 1);
    GeoRender_AdvanceFrameCounter();
    Diorama_DrawGroupTransitionOverlay();
    Diorama_DrawEndBackgroundTiles(D_86B0E5E8);
    Geo_RenderRootNode(gFragment17RootNode);
    Diorama_DrawFadeOverlay();
#if !defined(VERSION_JP)
    // The Japanese build has no Diorama_DrawEndCaption.
    //@src/fragments/demo_scenes/demo_scenes_161590.c
    Diorama_DrawEndCaption();
#endif
    BgStage_AdvanceFrame();
}

void Diorama_RunLoop(void) {
    s32 var_s1 = 1;
    s32 var_s0 = 0;

    while (var_s1 != 0) {
        Diorama_ReadControllerInput();
        Particle_UpdateFrameCounters();

        if (Diorama_UpdateStateMachine() != 0) {
            var_s1 = 0;
        }

        var_s0++;
        if (var_s0 == 0x50) {
            var_s0 = 0;
        }

        Diorama_LoopIdleAnimations();
        Diorama_RenderFrame();
    }
}

void Diorama_RenderFinalFrame(void) {
    Diorama_RenderFrame();
}

void Diorama_InitScene(void) {
    MemoryBlock* sp1C;

    sp1C = MainPool_AllocState(main_pool_get_available(), 0);
    gFragment17RootNode = process_geo_layout(sp1C, &gFragment17GeoLayoutCommands);

    MainPool_FinalizeAllocation(sp1C);
    ModelRenderer_InitDisplayRoots();
    PokeIcon_OpenModelArchives();
    Diorama_InitSceneState();

    gFragment17WindowAlpha = 0;
    gDioramaEndScreenActive = 0;
}

s32 Stage_ShowBootDiorama(void) {
    main_pool_push_state('MINI');

    Font_Init(6, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);
    Fragment_Load((((u32)D_8D000000 & 0x0FF00000) >> 0x14) - 0x10, _5C7A70_ROM_START, _5C7A70_ROM_END);

    Save_EnsureBankLoaded(2);
    Save_GetModeSettings(&D_86B0EBB8, 0);
    D_86B0E5E4 = ASSET_LOAD2(backgrounds, 1, 1);

    // The Japanese build compares the mode settings with 0x1FE instead of 0x1F8 when choosing
    // background file 0x11, and loads no string table (no Text_* calls before Diorama_InitScene).
#if defined(VERSION_JP)
    if (D_86B0EBB8 == 0x1FE) {
#else
    if (D_86B0EBB8 == 0x1F8) {
#endif
        D_86B0E5E8 = BinArchive_GetFile(D_86B0E5E4, 0x11);
    } else {
        D_86B0E5E8 = BinArchive_GetFile(D_86B0E5E4, 0);
    }

#if !defined(VERSION_JP)
    Text_InitStringTables();
    gDemoScenesControllerWarningBank = Text_GetStringTable(0x14);
#endif
    Diorama_InitScene();
    StageLoader_UpdateSegments();
    Diorama_RunLoop();
    Diorama_RenderFinalFrame();
    StageLoader_WaitForRetrace();
    Font_Free();

    main_pool_pop_state('MINI');

    return gFragment17Result;
}
