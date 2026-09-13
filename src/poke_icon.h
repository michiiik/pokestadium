#ifndef _1AB70_H_
#define _1AB70_H_

#include "global.h"
#include "gfx_rect.h"
#include "matrix.h"
#include "game_state.h"

typedef struct unk_func_8001B1FC {
    /* 0x00 */ u16 flags; // bit0=BattleMon variant set, bit1=input enabled, bit2=auto-rotate motion enabled
    /* 0x02 */ u16 paletteMask; // PokeIcon_SetPreviewMon/SetPreviewSpecies's arg2 & ~1-equivalent
    /* 0x04 */ s16 imageWidth; // PokeIcon_CreateModelPreview's arg1
    /* 0x06 */ s16 imageHeight; // PokeIcon_CreateModelPreview's arg2
    /* 0x08 */ s16 tileWidth; // PokeIcon_CreateModelPreview's arg5; PokeIcon_DrawPreview's blit width
    /* 0x0A */ s16 tileHeight; // PokeIcon_CreateModelPreview's arg6; PokeIcon_DrawPreview's row-loop bound
    /* 0x0C */ s32 unk_0C; // PokeIcon_CreateModelPreview's arg7 (u8*); mistyped s32 holding a pointer, consumed by PokeIcon_DrawPreview
    /* 0x10 */ s16 idleTimer; // PokeIcon_UpdatePreview: counts down before auto-rotate motion resumes
    /* 0x12 */ s16 rotateSpeed; // PokeIcon_AdvancePreviewMotion: ramps toward 0x100 by 0x10/tick
    /* 0x14 */ s32 unk_14; // PokeIcon_SetPreviewMon stores a BattleMon*, PokeIcon_SetPreviewSpecies stores a dexId; never read back
    /* 0x18 */ f32 modelScale;
    /* 0x1C */ unk_D_86002F58_004_000_010* loadContext;
    /* 0x20 */ GraphNode* sceneRoot;
    /* 0x24 */ unk_D_86002F58_004_000* displayObject;
    /* 0x28 */ unk_D_80068BB0* image;
    /* 0x2C */ unk_D_80068BB0* depthBuffer;
} unk_func_8001B1FC; // size = 0x30

typedef union unk_func_8001A024_008_014 {
    struct {
        /* 0x00 */ s16 unused;
        /* 0x02 */ s16 dexId; // PokeIcon_RenderSlot/ProcessNextSlot's non-BattleMon variant; species dexId
    };
    u8* monDexIdPtr; // PokeIcon_SetSlotFromMon's BattleMon variant; points at a BattleMon's species.dexId byte
    u32 raw;
} unk_func_8001A024_008_014; // size = 0x4

typedef struct unk_func_8001A024_008 {
    /* 0x00 */ s16 flags; // bit0=pending load, bit1=BattleMon-pointer variant selected in speciesRef
    /* 0x02 */ u16 paletteMask; // PokeIcon_SetSlotFromSpecies/Mon's arg3 & ~1
    /* 0x04 */ s16 cameraAngle; // PokeIcon_RenderSlot's Camera_ComputeEyeFromAngles arg
    /* 0x08 */ f32 scale;
    /* 0x0C */ unk_D_80068BB0* image; // this slot's rendered icon texture
    /* 0x10 */ arg1_func_80010CA8 sizeVariant; // Model_ComputeSizeVariant's output (PokeIcon_SetSlotFromMon)
    /* 0x14 */ unk_func_8001A024_008_014 speciesRef;
} unk_func_8001A024_008; // size = 0x18

typedef struct unk_func_8001A024 {
    /* 0x00 */ u8 flags; // bit0 = a load is in flight (PokeIcon_ResetRenderList/ProcessNextSlot)
    /* 0x01 */ u8 slotCount;
    /* 0x02 */ s8 loadCursor; // PokeIcon_ProcessNextSlot's round-robin scan index into slots
    /* 0x03 */ s8 lastRenderedSlot; // -1 sentinel; set to the slot index PokeIcon_RenderSlot just finished
    /* 0x04 */ unk_D_86002F58_004_000_010* loadContext; // PokeIcon_WaitFrameLoad/PollFrameLoad/RequestFrameLoad's arg0
    /* 0x08 */ unk_func_8001A024_008* slots;
    /* 0x0C */ GraphNode* sceneRoot; // process_geo_layout(D_8006F350) in PokeIcon_InitRenderScene
    /* 0x10 */ unk_D_86002F58_004_000* displayObject; // captured via PokeIcon_CaptureGeoNode; reused for every slot's render
    /* 0x14 */ unk_D_80068BB0* depthBuffer;
    /* 0x18 */ unk_D_8006FF00* nameTable;
} unk_func_8001A024; // size = 0x1C

typedef struct unk_D_84B25A28 {
    /* 0x00 */ u8 state; // BattlePrepRoster_Update's dispatch
    /* 0x01 */ u8 animTimer;
    /* 0x02 */ u8 isDoubleMode; // BattlePrepRoster_Init: set when either side needs the double-row layout
    /* 0x04 */ unk_D_86002F58_004_000* displayObjects; // 11-element array (indices 0-10) across InitDisplayObjects/PositionIcons*
    /* 0x08 */ unk_D_86002F34_00C* parentNode; // BattlePrepRoster_Init's arg1
    /* 0x0C */ GraphNode* teamAIconModel; // consumes displayObjects[1..4]
    /* 0x10 */ GraphNode* teamBIconModel; // consumes displayObjects[5..8]
    /* 0x14 */ GraphNode* leadIconModel; // consumes displayObjects[0]
    /* 0x18 */ GraphNode* extraIconModel; // consumes displayObjects[9]
    /* 0x1C */ GraphNode* backgroundModel; // consumes displayObjects[10]; positioned separately at (0,0,1000)
    /* 0x20 */ unk_func_8001A024* renderList; // PokeIcon_CreateRenderList/SetNameTable
    /* 0x24 */ unk_D_80068BB0* trainerIconTargetP1; // BattlePrep_DrawTrainerStatusIcon's arg0
    /* 0x28 */ unk_D_80068BB0* trainerIconTargetP2;
    /* 0x2C */ char unk2C[0x4];
} unk_D_84B25A28; // size = 0x30

unk_func_8001A024* PokeIcon_CreateRenderList(unk_D_86002F58_004_000_010*, s16, s16, s16);
void PokeIcon_AttachDepthBuffer(unk_func_8001A024*, unk_D_86002F58_004_000_010*, unk_D_80068BB0*);
void PokeIcon_SetNameTable(unk_func_8001A024*, unk_D_8006FF00*);
void PokeIcon_ResetRenderList(unk_func_8001A024*);
void PokeIcon_SetSlotFromSpecies(unk_func_8001A024*, s32, s32, u16);
void PokeIcon_SetSlotFromMon(unk_func_8001A024*, s32, BattleMon*, u16);
s32 PokeIcon_ProcessNextSlot(unk_func_8001A024*, s32);
s32 PokeIcon_ProcessAllSlots(unk_func_8001A024*, s32);
s32 PokeIcon_RenderNextSlotBlocking(unk_func_8001A024*, s32);
u8* PokeIcon_GetImage(unk_func_8001A024*, s32);
s32 PokeIcon_AreSlotsIdle(unk_func_8001A024*);
void PokeIcon_OpenModelArchive(void);
void PokeIcon_LoadModelTextureForMon(u16*, s32, BattleMon*);
unk_func_8001B1FC* PokeIcon_CreateModelPreview(unk_D_86002F58_004_000_010*, s16, s16, s16, s16, s16, s16, u8*);
void PokeIcon_SetPreviewMon(unk_func_8001B1FC* arg0, BattleMon* arg1, u16 arg2);
void PokeIcon_SetPreviewSpecies(unk_func_8001B1FC*, s32, u16);
u8* PokeIcon_RenderPreview(unk_func_8001B1FC*);

#endif // _1AB70_H_
