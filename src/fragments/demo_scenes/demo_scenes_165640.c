#include "demo_scenes.h"
#include "src/geo_node.h"
#include "src/geo_render.h"
#include "src/model_animation_events.h"
#include "src/geo_layout.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/game_state.h"
#include "src/jpeg_stream.h"
#include "src/audio_loop_point.h"
#include "src/audio_commands_category2.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/controller.h"
#include "src/fragments/minigame_actor_framework/minigame_actor_framework.h"
#include "src/fragments/particle_math/particle_math.h"
#include "src/memory.h"
#include "src/stage_loader.h"
#include "src/util.h"
#include "src/gb_save.h"

extern u8 D_3000180[];
extern u8 D_3000780[];
extern u8 D_3001220[];
extern u8 D_3001B80[];
extern u8 D_30024E0[];
extern u8 D_30032A0[];
extern u8 D_3004060[];
extern u8 D_3004E20[];
extern u8 D_3005780[];
extern u8 D_30060E0[];
extern u8 D_3006B80[];
extern u8 D_300C6D0[];

s32 Intro_LightAngleCallbackA(s32, GraphNode*);
s32 Intro_LightAngleCallbackB(s32, GraphNode*);
s32 Intro_FogColorCallback(s32, GraphNode*);

typedef struct unk_D_86B0DD58 {
    /* 0x00 */ u8 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
} unk_D_86B0DD58; // size = 0xC

static SpriteColorParams gUIElementColors[4] = {
    {
        0x00FF,
        0x00E1,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
    },
    {
        0x00FF,
        0x00FF,
        0x00FF,
        0x00FF,
        0x00FF,
        0x0064,
        0x00FF,
        0x0007,
        0x0040,
        0x0040,
        0x0040,
        0x0019,
    },
    {
        0x00FF,
        0x00FF,
        0x009B,
        0x009B,
        0x009B,
        0x0000,
        0x00FF,
        0x00FF,
        0x0032,
        0x0032,
        0x0032,
        0x0032,
    },
    {
        0x00FF,
        0x00FF,
        0x00FF,
        0x00FF,
        0x00FF,
        0x0064,
        0x00FF,
        0x00FF,
        0x0080,
        0x0080,
        0x0080,
        0x0032,
    },
};
static u8 gUIElementColorFlat[] = {
    0, 0xFF, 0, 0xE1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0, 3, 0, 0x11, 0, 0x11, 0, 0x11, 0, 7,
};

static unk_D_86B0DD58 gMonsterSpawnInfo[20] = {
    { 0xA7, 47.5f, 30.0f }, { 0xA8, 47.5f, 30.0f }, { 0xA9, 47.5f, 30.0f }, { 0xAA, 47.5f, 30.0f },
#if defined(VERSION_US_REV2)
    { 0xA5, 66.5f, 85.0f }, { 0xA6, 50.0f, 55.0f }, { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },
#else
    { 0xA5, 66.5f, 85.0f }, { 0xA5, 66.5f, 85.0f }, { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },
#endif
    { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },
    { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },
    { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },   { 0xA7, 0.0f, 0.0f },
};

static u8 gStageModelIndices[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
};

static FogRect gIntroOverlayRect;
static DisplayNodeHeader gMiniCameraRoot0;
static DisplayNodeHeader gMiniCameraRoot1;
static DisplayNodeHeader gMiniCameraRoot2;
static u32 gMiniCameraBgColor;
static Color_RGBA8_u32 gOverlayColor;
static u8 gMonsterSfxTriggered;
static s16 gCurrentMonsterModelId;
static s16 gCurrentMonsterSpawnIdx;
static s16 gMonsterLandingState;
static f32 gMonsterLandingYVel;
static void* gMonsterModelArchive;
static unk_D_86B0FA50* gIntroTextPanelState;
static unk_D_86B0FA50* gUIElementHealthBar;
static unk_D_86B0FA50* gUIElementHealthFill;
static unk_D_86B0FA50* gUIElementStatus;
static unk_D_86B0FA50* gUIElementBorder;
static s16 gCameraRotationYSlew;
static s16 gCameraZoomRotationX;
static s16 gCameraZoomRotationY;
static f32 gCameraZoomDist;
static s32 gBattleSequenceFrameCount;
static s32 gBattleSequenceStage;
unk_D_86B0FA78* gCameraTarget;
unk_D_86B0FA7C* gCameraState;
static u8 gMonsterModelData[0xBE0];
unk_70D3A0 gMonsterStageData;
static unk_D_70B10 gMonsterStageParams;
static TeamRoster* gMonsterDataPtr;
static BattleSessionTeams* gMonsterContextPtr;

static u32 gMiniCameraLayout[] = {
    0x0C00FFFF, 0x05000000,    0x0B00001E,  0x00000000,    0x014000F0,  0x0000000F,  0x00000000, 0x00000000,
    0x05000000, 0x0D000000,    0x05000000,  0x07000000,    &gIntroOverlayRect, 0x14000000,  0x002D0019, 0xFFFFFF28,
    0x08000000, Intro_LightAngleCallbackA, 0x00000000,  0x14000000,    0x002D0019,  0x80808028,  0x08000000, Intro_LightAngleCallbackB,
    0x00000000, 0x16646464,    0x0F000002,  0x05000000,    0x1F00FFFF,  0x00000000,  0x00000000, 0x00000000,
    0x00640064, 0x00640000,    0x08000000,  Intro_FogColorCallback, 0x00000000,  0x05000000,  0x07000000, &gMiniCameraRoot0,
    0x06000000, 0x06000000,    0x0F000003,  0x05000000,    0x1F00FFFF,  0x00000000,  0x00000000, 0x00000000,
    0x00640064, 0x00640000,    0x08000000,  Intro_FogColorCallback, 0x00000000,  0x05000000,  0x07000000, &gMiniCameraRoot1,
    0x06000000, 0x06000000,    0x0F000002,  0x05000000,    0x1F00FFFF,  0x00000000,  0x00000000, 0x00000000,
    0x00640064, 0x00640000,    0x08000000,  Intro_FogColorCallback, 0x00000000,  0x05000000,  0x07000000, &gMiniCameraRoot2,
    0x06000000, 0x06000000,    0x0F000003,  0x05000000,    0x0A000000,  &gGraphRoot0, 0x06000000, 0x0F000002,
    0x05000000, 0x0A000000,    &gGraphRoot1, 0x06000000,    0x09000000,  0x06000000,  0x06000000, 0x06000000,
    0x01000000,
};

s32 Intro_LightAngleCallbackA(s32 arg0, GraphNode* arg1) {
    unk_D_86002F34_alt18* ptr = (unk_D_86002F34_alt18*)arg1;
    f32 sp28;
    s16 sp26;
    s16 sp24;

    if (arg0 == 2) {
        Vec3f_CalculateDistanceAngles(&gCurrentCameraNode->view.targetPosition, &gCurrentCameraNode->view.eyePosition, &sp28, &sp26, &sp24);
        ptr->unk_1C = sp26 + 0x2000;
        ptr->unk_1E = sp24 - 0x2000;
    }

    return 0;
}

s32 Intro_LightAngleCallbackB(s32 arg0, GraphNode* arg1) {
    unk_D_86002F34_alt18* ptr = (unk_D_86002F34_alt18*)arg1;
    f32 sp28;
    s16 sp26;
    s16 sp24;

    if (arg0 == 2) {
        Vec3f_CalculateDistanceAngles(&gCurrentCameraNode->view.targetPosition, &gCurrentCameraNode->view.eyePosition, &sp28, &sp26, &sp24);
        ptr->unk_1C = -0x2000 - sp26;
        ptr->unk_1E = sp24 + 0x6000;
    }

    return 0;
}

s32 Intro_FogColorCallback(s32 arg0, GraphNode* arg1) {
    DisplayObject* ptr = (DisplayObject*)arg1;

    if (arg0 == 2) {
        ptr->textureTint.rgba = gOverlayColor.rgba;
    }

    return 0;
}

void Intro_SetBackgroundTint(u8 arg0, u8 arg1, u8 arg2) {
    u8 v = arg0 & arg1 & arg2;

    gOverlayColor.r = arg0;
    gOverlayColor.g = arg1;
    gOverlayColor.b = arg2;

    if (v == 0xFF) {
        if (gIntroOverlayRect.node.callbackArg == 1) {
            gIntroOverlayRect.node.flags |= 1;
        }
    } else {
        gIntroOverlayRect.node.flags &= ~1;
    }
}

void Intro_LoadPokemonTextures(void) {
    UNUSED s32 pad[2];
    u8* sp24;
    u8* temp_a1;
    u8* sp1C;

    temp_a1 = (u32)_70D3A0_ROM_START + (gStageRomOffsets[gCameraTarget->unk_174 - 1] & 0xFFFFFF);
    sp24 = (u32)_70D3A0_ROM_START +
           ((((gCameraTarget->unk_174 ^ 0) * sizeof(unk_D_70B10)) - sizeof(unk_D_70B10) + (u32)D_70B10) & 0xFFFFFF);
    sp1C = (u32)_70D3A0_ROM_START +
           (((gCameraTarget->unk_174 * sizeof(unk_70D3A0)) - sizeof(unk_70D3A0) + (u32)D_70110) & 0xFFFFFF);

    Dma_WriteChunks(gMonsterModelData, temp_a1, temp_a1 + 0xBC0, 0);
    Dma_WriteChunks(&gMonsterStageData, sp1C, sp1C + sizeof(unk_70D3A0), 0);
    Dma_WriteChunks(&gMonsterStageParams, sp24, sp24 + sizeof(unk_D_70B10), 0);
}

void Intro_LoadShowcaseModel(void) {
    gCameraTarget->unk_174 = gMonsterDataPtr->pokemonPicked[gCameraTarget->unk_17C].unk_00.unk_00;
    ModelRenderer_AttachDisplayObject(&gCameraTarget->unk_004);
    PokeIcon_OpenModelArchives();

    gCameraTarget->unk_000 = Model_LoadForPokemon(&gMonsterDataPtr->pokemonPicked[gCameraTarget->unk_17C]);
    Model_InitDisplayObject(&gCameraTarget->unk_004, 0, gCameraTarget->unk_174, gCameraTarget->unk_000->unk_08->unk_00[0]);
    ModelAnim_SetAnimation(&gCameraTarget->unk_004, 0);
    gCameraTarget->unk_004.effectSlot = 0;
    Intro_LoadPokemonTextures();
    gCameraTarget->unk_004.position.y += gMonsterStageData.unk_08;
    gCameraTarget->unk_170 = 0;
    ModelAnim_SetEventFrame(&gCameraTarget->unk_004, 0);
    ModelAnim_SetEventTrack(&gCameraTarget->unk_004, -1);
}

void Intro_ReplaceShowcaseModel(void) {
    ModelRenderer_ClearDisplayObject(&gCameraTarget->unk_004);

    main_pool_pop_state('MONS');

    main_pool_push_state('MONS');

    Intro_LoadShowcaseModel();
}

void Intro_SetupOrbitCamera(void) {
    gCameraState->unk_00->perspective.verticalFieldOfView = 30.0f;
    gCameraState->unk_00->perspective.nearPlane = 30.0f;
    gCameraState->unk_00->perspective.farPlane = 6400.0f;
    Intro_ComputeFollowPoint(gCameraTarget, &gCameraState->unk_04);
    Intro_LoadCameraKeyframe(&gIntroCameraKeyframes[gCameraTarget->unk_17C], gCameraState->unk_00);
    Intro_UpdateOrbitCamera();
}

void Intro_LoadStageModel(void) {
    ModelRenderer_ClearDisplayObject(&gCameraTarget->unk_004);

    main_pool_pop_state('MONS');

    main_pool_push_state('MONS');

    gCameraTarget->unk_174 = gMonsterSpawnInfo[gCurrentMonsterSpawnIdx].unk_00;
    ModelRenderer_AttachDisplayObject(&gCameraTarget->unk_004);
    PokeIcon_OpenModelArchives();
    gCameraTarget->unk_000 = Model_LoadByArchiveIndex(gCameraTarget->unk_174);
    Model_InitDisplayObject(&gCameraTarget->unk_004, 0, gCameraTarget->unk_174, gCameraTarget->unk_000->unk_08->unk_00[0]);
    ModelAnim_SetAnimation(&gCameraTarget->unk_004, 0);
    Vec3f_SetComponentsDuplicate(&gCameraTarget->unk_004.position, 0.0f, 300.0f, 0.0f);
}

void Intro_SetupStaticCamera(void) {
    gMonsterLandingState = 0;
    gMonsterLandingYVel = 0.0f;
    gCameraState->unk_00->perspective.verticalFieldOfView = 40.0f;
    gCameraState->unk_00->perspective.nearPlane = 30.0f;
    gCameraState->unk_00->perspective.farPlane = 6400.0f;

    Vec3f_SetComponentsDuplicate(&gCameraState->unk_00->view.targetPosition, gCameraTarget->unk_004.position.x,
                  gMonsterSpawnInfo[gCurrentMonsterSpawnIdx].unk_04 + gCameraTarget->unk_004.position.y, gCameraTarget->unk_004.position.z);

    if ((gCurrentMonsterSpawnIdx == 4) || (gCurrentMonsterSpawnIdx == 5)) {
        Vec3f_SetComponentsDuplicate(&gCameraState->unk_00->view.eyePosition, 0, 100.0f, 328.0f);
    } else {
        Vec3f_SetComponentsDuplicate(&gCameraState->unk_00->view.eyePosition, 0, 100.0f, 250.0f);
    }
}

s32 Intro_StepRotationOvershoot(DisplayObject* arg0, s16 arg1, s16 arg2) {
    if (arg1 > 0) {
        arg0->rotation.y -= arg1;
        if ((arg0->rotation.y < arg2) && ((arg2 - arg1) < arg0->rotation.y)) {
            return 1;
        }
    } else {
        arg0->rotation.y -= arg1;
        if ((arg2 < arg0->rotation.y) && (arg0->rotation.y < (arg2 - arg1))) {
            return 1;
        }
    }
    return 0;
}

s32 Intro_StepRotationToTarget(DisplayObject* arg0, s16 arg1, s16 arg2) {
    if ((((arg0->rotation.y - arg2) / 182) < 0x3D) && (((arg0->rotation.y - arg2) / 182) >= 0)) {
        arg1 = (arg0->rotation.y - arg2) / 9;
    }
    arg0->rotation.y -= arg1;

    if (arg1 > 0) {
        if ((arg2 >= arg0->rotation.y) && (arg0->rotation.y >= (arg2 - arg1))) {
            arg0->rotation.y = arg2;
            return 1;
        }
    } else if ((arg0->rotation.y >= arg2) && ((arg2 - arg1) >= arg0->rotation.y)) {
        arg0->rotation.y = arg2;
        return 1;
    }
    return 0;
}

s32 Intro_UpdateModelLanding(void) {
    if (gMonsterLandingState < 3) {
        if ((gMonsterLandingState <= 0) && (Intro_StepRotationOvershoot(&gCameraTarget->unk_004, 0x71C, 0) != 0)) {
            gMonsterLandingState++;
        }

        if ((gMonsterLandingState > 0) && (Intro_StepRotationToTarget(&gCameraTarget->unk_004, 0x71C, 0) != 0)) {
            gMonsterLandingState++;
        }
    }

    Math_EaseTowardF(&gMonsterLandingYVel, 0.1f, 0.02f);
    Math_EaseTowardF(&gCameraTarget->unk_004.position.y, 0.0f, gMonsterLandingYVel);

    if (gCameraTarget->unk_004.position.y <= 0.25f) {
        gCameraTarget->unk_004.position.y = 0.0f;
    }

    Vec3f_SetComponentsDuplicate(&gCameraState->unk_00->view.targetPosition, gCameraTarget->unk_004.position.x,
                  gMonsterSpawnInfo[gCurrentMonsterSpawnIdx].unk_04 + gCameraTarget->unk_004.position.y, gCameraTarget->unk_004.position.z);

    if (gCameraTarget->unk_004.position.y <= 0.5f) {
        return 1;
    }
    return 0;
}

void Intro_EmitLandingParticles(void) {
    Vec3f sp44;

    if (gMonsterSfxTriggered == 0) {
        gMonsterSfxTriggered = 1;
        Audio_PlayCategory11SoundCommand(0x0110000F, 0, 0);
    }

    if (gCameraTarget->unk_178 & 1) {
        Vec3f_SetComponentsDuplicate(&sp44, gCameraTarget->unk_004.position.x,
                      gMonsterSpawnInfo[gCurrentMonsterSpawnIdx].unk_08 + (MathUtil_RandomU16Range(0x28) - 0x14), gCameraTarget->unk_004.position.z);
        Particle_EmitBurstAtTransform(1.0f, sp44, gCameraTarget->unk_004.rotation, MiniFx_UpdatePalette17DelayedRevealFallParticle, &gMiniFxParticleDescriptorIa8Variant, 1);

        if ((gCameraTarget->unk_178 >= 0xA) && (gCameraTarget->unk_178 & 1)) {
            Vec3f_SetComponentsDuplicate(&sp44, gCameraTarget->unk_004.position.x,
                          gMonsterSpawnInfo[gCurrentMonsterSpawnIdx].unk_08 + (MathUtil_RandomU16Range(0x28) - 0x14), gCameraTarget->unk_004.position.z);
            Particle_EmitBurstAtTransform(1.0f, sp44, gCameraTarget->unk_004.rotation, MiniFx_UpdatePalette17DelayedRevealFallParticle, &gMiniFxParticleDescriptorIa8Variant, 1);
        }

        if ((gCameraTarget->unk_178 >= 0x14) && (gCameraTarget->unk_178 & 1)) {
            Vec3f_SetComponentsDuplicate(&sp44, gCameraTarget->unk_004.position.x,
                          gMonsterSpawnInfo[gCurrentMonsterSpawnIdx].unk_08 + (MathUtil_RandomU16Range(0x28) - 0x14), gCameraTarget->unk_004.position.z);
            Particle_EmitBurstAtTransform(1.0f, sp44, gCameraTarget->unk_004.rotation, MiniFx_UpdatePalette17DelayedRevealFallParticle, &gMiniFxParticleDescriptorIa8Variant, 1);
        }

        if ((gCameraTarget->unk_178 >= 0x1E) && (gCameraTarget->unk_178 & 1)) {
            Vec3f_SetComponentsDuplicate(&sp44, gCameraTarget->unk_004.position.x,
                          gMonsterSpawnInfo[gCurrentMonsterSpawnIdx].unk_08 + (MathUtil_RandomU16Range(0x28) - 0x14), gCameraTarget->unk_004.position.z);
            Particle_EmitBurstAtTransform(1.0f, sp44, gCameraTarget->unk_004.rotation, MiniFx_UpdatePalette17DelayedRevealFallParticle, &gMiniFxParticleDescriptorIa8Variant, 1);
        }
    }
}

void Intro_FadeInBackgroundTint(void) {
    gCameraTarget->unk_180 = Math_StepToF(gCameraTarget->unk_180, 100.0f, 2.0f, 2.0f);
    Intro_SetBackgroundTint(gCameraTarget->unk_180, gCameraTarget->unk_180, gCameraTarget->unk_180);
}

void Intro_UpdateCryEventTrack(void) {
    if (((gCameraTarget->unk_178 % 50) == 0) && (gCameraTarget->unk_170 == 0) && (gMonsterModelData[0xA51] != 0xFF)) {
        ModelAnim_ClearEventTrack(&gCameraTarget->unk_004);
        ModelAnim_SetEventTrack(&gCameraTarget->unk_004, gMonsterModelData[0xA51]);
        gCameraTarget->unk_170 = 1;
    } else if ((gCameraTarget->unk_170 == 1) && (ModelAnim_IsEventTrackNearEnd(&gCameraTarget->unk_004) == 1)) {
        gCameraTarget->unk_170 = 0;
        ModelAnim_SetEventFrame(&gCameraTarget->unk_004, 0);
        ModelAnim_SetEventTrack(&gCameraTarget->unk_004, -1);
    }
}

void Intro_SetupTextPanel(unk_D_86B0FA50* arg0, u8* arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5) {
    arg0->unk_80 = arg1;
    arg0->unk_98 = arg2;
    arg0->unk_9C = arg3;
    arg0->unk_9E = arg4;
    arg0->unk_A0 = arg5;

    arg0->unk_88 = 0;
    arg0->unk_8C = 0;
    arg0->unk_84 = 0;
    arg0->unk_A4 = 0;
    arg0->unk_AC = 0.0f;

    arg0->unk_B0.unk_00 = gUIElementColors->unk_00;
    arg0->unk_B0.unk_02 = gUIElementColors->unk_02;
    arg0->unk_B0.unk_04 = gUIElementColors->unk_04;
    arg0->unk_B0.unk_06 = gUIElementColors->unk_06;
    arg0->unk_B0.unk_08 = gUIElementColors->unk_08;
    arg0->unk_B0.unk_0A = gUIElementColors->unk_0A;
}

void Intro_ConfigureTextPanels(void) {
    if (gSessionContext.inRound2 == 1) {
        Intro_SetupTextPanel(gUIElementBorder, &D_300C6D0, -250.0f, 0xC8, 0x18, 0x18);
    }

    Intro_SetupTextPanel(gUIElementHealthBar, &D_3000180, -250.0f, 0xC8, 0x40, 0x18);

    if (gSessionContext.modeSelection == 0) {
        Intro_SetupTextPanel(gUIElementStatus, &D_30060E0, -170.0f, 0x3C, 0x88, 0x14);
    } else if (gSessionContext.modeSelection == 1) {
        Intro_SetupTextPanel(gUIElementStatus, &D_3006B80, -170.0f, 0x3C, 0x78, 0x14);
    } else if (gSessionContext.modeSelection == 2) {
        Intro_SetupTextPanel(gUIElementStatus, &D_3004E20, -170.0f, 0x3C, 0x78, 0x14);
    } else if (gSessionContext.modeSelection == 3) {
        Intro_SetupTextPanel(gUIElementStatus, &D_3005780, -170.0f, 0x3C, 0x78, 0x14);
    }

    switch (gSessionContext.ruleCategory) {
        case 1:
            Intro_SetupTextPanel(gUIElementHealthFill, &D_30024E0, -100.0f, 0x1E, 0xB0, 0x14);
            break;

        case 2:
            Intro_SetupTextPanel(gUIElementHealthFill, &D_30032A0, -100.0f, 0x1E, 0xB0, 0x14);
            break;

        case 3:
            Intro_SetupTextPanel(gUIElementHealthFill, &D_3004060, -100.0f, 0x1E, 0xB0, 0x14);
            break;

        case 4:
            Intro_SetupTextPanel(gUIElementHealthFill, &D_3000780, -100.0f, 0x1E, 0x88, 0x14);
            break;

        case 5:
            Intro_SetupTextPanel(gUIElementHealthFill, &D_3001220, -100.0f, 0x1E, 0x78, 0x14);
            break;

        case 6:
            Intro_SetupTextPanel(gUIElementHealthFill, &D_3001B80, -100.0f, 0x1E, 0x78, 0x14);
            break;

        default:
            Intro_SetupTextPanel(gUIElementHealthFill, &D_30024E0, -100.0f, 0x1E, 0xB0, 0x14);
            break;
    }
}

void Intro_StartFirstShowcase(void) {
    gCameraTarget->unk_16C = 0;
    gCameraTarget->unk_17C = 0;
    gCameraTarget->unk_174 = 0;

    main_pool_push_state('MONS');

    Intro_LoadShowcaseModel();
}

void Intro_BeginOrbitCamera(void) {
    gCameraState->unk_00 = gMiniCameraStruct->node.children;
    gMiniCameraPtr = gCameraState->unk_00;
    Intro_SetupOrbitCamera();
}

void Intro_StartFinalStageModel(void) {
    gCameraTarget->unk_16C = 6;
    gCameraTarget->unk_17C = 6;
    gCameraTarget->unk_174 = 0;

    main_pool_push_state('MONS');

    Intro_LoadStageModel();
}

void Intro_BeginStaticCamera(void) {
    gCameraState->unk_00 = gMiniCameraStruct->node.children;
    gMiniCameraPtr = gCameraState->unk_00;
    Intro_SetupStaticCamera();
}

void Intro_TriggerTextPanelExit(void) {
    gIntroTextPanelState->unk_90 = 3;
}

void Intro_SelectPartyRoster(void) {
    if (gSessionContext.battleSides[0].unk_1C == 1) {
        gMonsterDataPtr = gSessionContext.battleSides[0].teamMember[0];
        gMonsterContextPtr = &gSessionContext.battleSides[0];
    } else {
        gMonsterDataPtr = gSessionContext.battleSides[1].teamMember[0];
        gMonsterContextPtr = &gSessionContext.battleSides[1];
    }
}

void Intro_ReadControllerInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

s32 Intro_UpdateShowcaseSequence(void) {
    switch (gCameraTarget->unk_16C) {
        case 0:
            Intro_SetupOrbitCamera();
            Intro_UpdateCameraKeyframe(gCameraState->unk_00);
            gCameraTarget->unk_170 = 0;
            gCameraTarget->unk_16C += 1;
            break;

        case 1:
            Intro_UpdateCryEventTrack();
            if (gCameraTarget->unk_178++ == 0x19) {
                gIntroTextPanelState->unk_90 = 0;
            }

            if (Intro_UpdateCameraKeyframe(gCameraState->unk_00) != 0) {
                if ((gSessionContext.ruleCategory == 7) && (gCameraTarget->unk_17C == 5)) {
                    return 1;
                }
                WipeTransition_Start(3, 0x1E);
                gCameraTarget->unk_16C += 1;
            }
            break;

        case 2:
            WipeTransition_Update();
            if (WipeTransition_GetState() == 1) {
                gCameraTarget->unk_17C++;
                if (gCameraTarget->unk_17C == 6) {
                    gCameraTarget->unk_170 = 0;
                    Intro_LoadStageModel();
                    Intro_SetupStaticCamera();
                    gIntroTextPanelState->unk_90 = 3;
                    gCameraTarget->unk_178 = 0;
                    WipeTransition_Start(2, 0x1E);
                    gCameraTarget->unk_16C = 6;
                } else {
                    gCameraTarget->unk_170 = 0;
                    Intro_ReplaceShowcaseModel();
                    Intro_SetupOrbitCamera();
                    gIntroTextPanelState->unk_90 = 3;
                    gCameraTarget->unk_178 = 0;
                    WipeTransition_Start(2, 0x1E);
                    gCameraTarget->unk_16C++;
                }
            }
            break;

        case 3:
            WipeTransition_Update();
            if (gCameraTarget->unk_178++ > 0) {
                Intro_UpdateCameraKeyframe(gCameraState->unk_00);
            }

            if (WipeTransition_GetState() == 0) {
                gCameraTarget->unk_178 = 0;
                gCameraTarget->unk_16C = 1;
            }
            break;

        case 6:
            gCameraTarget->unk_180 = 0xFF;
            WipeTransition_Update();
            Intro_UpdateModelLanding();
            if (WipeTransition_GetState() == 0) {
                gCameraTarget->unk_16C++;
            }
            break;

        case 7:
            if (Intro_UpdateModelLanding() != 0) {
                gCameraTarget->unk_178 = 0;
                gIntroTextPanelState->unk_90 = 0;
                Audio_PlayCategory11SoundCommand(0x01100002, 0, 0);
                gMonsterSfxTriggered = 0;
                gCameraTarget->unk_16C++;
            }
            break;

        case 8:
            if (gUIElementHealthFill->unk_84 != 0) {
                Intro_EmitLandingParticles();
            }
            if (gCameraTarget->unk_178++ >= 0x41) {
                gCameraRotationYSlew = 0;
                Vec3f_CalculateDistanceAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, &gCameraZoomDist, &gCameraZoomRotationX, &gCameraZoomRotationY);
                gCameraTarget->unk_16C++;
            }
            break;

        case 9:
            Intro_FadeInBackgroundTint();

            if (gUIElementHealthFill->unk_84 != 0) {
                Intro_EmitLandingParticles();
            }

            if (gPlayer1Controller->buttonDown & 0xC000) {
                return 1;
            }

            if (gCameraTarget->unk_178++ >= 0x3E9) {
                gCameraTarget->unk_178 = 0x64;
            }

            gCameraRotationYSlew = Math_StepToS32(gCameraRotationYSlew, 0x3C, 1, 1);
            gCameraZoomRotationY += gCameraRotationYSlew;
            Camera_ComputeEyeFromAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, gCameraZoomDist, gCameraZoomRotationX, gCameraZoomRotationY);
            break;
    }

    if ((gBattleSequenceStage == 2) && (gBattleSequenceFrameCount == 0x12C)) {
        Audio_StartMusicTrack(0x5B);
    }
    return 0;
}

s32 Intro_StepPanelBounce(unk_D_86B0FA50* arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    f32 temp_fa1;
    f32 sp20;
    f32 temp_fv0;
    f32 temp_fv0_2;
    f32 temp_fv0_4;
    f32* new_var;

    sp20 = arg0->unk_A4;

    temp_fv0 = Math_StepToF(sp20, 0.0f, 0.0f, 0.0f);
    arg0->unk_A4 = temp_fv0;
    if (temp_fv0 >= 0.0f) {
        temp_fv0 = 0;
        if ((arg0 && arg0) && arg0) {}
    }
    temp_fv0_2 = COSS(0) * temp_fv0;
    temp_fa1 = -arg3;

    new_var = &arg0->unk_AC;

    temp_fv0_4 = (-temp_fa1) - (arg2 * ((sp20 * arg0->unk_AC) + temp_fv0_2));
    arg0->unk_AC = (temp_fv0_4 * arg5) + ((*new_var) + temp_fv0_2);

    if (arg0->unk_AC >= 28.0) {
        arg0->unk_AC = 28.0f;
    }

    arg0->unk_98 += arg0->unk_AC;
    if (temp_fa1 >= 0) {
        if ((arg0->unk_98 < arg4) && (arg0->unk_98 >= 0.0f)) {
            arg0->unk_98 = arg4;
            arg0->unk_AC *= -arg1;
        }
    } else if ((arg4 < arg0->unk_98) && (arg0->unk_98 >= 0.0f)) {
        arg0->unk_98 = arg4;
        arg0->unk_AC *= -arg1;
        if (arg0->unk_A8 == arg0->unk_AC) {
            return 1;
        }
        arg0->unk_A8 = arg0->unk_AC;
    }

    return 0;
}

void Intro_DrawTextPanels(void) {
    if (gCameraTarget->unk_17C == 6) {
        switch (gIntroTextPanelState->unk_90) {
            case 3:
                break;

            case 0:
                Intro_ConfigureTextPanels();
                gIntroTextPanelState->unk_90++;
                break;

            case 1:
                Intro_StepPanelBounce(gUIElementHealthBar, 0.25f, 0.0f, 3.0f, 0xA3 - (gUIElementHealthBar->unk_9E / 2), 1.7f);
                UI_DrawGradientTexture(gUIElementHealthBar->unk_80, gUIElementHealthBar->unk_98, gUIElementHealthBar->unk_9C, gUIElementHealthBar->unk_9E,
                              gUIElementHealthBar->unk_A0, &gUIElementHealthBar->unk_B0);
                gUIElementHealthFill->unk_84 =
                    Intro_StepPanelBounce(gUIElementHealthFill, 0.25f, 0.0f, 3.0f, 0xA3 - (gUIElementHealthFill->unk_9E / 2), 1.7f);
                UI_DrawGradientTexture(gUIElementHealthFill->unk_80, gUIElementHealthFill->unk_98, gUIElementHealthFill->unk_9C, gUIElementHealthFill->unk_9E,
                              gUIElementHealthFill->unk_A0, &gUIElementHealthBar->unk_B0);

                if (gSessionContext.inRound2 == 1) {
                    UI_DrawTextureRgba16(gUIElementBorder->unk_80, (s16)gUIElementHealthFill->unk_98 + gUIElementHealthFill->unk_9E,
                                  gUIElementHealthFill->unk_9C - 2, gUIElementBorder->unk_9E, gUIElementBorder->unk_A0);
                }

                if ((gCurrentMonsterSpawnIdx != 4) && (gCurrentMonsterSpawnIdx != 5)) {
                    Intro_StepPanelBounce(gUIElementStatus, 0.25f, 0.0f, 3.0f, 0xA3 - (gUIElementStatus->unk_9E / 2), 1.7f);
                    UI_DrawGradientTexture(gUIElementStatus->unk_80, gUIElementStatus->unk_98, gUIElementStatus->unk_9C, gUIElementStatus->unk_9E,
                                  gUIElementStatus->unk_A0, &gUIElementHealthBar->unk_B0);
                }

                if (gUIElementHealthFill->unk_84 != 0) {
                    gUIElementHealthBar->unk_B0.unk_00 = Math_StepToS32(
                        gUIElementHealthBar->unk_B0.unk_00, gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_00,
                        gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_0C, gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_0C);
                    gUIElementHealthBar->unk_B0.unk_02 = Math_StepToS32(
                        gUIElementHealthBar->unk_B0.unk_02, gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_02,
                        gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_0E, gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_0E);
                    gUIElementHealthBar->unk_B0.unk_04 = Math_StepToS32(
                        gUIElementHealthBar->unk_B0.unk_04, gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_04,
                        gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_10, gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_10);
                    gUIElementHealthBar->unk_B0.unk_06 = Math_StepToS32(
                        gUIElementHealthBar->unk_B0.unk_06, gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_06,
                        gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_12, gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_12);
                    gUIElementHealthBar->unk_B0.unk_08 = Math_StepToS32(
                        gUIElementHealthBar->unk_B0.unk_08, gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_08,
                        gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_14, gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_14);
                    gUIElementHealthBar->unk_B0.unk_0A = Math_StepToS32(
                        gUIElementHealthBar->unk_B0.unk_0A, gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_0A,
                        gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_16, gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_16);
                    if ((gUIElementHealthBar->unk_B0.unk_00 == gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_00) &&
                        (gUIElementHealthBar->unk_B0.unk_02 == gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_02) &&
                        (gUIElementHealthBar->unk_B0.unk_04 == gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_04) &&
                        (gUIElementHealthBar->unk_B0.unk_06 == gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_06) &&
                        (gUIElementHealthBar->unk_B0.unk_08 == gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_08) &&
                        (gUIElementHealthBar->unk_B0.unk_0A == gUIElementColors[gUIElementHealthBar->unk_88 + 1].unk_0A)) {

                        gUIElementHealthBar->unk_88++;
                        if (gUIElementHealthBar->unk_88 == 4) {
                            gIntroTextPanelState->unk_90++;
                        }
                    }
                }
                break;

            case 2:
                UI_DrawGradientTexture(gUIElementHealthBar->unk_80, gUIElementHealthBar->unk_98, gUIElementHealthBar->unk_9C, gUIElementHealthBar->unk_9E,
                              gUIElementHealthBar->unk_A0, gUIElementColorFlat);
                UI_DrawGradientTexture(gUIElementHealthFill->unk_80, gUIElementHealthFill->unk_98, gUIElementHealthFill->unk_9C, gUIElementHealthFill->unk_9E,
                              gUIElementHealthFill->unk_A0, gUIElementColorFlat);

                if (gSessionContext.inRound2 == 1) {
                    UI_DrawTextureRgba16(gUIElementBorder->unk_80, (s16)gUIElementHealthFill->unk_98 + gUIElementHealthFill->unk_9E,
                                  gUIElementHealthFill->unk_9C - 2, gUIElementBorder->unk_9E, gUIElementBorder->unk_A0);
                }

                if ((gCurrentMonsterSpawnIdx != 4) && (gCurrentMonsterSpawnIdx != 5)) {
                    UI_DrawGradientTexture(gUIElementStatus->unk_80, gUIElementStatus->unk_98, gUIElementStatus->unk_9C, gUIElementStatus->unk_9E,
                                  gUIElementStatus->unk_A0, gUIElementColorFlat);
                }
                break;
        }
    } else {
        switch (gIntroTextPanelState->unk_90) {
            case 3:
                break;

            case 0:
                gIntroTextPanelState->unk_98 = -80.0f;
                gIntroTextPanelState->unk_9C = 0xB4;
                gIntroTextPanelState->unk_A4 = 0.0f;
                gIntroTextPanelState->unk_AC = 0.0f;
                sprintf(gIntroTextPanelState->unk00, "%s", gMonsterDataPtr->pokemonPicked[gCameraTarget->unk_17C].unk_30);
                gIntroTextPanelState->unk_90++;
                break;

            case 1:
                Font_BeginTranslucentTextRendering();
                Font_EnableTwoCycleTexturing();
                Font_SetActive(0x10, 0);
                Gfx_SetPrimColor(0, 0x82, 0xFF, 0xFF);
                Gfx_SetEnvColor(0xC8, 0xFF, 0xFF, 0xFF);
                Intro_StepPanelBounce(gIntroTextPanelState, 0.35f, 0.0f, 4.0f, 160.0f, 1.4f);
                Font_Printf((s32)gIntroTextPanelState->unk_98 - (Font_MeasureTextExtent(0x10, 0, gIntroTextPanelState->unk00) / 2),
                              gIntroTextPanelState->unk_9C, gIntroTextPanelState->unk00);
                Font_DisableTwoCycleTexturing();
                Font_EndTexturedTextRendering();
                break;
        }
    }

    WipeTransition_Draw();
}

void Intro_RenderFrame(void) {
    BgStage_DrawFrame();

    if ((gMiniCameraBgColor == -1) || (gMiniCameraBgColor == 0)) {
        GfxImage_FillCurrent(&gDisplayListHead, 1);
    } else if (gMiniCameraBgColor < 0x10000) {
        GfxImage_FillCurrent(&gDisplayListHead, gMiniCameraBgColor);
    } else {
        GfxImage_FillCurrent(&gDisplayListHead, 0xA6BF);
    }

    GeoRender_AdvanceFrameCounter();
    Geo_RenderRootNode(&gMiniCameraStruct->node);
    Intro_DrawTextPanels();
    MiniFx_DrawParticles();
    BgStage_AdvanceFrame();
}

void Intro_RenderBlackFrame(void) {
    BgStage_DrawFrame();
    GfxImage_FillCurrent(&gDisplayListHead, 1);
    GeoRender_AdvanceFrameCounter();
    Geo_RenderRootNode(&gMiniCameraStruct->node);
    Gfx_FillRectWithDisplayState(0, 0, 0x140, 0xF0, 0, 0, 0, 0xFF);
    BgStage_AdvanceFrame();
}

void Intro_FadeInSequence(void) {
    s32 i;

    for (i = 0; i < 18; i++) {
        Intro_ReadControllerInput();

        if (i < 3) {
            StageContext_SetClearColor(1);
            StageFade_StartFromOpaque(0x10);
            Intro_RenderBlackFrame();
        }
    }

    if (gBattleSequenceStage == 2) {
        Audio_StartMusicTrack(0x44);
    } else {
        Audio_StartMusicTrack(8);
    }
}

void Intro_RunLoop(void) {
    s32 var_s0 = 1;
    void (*temp_s2)(void) = (void*)Util_ConvertAddrToVirtAddr(Particle_UpdateFrameCountersAlias);

    gBattleSequenceFrameCount = 0;
    while (var_s0 != 0) {
        gBattleSequenceFrameCount++;

        Intro_ReadControllerInput();
        MiniFx_UpdateParticles();

        if (Intro_UpdateShowcaseSequence() != 0) {
            var_s0 = 0;
        }

        temp_s2();
        Intro_RenderFrame();
    }
}

void Intro_FadeOutSequence(void) {
    s32 i;
    s32 var_s1;
    void (*temp_s2)(void) = Util_ConvertAddrToVirtAddr(Particle_UpdateFrameCountersAlias);

    if (gSessionContext.ruleCategory == 7) {
        for (i = 0; i < 100; i++) {
            Intro_ReadControllerInput();
            MiniFx_UpdateParticles();

            temp_s2();

            Intro_RenderFrame();
        }
    }

    if (gSessionContext.ruleCategory == 7) {
        var_s1 = 0x64;
    } else {
        var_s1 = 0xA;
    }

    StageFade_StartFromTransparent(var_s1);
    Audio_FadeOutAll(var_s1 - 2);

    for (i = 0; i < var_s1; i++) {
        if (gSessionContext.ruleCategory == 7) {
            Intro_ReadControllerInput();
            MiniFx_UpdateParticles();
            temp_s2();
            Intro_RenderFrame();
        } else {
            Intro_ReadControllerInput();
            MiniFx_UpdateParticles();
            Intro_UpdateShowcaseSequence();
            temp_s2();
            Intro_RenderFrame();
        }
    }

    if (gSessionContext.ruleCategory == 7) {
        for (i = 0; i < 30; i++) {
            Intro_ReadControllerInput();
            MiniFx_UpdateParticles();

            temp_s2();

            Intro_RenderFrame();
        }
    }
}

void Stage_LoadModels(void) {
    MemoryBlock* sp34;
    UNUSED s32 pad;
    FragmentEntry sp2C;
    void* temp_v0_3;
    CreditsSceneData* temp_v0_6;

    gCurrentMonsterModelId = gStageModelIndices[gSessionContext.ruleCategory];
    if (gSessionContext.ruleCategory == 7) {
        gCurrentMonsterModelId += gSessionContext.modeSelection;
    }

    switch (gSessionContext.ruleCategory) {
        case 1:
            gCurrentMonsterSpawnIdx = 4;
            break;

        case 2:
            gCurrentMonsterSpawnIdx = 4;
            break;

        case 3:
            gCurrentMonsterSpawnIdx = gSessionContext.modeSelection;
            break;

        case 4:
#if defined(VERSION_US_REV2)
            gCurrentMonsterSpawnIdx = 4;
#else
            gCurrentMonsterSpawnIdx = 5;
#endif
            break;

        case 5:
            gCurrentMonsterSpawnIdx = 5;
            break;

        case 6:
            gCurrentMonsterSpawnIdx = gSessionContext.modeSelection;
            break;

        default:
            gCurrentMonsterSpawnIdx = 0;
            break;
    }

    sp2C = BinArchive_GetFile(BinArchive_Open(stadium_models_ROM_START, _5C7A70_ROM_START, 1, 1), gCurrentMonsterModelId);

    gMonsterModelArchive = sp2C(2, 0);
    GeoNode_CreateFog(0, &gIntroOverlayRect, 0x3C0, 0x3E8, 0xFF, 0xFF, 0xFF, 0xFF);
    GeoNode_CreateContainer(NULL, &gMiniCameraRoot0);
    GeoNode_CreateContainer(NULL, &gMiniCameraRoot1);
    GeoNode_CreateContainer(NULL, &gMiniCameraRoot2);
    sp34 = MainPool_AllocState(main_pool_get_available(), 0);
    gMiniCameraStruct = process_geo_layout(sp34, gMiniCameraLayout);

    temp_v0_3 = sp2C(0, 0);
    if (temp_v0_3 != NULL) {
        GraphNode_AppendChild(&gMiniCameraRoot0, process_geo_layout(sp34, temp_v0_3));
    }

    temp_v0_3 = sp2C(1, 0);
    if (temp_v0_3 != NULL) {
        GraphNode_AppendChild(&gMiniCameraRoot1, process_geo_layout(sp34, temp_v0_3));
    }

    temp_v0_3 = sp2C(3, 0);
    if (temp_v0_3 != NULL) {
        GraphNode_AppendChild(&gMiniCameraRoot2, process_geo_layout(sp34, temp_v0_3));
    }

    gMiniCameraBgColor = sp2C(2, 0);
    MainPool_FinalizeAllocation(sp34);

    temp_v0_6 = sp2C(4, 0);
    if (temp_v0_6 == NULL) {
        gIntroOverlayRect.node.callbackArg = 0;
        gIntroOverlayRect.node.flags &= ~1;
    } else {
        gIntroOverlayRect.unk_18.unk_00 = temp_v0_6->unk_00;
        gIntroOverlayRect.unk_18.unk_02 = temp_v0_6->unk_02;
        gIntroOverlayRect.unk_18.unk_04.rgba = temp_v0_6->unk_04.rgba;
        gIntroOverlayRect.node.callbackArg = 1;
    }

    ModelRenderer_InitDisplayRoots();
    Intro_SetBackgroundTint(0xFF, 0xFF, 0xFF);
    MiniFx_Init();

    gCameraTarget = main_pool_alloc(sizeof(unk_D_86B0FA78), 0);
    bzero(gCameraTarget, sizeof(unk_D_86B0FA78));

    gCameraState = main_pool_alloc(sizeof(unk_D_86B0FA7C), 0);
    bzero(gCameraState, sizeof(unk_D_86B0FA7C));

    gIntroTextPanelState = main_pool_alloc(sizeof(unk_D_86B0FA50), 0);
    bzero(gIntroTextPanelState, sizeof(unk_D_86B0FA50));

    gUIElementHealthBar = main_pool_alloc(sizeof(unk_D_86B0FA50), 0);
    bzero(gUIElementHealthBar, sizeof(unk_D_86B0FA50));

    gUIElementHealthFill = main_pool_alloc(sizeof(unk_D_86B0FA50), 0);
    bzero(gUIElementHealthFill, sizeof(unk_D_86B0FA50));

    gUIElementStatus = main_pool_alloc(sizeof(unk_D_86B0FA50), 0);
    bzero(gUIElementStatus, sizeof(unk_D_86B0FA50));

    gUIElementBorder = main_pool_alloc(sizeof(unk_D_86B0FA50), 0);
    bzero(gUIElementBorder, sizeof(unk_D_86B0FA50));

    Intro_SelectPartyRoster();

    switch (gSessionContext.ruleCategory) {
        case 1:
        case 2:
        case 4:
        case 5:
        case 7:
            Intro_StartFirstShowcase();
            Intro_BeginOrbitCamera();
            Intro_TriggerTextPanelExit();
            gBattleSequenceStage = 0;
            break;

        default:
            if (gSessionContext.modeSelection == 3) {
                gBattleSequenceStage = 1;
                Intro_StartFirstShowcase();
                Intro_BeginOrbitCamera();
                Intro_TriggerTextPanelExit();
            } else {
                gBattleSequenceStage = 2;
                Intro_StartFinalStageModel();
                Intro_BeginStaticCamera();
                Intro_TriggerTextPanelExit();
            }
            break;
    }
}

s32 Stage_ShowIntro(void) {
    StageContext* sp24;

    main_pool_push_state('DEMO');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(0, 1, 3, 1, 2, 1);
    Font_Init(0x10, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_3000000, cup_clear_ui, 0);

    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    Stage_LoadModels();
    StageContext_Activate(sp24);
    Intro_FadeInSequence();
    Intro_RunLoop();
    Intro_FadeOutSequence();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('DEMO');

    return 0;
}
