#ifndef _FRAGMENT41_H_
#define _FRAGMENT41_H_

#include "global.h"
#include "src/poke_icon.h"
#include "src/game_state.h"

typedef struct unk_func_82F00838_sp30_00C {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ char unk01[0xD];
    /* 0x0E */ u16 unk_0E;
    /* 0x10 */ char unk10[0x14];
    /* 0x24 */ u8 unk_24;
    /* 0x25 */ char unk25[0xB];
    /* 0x30 */ char unk_30[11];
    /* 0x3B */ char unk_3B[11];
    /* 0x46 */ char unk46[0xA];
    /* 0x50 */ s32 force_align;
} unk_func_82F00838_sp30_00C; // size = 0x54

typedef struct PickerPokemonGroup {
    /* 0x000 */ s16 unk_000;
    /* 0x004 */ struct PickerPokemonGroup* unk_004;
    /* 0x008 */ struct PickerPokemonGroup* unk_008;
    /* 0x00C */ unk_func_82F00838_sp30_00C unk_00C[20];
} PickerPokemonGroup; // size = 0x69C

typedef struct PickerState {
    /* 0x0000 */ s16 unk_0000;
    /* 0x0002 */ s16 unk_0002;
    /* 0x0004 */ s16 unk_0004;
    /* 0x0006 */ s16 unk_0006;
    /* 0x0008 */ s16 unk_0008;
    /* 0x000A */ s16 unk_000A;
    /* 0x000C */ s16 unk_000C;
    /* 0x000E */ s16 unk_000E;
    /* 0x0010 */ s16 unk_0010;
    /* 0x0012 */ s16 unk_0012;
    /* 0x0014 */ s16 unk_0014;
    /* 0x0016 */ s16 unk_0016;
    /* 0x0018 */ s16 unk_0018;
    /* 0x001A */ s16 unk_001A;
    /* 0x001C */ s16 unk_001C;
    /* 0x001E */ s16 unk_001E;
    /* 0x0020 */ s16 unk_0020;
    /* 0x0022 */ char unk0022[0x6];
    /* 0x0028 */ u16 unk_0028[0x640];
    /* 0x0CA8 */ PickerPokemonGroup unk_0CA8[13];
    /* 0x6294 */ char unk6294[0x4];
} PickerState; // size = 0x6298

typedef struct unk_func_82F00838_sp30 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0C */ unk_func_82F00838_sp30_00C* unk_0C;
} unk_func_82F00838_sp30; // size = 0x10

typedef struct PlayerTradeInfo {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 unk_05;
    /* 0x06 */ u16 unk_06;
    /* 0x08 */ char unk_08[1];
    /* 0x09 */ char unk09[0xD];
} PlayerTradeInfo; // size = 0x16

typedef struct TextMenuListEntry {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ char* unk_04;
} TextMenuListEntry; // size = 0x8

typedef struct TradeInfoBoxState {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 unk_10;
    /* 0x14 */ BattleMon* unk_14;
    /* 0x18 */ PickerState* unk_18;
    /* 0x1C */ s32 unk_1C;
    /* 0x20 */ ModelLoadContext* unk_20;
    /* 0x24 */ ModelPreview* unk_24;
} TradeInfoBoxState; // size >= 0x28

typedef struct PanelRect {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
} PanelRect; // size = 0x8

typedef struct unk_D_82F20A40 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ PanelRect unk_06;
    /* 0x0E */ PanelRect unk_0E[3];
    /* 0x26 */ PanelRect unk_26;
    /* 0x2E */ PanelRect unk_2E;
    /* 0x36 */ PanelRect unk_36;
    /* 0x40 */ PanelRect* unk_40;
    /* 0x44 */ PanelRect* unk_44;
} unk_D_82F20A40; // size = 0x48

typedef struct TradeSlotInfo {
    /* 0x000 */ s16 unk_000;
    /* 0x002 */ s16 unk_002;
    /* 0x004 */ s16 unk_004;
    /* 0x006 */ s16 unk_006;
    /* 0x008 */ s16 unk_008;
    /* 0x00A */ s16 unk_00A;
    /* 0x00C */ s16 unk_00C;
    /* 0x00E */ s16 unk_00E;
    /* 0x010 */ s16 unk_010;
    /* 0x012 */ s16 unk_012;
    /* 0x014 */ s16 unk_014;
    /* 0x018 */ f32 unk_018;
    /* 0x01C */ f32 unk_01C;
    /* 0x020 */ f32 unk_020;
    /* 0x024 */ u8* unk_024;
    /* 0x028 */ DisplayObject unk_028;
} TradeSlotInfo; // size = 0x190

typedef struct RentalMenuState {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ u16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ PanelRect unk_14[2];
    /* 0x24 */ PanelRect unk_24;
    /* 0x2C */ PanelRect unk_2C;
    /* 0x34 */ PanelRect unk_34;
} RentalMenuState; // size = 0x3C

typedef struct TradeCableLight {
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
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
} TradeCableLight; // size = 0x20

typedef struct TradeCableState {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ u16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ u16 unk_14;
    /* 0x16 */ u16 unk_16;
    /* 0x18 */ s16 unk_18;
} TradeCableState; // size = 0x1C

typedef struct unk_D_82F211A0_010 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
} unk_D_82F211A0_010; // size = 0x6

typedef struct TradeBall {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x00 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ unk_D_82F211A0_010 unk_10[10];
} TradeBall; // size = 0x4C

typedef struct TradeEvoState {
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
    /* 0x14 */ s16 unk_14;
    /* 0x16 */ s16 unk_16;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ s16 unk_1E;
    /* 0x20 */ s32 unk_20;
    /* 0x24 */ BattleMon* unk_24;
    /* 0x28 */ ModelLoadContext* unk_28;
    /* 0x2C */ ModelPreview* unk_2C;
    /* 0x30 */ PanelRect unk_30;
} TradeEvoState; // size = 0x38

extern s32 gFragment41FrameCounter;
extern PlayerTradeInfo gPlayerInfo[4];

extern char** gTradeStrings;
extern char** gTradeMoveMenuStringsBank;

char* Trade_GetSpeciesName(s32 arg0);
char* Trade_GetMoveName(s32 arg0);
char* Trade_GetTypeName(s32 arg0);
void Trade_SetFadeRect(s16 arg0);
void Trade_ResetFade(void);
void Trade_DrawFadeRect(void);
void Trade_ResetTextPrinter(void);
void Trade_SetTextPrinter(s16 arg0, s16 arg1, s8* arg2, s32 arg3);
void Trade_ResetTextMenu(void);
void Trade_ShowTextMenu(TextMenuListEntry* arg0, s32 arg1);
s32 Trade_IsTextMenuAtEnd(TextMenuListEntry* arg0);
void Trade_UpdateTextPrinter(void);
s32 Trade_GetTextPrinterState(void);
void Trade_DrawTextPrinter(void);
void Trade_ReadInput(void);
void Trade_ScanPorts(void);
void Trade_SwapSelections(void);
void Trade_NoOp(void);
void Trade_Init(void);
void Trade_Draw(void);
void Trade_SetState(s32 arg0);
void Trade_ChangeTopic(s16 arg0);
void Trade_UpdateMainMenu(void);
void Trade_UpdateWaitForBoxConfirm(void);
void Trade_UpdateWaitForCableCondition(void);
void Trade_UpdateWaitForPickSelection(void);
void Trade_UpdateWaitForDepositSave(void);
void Trade_UpdateWaitForWithdrawSave(void);
s32 Trade_Update(void);
void Trade_Loop(void);
s32 Trade_Main(s32 arg0, s32 arg1);

s16 Trade_ReadDeckEntries(PickerState* arg0, s16 arg1);
u16* Trade_GetPickBufferSlot(u8* arg0, s16 arg1, s16 arg2);
void Trade_InitPickScreen(s32 arg0, s16 arg1);
s32 Trade_IsPickScreenActive(void);
void Trade_UpdatePickScreenOpen(PickerState* arg0);
void Trade_UpdatePickScreenInput(PickerState* arg0);
void Trade_UpdatePickScreenPageSwipe(PickerState* arg0);
void Trade_ConfirmPickSelection(PickerState* arg0);
void Trade_UpdatePickScreenClose(PickerState* arg0);
void Trade_DrawPickScreenFrame(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void Trade_DrawPlayerBanner(s16 arg0, s16 arg1, s16 arg2);
void Trade_FillRect(s16 arg0, s16 arg1, s16 arg2, s16 arg3, Color_RGB8* arg4, s16 arg5);
void Trade_DrawPickSlotBackdrop(s16 arg0, s16 arg1, s16 arg2);
void Trade_DrawDigitStrip(s16 arg0, s16 arg1, s16 arg2);
void Trade_DrawSelectionCorners(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void Trade_DrawTitleBarStrip(s16 arg0, s16 arg1, s16 arg2);
void Trade_DrawRoundedFrameSmall(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7);
void Trade_DrawRoundedFrameLarge(s16, s16, s16, s16, u8, u8, u8, u8);
void Trade_DrawBoxTabIcon(s16 arg0, s16 arg1, u8* arg2, s16 arg3);
void Trade_DrawBoxTabBar(PickerState* arg0, s16 arg1, s16 arg2, s32 arg3);
void Trade_DrawPortInfoPanel(s16 arg0, s16 arg1, s16 arg2);
void Trade_UpdateAndRenderPicker(PickerState* arg0);
void Trade_ResetPickScreens(void);
void Trade_UpdatePickScreens(void);
void Trade_DrawPickScreens(void);
u16* Trade_GetPickBuffer(s32);
void Trade_SwapPickBuffers(void);
s32 Trade_GetPickScreenSnapshot(s32 arg0, unk_func_82F00838_sp30* arg1);

void Trade_LoadPickBufferIcon(s16, BattleMon*);
void Trade_OpenInfoBox(PickerState* arg0, u8* arg1, s16 arg2, s16 arg3);
void Trade_UpdateInfoBoxOpenIn(TradeInfoBoxState* arg0);
void Trade_UpdateInfoBoxChoose(TradeInfoBoxState* arg0);
void Trade_UpdateInfoBoxCloseOut(TradeInfoBoxState* arg0);
void Trade_DrawSolidFillRect(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6);
void Trade_DrawFilledFrame(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6);
void Trade_DrawMovingSheen(s16 arg0, s16 arg1, s32 arg2);
void Trade_DrawNumberTwoDigits(s16 arg0, s16 arg1, s16 arg2);
s32 Trade_IsMoveInvalid(s32 arg0);
void Trade_DrawInfoBox(TradeInfoBoxState* arg0);
void Trade_ResetInfoBox(void);
void Trade_UpdateInfoBox(void);
void Trade_DrawInfoBoxIfActive(void);
s32 Trade_GetInfoBoxState(void);

s32 Trade_DrawBox3DModel(s32 arg0, GraphNode* arg1);
s16 Trade_GetBoxSlotType(PlayerTradeInfo* arg0);
void Trade_SetupBoxSlots(s16 arg0);
void Trade_SetBoxSlotAnimState(TradeSlotInfo* arg0, s16 arg1);
void Trade_UpdateBoxSlotFlyIn(TradeSlotInfo* arg0);
void Trade_UpdateBoxSlotBob(TradeSlotInfo* arg0);
void Trade_UpdateBoxSlotBobIdle(TradeSlotInfo* arg0);
void Trade_UpdateBoxSlotFlyOut(TradeSlotInfo* arg0);
void Trade_UpdateBoxSlotFadeOut(TradeSlotInfo* arg0);
void Trade_UpdateBoxSlotShrink(TradeSlotInfo* arg0);
void Trade_UpdateBoxSlotTransforms(void);
void Trade_ResetBoxMachineFlow(void);
s32 Trade_GetSelectedBox(s16 arg0);
s32 Trade_GetBoxMachineFlowState(void);
void Trade_UpdateBoxMachineFlowOpen(unk_D_82F20A40* arg0, s32 arg1);
void Trade_UpdateBoxMachineFlowSelect(unk_D_82F20A40* arg0);
void Trade_UpdateBoxMachineFlowFrameMove(unk_D_82F20A40* arg0);
void Trade_UpdateBoxMachineFlowConfirmWait(unk_D_82F20A40* arg0);
void Trade_UpdateBoxMachineFlowFrameReturn(unk_D_82F20A40* arg0);
void Trade_UpdateBoxMachineSlots(unk_D_82F20A40* arg0, s16 arg1, s16 arg2);
void Trade_UpdateBoxMachineFlowPanelsFull(unk_D_82F20A40* arg0);
void Trade_UpdateBoxMachineFlowConfirmOpen(unk_D_82F20A40* arg0);
void Trade_UpdateBoxMachineFlowConfirmInput(unk_D_82F20A40* arg0);
void Trade_UpdateBoxMachineRect(PanelRect* arg0, s16 arg1, s16 arg2);
void Trade_UpdateBoxMachineFlowConfirmDone(unk_D_82F20A40* arg0);
void Trade_UpdateBoxMachineFlowClose(unk_D_82F20A40* arg0);
void Trade_DrawBoxMachineTopBar(unk_D_82F20A40* arg0);
void Trade_DrawBoxMachineSlots(unk_D_82F20A40* arg0);
void Trade_DrawBoxMachineTitle(unk_D_82F20A40* arg0);
void Trade_DrawBoxMachineConfirm(unk_D_82F20A40* arg0);
void Trade_DrawBoxMachineBanner(unk_D_82F20A40* arg0);
void Trade_DrawBoxMachineTransferArrows(s16 arg0, s16 arg1);
void Trade_DrawBoxMachineCursor(s16 arg0);
void Trade_InitBoxMachine(void);
void Trade_UpdateBoxMachine(void);
void Trade_DrawBoxMachine(void);

s32 Trade_WriteDeckAndCommitCart(unk_func_82F00838_sp30* arg0);
s32 Trade_StepCartWriteSequence(RentalMenuState* arg0);
void Trade_InitSaveSequence(s32 arg0);
void Trade_UpdateSaveSeqPanelSlideIn(RentalMenuState* arg0);
void Trade_UpdateSaveSeqMenuInput(RentalMenuState* arg0);
void Trade_UpdateSaveSeqConfirmWait(RentalMenuState* arg0);
void Trade_UpdateSaveSeqPanelClose(RentalMenuState* arg0);
void Trade_UpdateSaveSeqPanelReopen(RentalMenuState* arg0);
void Trade_UpdateSaveSeqBeginWrite(RentalMenuState* arg0);
void Trade_UpdateSaveSeqWriteCarts(RentalMenuState* arg0);
void Trade_UpdateSaveSeqPanelSlideOut(RentalMenuState* arg0);
void Trade_DrawSaveSeqBoxThumbnail(s16 arg0, s16 arg1, s16 arg2);
void Trade_DrawSaveSeqBoxPanels(RentalMenuState* arg0);
void Trade_DrawSaveSeqMenuPanel(RentalMenuState* arg0);
void Trade_DrawSaveSeqBannerPanel(RentalMenuState* arg0);
void Trade_DrawSaveSeqSubBanner(RentalMenuState* arg0);
void Trade_DrawSaveSeqMessageBackdrop(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void RentalMenu_DrawMessage(RentalMenuState* arg0);
s32 Trade_GetSaveSeqState(void);
s32 Trade_GetSaveDirection(void);
s32 Trade_GetSaveSlotIndex(void);
void Trade_ResetSaveSeq(void);
void Trade_UpdateSaveSeq(void);
void Trade_DrawSaveSeq(void);

void TradeCable_DrawScissoredTexRect(s16 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6, s16 arg7, s32 arg8);
void TradeCable_ResetBalls(void);
void TradeCable_SetBallState(s32 arg0, s16 arg1);
void TradeCable_UpdateBallFadeIn(TradeBall* arg0);
void TradeCable_UpdateBallSpinPulse(TradeBall* arg0);
void TradeCable_SpawnBallTrailGhost(TradeBall* arg0);
void TradeCable_UpdateBallTrailGhosts(TradeBall* arg0);
void TradeCable_UpdateBallTravel(TradeBall* arg0);
void TradeCable_UpdateBalls(void);
void TradeCable_DrawBalls(void);
void TradeEvo_ResetState(void);
void TradeEvo_CreatePreviewModel(TradeEvoState* arg0, s32 arg1);
void TradeEvo_BeginSequence(s16 arg0);
void TradeEvo_UpdatePreviewModel(TradeEvoState* arg0);
void TradeEvo_UpdateOpenPreview(TradeEvoState* arg0);
s32 Trade_SpeciesEvolvesOnTransfer(s32 arg0);
void TradeEvo_UpdateWaitForConfirm(TradeEvoState* arg0);
void TradeEvo_UpdateFlashIn(TradeEvoState* arg0);
s32 Trade_NicknameIsSpeciesDefault(s16 arg0, s8* arg1);
void Trade_ApplyEvolution(s16 arg0, BattleMon* arg1);
void TradeEvo_UpdateApplyEvolution(TradeEvoState* arg0);
void TradeEvo_UpdateShowCaption(TradeEvoState* arg0);
void TradeEvo_UpdateClosePreview(TradeEvoState* arg0);
void TradeEvo_Update(void);
void TradeEvo_DrawGlowBackdrop(s16 arg0, s16 arg1, s16 arg2);
void TradeEvo_DrawPreviewFrame(s16 arg0, s16 arg1, s16 arg2, s16 arg3);
void TradeEvo_DrawPreviewStrips(TradeEvoState* arg0);
void TradeCable_NullFunc(void);
void TradeEvo_DrawEvolutionCaption(PanelRect* arg0, BattleMon* arg1);
void TradeEvo_Draw(void);
void TradeCable_ResetLights(void);
void TradeCable_SetLightState(s32 arg0, s16 arg1);
void TradeCable_UpdateLightShrinkToIdle(TradeCableLight* arg0);
void TradeCable_UpdateLightIdlePulse(TradeCableLight* arg0);
void TradeCable_UpdateLightSlideIn(TradeCableLight* arg0);
void TradeCable_UpdateLightWaitPulse(TradeCableLight* arg0);
void TradeCable_UpdateLightShrinkOut(TradeCableLight* arg0);
void TradeCable_UpdateLightGrowFlash(TradeCableLight* arg0);
void TradeCable_UpdateLightFlashPulse(TradeCableLight* arg0);
void TradeCable_UpdateLightFadeInThenPulse(TradeCableLight* arg0);
void TradeCable_UpdateLights(void);
void TradeCable_DrawLightSpark(s16 arg0, s16 arg1, f32 arg2, s16 arg3);
void TradeCable_DrawLights(void);
s32 TradeCable_IsSequenceActive(void);
void TradeCable_SetSequenceState(s16 arg0);
void TradeCable_UpdateSequenceIdle(TradeCableState* arg0);
void TradeCable_UpdateSequenceOpen(TradeCableState* arg0);
void TradeCable_UpdateSequenceSlideVertical(TradeCableState* arg0);
void TradeCable_UpdateSequenceWaitLight0(TradeCableState* arg0);
void TradeCable_UpdateSequenceSlideHorizontal(TradeCableState* arg0);
void TradeCable_UpdateSequenceWaitLight1(TradeCableState* arg0);
void TradeCable_UpdateSequenceSlideBack(TradeCableState* arg0);
void TradeCable_UpdateSequenceLaunchBalls(TradeCableState* arg0);
void TradeCable_UpdateSequenceWaitBallsArrived(TradeCableState* arg0);
void TradeCable_UpdateSequenceSlideToEvoSlot0(TradeCableState* arg0);
void TradeCable_UpdateSequenceWaitEvoSlot0(TradeCableState* arg0);
void TradeCable_UpdateSequenceSlideToEvoSlot1(TradeCableState* arg0);
void TradeCable_UpdateSequenceWaitEvoSlot1(TradeCableState* arg0);
void TradeCable_UpdateSequenceFinishEvo(TradeCableState* arg0);
void TradeCable_UpdateSequenceClose(TradeCableState* arg0);
void TradeCable_DrawSequenceBG(s16 arg0, s16 arg1, f32 arg2);
void TradeCable_DrawTubeRailBar(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5);
void TradeCable_DrawTubeCenterBar(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5);
void TradeCable_DrawTubeJoint(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5);
void TradeCable_DrawTubeSparkBandDim(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5);
void TradeCable_DrawTubeSparkBandBright(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6);
void TradeCable_DrawTubeEndCap(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5);
void TradeCable_RenderReflectionTexture(void);
void TradeCable_DrawTubeEndCapReflective(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5);
void TradeCable_InitSequence(void);
void TradeCable_UpdateSequence(void);
void TradeCable_DrawSequence(void);
s32 TradeCable_CheckSequenceCondition(s16 arg0);
void TradeCable_SetSequenceTimer(s16 arg0);
s16 TradeCable_GetSequenceState(void);

#endif // _FRAGMENT41_H_
