#ifndef _FRAGMENT58_H_
#define _FRAGMENT58_H_

#include "global.h"

void FreeBattle_DrawHeaderImage(s16 arg0, s16 arg1);
void FreeBattle_DrawBannerImage(s16 arg0, s16 arg1);
void FreeBattle_DrawTintedBar(s16 arg0, s16 arg1, s16 arg2, Color_RGBA8* arg3, Color_RGBA8* arg4);
void FreeBattle_DrawTitleBar(void);
void FreeBattle_DrawSelectionCorners(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void FreeBattle_DrawPanelFrame(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void FreeBattle_DrawSlicedFrame(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7);
s32 FreeBattle_IsRuleAvailable(s16 arg0);
void FreeBattle_DrawRuleMark(s16 arg0, s16 arg1, s16 arg2);
void FreeBattle_DrawRuleGrid(void);
void FreeBattle_UpdateRuleRowSelect(void);
void FreeBattle_UpdateRuleOptionSelect(void);
void FreeBattle_ClearRuleSelection(void);
s32 FreeBattle_SelectionHasSubrule(void);
void FreeBattle_DrawLevelOptionLine(s16 arg0, s16 arg1, s16 arg2, char* arg3);
void func_84001AD8(void);
void FreeBattle_DrawLevelSelectPanel(void);
void FreeBattle_UpdateLevelSelect(void);
void FreeBattle_ClearLevelSelection(void);
void FreeBattle_DrawConfirmLine(s16 arg0, char* arg1);
void FreeBattle_DrawConfirmPanel(void);
void FreeBattle_InitConfirmPanel(void);
void FreeBattle_OpenConfirmPanel(void);
void FreeBattle_CloseConfirmPanel(void);
void FreeBattle_UpdateConfirmDialog(void);
void FreeBattle_PollInputs(void);
void FreeBattle_DrawFrame(void);
void FreeBattle_FadeIn(void);
void FreeBattle_FadeOut(void);
void FreeBattle_SlideInPanel(void);
void FreeBattle_SlideOutPanel(void);
void FreeBattle_ApplyRuleSelection(void);
s32 FreeBattle_RuleSelectLoop(void);
s32 FreeBattle_RuleSelectMain(s32 arg0, s32 arg1);

#endif // _FRAGMENT58_H_
