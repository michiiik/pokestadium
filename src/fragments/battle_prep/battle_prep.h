#ifndef _FRAGMENT63_H_
#define _FRAGMENT63_H_

#include "global.h"
#include "src/jpeg_stream.h"
#include "src/poke_icon.h"
#include "src/geo_layout.h"

typedef struct unk_D_84B19910_002 {
    /* 0x02 */ s16 unk_00;
    /* 0x04 */ s16 unk_02;
    /* 0x06 */ s16 unk_04;
} unk_D_84B19910_002;

typedef struct unk_D_84B19910 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ unk_D_84B19910_002 unk_02;
    ///* 0x02 */ s16 unk_02;
    ///* 0x04 */ s16 unk_04;
    ///* 0x06 */ s16 unk_06;
} unk_D_84B19910; // size = 0x8

typedef struct unk_D_84B1A598 {
    /* 0x0000 */ char unk0000[0x1];
    /* 0x0001 */ u8 unk_0001;
    /* 0x0002 */ u8 unk_0002;
    /* 0x0003 */ u8 unk_0003;
    /* 0x0004 */ GraphNode* unk_0004;
    /* 0x0008 */ DisplayObject unk_0008[128];
    /* 0xB408 */ BinArchive* unk_B408;
    /* 0xB40C */ BinArchive* unk_B40C;
} unk_D_84B1A598; // size >= 0xB410

typedef struct BattlePrepRuleWindow {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ char unk06[0x2];
} BattlePrepRuleWindow; // size >= 0x8

typedef struct gBannerState {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ s16 unk_02[5];
    /* 0x0C */ void* unk_0C;
    /* 0x10 */ void* unk_10;
    /* 0x14 */ void* unk_14;
    /* 0x18 */ void* unk_18;
    /* 0x1C */ void* unk_1C;
    /* 0x20 */ DisplayObject* unk_20;
    /* 0x24 */ ShadowContextNode* unk_24;
    /* 0x28 */ ShadowContextNode* unk_28;
    /* 0x2C */ ShadowContextNode* unk_2C;
    /* 0x30 */ ShadowContextNode* unk_30;
    /* 0x34 */ ShadowContextNode* unk_34;
    /* 0x38 */ ShadowContextNode* unk_38;
    /* 0x3C */ char unk3C[0x4];
} gBannerState; // size >= 0x40

typedef struct gBadgeCarouselAnim {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ DisplayObject* unk_08;
    /* 0x0C */ GraphNode* unk_0C;
    /* 0x10 */ u8* unk_10[4];
    /* 0x20 */ char unk20[0x10];
    /* 0x30 */ DepthBuffer* unk_30[4];
} gBadgeCarouselAnim; // size >= 0x40

typedef struct gTeamIconsState {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x04 */ DisplayObject* unk_04;
    /* 0x08 */ ShadowContextNode* unk_08;
    /* 0x0C */ ShadowContextNode* unk_0C;
    /* 0x10 */ ShadowContextNode* unk_10;
    /* 0x14 */ ShadowContextNode* unk_14;
    /* 0x18 */ ShadowContextNode* unk_18;
    /* 0x1C */ SpriteDisplay* unk_1C;
    /* 0x20 */ u16 (*unk_20)[0x640];
    /* 0x24 */ DepthBuffer* unk_24;
    /* 0x28 */ DepthBuffer* unk_28;
    /* 0x2C */ u8 unk_2C[12];
} gTeamIconsState; // size = 0x38

typedef struct gBadgeAwardState {
    /* 0x00 */ u8 unk_00;
    /* 0x00 */ u8 unk_01;
    /* 0x04 */ DisplayObject* unk_04;
    /* 0x08 */ ShadowContextNode* unk_08;
    /* 0x0C */ ShadowContextNode* unk_0C;
    /* 0x10 */ ShadowContextNode* unk_10;
    /* 0x14 */ ShadowContextNode* unk_14;
    /* 0x18 */ ShadowContextNode* unk_18;
    /* 0x1C */ u8* unk_1C;
    /* 0x20 */ u8* unk_20;
    /* 0x24 */ u8** unk_24;
    /* 0x28 */ s8 unk_28[4];
    /* 0x2C */ char unk2C[0x4];
} gBadgeAwardState; // size = 0x30

typedef struct unk_D_84B25AC0_000 {
    /* 0x00 */ DisplayObject* unk_00;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ Color_RGB8 unk_08;
    /* 0x0C */ u8* unk_0C;
} unk_D_84B25AC0_000; // size = 0x10

typedef struct unk_D_84B25AC0_0A0 {
    /* 0x00 */ DisplayObject* unk_00;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ Color_RGBA8 unk_0A;
} unk_D_84B25AC0_0A0; // size = 0x10

typedef struct gStarBurstState {
    /* 0x000 */ unk_D_84B25AC0_000 unk_000[10];
    /* 0x0A0 */ unk_D_84B25AC0_0A0 unk_0A0[10][8];
    /* 0x5A0 */ u8 unk_5A0;
    /* 0x5A1 */ s8 unk_5A1;
    /* 0x5A2 */ s8 unk_5A2;
    /* 0x5A3 */ s8 unk_5A3;
    /* 0x5A4 */ f32 unk_5A4;
    /* 0x5A8 */ DisplayObject* unk_5A8;
    /* 0x5AC */ DisplayObject* unk_5AC;
    /* 0x5B0 */ ShadowContextNode* unk_5B0;
    /* 0x5B4 */ ShadowContextNode* unk_5B4;
    /* 0x5B8 */ ShadowContextNode* unk_5B8;
    /* 0x5BC */ char unk5BC[0x4];
} gStarBurstState; // size = 0x5C0

typedef struct unk_D_84B26640_010_010 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ s8 unk_01;
    /* 0x02 */ s8 unk_02;
    /* 0x04 */ DisplayObject* unk_04;
    /* 0x08 */ ShadowContextNode* unk_08;
} BarState; // size = 0xC

typedef struct unk_D_84B26640 {
    /* 0x00 */ BarState unk_00;
    /* 0x00 */ char unk0C[0x4];
    /* 0x00 */ BarState unk_10;
    /* 0x00 */ BattlePrepRuleWindow unk_1C;
    /* 0x00 */ BattlePrepRuleWindow unk_28;
} unk_D_84B26640; // size = 0x30

typedef struct unk_func_84B067B4 {
    /* 0x000 */ char unk00[0x10];
    /* 0x010 */ char unk_010[4];
    /* 0x014 */ char unk14[0x24];
    /* 0x038 */ u16 unk_038;
    /* 0x03A */ char unk03A[0x1F6];
} unk_func_84B067B4; // size = 0x230

typedef struct BattlePrepRuleWindowLayout {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s16 unk_10;
} BattlePrepRuleWindowLayout; // size = 0x12

extern SpeciesDisplayParams gSpeciesDisplayParamsFront[];
extern SpeciesDisplayParams gSpeciesDisplayParamsBack[];

void BattlePrep_SaveVictoryPalaceRecords(void);
void BattlePrep_MarkFirstClearBonus(void);
void BattlePrep_MarkCupCleared(void);
void BattlePrep_MarkNewRoundRecord(void);
char* BattlePrep_GetString(s32);
char* BattlePrep_FormatString(char* arg0, s32 arg1, u32 arg2);
s32 BattlePrep_StarfieldGeoCallback(s32 arg0, unk_D_86002F34_alt8* arg1);
void BattlePrep_ResetDisplayObject(DisplayObject* arg0);
void BattlePrep_ResetAllDisplayObjects(void);
void BattlePrep_DrawBorderedRect(s16, s16, s16, s16, u8, u8, u8, u8);
void BattlePrep_DrawTrainerStatusIcon(DepthBuffer*, s16, u8*, u8*);
void BattlePrep_AnimateBallBurstEffect(s16 arg0);
void BattlePrep_FadeInBallBurstObjects(s16 arg0);
void BattlePrep_RenderFrame(void);
void BattlePrep_UpdateSingleBattleIntroFrame(void);
s32 BattlePrep_ShowSingleBattleIntro(void);
void BattlePrep_UpdateRoundIntroFrame(void);
void BattlePrep_InitRoundIntroScene(void);
s32 BattlePrep_ShowRoundIntro(void);
void BattlePrep_CheckPikachuNeedsSurf(void);
void BattlePrep_UpdateRoundIntroScene(void);
void BattlePrep_InitRoundIntroSubScenes(void);
void BattlePrep_RunNormalRoundIntro(void);
void BattlePrep_RunFinalRoundIntro(void);
void BattlePrep_RunBadgeCarouselIntro(void);
void BattlePrep_RunBallBurstTransition(void);
void BattlePrep_FadeInBallBurstTransition(void);
void BattlePrep_FadeToTransparentEnding(void);
s32 BattlePrep_HandleRoundContinue(void);
s32 BattlePrep_ConsumeContinueToken(void);
s32 BattlePrep_ConfirmDefaultRules(void);
s32 BattlePrep_ConfirmSimpleRules(void);
s32 BattlePrep_ConfirmCupRules(void);
s32 BattlePrep_ConfirmCastleRules(void);
s32 BattlePrep_ConfirmMewtwoRules(void);
u8 BattlePrep_CheckTeamChosen(void);
s32 BattlePrep_DispatchRuleConfirm(void);
s32 BattlePrep_ShowCastleClearOutro(void);
void BattlePrep_InitScene(void);
s32 BattlePrep_RunRulesScreen(s32 arg0, UNUSED s32 arg1);

s16 BattlePrep_RandomStarOffset(void);
void BattlePrep_DrawStarfield(unk_D_84B19910* arg0);
void BattlePrep_UpdateStarfield(unk_D_84B19910* arg0);
void BattlePrep_InitStarfield(unk_D_84B19910* arg0);

s32 BattlePrep_BindCarouselSegment0(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrep_BindCarouselSegment1(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrep_BindCarouselSegment3(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrep_BindCarouselSegment2(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrep_CarouselDepthCallback(s32 arg0, unk_D_86002F34_alt5* arg1);
void BattlePrep_CarouselStepFirstItem(gBannerState* arg0, s16 arg1);
void BattlePrep_CarouselStepNTSC(gBannerState* arg0, s16 arg1);
void BattlePrep_CarouselStepPAL(gBannerState* arg0, s16 arg1);
void BattlePrep_CarouselStepFinalNTSC(gBannerState* arg0, s16 arg1);
void BattlePrep_CarouselStepFinalPAL(gBannerState* arg0, s16 arg1);
void BattlePrep_UpdateCarousel(gBannerState* arg0);
void BattlePrep_CarouselExitAnimation(gBannerState* arg0);
void BattlePrep_CarouselEntryAnimation(gBannerState* arg0);
void BattlePrep_UpdateCarouselState(gBannerState* arg0);
void BattlePrep_InitCarouselBanner(gBannerState* arg0, s16 arg1, s16 arg2);
void BattlePrep_InitCarouselGymBadgeSlot(gBannerState* arg0, s16 arg1, s16 arg2, s16 arg3);
void BattlePrep_InitCarouselLeftIcon(gBannerState* arg0, s16 arg1);
void BattlePrep_InitCarouselRightIcon(gBannerState* arg0, s16 arg1);
void BattlePrep_InitCarouselCupIcon(gBannerState* arg0, s16 arg1);
void BattlePrep_InitCarouselEntry(gBannerState* arg0, DisplayObject* arg1);
void BattlePrep_StartCarouselExit(gBannerState* arg0);
void BattlePrep_InitCarouselExit(gBannerState* arg0, DisplayObject* arg1);
void BattlePrep_LoadBannerAssets(gBannerState* arg0);
void BattlePrep_AdjustBannerTextureOffset(gBannerState* arg0);
void BattlePrep_LoadBannerGeoLayouts(gBannerState* arg0);
void BattlePrep_SetVertexGradientColors(ShadowContextNode* arg0, Color_RGBA8* arg1, Color_RGBA8* arg2);
void BattlePrep_ApplyBannerColors(gBannerState* arg0);
void BattlePrep_InitBannerScene(gBannerState* arg0);
s32 BattlePrep_BadgeCarouselGeoCallback(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrep_BadgeCarouselGlowCallback(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrep_BadgeCarouselTextureCallback(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrep_BadgeCarouselGlowCallback2(s32 arg0, SimpleGfxNode* arg1);
void BattlePrep_RenderBadgeGlowTexture(s32 arg0);
void BattlePrep_ResetBadgeSlideSlots(gBadgeCarouselAnim* arg0);
void BattlePrep_BadgeSlideInLeft(gBadgeCarouselAnim* arg0);
void BattlePrep_BadgeSlideInRight(gBadgeCarouselAnim* arg0);
void BattlePrep_BadgeRevealSpin(gBadgeCarouselAnim* arg0);
void BattlePrep_BadgeHideSpin(gBadgeCarouselAnim* arg0);
void BattlePrep_UpdateBadgeCarousel(gBadgeCarouselAnim* arg0);
void BattlePrep_InitBadgeCarousel(gBadgeCarouselAnim* arg0, DisplayObject* arg1, s16 arg2);
void BattlePrep_RenderBadgeBackground(gBadgeCarouselAnim* arg0, s16 arg1, char* arg2);
s32 BattlePrep_Align64PoolBlock(s32* arg0);
void BattlePrep_LoadBadgeTextures(gBadgeCarouselAnim* arg0, BinArchive* arg1, BinArchive* arg2);
void BattlePrep_InitBadgeCarouselScene(gBadgeCarouselAnim* arg0, BinArchive* arg1, BinArchive* arg2);

s32 BattlePrepRoster_DrawIconVariantA(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrepRoster_DrawIconVariantB(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrepRoster_DrawTrainerBadge(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrepRoster_ScrollBackgroundTexture(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrepRoster_ConvergeIconsStep(BattleUiState* arg0, s16 arg1);
void BattlePrepRoster_UpdateCameraSwoop(BattleUiState* arg0, s16 arg1);
s32 BattlePrepRoster_ZoomStepNTSC(BattleUiState* arg0, s16 arg1);
s32 BattlePrepRoster_ZoomStepPAL(BattleUiState* arg0, s16 arg1);
void BattlePrepRoster_UpdateAssembleStep(BattleUiState* arg0);
void BattlePrepRoster_UpdateLoadingStep(BattleUiState* arg0);
void BattlePrepRoster_Update(BattleUiState* arg0);
void BattlePrepRoster_LoadDexIdsSingle(BattleUiState* arg0);
void BattlePrepRoster_LoadDexIdsDouble(BattleUiState* arg0);
void BattlePrepRoster_Load(BattleUiState* arg0, BinArchive* arg1);
void BattlePrepRoster_PositionIconsSingle(BattleUiState* arg0);
void BattlePrepRoster_PositionIconsDouble(BattleUiState* arg0);
s32 BattlePrepRoster_InitDisplayObjects(BattleUiState* arg0, DisplayObject* arg1);
void BattlePrepRoster_Init(BattleUiState* arg0, CameraNode* arg1);

s32 BattlePrepTeamIcons_DrawActiveTeamBadge(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrepTeamIcons_DrawIconSingle(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrepTeamIcons_DrawIconDouble(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrepTeamIcons_DrawBadgeIconSingle(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrepTeamIcons_DrawBadgeIconDouble(s32 arg0, SimpleGfxNode* arg1);
void BattlePrepTeamIcons_DrawHpBarsSingle(gTeamIconsState* arg0, s32 arg1);
void BattlePrepTeamIcons_DrawHpBarsDouble(gTeamIconsState* arg0, s32 arg1);
void BattlePrepTeamIcons_MarkFaintedSingle(gTeamIconsState* arg0, s32 arg1);
void BattlePrepTeamIcons_MarkFaintedDouble(gTeamIconsState* arg0, s32 arg1);
void BattlePrepTeamIcons_InitDisplayObjectsSingleVisible(gTeamIconsState* arg0);
void BattlePrepTeamIcons_InitDisplayObjectsSingleHidden(gTeamIconsState* arg0);
void BattlePrepTeamIcons_InitDisplayObjectsDoubleVisible(gTeamIconsState* arg0);
void BattlePrepTeamIcons_InitDisplayObjectsDoubleHidden(gTeamIconsState* arg0);
void BattlePrepTeamIcons_RevealStepSingle(gTeamIconsState* arg0, s16 arg1);
void BattlePrepTeamIcons_RevealStepDouble(gTeamIconsState* arg0, s16 arg1);
void BattlePrepTeamIcons_HpDrainStepSingleNTSC(gTeamIconsState* arg0, s16 arg1);
void BattlePrepTeamIcons_HpDrainStepDoubleNTSC(gTeamIconsState* arg0, s16 arg1);
void BattlePrepTeamIcons_HpDrainStepSinglePAL(gTeamIconsState* arg0, s16 arg1);
void BattlePrepTeamIcons_HpDrainStepDoublePAL(gTeamIconsState* arg0, s16 arg1);
void BattlePrepTeamIcons_FinalRevealSingle(gTeamIconsState* arg0, s16 arg1);
void BattlePrepTeamIcons_FinalRevealDouble(gTeamIconsState* arg0, s16 arg1);
void BattlePrepTeamIcons_UpdateStep(gTeamIconsState* arg0);
void BattlePrepTeamIcons_UpdateLoadingStep(gTeamIconsState* arg0);
void BattlePrepTeamIcons_Update(gTeamIconsState* arg0);
void BattlePrepTeamIcons_LoadDexIdsSingle(gTeamIconsState* arg0);
void BattlePrepTeamIcons_LoadDexIdsDouble(gTeamIconsState* arg0);
void BattlePrepTeamIcons_Load(gTeamIconsState* arg0, BinArchive* arg1);
s32 BattlePrepTeamIcons_InitDisplayObjects(gTeamIconsState* arg0, DisplayObject* arg1);
void BattlePrepTeamIcons_LoadIconTexturesSingle(gTeamIconsState* arg0);
void BattlePrepTeamIcons_LoadIconTexturesDouble(gTeamIconsState* arg0);
void BattlePrepTeamIcons_Init(gTeamIconsState* arg0);

s32 BattlePrepBadgeAward_DrawBadgeIcon(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrepBadgeAward_DrawBadgeIconAlt(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrepBadgeAward_DrawSparkle(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrepBadgeAward_DrawGlowFade(s32 arg0, SimpleGfxNode* arg1);
void BattlePrepBadgeAward_NoOp(void);
void BattlePrepBadgeAward_SlideInStep(gBadgeAwardState* arg0);
void BattlePrepBadgeAward_RevealStep(gBadgeAwardState* arg0);
void BattlePrepBadgeAward_SpawnSparkleParticle(gBadgeAwardState* arg0, DisplayObject* arg1, s16 arg2);
void BattlePrepBadgeAward_UpdateSparkleStep(gBadgeAwardState* arg0);
void BattlePrepBadgeAward_ScatterStep(gBadgeAwardState* arg0);
void BattlePrepBadgeAward_Update(gBadgeAwardState* arg0);
void BattlePrepBadgeAward_InitDisplayObjects(gBadgeAwardState* arg0, DisplayObject* arg1);
void BattlePrepBadgeAward_StartScatter(gBadgeAwardState* arg0);
void BattlePrepBadgeAward_Init(gBadgeAwardState* arg0);

s32 BattlePrepStarBurst_DrawStarIcon(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrepStarBurst_DrawTrailIcon(s32 arg0, SimpleGfxNode* arg1);
s32 BattlePrepStarBurst_DrawSparkParticle(s32 arg0, SimpleGfxNode* arg1);
void BattlePrepStarBurst_InitSparkSlot(unk_D_84B25AC0_000* arg0, ShadowContextNode* arg1, s16 arg2, s16 arg3, s16 arg4);
void BattlePrepStarBurst_UpdateSparkSlot(unk_D_84B25AC0_000* arg0);
s32 BattlePrepStarBurst_DrawRainbowParticle(s32 arg0, SimpleGfxNode* arg1);
void BattlePrepStarBurst_InitRainbowParticle(unk_D_84B25AC0_0A0* arg0, ShadowContextNode* arg1, s16 arg2, s16 arg3, s16 arg4);
void BattlePrepStarBurst_SpawnTrailCopy(unk_D_84B25AC0_0A0* arg0, unk_D_84B25AC0_0A0* arg1);
void BattlePrepStarBurst_SpawnFlashCopy(unk_D_84B25AC0_0A0* arg0, unk_D_84B25AC0_0A0* arg1, s16 arg2);
void BattlePrepStarBurst_SpawnScatterParticle(unk_D_84B25AC0_0A0* arg0, ShadowContextNode* arg1, Vec3f* arg2);
void BattlePrepStarBurst_UpdateRainbowGradientStep(unk_D_84B25AC0_0A0* arg0);
void BattlePrepStarBurst_UpdateTrailFadeStep(unk_D_84B25AC0_0A0* arg0);
void BattlePrepStarBurst_UpdateFlashFadeStep(unk_D_84B25AC0_0A0* arg0);
void BattlePrepStarBurst_UpdateScatterFallStep(unk_D_84B25AC0_0A0* arg0);
void BattlePrepStarBurst_UpdateRainbowParticle(unk_D_84B25AC0_0A0* arg0);
void BattlePrepStarBurst_UpdateBurstEffect(gStarBurstState* arg0);
void BattlePrepStarBurst_UpdateFlightArc(gStarBurstState* arg0);
void BattlePrepStarBurst_UpdateRainbowAftermath(gStarBurstState* arg0);
void BattlePrepStarBurst_UpdateSparkAftermath(gStarBurstState* arg0);
void BattlePrepStarBurst_UpdateExitSlide(gStarBurstState* arg0);
void BattlePrepStarBurst_UpdateFirstClearExit(gStarBurstState* arg0);
void BattlePrepStarBurst_Update(gStarBurstState* arg0);
void BattlePrepStarBurst_Launch(gStarBurstState* arg0, DisplayObject* arg1, DisplayObject* arg2);
void BattlePrepStarBurst_StartExit(gStarBurstState* arg0);
void BattlePrepStarBurst_Init(gStarBurstState* arg0, s8 arg1, s8 arg2);
void BattlePrep_TrophyModelIdle(BarState* arg0);
void BattlePrep_TrophyModelGrow(BarState* arg0);
void BattlePrep_TrophyModelShrink(BarState* arg0);
void BattlePrep_UpdateTrophyModel(BarState* arg0);
void BattlePrep_StartTrophyModelGrow(BarState* arg0, DisplayObject* arg1);
void BattlePrep_StartTrophyModelShrink(BarState* arg0);
void BattlePrep_InitTrophyModel(BarState* arg0, u8 arg1);
void BattlePrep_DrawCursorArrow(BattlePrepRuleWindow* arg0, s16 arg1, s16 arg2);
void BattlePrep_DrawRuleOption(BattlePrepRuleWindow* arg0, s16 arg1, s16 arg2, s16 arg3, s32 arg4, s8* arg5);
void BattlePrep_DrawResultPrompt(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawContinuePrompt(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawGiveUpPrompt(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawRetireStreakPrompt(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawQuitPrompt(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawCupRuleSummary(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawCastleRuleSummary(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawRankBadge(s16 arg0, s16 arg1, BattleSessionTeams* arg2);
void BattlePrep_SetStreakColor(BattleSessionTeams* arg0);
void BattlePrep_DrawTeamPreview(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawStreakGiveUpPrompt(BattlePrepRuleWindow* arg0, BattlePrepRuleWindowLayout* arg1);
void BattlePrep_DrawRuleWindow(BattlePrepRuleWindow* arg0);
void BattlePrep_RunQuitMenu(BattlePrepRuleWindow* arg0);
void BattlePrep_RuleWindowExpanding(BattlePrepRuleWindow* arg0);
void BattlePrep_RuleWindowCollapsing(BattlePrepRuleWindow* arg0);
void BattlePrep_UpdateRuleWindow(BattlePrepRuleWindow* arg0);
void BattlePrep_InitRuleWindow(BattlePrepRuleWindow* arg0, s16 arg1);
void BattlePrep_CloseRuleWindow(BattlePrepRuleWindow* arg0);
s32 BattlePrep_PollRuleWindow(BattlePrepRuleWindow* arg0);


#endif // _FRAGMENT63_H_
