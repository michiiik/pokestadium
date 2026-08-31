#ifndef _FRAGMENT54_H_
#define _FRAGMENT54_H_

#include "global.h"
#include "src/save_data.h"

void RentalHub_DrawHeaderIcon(s16 arg0, s16 arg1);
void RentalHub_DrawHeaderBar(s16 arg0, s16 arg1, s16 arg2, Color_RGBA8* arg3, Color_RGBA8* arg4);
void RentalHub_DrawHeader(void);
void RentalHub_DrawSelectionCursor(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void RentalHub_DrawMenuItemBorder(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void RentalHub_DrawMenuItemBox(s16 arg0, s16 arg1, s16 arg2, char* arg3);
void RentalHub_DrawMenuItems(s16 arg0);
s32 RentalHub_HandleInput(void);
void RentalHub_Draw(s16 arg0);
void RentalHub_FadeInWait(void);
void RentalHub_FadeOutWait(void);
s16 RentalHub_MenuLoop(void);
void RentalHub_LoadBackgroundImage(ModeSettings* arg0);
s32 RentalHub_MenuMain(void);
s32 RentalHub_LoadRentalPicker(void);
s32 RentalHub_LoadRentalRules(void);
s32 RentalHub_Main(UNUSED s32 arg0, UNUSED s32 arg1);

#endif // _FRAGMENT54_H_
