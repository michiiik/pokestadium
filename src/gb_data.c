#include "gb_data.h"
#include "include/string.h"
#include "src/gb_data.h"
#include "src/save_data.h"
#include "src/text_system.h"
#include "src/gb_tower.h"
#include "src/hal_libc.h"
#include "src/util.h"

u16 GbData_ReadU16BE(u8* arg0) {
    return (arg0[0] << 8) | arg0[1];
}

u32 GbData_ReadU24BE(u8* arg0) {
    return (arg0[0] << 0x10) | (arg0[1] << 8) | arg0[2];
}

void GbData_WriteU16BE(u8* arg0, s32 arg1) {
    arg0[0] = (arg1 >> 8) & 0xFF;
    arg0[1] = arg1 & 0xFF;
}

void GbData_WriteU24BE(u8* arg0, s32 arg1) {
    arg0[0] = (arg1 >> 0x10) & 0xFF;
    arg0[1] = (arg1 >> 8) & 0xFF;
    arg0[2] = (arg1) & 0xFF;
}

char* Text_UntranscodeName(char* arg0, char* arg1) {
    extern s8 D_80073030[];

    char* sp34 = arg0;
    char* sp18;
    char* temp_s1 = Text_GetPlayerLabel(1);

    while (*arg1 != 'P') {
        if (*arg1 == ']') {
            memcpy(arg0, temp_s1, strlen(temp_s1) + 1);
            arg0 += strlen(temp_s1);
        } else {
            *arg0++ = D_80073030[*arg1];
        }
        arg1++;
    }

    *arg0++ = '\x0';

    return sp34;
}

void Text_UntranscodeNameWrapper(char* arg0, char* arg1) {
    Text_UntranscodeName(arg0, arg1);
}

char* Text_TranscodeName(char* arg0, char* arg1) {
    extern u8 D_80073130[];

    char* sp1C = arg0;
    char* sp18 = arg1;

    if (HAL_Strcmp(sp18, Text_GetPlayerLabel(1)) == 0) {
        *arg0++ = ']';
    } else {
        while (*sp18 != 0) {
            *arg0++ = D_80073130[*sp18++];
        }
    }

    *arg0++ = 'P';

    return sp1C;
}

char* Text_TranscodeNameWrapper(char* arg0, char* arg1) {
    return Text_TranscodeName(arg0, arg1);
}

char* Text_UntranscodeFixedName(char* arg0, char* arg1) {
    char sp1C[11];

    _bcopy(arg1, sp1C, 0xA);

    sp1C[10] = 'P';

    return Text_UntranscodeName(arg0, sp1C);
}

char* Text_CopyString(char* arg0, char* arg1) {
    HAL_Strcpy(arg0, arg1);
    return arg0;
}

char* Text_CopySpeciesName(char* arg0, s32 arg1) {
    Text_CopyString(arg0, (char*)*(u32*)D_8006FEE8[arg1]);
    return arg0;
}

char* Text_GetTmHmName(s32 arg0, char* arg1) {
    extern char* D_80072EE0[];
    // "ひでんマシン%02d"
    static const char D_0007CCA0[] = { 0xA4, 0xD2, 0xA4, 0xC7, 0xA4, 0xF3, 0xA5, 0xDE, 0xA5, 0xB7,
                                       0xA5, 0xF3, 0x25, 0x30, 0x32, 0x64, 0x00, 0x00, 0x00, 0x00 };
    // "わざマシン%02d"
    static const char D_0007CCB4[] = { 0xA4, 0xEF, 0xA4, 0xB6, 0xA5, 0xDE, 0xA5, 0xB7,
                                       0xA5, 0xF3, 0x25, 0x30, 0x32, 0x64, 0x00, 0x00 };

    if ((arg0 > 0) && (arg0 < 0x54)) {
        HAL_Strcpy(arg1, D_80072EE0[arg0 - 1]);
    } else if ((arg0 >= 0xC4) && (arg0 < 0xC9)) {
        sprintf(arg1, D_0007CCA0, arg0 - 0xC3, arg0);
    } else if ((arg0 >= 0xC9) && (arg0 < 0xFF)) {
        sprintf(arg1, D_0007CCB4, arg0 - 0xC8, arg0);
    } else {
        HAL_Strcpy(arg1, D_80072EE0[6]);
    }

    return arg1;
}

void Pokemon_FromGbRecord(BattleMon* arg0, GbPokemonRecord* arg1) {
    arg0->species.dexId = Species_FromInternalIndex(arg1->speciesInternalIndex);
    arg0->species.internalIndex = arg1->speciesInternalIndex;
    arg0->currentHP = GbData_ReadU16BE(arg1->currentHP);

    arg0->boxLevel = arg1->boxLevel;
    arg0->status = arg1->status;
    arg0->type1 = arg1->type1;
    arg0->type2 = arg1->type2;
    arg0->catchRate = arg1->catchRate;

    arg0->otId = GbData_ReadU16BE(arg1->otId);
    arg0->exp = GbData_ReadU24BE(arg1->exp);
    arg0->hpStatExp = GbData_ReadU16BE(arg1->hpStatExp);
    arg0->attackStatExp = GbData_ReadU16BE(arg1->attackStatExp);
    arg0->defenseStatExp = GbData_ReadU16BE(arg1->defenseStatExp);
    arg0->speedStatExp = GbData_ReadU16BE(arg1->speedStatExp);
    arg0->specialStatExp = GbData_ReadU16BE(arg1->specialStatExp);
    arg0->dvs = GbData_ReadU16BE(arg1->dvs);

    memcpy(arg0->moves, arg1->moves, 4);
    memcpy(arg0->pp, arg1->pp, 4);
}

void Pokemon_FromGbRecordExt(BattleMon* arg0, GbPokemonRecordExt* arg1) {
    Pokemon_FromGbRecord(arg0, &arg1->base);
    arg0->level = arg1->level;
    arg0->maxHP = GbData_ReadU16BE(&arg1->maxHP);
    arg0->attack = GbData_ReadU16BE(&arg1->attack);
    arg0->defense = GbData_ReadU16BE(&arg1->defense);
    arg0->speed = GbData_ReadU16BE(&arg1->speed);
    arg0->special = GbData_ReadU16BE(&arg1->special);
}

void Pokemon_ToGbRecord(BattleMon* arg0, GbPokemonRecord* arg1) {
    if (Species_ToInternalIndex(arg0->species.dexId) & 0xFF) {
        arg1->speciesInternalIndex = Species_ToInternalIndex(arg0->species.dexId);
    } else {
        arg1->speciesInternalIndex = arg0->species.internalIndex;
    }

    GbData_WriteU16BE(arg1->currentHP, (u16)arg0->currentHP);

    arg1->boxLevel = arg0->boxLevel;
    arg1->status = arg0->status;
    arg1->type1 = arg0->type1;
    arg1->type2 = arg0->type2;
    arg1->catchRate = arg0->catchRate;

    GbData_WriteU16BE(arg1->otId, arg0->otId);
    GbData_WriteU24BE(arg1->exp, arg0->exp);
    GbData_WriteU16BE(arg1->hpStatExp, arg0->hpStatExp);
    GbData_WriteU16BE(arg1->attackStatExp, arg0->attackStatExp);
    GbData_WriteU16BE(arg1->defenseStatExp, arg0->defenseStatExp);
    GbData_WriteU16BE(arg1->speedStatExp, arg0->speedStatExp);
    GbData_WriteU16BE(arg1->specialStatExp, arg0->specialStatExp);
    GbData_WriteU16BE(arg1->dvs, arg0->dvs);

    memcpy(arg1->moves, arg0->moves, 4);
    memcpy(arg1->pp, arg0->pp, 4);
}

void Pokemon_ToGbRecordExt(unk_func_88205880_00D0* arg0, GbPokemonRecordExt* arg1) {
    Pokemon_ToGbRecord(arg0->unk_000, arg1);
    arg1->level = arg0->unk_000[0].level;
    GbData_WriteU16BE(&arg1->maxHP, arg0->unk_000[0].maxHP);
    GbData_WriteU16BE(&arg1->attack, arg0->unk_000[0].attack);
    GbData_WriteU16BE(&arg1->defense, arg0->unk_000[0].defense);
    GbData_WriteU16BE(&arg1->speed, arg0->unk_000[0].speed);
    GbData_WriteU16BE(&arg1->special, arg0->unk_000[0].special);
}

u16 StatExpSqrt(u16 arg0) {
    u16 i;

    for (i = 1; i < 255; i++) {
        if (SQ(i) >= arg0) {
            break;
        }
    }
    return i;
}

void func_800220C4(void) {
}

u16 GetPokemonDV(s32 statId, u16 dvBits) { //GetPokemonDV
    u16 dvValue;

    switch (statId) {
        case STAT_HP: // HP DV (derived from the LSBs of other DVs)
            dvValue = ((dvBits & 0x1000) >> 9)  // Special DV bit 0 → HP DV bit 0
                    | ((dvBits & 0x0100) >> 6)  // Speed DV bit 0 → HP DV bit 1
                    | ((dvBits & 0x0010) >> 3)  // Defense DV bit 0 → HP DV bit 2
                    | (dvBits & 0x0001);        // Attack DV bit 0 → HP DV bit 3
            break;

        case STAT_ATTACK: // Special DV
            dvValue = (dvBits >> 12) & 0xF;
            break;

        case STAT_DEFENSE: // Speed DV
            dvValue = (dvBits >> 8) & 0xF;
            break;

        case STAT_SPEED: // Defense DV
            dvValue = (dvBits >> 4) & 0xF;
            break;

        case STAT_SPECIAL: // Attack DV
            dvValue = dvBits & 0xF;
            break;
    }

    return dvValue;
}

u32 CalculateStatValue(s32 statId, s32 speciesId, u16 statModifier, u16 level, u16 dvBits) {
    u16 statValue;
    u16 baseStat;
    u16 dvValue;

    switch (statId) {
        case STAT_HP:
            baseStat = gBaseStats[speciesId].baseHP;
            break;
        case STAT_ATTACK:
            baseStat = gBaseStats[speciesId].baseAttack;
            break;
        case STAT_DEFENSE:
            baseStat = gBaseStats[speciesId].baseDefense;
            break;
        case STAT_SPEED:
            baseStat = gBaseStats[speciesId].baseSpeed;
            break;
        case STAT_SPECIAL:
            baseStat = gBaseStats[speciesId].baseSpecial;
            break;
    }

    // Get the Pokémon's DV for the stat (or HP DV)
    dvValue = GetPokemonDV(statId, dvBits);

    // Compute the preliminary stat value
    statValue = (((StatExpSqrt(statModifier) / 4) + ((baseStat + dvValue) * 2)) * level) / 100;

    // Add final adjustments
    if (statId == STAT_HP) {
        statValue += level + 10;
    } else {
        statValue += 5;
    }

    // Cap at 999
    if (statValue > 999) {
        statValue = 999;
    }

    return statValue;
}

void Pokemon_RecalcStats(BattleMon* arg0) {
    arg0->level = Pokemon_LevelFromExp(arg0->species.dexId, arg0->exp);
    arg0->maxHP = CalculateStatValue(1, arg0->species.dexId, arg0->hpStatExp, arg0->level, arg0->dvs);
    arg0->attack = CalculateStatValue(2, arg0->species.dexId, arg0->attackStatExp, arg0->level, arg0->dvs);
    arg0->defense = CalculateStatValue(3, arg0->species.dexId, arg0->defenseStatExp, arg0->level, arg0->dvs);
    arg0->speed = CalculateStatValue(4, arg0->species.dexId, arg0->speedStatExp, arg0->level, arg0->dvs);
    arg0->special = CalculateStatValue(5, arg0->species.dexId, arg0->specialStatExp, arg0->level, arg0->dvs);
}

u16 Pokemon_StatExpForTarget(s32 arg0, s32 arg1, u16 arg2, u16 arg3, u16 arg4) {
    s32 i;
    s32 tmp;

    for (i = 1; i < 0xFFFF; i += 2) {
        tmp = CalculateStatValue(arg0, arg1, i + 2, arg3, arg4);
        if (arg2 < tmp) {
            break;
        }
    }

    return i;
}

u32 Pokemon_ExpForLevel(s32 arg0, u16 arg1) {
    u32 sp4;

    switch (gBaseStats[arg0].growthRate) {
        case 0:
            sp4 = arg1 * arg1 * arg1;
            break;

        case 1:
            sp4 = (((arg1 * arg1 * arg1 * 3) / 4) + (arg1 * 0xA * arg1)) - 0x1E;
            break;

        case 2:
            sp4 = (((arg1 * arg1 * arg1 * 3) / 4) + (arg1 * 0x14 * arg1)) - 0x1E;
            break;

        case 3:
            sp4 = ((((arg1 * arg1 * arg1 * 6) / 5) - (arg1 * 0xF * arg1)) + (arg1 * 0x64)) - 0x8C;
            break;

        case 4:
            sp4 = (arg1 * arg1 * arg1 * 4) / 5;
            break;

        case 5:
            sp4 = (arg1 * arg1 * arg1 * 5) / 4;
            break;
    }

    return sp4;
}
