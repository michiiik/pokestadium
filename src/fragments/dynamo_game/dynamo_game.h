#ifndef _FRAGMENT11_H_
#define _FRAGMENT11_H_

#include "global.h"
#include "src/geo_node.h"

typedef struct DynamoHealthBar {
    /* 0x000 */ s32 unk_000;
    /* 0x004 */ s32 unk_004;
    /* 0x008 */ s32 unk_008;
    /* 0x00C */ u8 unk_00C;
    /* 0x00D */ s8 unk_00D;
    /* 0x00E */ s8 unk_00E;
    /* 0x010 */ s16 unk_010;
    /* 0x010 */ s16 unk_012;
    /* 0x014 */ DisplayObject unk_014;
    /* 0x17C */ unk_D_86002F34_alt9* unk_17C;
} DynamoHealthBar; // size = 0x180

typedef struct unk_D_8650AB30_000 {
    /* 0x00 */ s8 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
} unk_D_8650AB30_000; // size = 0xC

typedef struct DynamoHealthBarEffect {
    /* 0x00 */ unk_D_8650AB30_000 unk_00[12];
} DynamoHealthBarEffect; // size = 0x90

extern s32 gHealthBarPaletteIndex;
extern s32 gHealthBarMode;
extern u8 gHealthBarTimer;
extern u8 gHealthBarDuration;
extern DynamoHealthBar gDynamoChargeBarData[4];
extern DynamoHealthBar* gCurrentHealthBar;
extern DynamoHealthBarEffect gHealthBarEffects[4];
extern DynamoHealthBarEffect* gHealthBarEffectData;
extern u32 HealthBarPalette0[];
extern u32 HealthBarPalette1[];
extern u32 HealthBarPalette2[];
extern u32 HealthBarPalette3[];
extern u32 HealthBarPalette4[];

s8 DynamoGame_RollAIChargeBias(s32, s32);
void DynamoGame_DrawSparkEffects(s32 arg0);
void DynamoGame_InitSparkEffects(void);
void DynamoGame_PickNextCrankCue(void);
void DynamoGame_DrawCrankIcon(void);
void DynamoGame_InitCrankIconDisplay(void);

#endif // _FRAGMENT11_H_
