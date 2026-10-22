#ifndef _FRAGMENT39_H_
#define _FRAGMENT39_H_

#include "global.h"
#include "gfx_rect.h"

typedef struct ControllerState {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ u16 unk_04;
    /* 0x06 */ u16 unk_06;
    /* 0x08 */ u16 unk_08;
    /* 0x0A */ s16 unk_0A;
} ControllerState; // size = 0xC

typedef struct unk_D_8250A288 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
} unk_D_8250A288; // size >= 0xA

typedef struct UIElementNode {
    /* 0x00 */ char unk00[0x1];
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ char unk02[0x12];
    /* 0x14 */ s32 unk_14;
    /* 0x18 */ char unk18[0x6];
    /* 0x1E */ s16 unk_1E;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ s16 unk_22;
    /* 0x24 */ Vec3f unk_24;
    /* 0x30 */ Vec3f unk_30;
} UIElementNode; // size >= 0x3C

typedef struct UIWidget {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ u16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x08 */ s16 unk_0A;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E[4];
    /* 0x16 */ char unk16[0x16];
    /* 0x2C */ struct UIWidget* unk_2C;
    /* 0x30 */ struct UIWidget* unk_30;
    /* 0x34 */ struct UIWidget* unk_34;
    /* 0x38 */ DepthBuffer* unk_38;
    /* 0x3C */ UIElementNode unk_3C;
} UIWidget; // size >= 0x78

typedef struct UIElement {
    /* 0x000 */ s16 unk_000;
    /* 0x002 */ u16 unk_002;
    /* 0x004 */ s16 unk_004;
    /* 0x004 */ u16 unk_006;
    /* 0x008 */ s16 unk_008;
    /* 0x00A */ s16 unk_00A;
    /* 0x00C */ s16 unk_00C;
    /* 0x00E */ s16 unk_00E[4];
    /* 0x018 */ f32 unk_018;
    /* 0x01C */ f32 unk_01C;
    /* 0x020 */ Vec3f unk_020;
    /* 0x02C */ UIWidget* unk_02C;
    /* 0x030 */ UIWidget* unk_030;
    /* 0x034 */ s32 unk_034;
    /* 0x034 */ s32 unk_038;
    /* 0x03C */ UIElementNode unk_03C;
    /* 0x078 */ char unk078[0x12C];
} UIElement; // size = 0x1A4

typedef struct unk_D_8250A308 {
    /* 0x000 */ DisplayObject unk_000;
    /* 0x168 */ s16 unk_168;
    /* 0x16A */ s16 unk_16A;
    /* 0x16C */ s16 unk_16C;
    /* 0x16E */ s16 unk_16E;
    /* 0x170 */ s16 unk_170;
} unk_D_8250A308; // size = 0x174

typedef struct MatchBanner {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0A */ s16 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ char** unk_14;
} MatchBanner; // size = 0x18

typedef struct RoundBanner {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ char** unk_14;
} RoundBanner; // size = 0x18

extern FontContext* gTextObjectHandle;

s32 MiniGameSelect_DrawMenuBackdrop(s32 arg0, GraphNode* arg1);
s32 MiniGameSelect_DrawGameBackdrop(s32 arg0, GraphNode* arg1);
s32 MiniGameSelect_DrawStaticBackdrop(s32 arg0, GraphNode* arg1);
s32 MiniGameSelect_DrawLabelStrips(s32 arg0, GraphNode* arg1);
void MiniGameSelect_CreateCursor(void);
void MiniGameSelect_UpdateCursor(DisplayObject* a0);
s32 MiniGameSelect_CursorNodeCallback(s32 arg0, GraphNode* arg1);
void MiniGameSelect_ResetInputStates(void);
void MiniGameSelect_ReadInputsWithRepeat(void);
void MiniGameSelect_PollInputs(void);
s32 MiniGameSelect_ResolveGameID(void);
s32 MiniGameSelect_GameIDToIndex(s32 arg0);
s32 MiniGameSelect_FindTableIndex(s16 arg0, s32 arg1);
void MiniGameSelect_ResetAttractShuffle(void);
void MiniGameSelect_ClearWidgetSlot(UIElement* arg0);
void MiniGameSelect_RenderPlayerPanel(UIElement* arg0);
void MiniGameSelect_RenderNumberSpinner(UIElement* arg0);
UIWidget* MiniGameSelect_FindFreeWidgetSlot(void);
UIWidget* MiniGameSelect_CreateMenuWidget(s16 arg0, s16 arg1, GraphNode* arg2);
void MiniGameSelect_LinkWidgets(UIWidget* arg0, UIWidget* arg1);
void MiniGameSelect_ChainWidget(UIWidget* arg0, UIWidget* arg1);
void MiniGameSelect_SetWidgetState(UIWidget* arg0, s16 arg1);
void MiniGameSelect_SetWidgetTargetState(UIWidget* arg0, s16 arg1, s32 arg2);
s32 MiniGameSelect_IsWidgetInState(UIWidget* arg0, s16 arg1);
void MiniGameSelect_UpdateWidgetFade(UIElement* arg0);
s32 MiniGameSelect_CountWidgetChannels(UIWidget* arg0, s16 arg1);
s32 MiniGameSelect_StaggerWidgetFade(UIElement* arg0);
void MiniGameSelect_UpdateWidgets(void);
void MiniGameSelect_ClearOptionPanelState(void);
void MiniGameSelect_ResetSelection(void);
void MiniGameSelect_LoadSavedSelection(s16 arg0, s16 arg1);
void MiniGameSelect_InitMenuNodes(s16 arg0, s16 arg1);
void MiniGameSelect_DrawSelectionMarker(void);
void MiniGameSelect_RenderDirtyPanels(void);
void MiniGameSelect_DrawOptionPanel(void);
void MiniGameSelect_DrawFrame(void);
void MiniGameSelect_ReturnToIdle(void);
void MiniGameSelect_UpdateMainMenu(void);
void MiniGameSelect_LeaveJoinMenu(void);
void MiniGameSelect_PlayerJoinUpdate(void);
void MiniGameSelect_UpdateOptionPanel(void);
void MiniGameSelect_UpdateRoundSpinner(void);
s32 MiniGameSelect_UpdateGridCursor(s16* arg0, s16 arg1);
s32 MiniGameSelect_AttractCursorShuffle(s16* arg0, s16 arg1);
void MiniGameSelect_GameGridUpdate(void);
void MiniGameSelect_ConfirmUpdate(void);
void MiniGameSelect_MainLoop(s16 arg0, s16 arg1);
void MiniGameSelect_LoadScene(void);
s32 MiniGameSelect_Entry(s32 arg0, s32 arg1);

void KidsClub_InitMatchBanner(char** arg0);
s32 KidsClub_GetMatchBannerState(void);
void KidsClub_SetMatchBannerState(s16 arg0);
void KidsClub_AdvanceMatchBannerOpen(MatchBanner* arg0);
void KidsClub_UpdateMatchBannerIdle(MatchBanner* arg0);
void KidsClub_AdvanceMatchBannerClose(MatchBanner* arg0);
void KidsClub_UpdateMatchBanner(void);
void KidsClub_DrawRoundedFrame(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7);
void KidsClub_DrawWavyText(s16 arg0, s16 arg1, s8* arg2, f32 arg3, s16 arg4);
void KidsClub_DrawMatchBanner(void);
void KidsClub_InitRoundBanner(char** arg0);
void Minigames_NoOp(void);
void KidsClub_SetRoundBannerLabel(s16 arg0);
void KidsClub_SetRoundBannerState(s16 arg0, s16 arg1);
void KidsClub_AdvanceRoundBannerOpen(RoundBanner* arg0);
void KidsClub_UpdateRoundBannerCrossfade(RoundBanner* arg0);
void KidsClub_AdvanceRoundBannerClose(RoundBanner* arg0);
void KidsClub_UpdateRoundBanner(void);
void KidsClub_DrawRoundBanner(void);

void KidsClub_LoadDifficultySetting(void);
void KidsClub_SetSelectedGameId(s16);
void KidsClub_ResetTournamentState(s16);
s16 KidsClub_PickUnplayedGame(void);

#endif // _FRAGMENT39_H_
