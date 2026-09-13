#ifndef _22630_H_
#define _22630_H_

#include "global.h"
#include "memory.h"
#include "game_state.h"
#include "table_view.h"

#define STAT_HP 1
#define STAT_ATTACK 2
#define STAT_DEFENSE 3
#define STAT_SPEED 4
#define STAT_SPECIAL 5

typedef struct PokemonStats {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 unk_05;
    /* 0x06 */ u8 baseHP;
    /* 0x07 */ u8 baseAttack;
    /* 0x08 */ u8 baseDefense;
    /* 0x09 */ u8 baseSpeed;
    /* 0x0A */ u8 baseSpecial;
    /* 0x0B */ char unk0B[0x4];
    /* 0x0F */ u8 unk_0F[1];
    /* 0x10 */ char unk10[0x3];
    /* 0x13 */ u8 growthRate;
    /* 0x14 */ char unk14[0x3];
} PokemonStats; // size = 0x17

typedef struct unk_D_80070F84 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 unk_05;
    /* 0x06 */ u8 type1; // 2C1C0.c/fragment23_1AE680.c: D_80070FA0[dexId-1].type1, mirrors BattleMon.type1
    /* 0x07 */ u8 type2;
    /* 0x08 */ u8 unk_08;
    /* 0x09 */ u8 unk_09;
    /* 0x0A */ u8 unk_0A[4];
    /* 0x0E */ char unk0E[1];
    /* 0x0F */ u8 unk_0F[1];
    /* 0x10 */ char unk10[0x3];
    /* 0x13 */ u8 unk_13;
    /* 0x14 */ char unk14[0x3];
    // NOTE: fragment62_359F90.c's Battle_CalculateStat indexes this same array by BattleMon.level (not species)
    // to read unk_01-unk_05 per stat case - contradicts the species-indexed type1/type2 usage above; not resolved
    // this pass (possibly two distinct tables sharing one overly-generic declaration, or a genuine engine reuse
    // of the same memory for two purposes).
} unk_D_80070F84; // size = 0x17

extern PokemonStats gBaseStats[];
extern unk_D_80070F84 D_80070FA0[]; //TODO: is this typed correctly?

typedef struct unk_D_80071D88 {
    /* 0x00 */ char* type_name;
    /* 0x04 */ Color_RGB8 color;
} unk_D_80071D88; // size = 0x8
extern unk_D_80071D88 D_80071D88[];

typedef struct GbPokemonRecord {
    /* 0x00 */ u8 speciesInternalIndex;
    /* 0x01 */ u8 currentHP[2];
    /* 0x03 */ u8 boxLevel;
    /* 0x04 */ u8 status;
    /* 0x05 */ u8 type1;
    /* 0x06 */ u8 type2;
    /* 0x07 */ u8 catchRate;
    /* 0x08 */ u8 moves[4];
    /* 0x0C */ u8 otId[2];
    /* 0x0E */ u8 exp[3];
    /* 0x11 */ u8 hpStatExp[2];
    /* 0x13 */ u8 attackStatExp[2];
    /* 0x15 */ u8 defenseStatExp[2];
    /* 0x17 */ u8 speedStatExp[2];
    /* 0x19 */ u8 specialStatExp[2];
    /* 0x1B */ u8 dvs[2];
    /* 0x1D */ u8 pp[4];
} GbPokemonRecord; // size = 0x21

typedef struct GbPokemonNamedRecord {
    /* 0x00 */ GbPokemonRecord record;
    /* 0x21 */ u8 nickname[11];
    /* 0x2C */ u8 otName[11];
} GbPokemonNamedRecord; // size = 0x37

typedef struct GbPokemonRecordExt {
    /* 0x00 */ GbPokemonRecord base;
    /* 0x21 */ u8 level;
    /* 0x22 */ u16 maxHP;
    /* 0x24 */ u16 attack;
    /* 0x26 */ u16 defense;
    /* 0x28 */ u16 speed;
    /* 0x2A */ u16 special;
} GbPokemonRecordExt; // size = 0x2C

typedef struct DeckHandle {
    /* 0x00 */ u8 mode; // Deck_Open's arg3; ==1 selected by Deck_Initialize*Backend to reset the entry count
    /* 0x01 */ u8 deckType; // Deck_Open's arg0; high nibble dispatches Deck_InitializeSaveBackend(0x10)/InitializeBoxBackend(0x20)
    /* 0x02 */ u8 port; // Deck_Open's arg1
    /* 0x03 */ u8 slot; // Deck_Open's arg2 (box/save-record index)
    /* 0x04 */ u8 dirty; // bit 0 set when the deck was modified; checked by Deck_CloseAndFlush/CloseAndFlushBox to decide whether to commit
    /* 0x05 */ u8 recordSize; // Deck_GetSaveRecordSize/Deck_GetEntrySize
    /* 0x06 */ s16 capacity; // Deck_GetSaveRecordCapacity/Deck_GetCapacity
    /* 0x08 */ s16 count; // Deck_GetSaveEntryCount/Deck_GetCount
    /* 0x0A */ s16 cursor; // Deck_SetCursor
} DeckHandle; // size = 0xC

typedef struct GbSaveParty {
    /* 0x000 */ u8 count; // GbSave_GetPartyCount/Deck_GetCount(32)/Deck_DeleteEntry/Deck_AppendEntry
    /* 0x001 */ u8 speciesIndices[1]; // GB internal species index per party slot, fed to GbSave_MarkSeenBits; declared size undersells the real 6-slot extent
    /* 0x002 */ char unk002[6];
    /* 0x008 */ GbPokemonRecordExt mons[6];
    /* 0x110 */ char nicknames[6][11];
    /* 0x152 */ char otNames[6][11];
} GbSaveParty; // size = 0x194

typedef struct GbSaveBoxEntry {
    /* 0x000 */ u8 count; // GbSave_GetBoxEntryCount/SetBoxEntryCount
    /* 0x000 */ u8 speciesIndices[1]; // GB internal species index per box slot, indexed up to count (declared size undersells)
    /* 0x002 */ char unk002[0x12];
} GbSaveBoxEntry; // size = 0x14

typedef struct GbSaveCurrentBox {
    /* 0x000 */ GbSaveBoxEntry header;
    /* 0x014 */ char unk014[0x2];
    /* 0x016 */ GbPokemonRecord mons[1]; // declared size undersells the real per-box slot count (GbSave_Load/SaveBoxPokemon index by arg3)
    /* 0x037 */ char unk037[0x273];
    /* 0x2AA */ char otNames[1][11]; // Text_UntranscodeFixedName(arg0->otName, ...) / also the _bcopy'd otNameEncoded source; declared size undersells
    /* 0x2B5 */ char unk2B5[0xD1];
    /* 0x386 */ char nicknames[1][11]; // Text_UntranscodeFixedName(arg0->nickname, ...); declared size undersells
} GbSaveCurrentBox; // size >= 0x391

typedef struct GbSaveChecksumBlock {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ char unk02[0x1];
    /* 0x03 */ u8 checksum; // GbSave_ComputeChecksum/VerifyChecksum/VerifyWriteback target byte; computed over [playerName, this) each write
    /* 0x04 */ char unk04[0x1C];
} GbSaveChecksumBlock; // size = 0x20

typedef struct GbSaveMainData {
    /* 0x000 */ u8 unk_000[1];
    /* 0x001 */ char unk001[0x17];
    /* 0x018 */ u8 playerName[11]; // Text_UntranscodeFixedName/Text_TranscodeName via GbSave_Get/SetPlayerName; also mirrored into GbSavePort.playerIdentity.playerName
    /* 0x023 */ u8 ownedBitmap[1]; // declared size undersells the real bitmap extent (0x97 species / 8); tier-2 bit of GbSave_SetSeenOwnedBits' arg2>=2 case
    /* 0x024 */ char unk024[0x12];
    /* 0x036 */ u8 seenBitmap[1]; // declared size undersells the real bitmap extent; tier-1 bit of GbSave_SetSeenOwnedBits' arg2>0 case
    /* 0x037 */ char unk037[0x12];
    /* 0x049 */ u8 unk_049; // "deck 35" count/entries (Deck_GetCount/Deck_GetCapacity=0x14); same structural shape as unk_266/GbSaveParty, no in-game label found yet
    /* 0x04A */ BattleMonSpecies unk_04A[1];
    /* 0x04C */ char unk04C[0x2A];
    /* 0x076 */ u8 secondaryName; // Text_UntranscodeFixedName/Text_TranscodeName via GbSave_Get/SetSecondaryName; declared size undersells the real string extent (rival name, same pattern as playerName)
    /* 0x077 */ char unk077[0xE];
    /* 0x085 */ u8 trainerId; // read/written as u16 via GbData_ReadU16BE/WriteU16BE in GbSave_Get/SetTrainerId; declared size undersells the real 2-byte extent
    /* 0x086 */ char unk086[0x4];
    /* 0x08A */ u8 currentLocationId; // checked against the D_80073640[12] Pokemon-Center-location table in GbSave_CheckLocationFlag, which sets GbSavePort.flags |=0x4000 (GbSave_SavedAtPokemonCenter)
    /* 0x08B */ char unk08B[0x1DB];
    /* 0x266 */ u8 unk_266; // "deck 34" count/entries (Deck_GetCount/Deck_GetCapacity=0x32); structurally a count+species-id-list pair like GbSaveParty but no in-game label found yet to say what this deck represents
    /* 0x267 */ BattleMonSpecies unk_267[1];
    /* 0x269 */ char unk269[0x63];
    /* 0x2CC */ s8 boxDataStatus; // u16 (declared size undersells): low byte's 0x80 bit set means "loaded" (GbSave_MarkBoxDataLoaded), high byte's 0x80/0x7F bits encode a box count (GbSave_GetBoxCount)
    /* 0x2CD */ char unk2CD[0x174];
    /* 0x441 */ u8 starterChoice; // read by GbSave_GetStarterChoice
    /* 0x442 */ char unk442[0x35];
    /* 0x477 */ s8 pokedexFlags; // bit 0x20 set means GbSavePort.flags |=0x2000 (GbSave_HasPokedex) in GbSave_RefreshPortState
    /* 0x478 */ char unk478[0xA5];
    /* 0x51D */ s8 progressFlags; // bit 0x01 set means GbSavePort.flags |=0x1000 (GbSave_HasProgressFlag1000) in GbSave_RefreshPortState
    /* 0x51E */ char unk51E[0x48E];
    /* 0x9AC */ GbSaveParty party;
    /* 0xB40 */ GbSaveCurrentBox currentBox; // GbSave_GetBoxEntry treats this as the last box slot (arg1 == boxCount-1)
    /* 0xED1 */ char unkED1[0xCF];
    /* 0xFA0 */ GbSaveChecksumBlock checksumBlock;
} GbSaveMainData; // size = 0xFC0

typedef struct GbSaveBoxPair {
    /* 0x000 */ GbSaveBoxEntry primary; // GbSave_GetBoxEntry: box index arg1 < 6 within a bank
    /* 0x000 */ GbSaveBoxEntry secondary; // GbSave_GetBoxEntry: box index arg1 >= 6 within a bank (indexed the same outer slot as primary -- looks like a pre-existing indexing bug in the original code, not fixed here)
    /* 0x028 */ char unk028[0x43A];
} GbSaveBoxPair; // size = 0x462

typedef struct GbSaveBoxBank {
    /* 0x0000 */ GbSaveBoxPair pairs[6];
    /* 0x1A4C */ char unk1A4C[0x14];
} GbSaveBoxBank; // size = 0x1A60

typedef struct GbSaveBoxData {
    /* 0x0000 */ GbSaveBoxBank banks[2]; // GbSave_ReadBoxBank/WriteBoxBank(arg1: 0 or 1), at ROM offsets 0x4000/0x6000
} GbSaveBoxData; // size = 0x34C0

typedef struct GbSavePlayerIdentity {
    /* 0x00 */ u16 trainerId; // mirrors GbSaveMainData.trainerId, cached by GbSave_RefreshPlayerIdentity
    /* 0x02 */ char playerName[11]; // mirrors GbSaveMainData.playerName, same refresh
    /* 0x0D */ u8 unk_0D[2];
    /* 0x0F */ char unkF[0x1];
} GbSavePlayerIdentity; // size = 0x10

typedef struct GbSavePort {
    /* 0x00 */ u16 flags; // bit0 = main data open, bit1 = box data open (GbSave_ReleasePortBuffers clears &~0x9/&~0x12)
    /* 0x02 */ u8 cartId; // compared against GbCart_IdentifyGame(arg0)
    /* 0x03 */ u8 pakState; // 0 = ok, gates GbSave_EnsureMainDataLoaded; returned by an accessor at 232C0.c:1192
    /* 0x04 */ u8 mainDataChecksum; // cached checksum byte for GbSave_CheckMainDataUnchanged's change detection
    /* 0x05 */ u8 partyCount; // returned by GbSave_GetPartyCount; refreshed from mainData->party.count in GbSave_RefreshPortState
    /* 0x06 */ u8 stadiumEncounteredBitmap[0x13]; // per-species bit, tier 3 of GbSave_GetSpeciesProgressLevel's 0-3 scale; every GbSave_SetSeenOwnedBits(..., 3) call site is a Stadium-side battle/roster encounter, not a GB-save seen/owned bit (those live in GbSaveMainData.ownedBitmap/seenBitmap)
    /* 0x19 */ u8 pakId[0x20]; // passed to GbTower_ProbePak/VerifyPakStillInserted as a pak-identity buffer
    /* 0x39 */ char unk39[0x7];
    /* 0x40 */ GbSavePlayerIdentity playerIdentity;
    /* 0x50 */ GbSaveMainData* mainData;
    /* 0x54 */ GbSaveBoxData* boxData;
} GbSavePort; // size = 0x58

u16 GbData_ReadU16BE(u8* arg0);
u32 GbData_ReadU24BE(u8* arg0);
void GbData_WriteU16BE(u8* arg0, s32 arg1);
void GbData_WriteU24BE(u8* arg0, s32 arg1);
char* Text_UntranscodeName(char* arg0, char* arg1);
void Text_UntranscodeNameWrapper(char* arg0, char* arg1);
char* Text_TranscodeName(char* arg0, char* arg1);
char* Text_TranscodeNameWrapper(char* arg0, char* arg1);
char* Text_UntranscodeFixedName(char* arg0, char* arg1);
char* Text_CopyString(char* arg0, char* arg1);
char* Text_CopySpeciesName(char* arg0, s32 arg1);
char* Text_GetTmHmName(s32 arg0, char* arg1);
void Pokemon_FromGbRecord(BattleMon* arg0, GbPokemonRecord* arg1);
void Pokemon_FromGbRecordExt(BattleMon* arg0, GbPokemonRecordExt* arg1);
void Pokemon_ToGbRecord(BattleMon* arg0, GbPokemonRecord* arg1);
void Pokemon_ToGbRecordExt(unk_func_88205880_00D0* arg0, GbPokemonRecordExt* arg1);
u16 StatExpSqrt(u16 arg0);
void func_800220C4(void);
u16 func_800220CC(s32 arg0, u16 arg1);
u32 CalculateStatValue(s32 statId, s32 speciesId, u16 statModifier, u16 level, u16 dvBits);
void Pokemon_RecalcStats(BattleMon* arg0);
u16 Pokemon_StatExpForTarget(s32 arg0, s32 arg1, u16 arg2, u16 arg3, u16 arg4);
u32 Pokemon_ExpForLevel(s32 arg0, u16 arg1);

#endif // _6BC0_H_
