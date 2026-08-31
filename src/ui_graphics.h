#ifndef _20470_H_
#define _20470_H_

#include "global.h"

extern Gfx D_8006F470[];
extern Gfx D_8006F498[];
extern Gfx D_8006F4C0[];
extern Gfx D_8006F4E0[];
extern Gfx D_8006F518[];
extern Gfx D_8006F558[];
extern Gfx D_8006F5A0[];
extern Gfx D_8006F608[];
extern Gfx D_8006F630[];
extern Gfx D_8006F710[];
extern Gfx D_8006F750[];
extern Gfx D_8006F768[];
extern Gfx D_8006F798[];

void Ui_DrawTexturedRect8x8(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u8* arg4);
void Ui_DrawBorderStyleA(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Ui_DrawBorderStyleB(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Ui_DrawBorderStyleC(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Ui_FillInsetRectangleA(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u16 arg4);
void Ui_FillInsetRectangleB(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u16 arg4);
void Ui_DrawGradientRectangle(s32 arg0, s32 arg1, s32 arg2, s32 arg3, Color_RGB8* arg4, Color_RGB8* arg5);
void Ui_DrawBorderedPanel(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u16 arg4);
void Ui_DrawGradientPanel(s32 arg0, s32 arg1, s32 arg2, s32 arg3, Color_RGB8* arg4, Color_RGB8* arg5);
void Ui_DrawBorderedPanelStyleB(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u16 arg4);
void Ui_DrawBorderedPanelNoFill(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Ui_DrawTextureMarker(s32 arg0, s32 arg1);
void Ui_DrawAnimatedTextureMarker(s16 arg0, s16 arg1);
void Ui_Draw20x20Icon(s32 arg0, s32 arg1, s32 arg2);
void BattleHud_DrawHpBarTexture(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
void BattleHud_DrawHpBar16x5(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Text_LoadNameTable(void);

#endif // _20470_H_
