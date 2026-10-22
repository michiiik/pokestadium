#ifndef _1AB70_H_
#define _1AB70_H_

#include "global.h"
#include "gfx_rect.h"
#include "matrix.h"
#include "game_state.h"

typedef struct ModelPreview {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ ModelLoadContext* unk_1C;
    /* 0x20 */ GraphNode* unk_20;
    /* 0x24 */ DisplayObject* unk_24;
    /* 0x28 */ DepthBuffer* unk_28;
    /* 0x2C */ DepthBuffer* unk_2C;
} ModelPreview; // size = 0x30

typedef union unk_func_8001A024_008_014 {
    struct {
        /* 0x00 */ s16 unk_00;
        /* 0x02 */ s16 unk_02;
    };
    u8* ptr;
    u32 raw;
} unk_func_8001A024_008_014; // size = 0x4

typedef struct SpriteSlot {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ DepthBuffer* unk_0C;
    /* 0x10 */ arg1_func_80010CA8 unk_10;
    /* 0x14 */ unk_func_8001A024_008_014 unk_14;
} SpriteSlot; // size = 0x18

typedef struct SpriteDisplay {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ s8 unk_02;
    /* 0x03 */ s8 unk_03;
    /* 0x04 */ ModelLoadContext* unk_04;
    /* 0x08 */ SpriteSlot* unk_08;
    /* 0x0C */ GraphNode* unk_0C;
    /* 0x10 */ DisplayObject* unk_10;
    /* 0x14 */ DepthBuffer* unk_14;
    /* 0x18 */ SpeciesDisplayParams* unk_18;
} SpriteDisplay; // size = 0x1C

typedef struct BattleUiState {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x04 */ DisplayObject* unk_04;
    /* 0x08 */ CameraNode* unk_08;
    /* 0x0C */ GraphNode* unk_0C;
    /* 0x10 */ GraphNode* unk_10;
    /* 0x14 */ GraphNode* unk_14;
    /* 0x18 */ GraphNode* unk_18;
    /* 0x1C */ GraphNode* unk_1C;
    /* 0x20 */ SpriteDisplay* unk_20;
    /* 0x24 */ DepthBuffer* unk_24;
    /* 0x28 */ DepthBuffer* unk_28;
    /* 0x2C */ char unk2C[0x4];
} BattleUiState; // size = 0x30

SpriteDisplay* PokeIcon_CreateRenderList(ModelLoadContext*, s16, s16, s16);
void PokeIcon_AttachDepthBuffer(SpriteDisplay*, ModelLoadContext*, DepthBuffer*);
void PokeIcon_SetNameTable(SpriteDisplay*, SpeciesDisplayParams*);
void PokeIcon_ResetRenderList(SpriteDisplay*);
void SpriteDisplay_SetupSlot(SpriteDisplay*, s32, s32, u16);
void SpriteDisplay_ConfigSlotWithAnim(SpriteDisplay*, s32, BattleMon*, u16);
s32 PokeIcon_ProcessNextSlot(SpriteDisplay*, s32);
s32 PokeIcon_ProcessAllSlots(SpriteDisplay*, s32);
s32 PokeIcon_RenderNextSlotBlocking(SpriteDisplay*, s32);
u8* PokeIcon_GetImage(SpriteDisplay*, s32);
s32 PokeIcon_AreSlotsIdle(SpriteDisplay*);
void PokeIcon_OpenModelArchive(void);
void PokeIcon_LoadModelTextureForMon(u16*, s32, BattleMon*);
ModelPreview* PokeIcon_CreateModelPreview(ModelLoadContext*, s16, s16, s16, s16, s16, s16, u8*);
void PokeIcon_SetPreviewMon(ModelPreview* arg0, BattleMon* arg1, u16 arg2);
void PokeIcon_SetPreviewSpecies(ModelPreview*, s32, u16);
u8* PokeIcon_RenderPreview(ModelPreview*);

#endif // _1AB70_H_
