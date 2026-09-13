#ifndef _29BA0_H_
#define _29BA0_H_

#include "global.h"
#include "jpeg_decoder.h"
#include "src/controller.h"
#include "PR/leo.h"

typedef struct arg1_func_80019420 {
    /* 0x00 */ s8 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ void* unk_04;
} arg1_func_80019420; // size = 0x8

typedef struct unk_D_800AE520 {
    /* 0x00 */ s16 pendingReload; // set to 1 by Fragment_QueueCall; consumed once, then cleared to 0
    /* 0x02 */ s16 fragment_id;
    /* 0x04 */ u8* rom_start;
    /* 0x08 */ u8* rom_end;
    /* 0x0C */ s32 arg0;
    /* 0x10 */ s32 arg1;
} unk_D_800AE520; // size >= 0x14

typedef struct BattleMonSpecies {
    /* 0x00 */ u8 dexId;         // Stadium species id (National Dex number)
    /* 0x01 */ u8 internalIndex; // Gen I GB internal index (Species_To/FromInternalIndex)
} BattleMonSpecies; // size = 0x2

// Layout pinned by Pokemon_FromGbRecord / Pokemon_ToGbRecord (22630.c), which
// round-trip the Gen I box/party record field by field.
typedef struct BattleMon {
    /* 0x00 */ BattleMonSpecies species;
    /* 0x02 */ u16 currentHP;
    /* 0x04 */ u8 boxLevel; // GB box-record level byte (party level is `level`)
    /* 0x05 */ u8 status;
    /* 0x06 */ u8 type1;
    /* 0x07 */ u8 type2;
    /* 0x08 */ u8 catchRate;
    /* 0x09 */ u8 moves[4];
    /* 0x0D */ u8 pad0D;
    /* 0x0E */ u16 otId;
    /* 0x10 */ u32 exp; // source is 24-bit on the GB side
    /* 0x14 */ u16 hpStatExp;
    /* 0x16 */ u16 attackStatExp;
    /* 0x18 */ u16 defenseStatExp;
    /* 0x1A */ u16 speedStatExp;
    /* 0x1C */ u16 specialStatExp;
    /* 0x1E */ u16 dvs; // packed Gen I DV nibbles; decode with GetPokemonDV
    /* 0x20 */ u8 pp[4];
    /* 0x24 */ u8 level; // active level (GB party record +0x21)
    /* 0x25 */ u8 faintOrder;
    /* 0x26 */ u16 maxHP;
    /* 0x28 */ u16 attack;
    /* 0x2A */ u16 defense;
    /* 0x2C */ u16 speed;
    /* 0x2E */ u16 special;
    /* 0x30 */ u8 nickname[11];
    /* 0x3B */ u8 otName[11];
    /* 0x46 */ u8 otNameEncoded[11];
    /* 0x51 */ u8 pad51;
    /* 0x52 */ u8 sourceAndFlags;
    /* 0x53 */ u8 sourceSlot;
} BattleMon; // size = 0x54

typedef struct ExtendedRosterInfo {
    /* 0x000 */ u16 unk_000;
    /* 0x002 */ u8 partyCount;
    /* 0x003 */ u8 trainerIdHigh;
    /* 0x004 */ char unk004[0x10];
    /* 0x014 */ char shortName[4]; // declared size undersells the real string extent; see HAL_Strcpy call sites
    /* 0x018 */ char unk018[0x10];
    /* 0x028 */ BattleMon party[1];
    /* 0x07C */ char unk07C[0x1A4];
    /* 0x220 */ char longName[4]; // declared size undersells the real string extent; see HAL_Strcpy call sites
    /* 0x224 */ char unk224[0x24];
} ExtendedRosterInfo; // size = 0x248

typedef struct TeamRoster {
    /* 0x000 */ u8 slotState; // Trainer_Create: 0 = free (scanned for), 1 = normal alloc, 3 = arg0==-1 special-case alloc (bit 2 set — matches the CPU-vs-player badge-icon test); Trainer_RequestPokeIcon's compare against iconSpeciesId looks incidental (both can be small ints early in a slot's life)
    /* 0x001 */ u8 trainerSlotId;
    /* 0x002 */ u8 partyCount;
    /* 0x004 */ Controller* controller;
    /* 0x008 */ char shortName[4]; // mirrors ExtendedRosterInfo.shortName (same HAL_Strcpy source in Trainer_Create); declared size undersells the real string extent
    /* 0x00C */ char unk00C[0xC];
    /* 0x018 */ s32 trainerId;
    /* 0x01C */ BattleMon party[6];
    /* 0x214 */ ExtendedRosterInfo* extendedRoster;
    /* 0x218 */ char unk218[0x4];
} TeamRoster; // size = 0x21C

typedef struct BattleSessionTeams {
    /* 0x00 */ u8 modelLoadFlags; // BattleAnim_IsSessionBusy reads &0xD0; set |=0x80, cleared &=~0x90, checked &0x10
    /* 0x01 */ u8 playerCount; // ==1: single controller drives both team[0]/team[1] slots (BattleScene_AssignActiveCoopController); else each side uses its own controller
    /* 0x02 */ s16 iconSpeciesId; // fed to PokeIcon_RequestFrameLoad; 0x98 sentinel = "show trainer, not a mon" (BattleAnim_BeginSessionRequest)
    /* 0x04 */ TeamRoster* activeMon; // Trainer_RequestPokeIcon reads this as TeamRoster (slotState/party); fragment62 battle code instead assigns &roster->party[idx] (a BattleMon*) into it - contradictory usage, not resolved further this pass
    /* 0x08 */ TeamRoster* teams[2];
    /* 0x10 */ unk_D_86002F58_004_000_010* unk_10;
    /* 0x14 */ unk_D_86002F58_004_000_004* moveEffectListRoot; // Battle_GetMoveEffectListEntry
    /* 0x18 */ unk_D_86002F58_004_000_004* auxModelLayout; // BattleScene_InitializeParticipantAuxiliaryModel's geo layout for the secondary model
    /* 0x1C */ u8 isActiveSide; // gates Intro_SelectPartyRoster + win-streak increment in BattlePrep_ConfirmDefaultRules
    /* 0x1D */ u8 assetLoadFlags; // BattleAnim_IsAssetLoadBusy reads &0xD0; BattleAnim_PollAssetLoadBusyFlags polls then reads &0xC0
    /* 0x1E */ u8 winStreak; // capped display via BattlePrep_SetStreakColor, "%2d" print, checked against 0x63
    /* 0x20 */ s32 unk_20;
    /* 0x24 */ arg1_func_80019420* unk_24;
} BattleSessionTeams; // size = 0x28

typedef struct SessionContext {
    /* 0x0000 */ s8 sessionMode;
    /* 0x0001 */ s8 modeCategory;
    /* 0x0002 */ s8 progressIndex;
    /* 0x0003 */ s8 opponentNumber;
    /* 0x0004 */ TeamRoster unk_0004[4];
    /* 0x0874 */ ExtendedRosterInfo unk_0874[4];
    /* 0x1194 */ BattleSessionTeams unk_1194[2];
    /* 0x11E4 */ u8 unk_11E4[2][4];
    /* 0x11EC */ u8 cpuTrainerId; // Battle_SelectCpuOrder's arg3; interchangeable with a TeamRoster's trainerId & 0xFF at another call site
    /* 0x11EC */ u8 levelEditable; // mirrors a Prime Cup rule's `editable` flag
    /* 0x11EC */ u8 levelMin; // mirrors a Prime Cup rule's `levelMin`
    /* 0x11EC */ u8 levelMax; // mirrors a Prime Cup rule's `levelMax`
    /* 0x11F0 */ u8 gbStarterChoice;
    /* 0x11F1 */ char unk11F1[0x1];
    /* 0x11F2 */ u8 roundSelector;
    /* 0x11F3 */ u8 badgeCount; // BattlePrep's "%s%d" badge-count display; decremented per award, incremented by the battle-prep scene singleton's teamChosen
    /* 0x11F4 */ u8 unk_11F4;
    /* 0x11F5 */ u8 sessionFlowFlags;
    /* 0x11F6 */ u16 battleFlowFlags;
} SessionContext; // size = 0x11F8

extern s32 gCurrentGameState;
extern unk_D_800AE520 D_800AE520;
extern SessionContext D_800AE540;
extern BinArchive* D_800AF738;

s32 Game_DoCopyProtection(s32 state);
s32 Fragment_LoadAndCall(s32 arg0, u8* romStart, u8* romEnd, u32 arg3, u32 arg4);
void Fragment_QueueCall(s32 arg0, u8* arg1, u8* arg2, s32 arg3, s32 arg4);
TeamRoster* Trainer_GetSlots(void);
s32 GbSave_GetActivePort(void);
void GbSave_SetActivePort(s32 arg0);
void Controller_PollInputs(void);
s32 Stage_RunDemoScene(s16 arg0);
void State_RunCredits(void);
void State_N64LogoIntro(void);
void State_TitleScreen(void);
void State_N64ddBootUnused(void);
void State_StubbedDebug(void);
void State_AreaSelect(void);
void State_EventBattle(void);
void State_Options(void);
void State_MenuSelect(void);
s16 RuleWizard_StadiumMenu(s16 arg0);
void State_StadiumMenu(void);
s32 RuleWizard_FreeBattle(s16 arg0);
void State_FreeBattle(void);
s16 RuleWizard_VsMewtwo(s16 arg0);
void State_VsMewtwo(void);
void State_KidsClub(void);
void State_Gallery(void);
void State_VictoryPalace(void);
void State_GbTower(void);
s16 RuleWizard_GymLeaderCastle(s16 arg0, s16 arg1);
void State_GymLeaderCastle(void);
void State_BattleNow(s32 arg0);
void State_BattleFromEvent(void);
void State_FastN64Logo(void);
void State_FastBattle(void);
void func_8002B244(void);
void State_KidsClubTitle(void);
void Game_ShutdownAndLoadFragment(s32 arg0, s32 arg1);
void Game_ResetStorageDevice(void);
void Game_Thread(UNUSED LEODiskID* arg);

#endif // _29BA0_H_
