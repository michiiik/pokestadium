#ifndef _FRAGMENT55_H_
#define _FRAGMENT55_H_

#include "global.h"
#include "src/game_state.h"

typedef struct unk_D_83003CE0 {
    /* 0x00 */ char* unk_00[29];
    /* 0x74 */ char* unk_74;
    /* 0x78 */ char* unk_78;
    /* 0x7C */ char* unk_7C;
    /* 0x80 */ char* unk_80;
    /* 0x84 */ char* unk_84;
    /* 0x88 */ char* unk_88;
    /* 0x8C */ char* unk_8C;
    /* 0x90 */ char* unk_90;
    /* 0x94 */ char* unk_94;
    /* 0x98 */ char* unk_98;
    /* 0x9C */ char unk9C[0x64];
} unk_D_83003CE0; // size = 0x100

typedef struct unk_D_83079E70_014_000 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
} unk_D_83079E70_014_000; // size = 0x4

typedef struct unk_D_83079E70_000 {
    /* 0x00 */ unk_D_83079E70_014_000 unk_00[6];
    /* 0x18 */ struct unk_D_83079E70_000* unk_18;
    /* 0x1C */ struct unk_D_83079E70_000* unk_1C;
} unk_D_83079E70_000; // size = 0x20

typedef struct unk_D_83079E70 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s16 unk_0C;
    /* 0x10 */ unk_D_83079E70_000* unk_10;
    /* 0x14 */ unk_D_83079E70_000 unk_14[6];
} unk_D_83079E70; // size = 0xD4

extern unk_D_83003CE0 D_83003CE0;
extern unk_D_83003CE0 D_83003DE0;

extern char** D_830039C0;

char* RentalRules_GetPokemonName(s32 arg0);
void RentalRules_LoadBackgroundArchive(void);
void RentalRules_PollInput(void);
s16* RentalRules_GetRuleIdList(SessionContext* arg0);
char* RentalRules_GetModeLabel(s8 arg0);
char* RentalRules_GetCategoryLabel(s8 arg0);
s32 RentalRules_RuleListHasEmptyEntry(s16* arg0);
void RentalRules_UpdateFadeIn(void);
void RentalRules_UpdateMainInput(void);
void RentalRules_UpdateWaitPopupClose(void);
void RentalRules_UpdateFadeOut(void);
s32 RentalRules_UpdateExitDelay(void);
void RentalRules_DrawBackgroundFallback(void);
void RentalRules_DrawHeaderBanner(void);
void RentalRules_DrawWindowFrame(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void RentalRules_DrawFilledPanel(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6);
void RentalRules_DrawBulletIcon(s16 arg0, s16 arg1);
void RentalRules_DrawRuleListText(s16 arg0, s16 arg1);
void RentalRules_DrawRuleListPanel(void);
void RentalRules_DrawFrame(void);
void RentalRules_LoadCategoryStrings(void);
void RentalRules_LoadDescriptionStrings(void);
void RentalRules_Init(void);
s32 RentalRules_Update(void);
void RentalRules_Loop(void);
s32 RentalRules_Main(UNUSED s32 arg0, UNUSED s32 arg1);

s32 RentalRules_CountTextLines(s8* arg0);
void RentalRules_PopupUpdateOpen(void);
void RentalRules_PopupUpdateShown(void);
void RentalRules_PopupUpdateClose(void);
void RentalRules_PopupReset(void);
void RentalRules_PopupUpdate(void);
void RentalRules_PopupDraw(void);
void RentalRules_PopupOpen(s16 arg0);
s32 RentalRules_PopupIsActive(void);
s32 RentalRules_PopupGetProgress(void);

s32 RentalRules_SpeciesMatchesCategory(s16 arg0, s16 arg1);
s16 RentalRules_CountMatchingSpecies(s16 arg0);
s32 RentalRules_BuildSpeciesPage(unk_D_83079E70_000* arg0, s16 arg1, s16 arg2);
unk_D_83079E70_000* RentalRules_AdvancePageNode(unk_D_83079E70_000* arg0, s32 arg1);
s32 RentalRules_ScrollPageUp(unk_D_83079E70* arg0);
s32 RentalRules_ScrollPageDown(unk_D_83079E70* arg0);
void RentalRules_ListUpdateOpen(unk_D_83079E70* arg0);
void RentalRules_ListUpdateShown(unk_D_83079E70* arg0);
void RentalRules_ListUpdateScrolling(unk_D_83079E70* arg0);
void RentalRules_ListUpdateClose(unk_D_83079E70* arg0);
void RentalRules_DrawScrollbar(s16 arg0, s16 arg1, unk_D_83079E70* arg2);
void RentalRules_DrawCloseButtonIcon(s16 arg0, s16 arg1);
void RentalRules_DrawSpeciesPage(s16 arg0, s16 arg1, unk_D_83079E70_000* arg2);
void RentalRules_ListInit(s32 arg0);
void RentalRules_ListUpdate(void);
void RentalRules_ListDraw(void);
void RentalRules_ListOpen(void);
s32 RentalRules_ListIsActive(void);
s32 RentalRules_ListGetProgress(void);


#endif // _FRAGMENT55_H_
