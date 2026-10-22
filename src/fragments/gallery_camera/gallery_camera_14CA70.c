#include "gallery_camera.h"
#include "src/fragments/minigame_actor_framework/minigame_actor_framework.h"
#include "src/fragments/particle_data_library/particle_data_library.h"
#include "src/fragments/particle_math/particle_math.h"
#include "src/fragments/gallery_backgrounds/gallery_backgrounds.h"
#include "src/geo_node.h"
#include "src/geo_render.h"
#include "src/model_animation.h"
#include "src/model_animation_events.h"
#include "src/geo_layout.h"
#include "src/display_object_textures.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/gb_save.h"
#include "src/save_data.h"
#include "src/text_system.h"
#include "src/jpeg_stream.h"
#include "src/audio_sfx.h"
#include "src/gfx_buffer.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/dp_intro.h"
#include "src/math_util.h"
#include "src/memory.h"
#include "src/stage_loader.h"
#include "src/util.h"

typedef struct GalleryState {
    /* 0x000 */ ModelLoadResult* unk_000;
    /* 0x004 */ DisplayObject unk_004;
    /* 0x16C */ s32 unk_16C;
} GalleryState; // size = 0x170

typedef struct GalleryAnimScriptEntry {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ char unk02[0xE];
} GalleryAnimScriptEntry; // size = 0x10

typedef struct GalleryCameraConfig {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x04 */ f32 unk_04;
} GalleryCameraConfig; // size = 0x8

typedef struct GalleryTextTimer {
    /* 0x0 */ s32 unk_00;
    /* 0x4 */ s32 unk_04;
} GalleryTextTimer; // size = 0x8

typedef struct GalleryScriptInput {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ char unk14[0xC];
} GalleryScriptInput; // size = 0x20

static u8 gStageFragmentIds[] = {
    0x00, 0x01, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x11,
};

static s32 gCameraTutorialTextIds[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xB, 0xFF };

static FogRect gStageBackgroundFog;
static DisplayNodeHeader gStagePanelGraphRoot0;
static DisplayNodeHeader gStagePanelGraphRoot1;
static DisplayNodeHeader gStagePanelGraphRoot2;

static u32 gMiniCameraGeoLayout[] = {
    0x0C00FFFF, 0x05000000, 0x0B00001E, 0x00000000, 0x014000F0, 0x0000000F, 0x00000000, 0x00000000, 0x05000000,
    0x0D000000, 0x05000000, 0x07000000, &gStageBackgroundFog, 0x14000000, 0x002D0019, 0xFFFFFF28, 0x08000000, GalleryCamera_BgAngleCallbackA,
    0x00000000, 0x14000000, 0x002D0019, 0x80808028, 0x08000000, GalleryCamera_BgAngleCallbackB, 0x00000000, 0x16646464, 0x0F000002,
    0x05000000, 0x1F00FFFF, 0x00000000, 0x00000000, 0x00000000, 0x00640064, 0x00640000, 0x08000000, GalleryCamera_FogColorCallback,
    0x00000000, 0x05000000, 0x07000000, &gStagePanelGraphRoot0, 0x06000000, 0x06000000, 0x0F000003, 0x05000000, 0x1F00FFFF,
    0x00000000, 0x00000000, 0x00000000, 0x00640064, 0x00640000, 0x08000000, GalleryCamera_FogColorCallback, 0x00000000, 0x05000000,
    0x07000000, &gStagePanelGraphRoot1, 0x06000000, 0x06000000, 0x0F000002, 0x05000000, 0x1F00FFFF, 0x00000000, 0x00000000,
    0x00000000, 0x00640064, 0x00640000, 0x08000000, GalleryCamera_FogColorCallback, 0x00000000, 0x05000000, 0x07000000, &gStagePanelGraphRoot2,
    0x06000000, 0x06000000, 0x0F000003, 0x05000000, 0x0A000000, &gGraphRoot0, 0x06000000, 0x0F000002, 0x05000000,
    0x0A000000, &gGraphRoot1, 0x06000000, 0x09000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000,
};

Vec3f gCameraClampCenter = { 0.0f, 0.0f, 0.0f };
Vec3f gCameraClampCenterMode = { 0.0f, 0.0f, 0.0f };
Vec3f gCameraConstraintCenter = { 0.0f, 0.0f, 0.0f };
Vec3f gCameraConstraintCenterMode = { 0.0f, 0.0f, 0.0f };

Color_RGB8 gStagePanelBlueColor = { 0, 0, 150 };
Color_RGB8 gStagePanelBrightBlueColor = { 0, 0, 250 };

char* gStagePanelLabelStrings[] = {
    gCupNameFreeBattle, gCupNameNintendoCup, gCupNameNintendoCupRound2, gCupNamePetitCup, gCupNamePikaCup, gCupNamePrimeCup, gCupNamePewterGym, gCupNameCeruleanGym,
    gCupNameVermilionGym, gCupNameCeladonGym, gCupNameFuchsiaGym, gCupNameSaffronGym, gCupNameCinnabarGym, gCupNameViridianGym, gCupNameEliteFour, gCupNameMewtwo,
};

Color_RGB8 gStagePanelGradientStartColor = { 0, 100, 255 };
Color_RGB8 gStagePanelGradientEndColor = { 100, 255, 255 };

static GalleryCameraConfig gStageCameraConfig[] = {
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 5.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 4.0f },   { 0xAB, 0xAC, 1.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, -10.0f }, { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, -4.0f },  { 0xAB, 0xAC, 10.0f },  { 0xAB, 0xAC, 5.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 15.0f },  { 0xAB, 0xAC, 35.0f },  { 0xAB, 0xAC, -5.0f },  { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 1.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0x5A, -8.0f },  { 0xAB, 0xAC, 11.0f },  { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, -8.0f },  { 0xAB, 0xAC, 5.0f },   { 0xAB, 0xAC, 5.0f },   { 0xAB, 0xAC, -5.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, -12.0f }, { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 10.0f },  { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 10.0f },
    { 0xAB, 0xAC, 25.0f },  { 0xAB, 0xAC, 18.0f },  { 0xAB, 0xAC, -8.0f },  { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 30.0f },  { 0xAB, 0xAC, -10.0f }, { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 10.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, -10.0f }, { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 5.0f },   { 0xAB, 0xAC, -5.0f },
    { 0xAB, 0xAC, 20.0f },  { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 13.0f },  { 0xAB, 0xAC, 5.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 5.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 10.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 5.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 5.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, -5.0f },  { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 6, 0.0f },      { 0xAB, 6, 5.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 13.0f },  { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 10.0f },  { 0xAB, 0xAC, 30.0f },
    { 0xAB, 0xAC, 5.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 10.0f },  { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 25.0f },  { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 10.0f },  { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 10.0f },  { 0xAB, 0xAC, -5.0f },
    { 0xAB, 0xAC, 15.0f },  { 0xAB, 0xAC, 35.0f },  { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 50.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 10.0f },  { 0xAB, 0xAC, 5.0f },   { 0xAB, 0xAC, 10.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 15.0f },  { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, -5.0f },  { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 5.0f },   { 0x75, 0xAC, 0.0f },   { 0xAB, 0xAC, 5.0f },   { 0xAB, 0x10, 7.0f },
    { 0xAB, 0xAC, 10.0f },  { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 5.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 10.0f },  { 0xAB, 0xAC, 10.0f },  { 0xAB, 0xAC, -10.0f },
    { 0xAB, 0xAC, -20.0f }, { 0xAB, 0xAC, 15.0f },  { 0xAB, 0xAC, 5.0f },   { 0xAB, 0xAC, 0.0f },
    { 0xAB, 0xAC, 30.0f },  { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, -10.0f },
    { 0xAB, 0xAC, 10.0f },  { 0xAB, 0xAC, 20.0f },  { 0xAB, 0xAC, 0.0f },   { 0xAB, 0xAC, 20.0f },
    { 0xAB, 0xAC, 35.0f },  { 0xAB, 0xAC, 20.0f },  { 0xAB, 0xAC, 15.0f },  { 0xAB, 0xAC, 10.0f },
    { 0xAB, 0xAC, 10.0f },  { 0xAB, 0xAC, 20.0f },  { 0xAB, 0xAC, -1.0f },  { 0, 0, 0.0f },
    { 0xAB, 0xAC, -8.0f },
};

static GalleryTextTimer gTutorialTextTimers[] = {
    { 0x1A, 0x60 },   { 0x7F, 0x117 },  { 0x135, 0x230 }, { 0x24A, 0x361 }, { 0x387, 0x46C },
    { 0x48D, 0x543 }, { 0x564, 0x653 }, { 0x673, 0x6D0 }, { 0x76C, 0x858 }, { 0x87D, 0x1FCC },
};

static GalleryScriptInput gControllerScriptInputs[] = {
    { 0xC4, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0x8000, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xD, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0x8000, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x10, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 6, 0x8000, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x7F, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x28, 0x200, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 9, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x10, 0x800, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 9, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x23, 0x100, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xB, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x13, 0x400, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xF, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xF, 0x200, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x7C, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xA, 2, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x15, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x14, 1, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x15, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xC, 2, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x15, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 8, 8, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x15, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xA, 4, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x15, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 8, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x7B, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 2.0f, 0.0f, 2.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 3.0f, 0.0f, 3.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 5.0f, 0.0f, 5.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 7.0f, 0.0f, 7.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 8.0f, 0.0f, 8.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 9.0f, 0.0f, 9.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 10.0f, 0.0f, 10.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 12.0f, 0.0f, 12.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 13.0f, 0.0f, 13.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xE, 0, 14.0f, 0.0f, 14.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 13.0f, 0.0f, 13.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 5.0f, 0.0f, 5.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x1E, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 2.0f, 0.0f, -2.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 3.0f, 0.0f, -3.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 4.472136f, 2.0f, -4.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 2, 0, 5.385165f, 2.0f, -5.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 6.708204f, 3.0f, -6.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 6, 0, 8.062257f, 4.0f, -7.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 8.602324f, 5.0f, -7.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 9.433981f, 5.0f, -8.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 2, 0, 10.816653f, 6.0f, -9.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 11.661903f, 6.0f, -10.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 3, 0, 12.206555f, 7.0f, -10.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x12, 0, 13.038404f, 7.0f, -11.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 10.816653f, 6.0f, -9.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x14, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 2.0f, 0.0f, 2.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 4.0f, 0.0f, 4.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 5.0f, 0.0f, 5.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 6.0f, 0.0f, 6.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 7.0f, 0.0f, 7.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 9.0f, 0.0f, 9.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 10.0f, 0.0f, 10.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 2, 0, 11.0f, 0.0f, 11.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 3, 0, 12.0f, 0.0f, 12.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x12, 0, 13.0f, 0.0f, 13.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 5.0f, 0.0f, 5.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x71, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 3.0f, 0.0f, 3.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 5.0f, 0.0f, 5.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 7.0f, 0.0f, 7.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 9.0f, 0.0f, 9.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 12.0f, 0.0f, 12.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 14.0f, 0.0f, 14.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 19.0f, 0.0f, 19.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 27.0f, 0.0f, 27.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 40.0f, 0.0f, 40.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 8, 0, 64.0f, 0.0f, 63.999996f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 64.0f, 0.0f, 63.999992f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 46.0f, 0.0f, 46.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 10.0f, 0.0f, 10.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 8, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 3.0f, 0.0f, -3.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 6, 0, 5.385165f, 2.0f, -5.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 3.0f, 0.0f, -3.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 8, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 55.0f, 0.0f, 55.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 64.0f, -2.52435f, 63.950195f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 64.0f, -5.869891f, 63.730244f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 64.0f, -8.349089f, 63.453075f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 64.0f, -9.981604f, 63.216824f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0, 64.0f, -11.743815f, 62.9133f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 64.0f, -7.83112f, 63.51908f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 46.09772f, -3.0f, 46.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 14.0f, 0.0f, 14.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 6, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 2.0f, 0.0f, -2.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 6.0f, 0.0f, -6.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 9.486833f, 3.0f, -9.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 3, 0, 11.7047f, 4.0f, -11.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 13.0f, 5.0f, -12.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0, 13.928388f, 5.0f, -13.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 10.29563f, 5.0f, -9.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 3.0f, 0.0f, -3.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 7, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 34.0f, 0.0f, 34.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xA, 0, 64.0f, 0.0f, 63.999996f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 64.0f, 0.0f, 63.999992f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 60.0f, 0.0f, 60.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0, 29.0f, 0.0f, 29.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x8D, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0x10, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x28, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0x10, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x1A, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0x10, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x22, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0x10, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xB9, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0x20, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xF, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 5, 0x400, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xF, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 5, 0x400, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xF, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 5, 0x400, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xF, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x30, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 3, 0x20, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x73, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0x2000, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x28, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0x2000, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x28, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0x2000, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0xB0, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x10, 0x100, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 2, 0x8100, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x12, 0x100, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 3, 0x8100, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x18, 0x100, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0x8100, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 3, 0x100, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 3, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 9, 0x100, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 4, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 3, 0x8000, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 0x34, 0, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    { 1, 0x1000, 0.0f, 0.0f, 0.0f, 0xFF, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
};

char** gCameraHelpText;
static s32 gStageReturnCode;
static s32 gCameraNavRepeatCounter;
static s32 pad_D_8690A684;
static u32 gStagePanelFillColor;
static Color_RGBA8_u32 gGlobalColorRGBA;
static s32 pad_D_8690A690;
static s32 pad_D_8690A694;
static BinArchive* D_8690A698;
static GalleryState* gStagePanelState;
static BattleMon gStagePanelPlayerData;
static s32 gStageAnimTimer;
static s32 gStageSoundTimer;
static s32 gStageAnimStateFlag;
static s32 gGalleryPrimaryAnimIndex;
static s32 gGallerySecondaryAnimIndex;
f32 gCameraZoomOffset;
static s32 gStagePanelMode;
GalleryCameraParams* gCameraParams;
static GalleryAnimScriptEntry gStageAnimScriptTable[190];
GalleryCameraConfigState gCameraConfig;
GalleryAnimData gStagePanelAnimData;
static s32 gStagePanelTransitionTimer;
s32 gStageSnapshotCount;
s32 gStagePanelStateIndex;
GalleryFadeData gStagePanelFadeData;
GalleryAnimState gStagePanelAnimState;
static ModeSettings gGallerySessionModeSettings;
s32 gCameraNavActiveFlag;
s32 gCameraNavSubIndex;
s32 gCameraNavTransition;
s32 gCameraNavMaxIndex;
Controller gControllerInput;
static u16 gControllerScriptButtonPrev;
static s32 gControllerScriptIndex;
static u16 gControllerScriptButtonTimer;
GalleryTextBoxState gTutorialTextBoxState;
s8 gTutorialTextBoxCharBuffer[255];
static s8 gTutorialTextBoxStringBuffer[0x100];
static Controller gControllerScriptTemp;
static s32 gTutorialMessageIndex;
static s32 gTutorialMessageTimer;

s32 GalleryCamera_BgAngleCallbackA(s32 arg0, GraphNode* arg1) {
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

s32 GalleryCamera_BgAngleCallbackB(s32 arg0, GraphNode* arg1) {
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

s32 GalleryCamera_FogColorCallback(s32 arg0, GraphNode* arg1) {
    DisplayObject* ptr = (DisplayObject*)arg1;

    if (arg0 == 2) {
        ptr->textureTint.rgba = gGlobalColorRGBA.rgba;
    }
    return 0;
}

void Gallery_SetBackgroundTint(u8 arg0, u8 arg1, u8 arg2) {
    u8 v = arg0 & arg1 & arg2;

    gGlobalColorRGBA.r = arg0;
    gGlobalColorRGBA.g = arg1;
    gGlobalColorRGBA.b = arg2;

    if (v == 0xFF) {
        if (gStageBackgroundFog.node.callbackArg == 1) {
            gStageBackgroundFog.node.flags |= 1;
        }
    } else {
        gStageBackgroundFog.node.flags &= ~1;
    }
}

void Gallery_LoadPhotoTextures(void) {
    UNUSED s32 pad[2];
    u8* temp_a1 = (u32)_70D3A0_ROM_START + (gStageRomOffsets[gStagePanelState->unk_16C - 1] & 0xFFFFFF);
    u8* sp20 = (u32)_70D3A0_ROM_START +
               (((gStagePanelState->unk_16C * sizeof(unk_70D3A0)) - sizeof(unk_70D3A0) + (u32)D_70110) & 0xFFFFFF);
    u8* sp1C = sp20 + sizeof(unk_70D3A0);

    if (gStagePanelState->unk_16C == 153) {
        sp20 = (u32)_70D3A0_ROM_START + ((u32)D_70290 & 0xFFFFFF);
        sp1C = sp20 + sizeof(unk_70D3A0);
    }

    Dma_WriteChunks(&gStageAnimScriptTable->unk_00, temp_a1, temp_a1 + 0xBC0, 0);
    Dma_WriteChunks(&gCameraConfig, sp20, sp1C, 0);
}

ModelLoadResult* Gallery_LoadSurfingPikachuIcon(BattleMon* arg0) {
    arg1_func_80010CA8 sp1C;
    ModelLoadContext* sp18;

    arg0->unk_00.unk_00 = 25;
    sp18 = PokeIcon_AllocFramebuffers(1);
    Model_ComputeSizeVariant(&sp1C, arg0);
    PokeIcon_RequestFrameLoad(sp18, arg0->unk_00.unk_00 = 153, sp1C);
    PokeIcon_WaitFrameLoad(sp18);

    return sp18->loadedModel;
}

void Gallery_SetupPhotoModel(void) {
    Gallery_CopyBattleMonFromPhotoMon(&gStagePanelPlayerData, &gGallerySpecialSpecies);
    gStagePanelState->unk_16C = gGallerySpecialSpecies.unk_02;

    gCameraZoomOffset = gStageCameraConfig[gStagePanelState->unk_16C - 1].unk_04;
    gGalleryPrimaryAnimIndex = gStageCameraConfig[gStagePanelState->unk_16C - 1].unk_00 - 6;
    gGallerySecondaryAnimIndex = gStageCameraConfig[gStagePanelState->unk_16C - 1].unk_01 - 6;

    ModelRenderer_AttachDisplayObject(&gStagePanelState->unk_004);
    PokeIcon_OpenModelArchives();

    if (gStagePanelState->unk_16C == 153) {
        gStagePanelState->unk_000 = Gallery_LoadSurfingPikachuIcon(&gStagePanelPlayerData);
    } else {
        gStagePanelState->unk_000 = Model_LoadForPokemon(&gStagePanelPlayerData);
    }

    Model_InitDisplayObject(&gStagePanelState->unk_004, 0, gStagePanelState->unk_16C, gStagePanelState->unk_000->modelRoot->unk_00[0]);
    Gallery_LoadPhotoTextures();
    ModelAnim_SetAnimation(&gStagePanelState->unk_004, gStageAnimScriptTable[gGalleryPrimaryAnimIndex].unk_00);

    gStagePanelState->unk_004.position.y += gCameraConfig.unk_08;
    gStagePanelState->unk_004.effectSlot = 0;
}

void Gallery_FinalizeBackgroundLoad(void) {
    while (Display_IsFrameReady() == 0) {}

    GeoNode_CreateContainer(NULL, &gStagePanelGraphRoot0);
    GeoNode_CreateContainer(NULL, &gStagePanelGraphRoot1);
    GeoNode_CreateContainer(NULL, &gStagePanelGraphRoot2);

    main_pool_pop_state('BACK');
}

void Gallery_LoadBackgroundSet(s32 arg0) {
    MemoryBlock* sp1C;
    FragmentEntry sp18;
    u32* temp_v0;
    CreditsSceneData* temp_v1;

    main_pool_push_state('BACK');

    sp18 = BinArchive_GetFile(D_8690A698, gStageFragmentIds[arg0]);
    sp1C = MainPool_AllocState(main_pool_get_available(), 0);

    temp_v0 = sp18(0, 0);
    if (temp_v0 != NULL) {
        GraphNode_AppendChild(&gStagePanelGraphRoot0, process_geo_layout(sp1C, temp_v0));
    }

    temp_v0 = sp18(1, 0);
    if (temp_v0 != NULL) {
        GraphNode_AppendChild(&gStagePanelGraphRoot1, process_geo_layout(sp1C, temp_v0));
    }

    temp_v0 = sp18(3, 0);
    if (temp_v0 != NULL) {
        GraphNode_AppendChild(&gStagePanelGraphRoot2, process_geo_layout(sp1C, temp_v0));
    }

    gStagePanelFillColor = sp18(2, 0);
    MainPool_FinalizeAllocation(sp1C);

    temp_v1 = sp18(4, 0);
    if (temp_v1 == NULL) {
        gStageBackgroundFog.node.callbackArg = 0;
        gStageBackgroundFog.node.flags &= ~1;
    } else {
        gStageBackgroundFog.unk_18.unk_00 = temp_v1->unk_00;
        gStageBackgroundFog.unk_18.unk_02 = temp_v1->unk_02;
        gStageBackgroundFog.unk_18.unk_04.rgba = temp_v1->unk_04.rgba;
        gStageBackgroundFog.node.callbackArg = 1;
    }

    Gallery_SetBackgroundTint(0xFF, 0xFF, 0xFF);
}

void Gallery_InitBackgroundScene(void) {
    MemoryBlock* sp2C;

    GeoNode_CreateFog(0, &gStageBackgroundFog, 0x3C0, 0x3E8, 0xFF, 0xFF, 0xFF, 0xFF);
    GeoNode_CreateContainer(NULL, &gStagePanelGraphRoot0);
    GeoNode_CreateContainer(NULL, &gStagePanelGraphRoot1);
    GeoNode_CreateContainer(NULL, &gStagePanelGraphRoot2);
    Gallery_SetBackgroundTint(0xFF, 0xFF, 0xFF);
    sp2C = MainPool_AllocState(main_pool_get_available(), 0);
    gMiniCameraStruct = process_geo_layout(sp2C, &gMiniCameraGeoLayout);
    MainPool_FinalizeAllocation(sp2C);
    ModelRenderer_InitDisplayRoots();
}

void GalleryCamera_ComputeFrameCornerPos(f32* arg0, f32* arg1, s16* arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, s16 arg7) {
    *arg2 = MathUtil_Atan2s(arg4 - arg6, arg3 - arg5);

    *arg0 = COSS(*arg2 + arg7) * 60.0f + arg5;
    *arg1 = SINS(*arg2 + arg7) * 60.0f + arg6;
}

void GalleryCamera_InitIntroSequence(void) {
    gStagePanelTransitionTimer = 0;
    if (gStagePanelMode == 1) {
        gStageSnapshotCount = 0;
    } else {
        gStageSnapshotCount = gGalleryPhotoPoolCount;
    }

    gCameraParams->unk_014 = 1;
    gCameraNavActiveFlag = 0;
    gCameraNavSubIndex = 0;
    gCameraNavTransition = 0;
    gCameraParams->unk_00C = 0;
    gStageAnimTimer = 0;
    gStageSoundTimer = 0;
    gStageAnimStateFlag = 0;
    gStagePanelStateIndex = 0;

    gStagePanelAnimState.unk_02 = 0;
    gStagePanelAnimState.unk_06 = 0;
    gStagePanelAnimState.unk_0A = 0;

    gStagePanelAnimData.unk_00[0] = 0xA0;
    gStagePanelAnimData.unk_00[4] = -0x14;
    gStagePanelAnimData.unk_00[1] = 0x14A;
    gStagePanelAnimData.unk_00[5] = 0x78;
    gStagePanelAnimData.unk_00[2] = 0xA0;
    gStagePanelAnimData.unk_00[6] = 0x104;
    gStagePanelAnimData.unk_00[3] = -0xA;
    gStagePanelAnimData.unk_00[7] = 0x78;

    GalleryCamera_ComputeFrameCornerPos(&gStagePanelAnimData.unk_10, &gStagePanelAnimData.unk_20, &gStagePanelAnimData.unk_30, 0.0f, 0.0f, 12.5f, 15.5f, 0);
    GalleryCamera_ComputeFrameCornerPos(&gStagePanelAnimData.unk_14, &gStagePanelAnimData.unk_24, &gStagePanelAnimData.unk_32, 320.0f, 0.0f, 307.5f, 15.5f, -0x8000);
    GalleryCamera_ComputeFrameCornerPos(&gStagePanelAnimData.unk_18, &gStagePanelAnimData.unk_28, &gStagePanelAnimData.unk_34, 320.0f, 240.0f, 308.8f, 224.5f, 0);
    GalleryCamera_ComputeFrameCornerPos(&gStagePanelAnimData.unk_1C, &gStagePanelAnimData.unk_2C, &gStagePanelAnimData.unk_36, 0.0f, 240.0f, 12.5f, 224.5f, -0x8000);

    gTutorialTextBoxState.unk_00 = 0xFF;
    gControllerScriptIndex = 0;
    gControllerScriptButtonTimer = gControllerScriptInputs[0].unk_00;
    gControllerScriptButtonPrev = 0;
    gTutorialMessageIndex = 0;
    gTutorialMessageTimer = 0;
}

void GalleryCamera_CopyControllerState(Controller* arg0, Controller* arg1) {
    arg0->buttonPressed = arg1->buttonPressed;
    arg0->buttonDown = arg1->buttonDown;
    arg0->stickX = arg1->stickX;
    arg0->stickY = arg1->stickY;
    arg0->stickMag = arg1->stickMag;
}

void SimulateControllerInputFromScript(void) {
    Controller* c = &gControllerScriptTemp;

    gControllerScriptButtonPrev = gControllerScriptInputs[gControllerScriptIndex].unk_02;
    if (gControllerScriptButtonTimer == 0) {
        gControllerScriptIndex++;
        gControllerScriptButtonTimer = gControllerScriptInputs[gControllerScriptIndex].unk_00;
    }

    c->stickMag = gControllerScriptInputs[gControllerScriptIndex].unk_04;
    c->stickY = gControllerScriptInputs[gControllerScriptIndex].unk_0C;
    c->buttonPressed = gControllerScriptInputs[gControllerScriptIndex].unk_02 & (gControllerScriptInputs[gControllerScriptIndex].unk_02 ^ gControllerScriptButtonPrev);
    c->buttonDown = gControllerScriptInputs[gControllerScriptIndex].unk_02;

    if (gControllerScriptButtonTimer > 0) {
        gControllerScriptButtonTimer--;
    }

    GalleryCamera_CopyControllerState(&gControllerInput, &gControllerScriptTemp);
}

void GalleryCamera_AddPolarOffset(f32* arg0, f32* arg1, s16 arg2, s16 arg3) {
    *arg0 += arg3 * COSS(arg2);
    *arg1 += arg3 * SINS(arg2);
}

void Gallery_CameraReadInput(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void GalleryCamera_UpdatePokemonIdleAnim(void) {
    if (gStageAnimTimer <= 0x96) {
        gStageAnimTimer++;
    }

    if ((gStageAnimTimer >= 0x96) && (ModelAnim_IsAnimationDone(&gStagePanelState->unk_004) != 0) && (gStageAnimStateFlag == 0)) {
        ModelAnim_SetAnimation(&gStagePanelState->unk_004, gStageAnimScriptTable[gGallerySecondaryAnimIndex].unk_00);
        ModelAnim_SetFrame(&gStagePanelState->unk_004, 0);
        ModelAnim_SetEventFrame(&gStagePanelState->unk_004, 0);
        ModelAnim_SetEventTrack(&gStagePanelState->unk_004, gStageAnimScriptTable[gGallerySecondaryAnimIndex].unk_01);
        gStageAnimStateFlag = 1;
    }

    if ((ModelAnim_IsAnimationDone(&gStagePanelState->unk_004) != 0) && (gStageAnimStateFlag == 1)) {
        ModelAnim_SetAnimation(&gStagePanelState->unk_004, gStageAnimScriptTable[gGalleryPrimaryAnimIndex].unk_00);
        ModelAnim_SetFrame(&gStagePanelState->unk_004, 0);
        gStageAnimTimer = 0;
        gStageAnimStateFlag = 0;
    }
}

void GalleryCamera_UpdatePokemonEventLoop(void) {
    gStageSoundTimer++;
    if (gStageSoundTimer == 60) {
        gStageSoundTimer = 0;
        if (gStageAnimStateFlag == 0) {
            ModelAnim_SetEventFrame(&gStagePanelState->unk_004, 0);
            ModelAnim_SetEventTrack(&gStagePanelState->unk_004, gStageAnimScriptTable[gGalleryPrimaryAnimIndex].unk_01);
        }
    }
}

void GalleryCamera_ClearFinishedEventTrack(void) {
    if ((gStagePanelState->unk_004.eventTrack.trackId != -1) && (ModelAnim_IsEventTrackNearEnd(&gStagePanelState->unk_004) == 1)) {
        ModelAnim_SetEventFrame(&gStagePanelState->unk_004, 0);
        ModelAnim_SetEventTrack(&gStagePanelState->unk_004, -1);
    }
}

void Gallery_CapturePhoto(void) {
    if ((gControllerInput.buttonPressed & 0x8000) && (gStageSnapshotCount < 0x18)) {
        Audio_PlaySoundEffectById(SFX_PHOTO_CAPTURE);
        gStagePanelTransitionTimer = 0xA;
        if (gStagePanelMode != 1) {
            Vec3f_SetComponentsDuplicate(&gDisplayableMonIndices[gStageSnapshotCount].unk_50, gMiniCameraPtr->view.eyePosition.x, gMiniCameraPtr->view.eyePosition.y,
                          gMiniCameraPtr->view.eyePosition.z);
            Vec3f_SetComponentsDuplicate(&gDisplayableMonIndices[gStageSnapshotCount].unk_5C, gMiniCameraPtr->view.targetPosition.x, gMiniCameraPtr->view.targetPosition.y,
                          gMiniCameraPtr->view.targetPosition.z);
            gDisplayableMonIndices[gStageSnapshotCount].unk_6C = 0;

            gDisplayableMonIndices[gStageSnapshotCount].unk_6C = gCameraParams->unk_014 * 4;
            if (gCameraParams->unk_008 == 25.0f) {
                gDisplayableMonIndices[gStageSnapshotCount].unk_6C |= 2;
            }

            gDisplayableMonIndices[gStageSnapshotCount].unk_6C |= gCameraParams->unk_00C * 0x10;
            gDisplayableMonIndices[gStageSnapshotCount].unk_68 = gParticleFrameCounter;

            gDisplayableMonIndices[gStageSnapshotCount].unk_00 = gStagePanelState->unk_004.position.y;
            gDisplayableMonIndices[gStageSnapshotCount].unk_04 = gStagePanelState->unk_16C;
            gDisplayableMonIndices[gStageSnapshotCount].unk_06 = gStagePanelState->unk_004.transformAnim.frameFixed >> 0x10;
            gDisplayableMonIndices[gStageSnapshotCount].unk_05 = gStagePanelState->unk_004.transformAnim.animationId;
            gDisplayableMonIndices[gStageSnapshotCount].unk_08 = gStagePanelState->unk_004.eventTrack.trackId;
            gDisplayableMonIndices[gStageSnapshotCount].unk_09 = gStagePanelState->unk_004.eventTrack.frame;

            Particle31_PackSlotData(gDisplayableMonIndices[gStageSnapshotCount].unk_0A);
            Gallery_CopyPhotoMonFromBattleMon(&gDisplayableMonIndices[gStageSnapshotCount].unk_70, &gStagePanelPlayerData);
        }

        gStagePanelAnimState.unk_00 = 0x14;

        gStageSnapshotCount++;
        if (gStagePanelMode != 1) {
            gGalleryPhotoPoolCount = gStageSnapshotCount;
        }

        StageLoader_RunFrames(1);
    }
}

void Gallery_SwitchBackgroundSet(u16 arg0) {
    if (arg0 & 0xC00) {
        Gallery_FinalizeBackgroundLoad();
        Gallery_LoadBackgroundSet(gCameraParams->unk_00C);
        gStagePanelTransitionTimer = 5;
    }
}

s32 Gallery_HandleBackgroundSwitchInput(void) {
    u16 sp1E;

    if ((gControllerInput.buttonPressed & 0x8020) && (gCameraNavActiveFlag == 1)) {
        gCameraNavActiveFlag = ~gCameraNavActiveFlag + 2;
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
        return 1;
    }

    if (gControllerInput.buttonPressed & 0x20) {
        gCameraNavActiveFlag = ~gCameraNavActiveFlag + 2;
        Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
    }

    if (gCameraNavActiveFlag == 0) {
        return 0;
    }

    if (!(gControllerInput.buttonDown & 0xC00)) {
        gCameraNavRepeatCounter = -1;
        return 1;
    }

    sp1E = gControllerInput.buttonDown;
    gCameraNavRepeatCounter++;
    if ((gCameraNavRepeatCounter >= 8) || (gCameraNavRepeatCounter == 0)) {
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
        if (gCameraNavRepeatCounter != 0) {
            gCameraNavRepeatCounter = 6;
        }
    } else {
        return 1;
    }

    gCameraNavMaxIndex = 0xF;
    if (gGallerySessionModeSettings.mewtwoUnlocked == 0) {
        if (gGallerySessionModeSettings.castleClears == 8) {
            gCameraNavMaxIndex--;
        } else if (gGallerySessionModeSettings.castleClears == 7) {
            gCameraNavMaxIndex -= 2;
        } else {
            gCameraNavMaxIndex -= 2;
            gCameraNavMaxIndex += gGallerySessionModeSettings.castleClears - 7;
        }
    }

    if ((sp1E & 0x800) && (gCameraNavTransition == 0) && (gCameraNavSubIndex == 0) && (gCameraParams->unk_00C == 0)) {
        gCameraNavSubIndex = gCameraNavMaxIndex - 4;
        gCameraParams->unk_00C = gCameraNavMaxIndex;
        gCameraNavTransition = 4;
        Gallery_SwitchBackgroundSet(sp1E);
    } else if ((sp1E & 0x400) && (gCameraNavTransition == 4) && (gCameraNavSubIndex == (gCameraNavMaxIndex - 4)) &&
               (gCameraNavMaxIndex == gCameraParams->unk_00C)) {
        gCameraNavSubIndex = 0;
        gCameraParams->unk_00C = 0;
        gCameraNavTransition = 0;
        Gallery_SwitchBackgroundSet(sp1E);
    } else {
        if (sp1E & 0x800) {
            if (gCameraParams->unk_00C > 0) {
                gCameraParams->unk_00C--;
            }
        }

        if (sp1E & 0x400) {
            if (gCameraParams->unk_00C < gCameraNavMaxIndex) {
                gCameraParams->unk_00C++;
            }
        }

        if ((sp1E & 0x800) && (gCameraNavTransition == 0) && (gCameraNavSubIndex > 0)) {
            gCameraNavSubIndex--;
        }

        if ((sp1E & 0x400) && (gCameraNavTransition == 4) && (gCameraNavSubIndex < (gCameraNavMaxIndex - 4))) {
            gCameraNavSubIndex++;
        }

        if ((sp1E & 0x800) && (gCameraNavTransition > 0)) {
            gCameraNavTransition--;
        }

        if ((sp1E & 0x400) && (gCameraNavTransition < 4)) {
            gCameraNavTransition++;
        }

        Gallery_SwitchBackgroundSet(sp1E);
    }

    return 1;
}

void Gallery_CameraUpdateTutorialText(void) {
    s32 i;

    if ((gStagePanelMode == 1) && (gCameraTutorialTextIds[gTutorialMessageIndex] != 0xFF)) {
        gTutorialMessageTimer++;
        if (gTutorialMessageTimer == gTutorialTextTimers[gTutorialMessageIndex].unk_00) {
            gTutorialTextBoxState.unk_00 = 0;
        }

        if (gTutorialMessageTimer == gTutorialTextTimers[gTutorialMessageIndex].unk_04) {
            gTutorialTextBoxState.unk_00 = 0xFF;
            gTutorialMessageIndex++;
        }

        switch (gTutorialTextBoxState.unk_00) {
            case 0x0:
                gTutorialTextBoxState.unk_04 = 0;
                gTutorialTextBoxState.unk_08 = 0xC8;
                gTutorialTextBoxState.unk_0C = 0x14;

                // clang-format off
                for (i = 0; i < 255; i++) { gTutorialTextBoxCharBuffer[i] = 0; }
                // clang-format on

                sprintf(gTutorialTextBoxStringBuffer, Text_GetString(NULL, 0, gCameraHelpText, gCameraTutorialTextIds[gTutorialMessageIndex]));
                gTutorialTextBoxState.unk_00++;
                break;

            case 0x1:
                gTutorialTextBoxState.unk_0C = Math_StepToS32(gTutorialTextBoxState.unk_0C, 0x23, 0xA, 0xA);
                if (gTutorialTextBoxState.unk_0C == 0x23) {
                    gTutorialTextBoxState.unk_00++;
                }
                break;

            case 0x2:
                if (gTutorialTextBoxStringBuffer[gTutorialTextBoxState.unk_04] != 0) {
                    gTutorialTextBoxCharBuffer[gTutorialTextBoxState.unk_04] = gTutorialTextBoxStringBuffer[gTutorialTextBoxState.unk_04];
                    gTutorialTextBoxState.unk_04++;
                    Audio_PlaySoundEffectById(SFX_CAMERA_TEXT);
                }
                break;

            case 0xFF:
                break;
        }
    }
}

s32 Gallery_CameraExitState_Active(void) {
    Gallery_CameraUpdateTutorialText();

    if (gStagePanelAnimState.unk_00 != 0) {
        gStagePanelAnimState.unk_02 -= 3;
    }

    gStagePanelAnimState.unk_00 = Math_StepToS32(gStagePanelAnimState.unk_00, 0, 2, 2);

    if (gStagePanelTransitionTimer) {
        gStagePanelTransitionTimer--;
        return 0;
    }

    if (gStageSnapshotCount && gStageSnapshotCount && gStageSnapshotCount) {}

    if (gStageSnapshotCount == 0x18) {
        gStagePanelAnimState.unk_00 = 0x14;
        gStagePanelFadeData.unk_04 = 0;
        gStagePanelStateIndex = 1;
        gStageSnapshotCount--;
        Audio_PlaySoundEffectById(SFX_CAMERA_FULL);
        return 0;
    }

    if (Gallery_HandleBackgroundSwitchInput() != 0) {
        return 0;
    }

    if (GalleryCamera_UpdateZoomFov() != 0) {
        return 0;
    }

    if (gPlayer1Controller->buttonPressed & 0x4000) {
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
        gStageReturnCode = 1;
        return gStageReturnCode;
    }

    if (gControllerInput.buttonPressed & 0x1000) {
        Audio_PlaySoundEffectById(SFX_CAMERA_EXIT);
        gStageReturnCode = 2;
        return gStageReturnCode;
    }

    if (gControllerInput.buttonPressed & 0x2000) {
        gCameraParams->unk_014++;
        if (gCameraParams->unk_014 == 3) {
            gCameraParams->unk_014 = 0;
        }

        if (gCameraParams->unk_014 == 0) {
            Audio_PlaySoundEffectById(SFX_MENU_BACK);
        } else {
            Audio_PlaySoundEffectById(SFX_MENU_CONFIRM);
        }
    }

    GalleryCamera_HandleDPadInput();
    GalleryCamera_HandleFreeMoveInput();
    Gallery_CapturePhoto();
    return 0;
}

s32 Gallery_CameraExitState_CountDown(void) {
    gStagePanelAnimState.unk_04 = 0;
    gStagePanelAnimState.unk_02 += 4;

    if (gStagePanelAnimState.unk_00 == 0) {
        if (gStageSnapshotCount != 0) {
            gStageSnapshotCount--;
            gStagePanelAnimState.unk_00 = 0x14;
        } else {
            gStagePanelStateIndex = 2;
            Audio_PlaySoundEffectById(SFX_CAMERA_EXIT);
            gStagePanelAnimState.unk_08 = 0xE38;
        }
    }

    gStagePanelAnimState.unk_00 = Math_StepToS32(gStagePanelAnimState.unk_00, 0, 0xF, 0xF);
    return 0;
}

s32 Gallery_CameraExitState_SpinOut(void) {
    gStagePanelAnimState.unk_02 += 4;
    gStagePanelAnimState.unk_04 = Math_StepToS32(gStagePanelAnimState.unk_04, 0x30, 0xA, 0xA);

    if (gStagePanelAnimState.unk_04 >= 0x30) {
        gStagePanelAnimState.unk_08 = Math_StepToS32(gStagePanelAnimState.unk_08, 0, 0xC8, 0xC8);
        gStagePanelAnimState.unk_0A -= 0x31C7;
        gStagePanelAnimState.unk_06 = SINS(gStagePanelAnimState.unk_0A) * gStagePanelAnimState.unk_08;
        if (gStagePanelAnimState.unk_08 == 0) {
            gStagePanelAnimState.unk_06 = 0;
            gStagePanelStateIndex = 3;
        }
    }
    return 0;
}

s32 Gallery_CameraExitState_ExpandBox(void) {
    gStagePanelFadeData.unk_04 = Math_StepToS32(gStagePanelFadeData.unk_04, 0x50, 0xA, 0xA);
    if (gStagePanelFadeData.unk_04 == 0x50) {
        gStagePanelFadeData.unk_0C = 0xAA;
        gStagePanelFadeData.unk_0E = 0xA;
        gStagePanelFadeData.unk_08 = 0xA0 - (gStagePanelFadeData.unk_0C / 2);
        gStagePanelFadeData.unk_0A = 0x78;
        gStagePanelFadeData.unk_00 = 0;
        gStagePanelStateIndex = 4;
        Audio_PlaySoundEffectById(0x30);
    }
    return 0;
}

s32 Gallery_CameraExitState_FadeOut(void) {
    gStagePanelFadeData.unk_0E = Math_StepToS32(gStagePanelFadeData.unk_0E, 0x32, 0xA, 0xA);

    if (gStagePanelFadeData.unk_0E == 0x32) {
        gStagePanelFadeData.unk_00++;
    }

    if (gStagePanelFadeData.unk_00 == 0x32) {
        gStageReturnCode = 2;
        return gStageReturnCode;
    }

    return 0;
}

s32 Gallery_CameraCheckExit(void) {
    switch (gStagePanelStateIndex) {
        case 0:
            return Gallery_CameraExitState_Active();

        case 1:
            return Gallery_CameraExitState_CountDown();

        case 2:
            return Gallery_CameraExitState_SpinOut();

        case 3:
            return Gallery_CameraExitState_ExpandBox();

        case 4:
            return Gallery_CameraExitState_FadeOut();
    }
}

void Gallery_CameraDraw(void) {
    BgStage_DrawFrame();

    if ((gStagePanelFillColor == -1) || (gStagePanelFillColor == 0)) {
        GfxImage_FillCurrent(&gDisplayListHead, 1);
    } else if (gStagePanelFillColor < 0x10000) {
        GfxImage_FillCurrent(&gDisplayListHead, gStagePanelFillColor);
    } else {
        GfxImage_FillCurrent(&gDisplayListHead, 0xA6BF);
    }

    GeoRender_AdvanceFrameCounter();
    Geo_RenderRootNode(gMiniCameraStruct);
    Gallery_DrawFrameMarkers();
    Gallery_CameraDrawHelpBalloon();
    Gallery_DrawPhotoCounterReel();
    Gallery_DrawPhotoCounterReelExit();
    Gallery_DrawTutorialTextBox();
    Gallery_DrawNicknameBanner(gStagePanelPlayerData.unk_30);
    Gallery_DrawBackgroundSelectMenu(gCameraParams->unk_00C);
    BgStage_AdvanceFrame();
}

void Gallery_CameraIntro(void) {
    f32 temp_fv0;
    f32 tmp1;
    f32 tmp2;
    f32 tmp3;
    f32 tmp4;
    s32 i;
    s16 var_s3 = 0;

    StageFade_StartFromOpaque(0x14);
    gStagePanelAnimState.unk_10 -= 0.8;
    gStagePanelAnimState.unk_14 = 0.0f;
    gStagePanelAnimState.unk_0C = 351.0f;

    for (i = 0; i < 15; i++) {
        Gallery_CameraReadInput();
        Gallery_CameraDraw();
    }

    tmp2 = 5.7f;
    tmp3 = 0.6f;
    tmp1 = 19.0f;
    Audio_PlaySoundEffectById(0x2F);
    tmp4 = tmp2 * tmp3;

    while (true) {
        Gallery_CameraReadInput();
        var_s3 = Math_StepToS32(var_s3, 0x46, 0xA, 0xA);
        gStagePanelAnimData.unk_00[0] = Math_StepToS32(gStagePanelAnimData.unk_00[0], 0xA0, 0xA, 0xA);
        gStagePanelAnimData.unk_00[4] = Math_StepToS32(gStagePanelAnimData.unk_00[4], 0x24, 0xA, 0xA);
        gStagePanelAnimData.unk_00[1] = Math_StepToS32(gStagePanelAnimData.unk_00[1], 0x120, 0xA, 0xA);
        gStagePanelAnimData.unk_00[5] = Math_StepToS32(gStagePanelAnimData.unk_00[5], 0x78, 0xA, 0xA);
        gStagePanelAnimData.unk_00[2] = Math_StepToS32(gStagePanelAnimData.unk_00[2], 0xA0, 0xA, 0xA);
        gStagePanelAnimData.unk_00[6] = Math_StepToS32(gStagePanelAnimData.unk_00[6], 0xCC, 0xA, 0xA);
        gStagePanelAnimData.unk_00[3] = Math_StepToS32(gStagePanelAnimData.unk_00[3], 0x20, 0xA, 0xA);
        gStagePanelAnimData.unk_00[7] = Math_StepToS32(gStagePanelAnimData.unk_00[7], 0x78, 0xA, 0xA);

        if (var_s3 < 0x3D) {
            GalleryCamera_AddPolarOffset(&gStagePanelAnimData.unk_10, &gStagePanelAnimData.unk_20, gStagePanelAnimData.unk_30 + 0x8000, 0xA);
            GalleryCamera_AddPolarOffset(&gStagePanelAnimData.unk_14, &gStagePanelAnimData.unk_24, gStagePanelAnimData.unk_32, 0xA);
            GalleryCamera_AddPolarOffset(&gStagePanelAnimData.unk_18, &gStagePanelAnimData.unk_28, gStagePanelAnimData.unk_34 + 0x8000, 0xA);
            GalleryCamera_AddPolarOffset(&gStagePanelAnimData.unk_1C, &gStagePanelAnimData.unk_2C, gStagePanelAnimData.unk_36, 0xA);
        }

        gStagePanelAnimState.unk_10 = -tmp1;

        temp_fv0 = -(gStagePanelAnimState.unk_0C - (tmp4 + 251.0f));
        if (temp_fv0 >= 0.0) {
            gStagePanelAnimState.unk_10 -= tmp2 * (0.0 - temp_fv0);
        }

        gStagePanelAnimState.unk_10 *= tmp3;
        gStagePanelAnimState.unk_14 += gStagePanelAnimState.unk_10;
        gStagePanelAnimState.unk_14 *= tmp3;

        gStagePanelAnimState.unk_0C += gStagePanelAnimState.unk_14;

        if ((gStagePanelAnimState.unk_14 >= 0.0f) && (gStagePanelAnimState.unk_14 <= 1.0f)) {
            break;
        }

        Gallery_CameraDraw();
    }

    Gallery_CameraReadInput();
    gStagePanelAnimState.unk_0C = 251.0f;
    Gallery_CameraDraw();
}

void Gallery_CameraLoop(void) {
    s32 var_s0 = 1;
    void (*temp_s1)(void) = Util_ConvertAddrToVirtAddr(Particle_UpdateFrameCountersAlias);

    while (var_s0 != 0) {
        Gallery_CameraReadInput();

        if (gStagePanelMode != 1) {
            GalleryCamera_CopyControllerState(&gControllerInput, gPlayer1Controller);
        } else {
            SimulateControllerInputFromScript();
        }

        if (Gallery_CameraCheckExit() != 0) {
            var_s0 = 0;
        }

        GalleryCamera_UpdatePokemonIdleAnim();
        GalleryCamera_UpdatePokemonEventLoop();
        GalleryCamera_ClearFinishedEventTrack();

        temp_s1();

        Gallery_CameraDraw();
    }
}

void Gallery_CameraOutro(void) {
    s32 i;

    StageFade_StartFromTransparent(0x1E);

    for (i = 0; i < 30; i++) {
        Gallery_CameraReadInput();
        Gallery_CameraDraw();
    }
}

void Gallery_CameraInit(s32 arg0) {
    gStagePanelMode = arg0;
    Gallery_InitBackgroundScene();

    gStagePanelState = main_pool_alloc(sizeof(GalleryState), 0);
    bzero(gStagePanelState, sizeof(GalleryState));

    gCameraParams = main_pool_alloc(sizeof(GalleryCameraParams), 0);
    bzero(gCameraParams, sizeof(GalleryCameraParams));

    GalleryCamera_InitIntroSequence();
    Gallery_SetupPhotoModel();
    GalleryCamera_InitFreeView();
    Gallery_LoadBackgroundSet(gCameraParams->unk_00C);
}

s32 Gallery_CameraMain(s32 arg0, UNUSED s32 arg1) {
    StageContext* sp24;

    main_pool_push_state('SNAP');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(0, 1, 3, 1, 2, 1);
    Font_Init(0x16, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_5000000, snap_mode_ui, 0);
    ASSET_LOAD(D_2000000, common_menu2_ui, 0);

    Save_EnsureBankLoaded(2);
    Save_GetModeSettings(&gGallerySessionModeSettings, 0);

    D_8690A698 = ASSET_LOAD2(stadium_models, 1, 1);
    Text_InitStringTables();
    gCameraHelpText = Text_GetStringTable(0x2B);
    Gallery_CameraInit(arg0);
    StageContext_Activate(sp24);
    Gallery_CameraIntro();
    Gallery_CameraLoop();
    Gallery_CameraOutro();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('SNAP');

    return gStageReturnCode;
}
