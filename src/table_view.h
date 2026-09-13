#ifndef _3D340_H_
#define _3D340_H_

#include "global.h"
#include "src/model_renderer.h"
#include "src/poke_icon.h"

typedef struct unk_func_88205880_00D0 {
    /* 0x000 */ BattleMon unk_000[20];
    /* 0x690 */ s32 partyCount; // LabPC_ReadBoxEntries's return value
    /* 0x694 */ s32 category; // deck-type constant (e.g. 0x21)
    /* 0x698 */ s32 boxIndex;
    /* 0x69C */ u32 isDirty;
} unk_func_88205880_00D0; // size = 0x6A0

typedef struct unk_func_88200FA0_030_038 {
    /* 0x00 */ unk_func_88205880_00D0** data; // PointerList_Allocate/Insert
    /* 0x04 */ s32 capacity;
    /* 0x08 */ s32 count;
} unk_func_88200FA0_030_038; // size = 0xC

typedef struct unk_func_88201DA0_034 {
    /* 0x0000 */ s32 mon; // BattleMon* into the box's party array (LabPC_CheckBox_ShowBox)
    /* 0x0004 */ char unk004[0x4];
    /* 0x0008 */ u16 iconTexture[0x640]; // PokeIcon_LoadModelTextureForMon's destination
} unk_func_88201DA0_034; // size = 0xC88

typedef struct unk_func_88201DA0_038 {
    /* 0x00 */ unk_func_88201DA0_034* data; // TableView_Initialize's arg1
    /* 0x04 */ s32 capacity; // TableView_Initialize's arg3
    /* 0x08 */ s32 count; // TableView_Initialize's arg4
    /* 0x0C */ s32 rowStride; // TableView_Initialize's arg2
} unk_func_88201DA0_038; // size = 0x10

typedef struct unk_func_88205880_A030 {
    /* 0x00 */ unk_func_88205880_00D0** data; // PointerList_Initialize's arg1
    /* 0x04 */ s32 capacity; // PointerList_Initialize's arg2
    /* 0x08 */ s32 count; // PointerList_Initialize's arg3
} unk_func_88205880_A030; // size = 0xC

typedef struct unk_func_8820BE14_06C_000 {
    /* 0x00 */ s32 owner; // LabPCList_AllocateBoxSlots: back-reference to the owning box-slot set
    /* 0x04 */ unk_func_88200FA0_030_038 unk_04;
} unk_func_8820BE14_06C_000; // size = 0x10

typedef struct unk_func_8820BE14_06C_000_alt {
    /* 0x00 */ s32 owner;
    /* 0x04 */ unk_func_88200FA0_030_038 unk_04; // LabPCList_AllocateBoxSlots: .data reused as a plain slot index, not a pointer
    /* 0x10 */ unk_func_88205880_A030 unk_10;
} unk_func_8820BE14_06C_000_alt; // size = 0x1C

typedef struct unk_func_8820BE14_06C {
    /* 0x00 */ unk_func_8820BE14_06C_000* data; // ByteMatrix_Allocate: raw byte buffer, mistyped as a row-struct pointer
    /* 0x04 */ s32 capacity; // ByteMatrix_Allocate's arg2 (row capacity)
    /* 0x08 */ s32 count; // current row count
    /* 0x0C */ s32 rowStride; // ByteMatrix_Allocate's arg1 (bytes per row)
} unk_func_8820BE14_06C; // size = 0x10

typedef struct unk_func_8830867C_02C_0CC_000_000_000 {
    /* 0x00 */ char unk00[0x8];
    /* 0x08 */ s32 unk_08;
} unk_func_8830867C_02C_0CC_000_000_000; // size >= 0xC

typedef struct unk_func_8830867C_02C_0CC_000_000_010_000_000 {
    /* 0x00 */ struct unk_func_8830867C_02C_0CC_000_000* boxRef; // LabPCList_ReindexBoxSlots writes back through this
    /* 0x04 */ s32 slotIndex; // LabPCList_ReindexBoxSlots: reassigned to the entry's position after a remove/insert
    /* 0x08 */ BattleMon mon;
} unk_func_8830867C_02C_0CC_000_000_010_000_000; // size >= 0x5C

typedef struct unk_func_8830867C_02C_0CC_000_000_010_000 {
    /* 0x00 */ unk_func_8830867C_02C_0CC_000_000_010_000_000* entry;
} unk_func_8830867C_02C_0CC_000_000_010_000; // size >= 0x4

typedef struct unk_func_8830867C_02C_0CC_000_000_010 {
    /* 0x00 */ unk_func_8830867C_02C_0CC_000_000_010_000* data; // LabPCList_ReindexBoxSlots/MoveEntryToBox; box's slot-entry list
    /* 0x04 */ char unk04[0x4];
    /* 0x08 */ s32 count;
} unk_func_8830867C_02C_0CC_000_000_010; // size = 0xC

typedef struct unk_func_8830867C_02C_0CC_000_000 {
    /* 0x00 */ unk_func_8830867C_02C_0CC_000_000_000* unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 isPopulated; // LabPCList_FlattenEntries gates flattening this box's entries on this being nonzero
    /* 0x0C */ s32 isDirty; // LabPCList_MoveEntryToBox sets this to 1 on both the source and destination box
    /* 0x10 */ unk_func_8830867C_02C_0CC_000_000_010 entries; // this box's slot-entry list (LabPCList_ReindexBoxSlots/MoveEntryToBox)
} unk_func_8830867C_02C_0CC_000_000; // size = 0x1C

typedef struct unk_func_8830867C_02C_0CC_000_008 {
    /* 0x00 */ u8 species; // CalculateStatValue's speciesId arg
    /* 0x01 */ char unk01[0x1];
    /* 0x02 */ u16 currentHP;
    /* 0x04 */ char unk04[0x5];
    /* 0x09 */ u8 moveIds[4]; // LabPCList_ApplyPpUpItem: Move_GetData(moveIds[arg2])
    /* 0x0D */ char unk0D[0x3];
    /* 0x10 */ u8 unk_10; // LabPCList_ApplyVitaminItem writes *(s32*)&unk_10 (exp, for Pokemon_ExpForLevel); declared size undersells: spans through unk_11 and the 2 pad bytes at 0x12
    /* 0x11 */ u8 unk_11;
    /* 0x12 */ char unk12[0x2];
    /* 0x14 */ u16 hpStatExp; // CalculateStatValue's EV arg for STAT_HP
    /* 0x16 */ u16 attackStatExp;
    /* 0x18 */ u16 defenseStatExp;
    /* 0x1A */ u16 speedStatExp;
    /* 0x1C */ u16 specialStatExp;
    /* 0x1E */ u16 dvs; // CalculateStatValue's IV-bits arg
    /* 0x20 */ u8 ppUpCounts[4]; // LabPCList_ApplyPpUpItem: (ppUpCounts[arg2] >> 6) scales basePP
    /* 0x24 */ u8 level;
    /* 0x26 */ u16 maxHP; // CalculateStatValue(STAT_HP, ...) result
    /* 0x28 */ u16 attack;
    /* 0x2A */ u16 defense;
    /* 0x2C */ u16 speed;
    /* 0x2E */ u16 special;
} unk_func_8830867C_02C_0CC_000_008; // size = 0x30

typedef struct unk_func_8830867C_02C_0CC_000 {
    /* 0x00 */ unk_func_8830867C_02C_0CC_000_000* box; // LabPCList_MoveEntryToBox's arg1->unk_00; owning box
    /* 0x04 */ s32 slotIndex; // LabPCList_MoveEntryToBox: set to the destination box's PointerList count on insert
    /* 0x08 */ unk_func_8830867C_02C_0CC_000_008 itemState; // LabPCList_ApplyVitaminItem's per-mon vitamin/stat-boost counters
    /* 0x38 */ char unk_38[4];
    /* 0x3C */ char unk3C[0x20];
} unk_func_8830867C_02C_0CC_000; // size = 0x5C

typedef struct unk_func_8830867C_02C_0CC {
    /* 0x00 */ unk_func_8830867C_02C_0CC_000** data; // PointerList_Remove/Sort/Filter
    /* 0x04 */ s32 capacity;
    /* 0x08 */ s32 count;
} unk_func_8830867C_02C_0CC; // size = 0xC

typedef struct unk_func_8830867C_04C_030_02C_000_000_00C {
    /* 0x00 */ s32 data; // WordList_Remove: (s16*)data, mistyped s32
    /* 0x04 */ char unk04[0x4];
    /* 0x08 */ s32 count;
} unk_func_8830867C_04C_030_02C_000_000_00C; // size >= 0xC

typedef s32 (*PointerListFilterPredicate)(unk_func_8830867C_02C_0CC_000*, s32*);
typedef s32 (*unk_D_8831A1F8)(s32, s32);

void TableView_Initialize(unk_func_88201DA0_038*, unk_func_88201DA0_034*, s32, s32, s32);
void ByteMatrix_Allocate(unk_func_8820BE14_06C*, s32, s32, MemoryPool*);
s32 ByteMatrix_InsertRow(unk_func_8820BE14_06C*, s8*, s32);
s32 ByteMatrix_RemoveRow(unk_func_8820BE14_06C*, s32);
s32 WordList_Remove(unk_func_8830867C_04C_030_02C_000_000_00C*, s32);
void PointerList_Initialize(unk_func_88205880_A030*, void**, s32, s32);
void PointerList_Allocate(unk_func_88200FA0_030_038*, s32, MemoryPool*);
void PointerList_Insert(unk_func_88200FA0_030_038*, char*, s32);
void PointerList_Remove(unk_func_8830867C_02C_0CC*, s32);
void PointerList_Sort(unk_func_8830867C_02C_0CC*, unk_D_8831A1F8);
void PointerList_Filter(unk_func_8830867C_02C_0CC*, unk_func_8830867C_02C_0CC*, PointerListFilterPredicate, s32*);
s32 Text_CompareGlyphCodes(u8*, u8*);
void Vec2f_Normalize(f32*);
Color_RGBA8 Color_ScaleRgb(Color_RGBA8, f32);

#endif // _3D340_H_
