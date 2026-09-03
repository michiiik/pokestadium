#ifndef _FRAGMENT56_H_
#define _FRAGMENT56_H_

#include "global.h"

void Options_DrawBackground(void);
void Options_DrawPanelBox(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7);
void Options_DrawValueCapsule(s16 arg0, s16 arg1);
void Options_DrawMainPanel(s16 arg0, s32 arg1);
void Options_DrawConfirmPrompt(s32 arg0, s32 arg1, s32 arg2);
void Options_DrawEraseCompleteMessage(s32 arg0);
void Options_Draw(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 Options_ConfirmPrompt(s32 arg0);
void Options_ShowEraseCompleteScreen(void);
void Options_EraseSaveData(void);
void Options_ApplySoundSettings(u16 arg0);
s16 Options_ApplySelection(void);
void Options_Loop(void);
s32 Options_Main(UNUSED s32 arg0, UNUSED s32 arg1);

#endif // _FRAGMENT56_H_
