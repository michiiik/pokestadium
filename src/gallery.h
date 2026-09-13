#ifndef _30640_H_
#define _30640_H_

#include "global.h"
#include "model_renderer.h"
#include "src/gfx_rect.h"

// Compact mon summary stored inside a GalleryPhotoRecord. Gallery_CopyPhotoMonFromBattleMon packs a
// BattleMon down to this; Gallery_CopyBattleMonFromPhotoMon expands it back for display.
typedef struct GalleryPhotoMon {
    /* 0x00 */ union {
                    u16 otId;
                    u16 unk_00;
                };
    /* 0x02 */ union {
                    u8 species; // dex/display id; 0x99 = Surfing Pikachu
                    u8 unk_02;
                };
    /* 0x03 */ union {
                    u8 nickname[11];
                    s8 unk_03[11];
                };
    /* 0x0E */ union {
                    u8 otName[11];
                    s8 unk_0E[11];
                };
    /* 0x19 */ union {
                    char pad19[0x1];
                    char unk19[0x1];
                };
} GalleryPhotoMon; // size = 0x1A

typedef GalleryPhotoMon unk_D_83407AC8;
typedef GalleryPhotoMon unk_D_83402EE0_070;

typedef struct unk_D_83402EE0_00A {
    /* 0x00 */ u8 unk_00[7];
} unk_D_83402EE0_00A; // size = 0x7

// A saved Gallery photo. Written field-by-field on the shoot scene's A-press
// (fragment15_14CA70.c:653), persisted as save records 0x16/0x17, and expanded
// back into a BattleMon (from `mon`) when displayed. Historically decompiled as
// two types — unk_D_83402EE0 (shoot side) and unk_D_83403C60 (album side), the
// latter with a bogus BattleMon at +0x00; the writer proves the layout below.
typedef struct GalleryPhotoRecord {
    /* 0x00 */ union {
                    f32 yPos; // model y position at capture
                    f32 unk_00;
                };
    /* 0x04 */ union {
                    u8 species; // display id; Gallery_IsPhotoSpeciesValid's occupancy test reads this
                    u8 unk_04;
                };
    /* 0x05 */ union {
                    u8 animIndex; // ModelAnim_SetAnimation arg on restore
                    u8 unk_05;
                };
    /* 0x06 */ union {
                    s16 animFrame; // ModelAnim_SetFrame arg on restore
                    s16 unk_06;
                };
    /* 0x08 */ union {
                    u8 eventTrack; // ModelAnim_SetEventTrack arg on restore
                    u8 unk_08;
                };
    /* 0x09 */ union {
                    u8 eventFrame; // ModelAnim_SetEventFrame arg on restore
                    u8 unk_09;
                };
    /* 0x0A */ union {
                    unk_D_83402EE0_00A parts[10];
                    unk_D_83402EE0_00A unk_0A[10];
                };
    /* 0x50 */ union {
                    Vec3f cameraEye;
                    Vec3f unk_50;
                };
    /* 0x5C */ union {
                    Vec3f cameraAt;
                    Vec3f unk_5C;
                };
    /* 0x68 */ u32 captureFrame; // gParticleFrameCounter at capture time (Gallery_CapturePhoto)
    /* 0x6C */ union {
                    u8 flags;
                    u8 unk_6C;
                };
    /* 0x6D */ char unk6D[3];
    /* 0x70 */ union {
                    GalleryPhotoMon mon;
                    GalleryPhotoMon unk_70;
                };
    /* 0x8A */ char unk8A[0x2];
} GalleryPhotoRecord; // size = 0x8C

typedef GalleryPhotoRecord unk_D_83402EE0;
typedef GalleryPhotoRecord unk_D_83403C60;

typedef struct unk_D_83406EB0 {
    /* 0x00 */ char unk00[0x6C];
    /* 0x6C */ u8 flags; // Gallery_ControlRecordIsPopulated/IsDirty: bit0=populated, bit1=dirty
    /* 0x6D */ char unk6D[0x1F];
} unk_D_83406EB0; // size = 0x8C

typedef struct unk_D_83407B00 {
    /* 0x00 */ union {
                    struct {
                        s16 unk_00s;
                        s16 unk_02s;
                    };
                    s32 savedIndex; // Gallery_Save/RestorePosition; mirrors unk_D_83407B38.selectedIndex
                };
    /* 0x04 */ union {
                    struct {
                        s16 unk_04s;
                        s16 unk_06s;
                    };
                    s32 savedPage; // mirrors unk_D_83407B38.currentPage
                };
} unk_D_83407B00; // size = 0x8

typedef struct unk_func_800314BC_temp_v4 {
    /* 0x00 */ s16 animParamA; // Gallery_LoadPhotoSceneGraph: archive file 4's overlay record; copied into photoModel's animParamA
    /* 0x02 */ s16 animParamB;
    /* 0x04 */ s32 animParamC;
} unk_func_800314BC_temp_v4; // size = 0x8

typedef struct unk_func_80031270_024 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 flags; // bit0 = hasAnimData, set/cleared alongside hasAnimData
    /* 0x02 */ char pad2[0x12];
    /* 0x14 */ s32 hasAnimData; // Gallery_LoadPhotoSceneGraph: 1 if archive file 4 present, else 0
    /* 0x18 */ s16 animParamA; // copied from unk_func_800314BC_temp_v4.animParamA when hasAnimData
    /* 0x1A */ s16 animParamB;
    /* 0x1C */ s32 animParamC;
} unk_func_80031270_024; // size = 0x20

typedef struct unk_func_80031270 {
    /* 0x00 */ u16 state; // 0=pending, 1=processing, 2=done (Gallery_ProcessSceneInstance)
    /* 0x02 */ s16 renderStep; // 2=init model+scene graph, 1=render frame (decremented to 0, then state becomes done)
    /* 0x04 */ s16 width;
    /* 0x06 */ s16 height;
    /* 0x08 */ unk_D_80068BB0* colorBuffer;
    /* 0x0C */ unk_D_80068BB0* depthBuffer;
    /* 0x10 */ unk_D_86002F58_004_000_010* parentNode; // Gallery_RebindSceneGridResources's arg1
    /* 0x14 */ BinArchive* archive;
    /* 0x18 */ GalleryPhotoRecord* photoRecord;
    /* 0x1C */ GraphNode* sceneGraph; // Geo_LoadSceneGraphNode / Geo_RenderRootNode
    /* 0x20 */ unk_D_86002F58_004_000* modelNode; // Gallery_SetSceneModelNode
    /* 0x24 */ unk_func_80031270_024* photoModel; // Gallery_InitializePhotoModel
    /* 0x28 */ GraphNode* sceneContainer0; // Gallery_SetSceneGraphNode's case 0; Gallery_LoadPhotoSceneGraph appends archive file 0's geo layout
    /* 0x2C */ GraphNode* sceneContainer1; // case 1; archive file 1
    /* 0x30 */ GraphNode* sceneContainer2; // case 2; archive file 3
    /* 0x34 */ s32 fillColorArg; // GfxImage_FillCurrent's arg; -1/0 = plain fill, else a clamped species/index-derived value
} unk_func_80031270; // size = 0x38

typedef struct unk_D_83407B38_0B0 {
    /* 0x00 */ s32 unk_00;
    /* 0x00 */ s32 unk_08;
} unk_D_83407B38_0B0; // size >= 0xC

typedef struct unk_D_83407B38 {
    /* 0x00 */ s32 selectedIndex;
    /* 0x04 */ s32 hasExtraScene; // -1 = no extra/detail scene loaded, else uses sceneInstances[36] (extraScene)
    /* 0x08 */ s32 currentPage;
    /* 0x0C */ s32 pageCount;
    /* 0x10 */ s32 unk_10;
    /* 0x14 */ s32 scenesPerPage;
    /* 0x18 */ s32 gridWidth; // Gallery_IndexToGridPos/GridPosToIndex's row/column divisor
    /* 0x1C */ s32 gridHeight; // scenesPerPage / gridWidth
    /* 0x20 */ unk_func_80031270* sceneInstances[36];
    /* 0xB0 */ unk_func_80031270* extraScene; // the 37th "detail view" scene slot, used when hasExtraScene != -1
} unk_D_83407B38; // size = 0xB4

extern unk_D_83407B00 D_83407B30;

void Gallery_Dispatch(void);
void Gallery_ResetActiveScene(void);
void Gallery_ClearPhotoRenderTarget(void);
void Gallery_CopyPhotoMonFromBattleMon(GalleryPhotoMon*, BattleMon*);
void Gallery_CopyBattleMonFromPhotoMon(BattleMon*, GalleryPhotoMon*);
unk_func_80031270* Geo_CreateSceneInstance(s16, s16, unk_D_80068BB0*, unk_D_80068BB0*, unk_D_86002F58_004_000_010*, BinArchive*, GalleryPhotoRecord*);
u8* Gallery_ProcessSceneInstance(unk_func_80031270*);
s32 Gallery_IsSceneReady(unk_func_80031270*);
s32 Gallery_FindReadyScene(unk_func_80031270**, s32);
unk_func_80031270* Gallery_GetActiveScene(void);
void Gallery_ClearActiveScene(void);
s32 Gallery_IsPhotoSpeciesValid(GalleryPhotoRecord*);
f32 Math_AbsF32(f32);
void Vec3f_CrossProductComponents(f32, f32, f32, f32, f32, f32, f32*, f32*, f32*);
void Vec3f_NormalizeComponents(f32*, f32*, f32*);
void Mtx_BuildTranslatedDirectionRotation(Mtx*, f32, f32, f32, f32, f32, f32);
void MtxF_ExtractScale(MtxF*, MtxF*);
void MtxF_GetScaleVector(MtxF* arg0, Vec3f* arg1);

#endif // _30640_H_
