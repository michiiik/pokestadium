#ifndef _FRAGMENT63_H_
#define _FRAGMENT63_H_

#include "global.h"
#include "src/jpeg_decoder.h"
#include "src/poke_icon.h"
#include "src/geo_layout.h"

typedef struct unk_D_84B19910_002 {
    /* 0x02 */ s16 x;
    /* 0x04 */ s16 y;
    /* 0x06 */ s16 z; // BattlePrep_UpdateStarfield: depth, driven toward 0 each tick; respawns the star when it crosses 0
} unk_D_84B19910_002;

typedef struct unk_D_84B19910 {
    /* 0x00 */ s16 unk_00; // written via arg0[i+1].unk_00 in BattlePrep_InitStarfield (off-by-one aliasing into the next star's pos); never read
    /* 0x02 */ unk_D_84B19910_002 pos;
    ///* 0x02 */ s16 unk_02;
    ///* 0x04 */ s16 unk_04;
    ///* 0x06 */ s16 unk_06;
} unk_D_84B19910; // size = 0x8

typedef struct unk_D_84B1A598 {
    /* 0x0000 */ char unk0000[0x1];
    /* 0x0001 */ u8 side1Active; // BattlePrep_InitScene: set when side 0's isActiveSide==1 (and sides differ); StarBurst_Init's arg2/mode
    /* 0x0002 */ u8 teamChosen; // BattlePrep_CheckTeamChosen's result
    /* 0x0003 */ u8 bothSidesSameActive; // BattlePrep_InitScene: set when both sides' isActiveSide match
    /* 0x0004 */ GraphNode* sceneRootNode; // process_geo_layout(D_84B0FB20); root of the battle-prep scene graph
    /* 0x0008 */ unk_D_86002F58_004_000 displayObjectPool[128]; // shared pool distributed across all BattlePrep sub-scenes
    /* 0xB408 */ BinArchive* portraitArchiveRaw; // BinArchive_Open
    /* 0xB40C */ BinArchive* portraitArchive; // ASSET_LOAD2(battle_portraits, ...)
} unk_D_84B1A598; // size >= 0xB410

typedef struct BattlePrepRuleWindow {
    /* 0x00 */ u8 state; // BattlePrep_UpdateRuleWindow's dispatch (1=expanding, 2=collapsing, 3=quit-menu active)
    /* 0x01 */ u8 windowType; // BattlePrep_InitRuleWindow's arg1; indexes gBattlePrepRuleWindowLayouts
    /* 0x02 */ s16 expandProgress; // BattlePrep_RuleWindowExpanding counts 0-4
    /* 0x04 */ s16 selectedOption; // BattlePrep_RunQuitMenu's cursor into the window's enabled-option bitmask
    /* 0x06 */ char unk06[0x2];
} BattlePrepRuleWindow; // size >= 0x8

typedef struct unk_D_84B259A8 {
    /* 0x00 */ u8 state; // BattlePrep_UpdateCarousel's dispatch
    /* 0x01 */ u8 animTimer;
    /* 0x02 */ s16 unk_02[5];
    /* 0x0C */ void* bannerTextureData; // BattlePrep_LoadBannerAssets: BinArchive_GetFile result
    /* 0x10 */ void* gymBadgeTextureData; // BattlePrep_LoadBannerAssets: virtual-addr texture data for the badge slots
    /* 0x14 */ void* gymBadgeTextureDataAlt; // sessionMode!=7 branch's extra texture data (progressIndex-indexed)
    /* 0x18 */ void* cupIconTextureData; // BattlePrep_LoadBannerAssets: opponentNumber-selected texture data
    /* 0x1C */ void* leftRightIconTextureData; // BattlePrep_LoadBannerAssets: opponentNumber-indexed texture data
    /* 0x20 */ unk_D_86002F58_004_000* displayObjects; // 17-element array (indices 0-16) across the carousel Init*/Draw* functions
    /* 0x24 */ unk_D_86002F34* bannerModel; // BattlePrep_LoadBannerGeoLayouts; consumed by BattlePrep_InitCarouselBanner
    /* 0x28 */ unk_D_86002F34* gymBadgeModel; // consumed by BattlePrep_InitCarouselGymBadgeSlot's first display object
    /* 0x2C */ unk_D_86002F34* gymBadgeModelAlt; // consumed by BattlePrep_InitCarouselGymBadgeSlot's second display object
    /* 0x30 */ unk_D_86002F34* leftIconModel; // consumed by BattlePrep_InitCarouselLeftIcon
    /* 0x34 */ unk_D_86002F34* cupIconModel; // consumed by BattlePrep_InitCarouselCupIcon
    /* 0x38 */ unk_D_86002F34* rightIconModel; // consumed by BattlePrep_InitCarouselRightIcon
    /* 0x3C */ char unk3C[0x4];
} unk_D_84B259A8; // size >= 0x40

typedef struct unk_D_84B259E8 {
    /* 0x00 */ u8 state; // BattlePrep_UpdateBadgeCarousel's dispatch (1=slideInLeft, 2=slideInRight, 3=revealSpin, 4=hideSpin)
    /* 0x01 */ u8 animTimer; // BattlePrep_InitBadgeCarousel resets to 0; incremented per tick by each phase function
    /* 0x02 */ u8 direction; // BattlePrep_InitBadgeCarousel's arg2 branch (0 or 1)
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ unk_D_86002F58_004_000* displayObjects; // 16-element array, indexed 0-15 across the badge-slide/spin functions
    /* 0x0C */ GraphNode* badgeTextureData; // BattlePrep_LoadBannerAssets/BindCarouselSegment0; cast to u32 for gSPSegment(0x0F) target
    /* 0x10 */ u8* glowTextureData[4]; // per-opponent glow texture buffers, indexed by opponentNumber-1; BindCarouselSegment1/2/3
    /* 0x20 */ char unk20[0x10];
    /* 0x30 */ unk_D_80068BB0* glowRenderTargets[4]; // per-opponent render targets, indexed by opponentNumber-1
} unk_D_84B259E8; // size >= 0x40

typedef struct unk_D_84B25A58 {
    /* 0x00 */ u8 state; // BattlePrepTeamIcons_Update's dispatch (1=loading, 3=reveal/HP-drain steps)
    /* 0x01 */ u8 animTimer; // BattlePrepTeamIcons_UpdateStep/UpdateLoadingStep's frame counter
    /* 0x02 */ u8 isDoubleMode; // BattlePrepTeamIcons_Init: set when either side's party needs the double-row layout
    /* 0x04 */ unk_D_86002F58_004_000* displayObjects;
    /* 0x08 */ unk_D_86002F34* visibleIconModel; // InitDisplayObjectsSingle/DoubleVisible; row-A icon geo layout
    /* 0x0C */ unk_D_86002F34* visibleIconModelAlt; // InitDisplayObjectsDoubleVisible's row-B icon geo layout (double mode only)
    /* 0x10 */ unk_D_86002F34* hiddenIconModel; // InitDisplayObjectsSingle/DoubleHidden; row-A icon geo layout
    /* 0x14 */ unk_D_86002F34* hiddenIconModelAlt; // InitDisplayObjectsDoubleHidden's row-B icon geo layout (double mode only)
    /* 0x18 */ unk_D_86002F34* activeIconModel; // BattlePrepTeamIcons_InitDisplayObjects; both players' active-mon icons (slots 0 and 7)
    /* 0x1C */ unk_func_8001A024* renderList; // PokeIcon_CreateRenderList/ProcessNextSlot/AreSlotsIdle/SetNameTable
    /* 0x20 */ u16 (*pixelBuffer)[0x640];
    /* 0x24 */ unk_D_80068BB0* unk_24;
    /* 0x28 */ unk_D_80068BB0* unk_28;
    /* 0x2C */ u8 dexIds[12]; // BattlePrepTeamIcons_LoadDexIdsSingle/Double
} unk_D_84B25A58; // size = 0x38

typedef struct unk_D_84B25A90 {
    /* 0x00 */ u8 state; // BattlePrepBadgeAward_Update's dispatch
    /* 0x00 */ u8 animTimer;
    /* 0x04 */ unk_D_86002F58_004_000* displayObjects; // indexed up to 41 across SlideInStep/InitDisplayObjects/StartScatter
    /* 0x08 */ unk_D_86002F34* badgeModel; // geo layout for the slide-in badge display objects
    /* 0x0C */ unk_D_86002F34* ringModel; // geo layout for the reveal-step ring display objects
    /* 0x10 */ unk_D_86002F34* haloModel; // geo layout for the reveal-step secondary display objects
    /* 0x14 */ unk_D_86002F34* flashModel; // geo layout for the single flash display object
    /* 0x18 */ unk_D_86002F34* sparkleModel; // geo layout for BattlePrepBadgeAward_SpawnSparkleParticle
    /* 0x1C */ u8* badgeIconFrames; // BattlePrepBadgeAward_DrawBadgeIcon; indexed by an 8-frame animation cycle
    /* 0x20 */ u8* badgeIconFramesAlt; // BattlePrepBadgeAward_DrawBadgeIconAlt
    /* 0x24 */ u8** sparkleFrames; // BattlePrepBadgeAward_DrawSparkle; indexed by slotState[tmp]
    /* 0x28 */ s8 slotState[4]; // declared size undersells: BattlePrepBadgeAward_SlideInStep writes indices 0-7
    /* 0x2C */ char unk2C[0x4];
} unk_D_84B25A90; // size = 0x30

typedef struct unk_D_84B25AC0_000 {
    /* 0x00 */ unk_D_86002F58_004_000* displayObject;
    /* 0x04 */ s16 lifeStage; // 0-0x10 countdown-ish counter, BattlePrepStarBurst_UpdateSparkSlot
    /* 0x06 */ s16 angle; // fixed spark direction, consumed by COSS/SINS
    /* 0x08 */ Color_RGB8 color;
    /* 0x0C */ u8* textureFrame;
} unk_D_84B25AC0_000; // size = 0x10

typedef struct unk_D_84B25AC0_0A0 {
    /* 0x00 */ unk_D_86002F58_004_000* displayObject;
    /* 0x04 */ s16 particleType; // 0=normal, 1=trailCopy, 2=flashCopy, 3=scatterParticle (BattlePrepStarBurst_Spawn*)
    /* 0x06 */ s16 lifeStage; // active when >= 0 (BattlePrepStarBurst_UpdateBurstEffect)
    /* 0x08 */ s16 angle;
    /* 0x0A */ Color_RGBA8 color;
} unk_D_84B25AC0_0A0; // size = 0x10

typedef struct unk_D_84B25AC0 {
    /* 0x000 */ unk_D_84B25AC0_000 burstParticles[10];
    /* 0x0A0 */ unk_D_84B25AC0_0A0 trailParticles[10][8];
    /* 0x5A0 */ u8 state; // BattlePrepStarBurst_Update's dispatch
    /* 0x5A1 */ s8 animTimer;
    /* 0x5A2 */ s8 mode; // BattlePrepStarBurst_Init's arg2; -1 = a distinct variant, else selects a geo layout/exit behavior
    /* 0x5A3 */ s8 side; // BattlePrepStarBurst_Init's arg1
    /* 0x5A4 */ f32 driftSpeed; // BattlePrepStarBurst_StartExit's aftermath drift velocity
    /* 0x5A8 */ unk_D_86002F58_004_000* starModel; // BattlePrepStarBurst_Launch's arg1
    /* 0x5AC */ unk_D_86002F58_004_000* particlePool; // BattlePrepStarBurst_Launch's arg2, distributed across burstParticles/trailParticles
    /* 0x5B0 */ unk_D_86002F34* starModelGeo; // BattlePrepStarBurst_Init's geo layout for starModel (mode==-1 picks a distinct variant)
    /* 0x5B4 */ unk_D_86002F34* sparkModel; // BattlePrepStarBurst_InitSparkSlot's geo layout for burstParticles
    /* 0x5B8 */ unk_D_86002F34* rainbowModel; // BattlePrepStarBurst_InitRainbowParticle's geo layout for trailParticles
    /* 0x5BC */ char unk5BC[0x4];
} unk_D_84B25AC0; // size = 0x5C0

typedef struct unk_D_84B26640_010_010 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ s8 unk_01;
    /* 0x02 */ s8 unk_02;
    /* 0x04 */ unk_D_86002F58_004_000* unk_04;
    /* 0x08 */ unk_D_86002F34* unk_08;
} unk_D_84B26640_010; // size = 0xC

typedef struct unk_D_84B26640 {
    /* 0x00 */ unk_D_84B26640_010 unk_00;
    /* 0x00 */ char unk0C[0x4];
    /* 0x00 */ unk_D_84B26640_010 unk_10;
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

extern unk_D_8006FF00 D_84B17620[];
extern unk_D_8006FF00 D_84B186C0[];

void BattlePrep_SaveVictoryPalaceRecords(void);
void BattlePrep_MarkFirstClearBonus(void);
void BattlePrep_MarkCupCleared(void);
void BattlePrep_MarkNewRoundRecord(void);
char* BattlePrep_GetString(s32);
char* BattlePrep_FormatString(char* arg0, s32 arg1, u32 arg2);
s32 BattlePrep_StarfieldGeoCallback(s32 arg0, unk_D_86002F34_alt8* arg1);
void BattlePrep_ResetDisplayObject(unk_D_86002F58_004_000* arg0);
void BattlePrep_ResetAllDisplayObjects(void);
void BattlePrep_DrawBorderedRect(s16, s16, s16, s16, u8, u8, u8, u8);
void BattlePrep_DrawTrainerStatusIcon(unk_D_80068BB0*, s16, u8*, u8*);
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

s32 BattlePrep_BindCarouselSegment0(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_BindCarouselSegment1(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_BindCarouselSegment3(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_BindCarouselSegment2(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_CarouselDepthCallback(s32 arg0, unk_D_86002F34_alt5* arg1);
void BattlePrep_CarouselStepFirstItem(unk_D_84B259A8* arg0, s16 arg1);
void BattlePrep_CarouselStepNTSC(unk_D_84B259A8* arg0, s16 arg1);
void BattlePrep_CarouselStepPAL(unk_D_84B259A8* arg0, s16 arg1);
void BattlePrep_CarouselStepFinalNTSC(unk_D_84B259A8* arg0, s16 arg1);
void BattlePrep_CarouselStepFinalPAL(unk_D_84B259A8* arg0, s16 arg1);
void BattlePrep_UpdateCarousel(unk_D_84B259A8* arg0);
void BattlePrep_CarouselExitAnimation(unk_D_84B259A8* arg0);
void BattlePrep_CarouselEntryAnimation(unk_D_84B259A8* arg0);
void BattlePrep_UpdateCarouselState(unk_D_84B259A8* arg0);
void BattlePrep_InitCarouselBanner(unk_D_84B259A8* arg0, s16 arg1, s16 arg2);
void BattlePrep_InitCarouselGymBadgeSlot(unk_D_84B259A8* arg0, s16 arg1, s16 arg2, s16 arg3);
void BattlePrep_InitCarouselLeftIcon(unk_D_84B259A8* arg0, s16 arg1);
void BattlePrep_InitCarouselRightIcon(unk_D_84B259A8* arg0, s16 arg1);
void BattlePrep_InitCarouselCupIcon(unk_D_84B259A8* arg0, s16 arg1);
void BattlePrep_InitCarouselEntry(unk_D_84B259A8* arg0, unk_D_86002F58_004_000* arg1);
void BattlePrep_StartCarouselExit(unk_D_84B259A8* arg0);
void BattlePrep_InitCarouselExit(unk_D_84B259A8* arg0, unk_D_86002F58_004_000* arg1);
void BattlePrep_LoadBannerAssets(unk_D_84B259A8* arg0);
void BattlePrep_AdjustBannerTextureOffset(unk_D_84B259A8* arg0);
void BattlePrep_LoadBannerGeoLayouts(unk_D_84B259A8* arg0);
void BattlePrep_SetVertexGradientColors(unk_D_86002F34* arg0, Color_RGBA8* arg1, Color_RGBA8* arg2);
void BattlePrep_ApplyBannerColors(unk_D_84B259A8* arg0);
void BattlePrep_InitBannerScene(unk_D_84B259A8* arg0);
s32 BattlePrep_BadgeCarouselGeoCallback(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_BadgeCarouselGlowCallback(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_BadgeCarouselTextureCallback(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrep_BadgeCarouselGlowCallback2(s32 arg0, unk_func_80011B94* arg1);
void BattlePrep_RenderBadgeGlowTexture(s32 arg0);
void BattlePrep_ResetBadgeSlideSlots(unk_D_84B259E8* arg0);
void BattlePrep_BadgeSlideInLeft(unk_D_84B259E8* arg0);
void BattlePrep_BadgeSlideInRight(unk_D_84B259E8* arg0);
void BattlePrep_BadgeRevealSpin(unk_D_84B259E8* arg0);
void BattlePrep_BadgeHideSpin(unk_D_84B259E8* arg0);
void BattlePrep_UpdateBadgeCarousel(unk_D_84B259E8* arg0);
void BattlePrep_InitBadgeCarousel(unk_D_84B259E8* arg0, unk_D_86002F58_004_000* arg1, s16 arg2);
void BattlePrep_RenderBadgeBackground(unk_D_84B259E8* arg0, s16 arg1, char* arg2);
s32 BattlePrep_Align64PoolBlock(s32* arg0);
void BattlePrep_LoadBadgeTextures(unk_D_84B259E8* arg0, BinArchive* arg1, BinArchive* arg2);
void BattlePrep_InitBadgeCarouselScene(unk_D_84B259E8* arg0, BinArchive* arg1, BinArchive* arg2);

s32 BattlePrepRoster_DrawIconVariantA(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepRoster_DrawIconVariantB(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepRoster_DrawTrainerBadge(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepRoster_ScrollBackgroundTexture(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepRoster_ConvergeIconsStep(unk_D_84B25A28* arg0, s16 arg1);
void BattlePrepRoster_UpdateCameraSwoop(unk_D_84B25A28* arg0, s16 arg1);
s32 BattlePrepRoster_ZoomStepNTSC(unk_D_84B25A28* arg0, s16 arg1);
s32 BattlePrepRoster_ZoomStepPAL(unk_D_84B25A28* arg0, s16 arg1);
void BattlePrepRoster_UpdateAssembleStep(unk_D_84B25A28* arg0);
void BattlePrepRoster_UpdateLoadingStep(unk_D_84B25A28* arg0);
void BattlePrepRoster_Update(unk_D_84B25A28* arg0);
void BattlePrepRoster_LoadDexIdsSingle(unk_D_84B25A28* arg0);
void BattlePrepRoster_LoadDexIdsDouble(unk_D_84B25A28* arg0);
void BattlePrepRoster_Load(unk_D_84B25A28* arg0, BinArchive* arg1);
void BattlePrepRoster_PositionIconsSingle(unk_D_84B25A28* arg0);
void BattlePrepRoster_PositionIconsDouble(unk_D_84B25A28* arg0);
s32 BattlePrepRoster_InitDisplayObjects(unk_D_84B25A28* arg0, unk_D_86002F58_004_000* arg1);
void BattlePrepRoster_Init(unk_D_84B25A28* arg0, unk_D_86002F34_00C* arg1);

s32 BattlePrepTeamIcons_DrawActiveTeamBadge(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepTeamIcons_DrawIconSingle(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepTeamIcons_DrawIconDouble(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepTeamIcons_DrawBadgeIconSingle(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepTeamIcons_DrawBadgeIconDouble(s32 arg0, unk_func_80011B94* arg1);
void BattlePrepTeamIcons_DrawHpBarsSingle(unk_D_84B25A58* arg0, s32 arg1);
void BattlePrepTeamIcons_DrawHpBarsDouble(unk_D_84B25A58* arg0, s32 arg1);
void BattlePrepTeamIcons_MarkFaintedSingle(unk_D_84B25A58* arg0, s32 arg1);
void BattlePrepTeamIcons_MarkFaintedDouble(unk_D_84B25A58* arg0, s32 arg1);
void BattlePrepTeamIcons_InitDisplayObjectsSingleVisible(unk_D_84B25A58* arg0);
void BattlePrepTeamIcons_InitDisplayObjectsSingleHidden(unk_D_84B25A58* arg0);
void BattlePrepTeamIcons_InitDisplayObjectsDoubleVisible(unk_D_84B25A58* arg0);
void BattlePrepTeamIcons_InitDisplayObjectsDoubleHidden(unk_D_84B25A58* arg0);
void BattlePrepTeamIcons_RevealStepSingle(unk_D_84B25A58* arg0, s16 arg1);
void BattlePrepTeamIcons_RevealStepDouble(unk_D_84B25A58* arg0, s16 arg1);
void BattlePrepTeamIcons_HpDrainStepSingleNTSC(unk_D_84B25A58* arg0, s16 arg1);
void BattlePrepTeamIcons_HpDrainStepDoubleNTSC(unk_D_84B25A58* arg0, s16 arg1);
void BattlePrepTeamIcons_HpDrainStepSinglePAL(unk_D_84B25A58* arg0, s16 arg1);
void BattlePrepTeamIcons_HpDrainStepDoublePAL(unk_D_84B25A58* arg0, s16 arg1);
void BattlePrepTeamIcons_FinalRevealSingle(unk_D_84B25A58* arg0, s16 arg1);
void BattlePrepTeamIcons_FinalRevealDouble(unk_D_84B25A58* arg0, s16 arg1);
void BattlePrepTeamIcons_UpdateStep(unk_D_84B25A58* arg0);
void BattlePrepTeamIcons_UpdateLoadingStep(unk_D_84B25A58* arg0);
void BattlePrepTeamIcons_Update(unk_D_84B25A58* arg0);
void BattlePrepTeamIcons_LoadDexIdsSingle(unk_D_84B25A58* arg0);
void BattlePrepTeamIcons_LoadDexIdsDouble(unk_D_84B25A58* arg0);
void BattlePrepTeamIcons_Load(unk_D_84B25A58* arg0, BinArchive* arg1);
s32 BattlePrepTeamIcons_InitDisplayObjects(unk_D_84B25A58* arg0, unk_D_86002F58_004_000* arg1);
void BattlePrepTeamIcons_LoadIconTexturesSingle(unk_D_84B25A58* arg0);
void BattlePrepTeamIcons_LoadIconTexturesDouble(unk_D_84B25A58* arg0);
void BattlePrepTeamIcons_Init(unk_D_84B25A58* arg0);

s32 BattlePrepBadgeAward_DrawBadgeIcon(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepBadgeAward_DrawBadgeIconAlt(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepBadgeAward_DrawSparkle(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepBadgeAward_DrawGlowFade(s32 arg0, unk_func_80011B94* arg1);
void func_84B0B054(void);
void BattlePrepBadgeAward_SlideInStep(unk_D_84B25A90* arg0);
void BattlePrepBadgeAward_RevealStep(unk_D_84B25A90* arg0);
void BattlePrepBadgeAward_SpawnSparkleParticle(unk_D_84B25A90* arg0, unk_D_86002F58_004_000* arg1, s16 arg2);
void BattlePrepBadgeAward_UpdateSparkleStep(unk_D_84B25A90* arg0);
void BattlePrepBadgeAward_ScatterStep(unk_D_84B25A90* arg0);
void BattlePrepBadgeAward_Update(unk_D_84B25A90* arg0);
void BattlePrepBadgeAward_InitDisplayObjects(unk_D_84B25A90* arg0, unk_D_86002F58_004_000* arg1);
void BattlePrepBadgeAward_StartScatter(unk_D_84B25A90* arg0);
void BattlePrepBadgeAward_Init(unk_D_84B25A90* arg0);

s32 BattlePrepStarBurst_DrawStarIcon(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepStarBurst_DrawTrailIcon(s32 arg0, unk_func_80011B94* arg1);
s32 BattlePrepStarBurst_DrawSparkParticle(s32 arg0, unk_func_80011B94* arg1);
void BattlePrepStarBurst_InitSparkSlot(unk_D_84B25AC0_000* arg0, unk_D_86002F34* arg1, s16 arg2, s16 arg3, s16 arg4);
void BattlePrepStarBurst_UpdateSparkSlot(unk_D_84B25AC0_000* arg0);
s32 BattlePrepStarBurst_DrawRainbowParticle(s32 arg0, unk_func_80011B94* arg1);
void BattlePrepStarBurst_InitRainbowParticle(unk_D_84B25AC0_0A0* arg0, unk_D_86002F34* arg1, s16 arg2, s16 arg3, s16 arg4);
void BattlePrepStarBurst_SpawnTrailCopy(unk_D_84B25AC0_0A0* arg0, unk_D_84B25AC0_0A0* arg1);
void BattlePrepStarBurst_SpawnFlashCopy(unk_D_84B25AC0_0A0* arg0, unk_D_84B25AC0_0A0* arg1, s16 arg2);
void BattlePrepStarBurst_SpawnScatterParticle(unk_D_84B25AC0_0A0* arg0, unk_D_86002F34* arg1, Vec3f* arg2);
void BattlePrepStarBurst_UpdateRainbowGradientStep(unk_D_84B25AC0_0A0* arg0);
void BattlePrepStarBurst_UpdateTrailFadeStep(unk_D_84B25AC0_0A0* arg0);
void BattlePrepStarBurst_UpdateFlashFadeStep(unk_D_84B25AC0_0A0* arg0);
void BattlePrepStarBurst_UpdateScatterFallStep(unk_D_84B25AC0_0A0* arg0);
void BattlePrepStarBurst_UpdateRainbowParticle(unk_D_84B25AC0_0A0* arg0);
void BattlePrepStarBurst_UpdateBurstEffect(unk_D_84B25AC0* arg0);
void BattlePrepStarBurst_UpdateFlightArc(unk_D_84B25AC0* arg0);
void BattlePrepStarBurst_UpdateRainbowAftermath(unk_D_84B25AC0* arg0);
void BattlePrepStarBurst_UpdateSparkAftermath(unk_D_84B25AC0* arg0);
void BattlePrepStarBurst_UpdateExitSlide(unk_D_84B25AC0* arg0);
void BattlePrepStarBurst_UpdateFirstClearExit(unk_D_84B25AC0* arg0);
void BattlePrepStarBurst_Update(unk_D_84B25AC0* arg0);
void BattlePrepStarBurst_Launch(unk_D_84B25AC0* arg0, unk_D_86002F58_004_000* arg1, unk_D_86002F58_004_000* arg2);
void BattlePrepStarBurst_StartExit(unk_D_84B25AC0* arg0);
void BattlePrepStarBurst_Init(unk_D_84B25AC0* arg0, s8 arg1, s8 arg2);
void BattlePrep_TrophyModelIdle(unk_D_84B26640_010* arg0);
void BattlePrep_TrophyModelGrow(unk_D_84B26640_010* arg0);
void BattlePrep_TrophyModelShrink(unk_D_84B26640_010* arg0);
void BattlePrep_UpdateTrophyModel(unk_D_84B26640_010* arg0);
void BattlePrep_StartTrophyModelGrow(unk_D_84B26640_010* arg0, unk_D_86002F58_004_000* arg1);
void BattlePrep_StartTrophyModelShrink(unk_D_84B26640_010* arg0);
void BattlePrep_InitTrophyModel(unk_D_84B26640_010* arg0, u8 arg1);
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
