#ifndef _FRAGMENT65_H_
#define _FRAGMENT65_H_

#include "global.h"

typedef struct unk_D_84A03138 {
    /* 0x00 */ u8 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ char* unk_08;
} unk_D_84A03138; // size = 0xC

typedef struct unk_D_84A02F00 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ s8 unk_14;
    /* 0x15 */ s8 unk_15;
    /* 0x16 */ s8 unk_16;
    /* 0x17 */ s8 unk_17;
    /* 0x18 */ Color_RGBA8 unk_18;
    /* 0x1C */ u8* unk_1C;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ s16 unk_22;
    /* 0x24 */ u8* unk_24;
} unk_D_84A02F00; // size = 0x28

void Glc_DrawBackgroundCrossfade(u8* arg0, u8* arg1, u8 arg2);
void Glc_DrawScaledTextureRgba(s16 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, u8* arg5, f32 arg6);
void func_84A00630(void);
void Glc_DrawScaledTextureIa8(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8* arg4, f32 arg5);
void Glc_DrawRoomDescription(void);
void Glc_DrawTrainerIntroPanels(void);
void Glc_DrawMapBorder(void);
void Glc_DrawRoomMarkers(void);
void Glc_DrawRoomLabels(void);
void Glc_DrawRoomInfoPanel(unk_D_84A02F00* arg0, u8 arg1, u8 arg2);
void Glc_DrawFinalRoomInfoPanel(unk_D_84A02F00* arg0, u8 arg1, u8 arg2);
void Glc_UpdateRoomInfoPanelFade(void);
void Glc_UpdateMapCursor(void);
void Glc_Draw(void);
s32 func_84A02074(void);
s32 Glc_SelectRoom(void);
void Glc_LoadTrainerPanels(void);
void Glc_ClearTrainerPanels(void);
void Glc_AnimateTrainerPanelsIn(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
s32 GymLeaderCastle_ShowIntro(void);
s32 Glc_AdvanceRoom(void);
s16 Glc_RunMenu(s16 arg0);
s16 Glc_InitMenu(s16 arg0);
s32 GymLeaderCastle_Main(s32 arg0, UNUSED s32 arg1);

#endif // _FRAGMENT65_H_
