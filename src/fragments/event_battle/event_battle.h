#ifndef _FRAGMENT38_H_
#define _FRAGMENT38_H_

#include "global.h"
#include "src/game_state.h"

typedef struct unk_D_82305A28 {
    /* 0x00 */ u8 state; // EventBattle_RunSelect's top-level state machine
    /* 0x01 */ u8 teamValid; // AND of EventBattle_Validate*'s per-team checks; drives the notice banner
    /* 0x02 */ s16 cursorIndex; // index into gEventBattleModes, moved by the mode-select 2-column grid input
    /* 0x04 */ s16 firstInvalidIndex; // 0 or 1, !teamValid; used as the loop start when re-flagging invalid trainer cards
    /* 0x06 */ s16 nextState; // EventBattle_UpdateModeSelect sets this on exit for the caller to pick up
} unk_D_82305A28; // size = 0x8

typedef struct unk_D_82305A50 {
    /* 0x00 */ u8 state; // EventBattle_InitRuleCard/UpdateRuleCardSlide
    /* 0x01 */ u8 isLegal; // EventBattle_CheckTeamLegality AND of both teams; drives DrawRuleCard's red/white color
    /* 0x02 */ s16 timer; // slide-in/out countdown
    /* 0x04 */ s16 baseX; // EventBattle_InitRuleCard's arg1
    /* 0x06 */ s16 baseY; // EventBattle_InitRuleCard's arg2
    /* 0x08 */ s16 screenX; // live slide-animated position, interpolated from baseX by timer
    /* 0x0A */ s16 screenY;
    /* 0x0C */ u8* iconData; // rule icon texture, NULL = text-only card
    /* 0x10 */ char* labelText;
    /* 0x14 */ s16 minLevel; // printed as "minLevel~maxLevel" in EventBattle_DrawRuleCard
    /* 0x16 */ s16 maxLevel;
    /* 0x18 */ s16 unk_18;
} unk_D_82305A50; // size = 0x1C

typedef struct SlidingPanel {
    /* 0x00 */ u8 state; // 0 = idle, 1 = opening, 2 = closing (EventBattle_Update{InfoBox,NoticeBanner})
    /* 0x01 */ u8 isValid; // EventBattle_ShowNoticeBanner's arg1; gates the banner's message text and error sound
    /* 0x02 */ s16 height; // slide-open extent, animated toward a per-use max by state
    /* 0x04 */ s16 x;
    /* 0x06 */ s16 y;
} SlidingPanel; // size = 0x8

typedef struct unk_D_82305A40 {
    /* 0x00 */ u8 state; // EventBattle_UpdateSettingsPanel's dispatch
    /* 0x00 */ u8 editable; // EventBattle_InitSettingsPanel's arg3; 0 locks the spinners/cursor
    /* 0x02 */ s16 levelMin; // spinner value, step of 5, range up to 0x5A
    /* 0x04 */ s16 levelMax; // spinner value, step of 0xA
    /* 0x06 */ s16 height; // slide-open panel height
    /* 0x08 */ s16 x;
    /* 0x0A */ s16 y;
    /* 0x0C */ s16 cursorLine; // 0-3, selects the highlighted/editable setting row
} unk_D_82305A40; // size = 0x10

typedef struct unk_D_82305AF8 {
    /* 0x0000 */ u8 state; // EventBattle_InitTrainerCard/ValidateTeamForRule
    /* 0x0001 */ u8 gbPort; // EventBattle_InitTrainerCard's arg3
    /* 0x0002 */ u8 allLegal; // EventBattle_CheckTeamLegality: all 6 party mons pass the rule's species/level check
    /* 0x0003 */ u8 weightValid; // sum of the 3 lowest levels <= the rule's cap
    /* 0x0004 */ s16 ruleId; // EventBattle_ValidateTeamForRule's arg1
    /* 0x0006 */ s16 timer;
    /* 0x0008 */ s16 baseX;
    /* 0x000A */ s16 baseY;
    /* 0x000C */ s16 screenX;
    /* 0x000E */ s16 screenY;
    /* 0x0010 */ s16 unk_0010;
    /* 0x0012 */ u16 trainerId; // GbSave_GetTrainerId
    /* 0x0014 */ char playerName[4]; // GbSave_GetPlayerName; declared size undersells the real string extent, same pattern seen elsewhere this session
    /* 0x0018 */ char unk0018[0x8];
    /* 0x0020 */ BattleMon party[6];
    /* 0x0218 */ u8 iconTextures[6][0xC80]; // PokeIcon_LoadModelTextureForMon per party slot
} unk_D_82305AF8; // size = 0x4D18

typedef struct unk_D_8230F528 {
    /* 0x00 */ u8 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ char* unk_08;
} unk_D_8230F528; // size = 0x0C

typedef struct EventBattleModeEntry {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
} EventBattleModeEntry; // size = 0xA

typedef struct unk_D_82305978 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
} unk_D_82305978; // size = 0x8

u8 EventBattle_CheckTeamLegality(unk_D_82305AF8* arg0, s16 arg1);

#endif // _FRAGMENT38_H_
