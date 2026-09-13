#include "battle_engine.h"
#include "src/gb_data.h"
#include "src/text_system.h"
#include "src/matrix.h"

typedef struct unk_D_843C4DA0 {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 unk_05;
    /* 0x06 */ char unk06[0x3];
    /* 0x09 */ u8 unk_09;
    /* 0x0A */ char unk0A[0xA];
    /* 0x13 */ u8 unk_13;
    /* 0x14 */ char unk14[0x1];
    /* 0x15 */ u8 unk_15;
    /* 0x16 */ char unk16[0x1];
    /* 0x17 */ u8 unk_17;
    /* 0x18 */ char unk18[0xB];
    /* 0x22 */ u8 unk_22;
    /* 0x24 */ u16 unk_24;
    /* 0x26 */ u16 unk_26;
    /* 0x28 */ u16 unk_28;
    /* 0x2A */ u16 unk_2A;
    /* 0x2C */ u16 unk_2C;
    /* 0x2E */ char unk2E[0x1E];
} unk_D_843C4DA0; // size = 0x4C

static u16 D_843C4DA0;
static u8 D_843C4DA2;
static u8 D_843C4DA3;
u8 gBattleMoveFailed;
u8 gBattleCritFlag;
static u8 D_843C4DA6;
static u8 D_843C4DA7;
static u8 D_843C4DA8;
u8 D_843C4DA9;
u8 D_843C4DAA;
static u8 D_843C4DAB;
static u8 D_843C4DAC;
static u8 D_843C4DAD;
static u8 D_843C4DAE;
static u8 D_843C4DAF;
static u8 D_843C4DB0;
static u8 D_843C4DB1;
static u8 D_843C4DB2;
static u8 D_843C4DB3;
static u8 D_843C4DB4;
static u8 D_843C4DB5;
static u8 D_843C4DB6;
static u8 D_843C4DB7;
static u8 D_843C4DB8;
static u8 D_843C4DB9;
static u8 D_843C4DBA;
static u8 D_843C4DBB;
static u8 D_843C4DBC;
static u8 D_843C4DBD;
static u8 D_843C4DBE;
static u8 D_843C4DBF;
static u8 D_843C4DC0;
static u8 D_843C4DC1;
static u8 D_843C4DC2;
u16 gBattleDamage;
static u16 D_843C4DC6;
static u16 D_843C4DC8;
static u16 D_843C4DCA;
static u16 D_843C4DCC;
static s32 pad_D_843C4DD0;
static s32 pad_D_843C4DD4;
static s32 pad_D_843C4DD8;
static s32 pad_D_843C4DDC;
static s32 pad_D_843C4DE0;
static s32 pad_D_843C4DE4;
static s32 pad_D_843C4DE8;
s32 D_843C4DEC;
static s32 pad_D_843C4DF0[21];
u8 D_843C4E44;
u8 D_843C4E45;
static s32 pad_D_843C4E48[252];
BattleMonRuntime* gBattleUser;
BattleMonRuntime* gBattleTarget;
static BattleMonRuntime D_843C5240[2];

u8 D_8438AC60[] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x4A, 0x00, 0x00,
    0x60, 0x69, 0x75, 0x78, 0x85, 0x00, 0x00, 0x00, 0x61, 0x64, 0x66, 0x68, 0x8C, 0x00, 0x00, 0x00,
    0x01, 0x0A, 0x0B, 0x11, 0x13, 0x14, 0x15, 0x1E, 0x22, 0x23, 0x25, 0x27, 0x2B, 0x2C, 0x2D, 0x2E,
    0x2F, 0x40, 0x44, 0x46, 0x47, 0x51, 0x59, 0x5A, 0x5B, 0x5C, 0x67, 0x6A, 0x6B, 0x6E, 0x6F, 0x76,
    0x7A, 0x80, 0x84, 0x8B, 0x8D, 0x91, 0x94, 0x96, 0x97, 0x9A, 0x9C, 0x9F, 0xA3, 0xA4, 0x00, 0x00,
};
static u8 D_8438ACB0[] = { 1, 0x20, 0x43, 0x54, 0x56 };
StatStageMultiplier gStatStageMultipliers[] = {
    { 0x19, 0x64 }, { 0x1C, 0x64 }, { 0x21, 0x64 }, { 0x28, 0x64 }, { 0x32, 0x64 }, { 0x42, 0x64 }, { 0x01, 0x01 },
    { 0x0F, 0x0A }, { 0x02, 0x01 }, { 0x19, 0x0A }, { 0x03, 0x01 }, { 0x23, 0x0A }, { 0x04, 0x01 },
};
u8 D_8438ACD4[] = {
    0x01, 0x03, 0x24, 0x64, 0x2B, 0x64, 0x01, 0x02, 0x42, 0x64, 0x4B, 0x64, 0x01,
    0x01, 0x85, 0x64, 0xA6, 0x64, 0x02, 0x01, 0xE9, 0x64, 0x85, 0x32, 0x03, 0x01,
};
u8 gTypeEffects[] = {
    0x15, 0x14, 0x14, 0x14, 0x16, 0x14, 0x14, 0x19, 0x14, 0x16, 0x15, 0x14, 0x17, 0x15, 0x14, 0x15, 5,    0x14,
    4,    2,    0,    0x15, 0x15, 5,    0x14, 0x14, 5,    0x17, 0x17, 5,    0x19, 0x19, 5,    0x16, 0x16, 5,
    0x18, 0x18, 5,    0x14, 0x15, 5,    0x16, 0x14, 5,    0x15, 0x16, 5,    0x17, 0x16, 5,    0,    5,    5,
    0,    8,    0,    8,    8,    0x14, 0x14, 7,    0x14, 0x14, 5,    5,    0x15, 4,    0x14, 0x17, 4,    0,
    0x17, 2,    0x14, 0x16, 4,    0x14, 0x16, 7,    5,    0x16, 3,    5,    0x16, 5,    0x14, 0x16, 2,    5,
    0x19, 0x15, 5,    0x19, 0x16, 0x14, 0x19, 4,    0x14, 0x19, 2,    0x14, 1,    0,    0x14, 1,    3,    5,
    1,    2,    5,    1,    0x18, 5,    1,    7,    5,    1,    5,    0x14, 1,    0x19, 0x14, 1,    8,    0,
    3,    0x16, 0x14, 3,    3,    5,    3,    4,    5,    3,    7,    0x14, 3,    5,    5,    3,    8,    5,
    4,    0x14, 0x14, 4,    0x17, 0x14, 4,    0x16, 5,    4,    7,    5,    4,    5,    0x14, 4,    3,    0x14,
    2,    0x17, 5,    2,    1,    0x14, 2,    7,    0x14, 2,    0x16, 0x14, 2,    5,    5,    0x18, 1,    0x14,
    0x18, 3,    0x14, 7,    0x14, 5,    7,    0x16, 0x14, 7,    1,    5,    7,    2,    5,    7,    0x18, 0x14,
    7,    8,    5,    7,    3,    0x14, 5,    0x14, 0x14, 5,    1,    5,    5,    4,    5,    5,    2,    0x14,
    5,    7,    0x14, 5,    0x19, 0x14, 8,    0,    0,    8,    0x18, 0,    0x14, 0x1A, 5,    0x15, 0x1A, 5,
    0x17, 0x1A, 5,    0x16, 0x1A, 5,    0x19, 0x1A, 0x14, 0x1A, 0x1A, 0x14, 0xFF, 0,    0,
};
static u32 D_8438ADEC[] = {
    0x18191C2E,
    0x2F313839,
    0x40414243,
    0x4F525455,
};
static u32 D_8438ADFC[] = {
    0x1B000000,
};
static u32 D_8438AE00[] = {
    0x28290000,
};
static u32 D_8438AE04[] = {
    0x010A0B0C, 0x0D0E0F12, 0x13141516, 0x171A2032, 0x33343536, 0x373A3B3C, 0x3D3E3F00,
};
static u32 D_8438AE20[] = {
    0x03070810,
    0x1D1E2C30,
    0x4D510000,
};
static u32 D_8438AE2C[] = {
    0x03070810,
    0x111D1E27,
    0x28292B2C,
    0x2D301B00,
};
static s32 D_8438AE3C = 0x4D2;
static u32 D_8438AE40 = 0x1A85;
static u32 D_8438AE44 = 0x3039;
static u32 D_8438AE48 = 0x10932;
static u8 gHighCritMoves[] = { 2, 0x4B, 0x98, 0xA3 };

void func_8436F6D0(void) {
}

u8 Battle_Random(void) {
    u8 var_v1;
    s32 ret;

    var_v1 = (osGetCount() & 0xFF0) >> 4;

    while (var_v1 > 0) {
        D_8438AE3C = (D_8438AE3C * 0x41C64E6D) + 0x303B;
        if (D_8438AE40 & 1) {
            D_8438AE40 ^= 0x11020;
        }
        D_8438AE40 >>= 1;
        var_v1--;
    }

    ret = D_8438AE3C ^ D_8438AE40;
    return ret;
}

u8 Battle_RandomAccuracy(void) {
    u8 var_v1;
    s32 ret;

    var_v1 = (osGetCount() & 0xFF0) >> 4;

    while (var_v1 > 0) {
        D_8438AE44 = (D_8438AE44 * 0x41C64E6D) + 0x303B;
        if (D_8438AE48 & 1) {
            D_8438AE48 ^= 0x11020;
        }
        D_8438AE48 >>= 1;
        var_v1--;
    }
    ret = D_8438AE44 ^ D_8438AE48;
    return ret;
}

void Battle_QueueHpStatusMessage(s32 arg0, s32 arg1) {
    TeamRoster* temp_a3;
    BattleMonRuntime* temp_a2_2;
    BattlerState* temp_v1;
    s32 sp18;

    temp_v1 = &D_84390010[arg0]->unk_654;
    temp_a2_2 = &D_84390010[arg1]->unk_654.monRuntime;
    temp_a3 = D_84390010[arg0]->sessionTeams->teams[temp_v1->sideIndex];

    if (temp_a2_2->currentHP == 0) {
        Text_SetStringToken(0x19, temp_a3->party[temp_v1->selectedPartyIndex].nickname);
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 0x21);
        return;
    }

    temp_v1->unk_28 = temp_a2_2->currentHP;
    sp18 = (temp_a2_2->currentHP * 0x19) / (temp_a2_2->maxHP >> 2);

    Text_SetStringToken(0x19, temp_a3->party[temp_v1->selectedPartyIndex].nickname);

    if (sp18 >= 0x46) {
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 0x21);
    } else if (sp18 >= 0x28) {
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 0x22);
    } else if (sp18 >= 0xA) {
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 0x23);
    } else {
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 0x24);
    }
}

void Battle_ResetTransientRuntimeState(s32 arg0, s32 arg1) {
    BattleMonRuntime* sp1C = &D_84390010[arg0]->unk_654.monRuntime;
    BattleMonRuntime* sp18 = &D_84390010[arg1]->unk_654.monRuntime;

    Battle_QueueHpStatusMessage(arg0, arg1);
    D_843C4DC2 = 0;
    D_843C4DB3 = 0;
    sp1C->cachedMove.moveId = 0;
    sp1C->lastMoveUsedId = 0;
    sp18->lastMoveUsedId = 0;
    sp1C->unk_4A = 0;
    sp1C->unk_4B = 0;
    sp1C->lockedEffectFlags = 0;
    sp1C->volatileStatusFlags = 0;
    sp1C->auxStatusFlags = 0;
    sp1C->disabledSlotAndTurns = 0;
    sp1C->disabledMoveId = 0;
    sp1C->statChangeFlag = 0;
    sp18->lockedEffectFlags &= ~0x20;
    D_843C4DA3 = 1;
}

void Battle_QueueDamageSummaryMessages(Battler* arg0) {
    BattlerState* sp1C = &D_84390010[0]->unk_654;
    BattlerState* sp18 = &D_84390010[1]->unk_654;
    BattleMonRuntime* temp_a1 = &D_84390010[0]->unk_654.monRuntime;
    BattleMonRuntime* temp_a2 = &D_84390010[1]->unk_654.monRuntime;
    s32 var_v0;

    if (arg0 == D_84390010[0]) {
        var_v0 = ((sp1C->unk_28 - temp_a2->currentHP) * 0x19) / (temp_a2->maxHP >> 2);
    } else {
        var_v0 = ((sp18->unk_28 - temp_a1->currentHP) * 0x19) / (temp_a1->maxHP >> 2);
    }

    if (var_v0 == 0) {
        Battle_QueueMessage(gBattleMessageQueues->unk_1C8, 0x25);
    } else if (var_v0 < 0x1E) {
        Battle_QueueMessage(gBattleMessageQueues->unk_1C8, 0x26);
    } else if (var_v0 < 0x46) {
        Battle_QueueMessage(gBattleMessageQueues->unk_1C8, 0x27);
    } else {
        Battle_QueueMessage(gBattleMessageQueues->unk_1C8, 0x28);
    }

    if (sp1C->faintSequenceState == 0x10) {
        Battle_QueueMessage(gBattleMessageQueues->unk_1C8, 0);
    }
}

void Battle_ApplyDamageRandomFactor(void) {
    s32 temp_v0;

    if (gBattleDamage >= 2) {
        temp_v0 = 0;
        while (temp_v0 < 0xD9) {
            temp_v0 = Battle_Random();
        }
        gBattleDamage = (gBattleDamage * temp_v0) / 255;
    }
}

u16 Battle_DetermineMoveOrder(Battler* arg0, Battler* arg1) {
    BattleMonRuntime* ptr1 = &arg0->unk_654.monRuntime;
    BattleMonRuntime* ptr2 = &arg1->unk_654.monRuntime;

    if (ptr1->currentMoveId == 0x62) {
        if (ptr2->currentMoveId != 0x62) {
            return 0;
        }
    } else if (ptr2->currentMoveId == 0x62) {
        return 1;
    }

    if (ptr1->currentMoveId == 0x44) {
        if (ptr2->currentMoveId != 0x44) {
            return 1;
        }
    } else if (ptr2->currentMoveId == 0x44) {
        return 0;
    }

    if (ptr2->speed < ptr1->speed) {
        return 0;
    }

    if (ptr1->speed < ptr2->speed) {
        return 1;
    }

    if (Battle_Random() < 0x80) {
        return 0;
    }
    return 1;
}

s32 Battle_ValueInList(u8 arg0, u8* arg1, s32 arg2) {
    while (arg2--) {
        if (arg0 == *arg1) {
            return 1;
        }
        arg1++;
    }

    return 0;
}

void Battle_SetActiveMoveFromSlotIndex(void) {
    D_843C5240[0].currentMoveId = D_843C5240[0].moveIds[D_843C4DC2];
}

s32 Battle_ExecuteMoveEffectAndCheckSuccess(void) {
    Battle_ExecuteMoveEffect();

    if (gBattleMoveFailed == 1) {
        return 0;
    }
    return 1;
}

void Battle_CalculateStat(s32 arg0, BattleMon* arg1) {
    u16 i;
    u16 spC;
    u16 spA;
    u16 tmp;
    u16 sp6;
    u16 temp_a0;

    switch (arg0) {
        case 1:
            spA = D_80070FA0[arg1->level].unk_01;
            spC = arg1->hpStatExp;
            break;

        case 2:
            spA = D_80070FA0[arg1->level].unk_02;
            spC = arg1->attackStatExp;
            break;

        case 3:
            spA = D_80070FA0[arg1->level].unk_03;
            spC = arg1->defenseStatExp;
            break;

        case 4:
            spA = D_80070FA0[arg1->level].unk_04;
            spC = arg1->speedStatExp;
            break;

        case 5:
            spA = D_80070FA0[arg1->level].unk_05;
            spC = arg1->specialStatExp;
            break;
    }

    for (i = 1; i < 255; i++) {
        if (SQ(i) >= spC) {
            break;
        }
    }

    tmp = D_843C5240[0].dvs;

    switch (arg0) {
        case 1:
            sp6 = ((tmp & 0x1000) >> 9) | ((tmp & 0x100) >> 6) | ((tmp & 0x10) >> 3) | (tmp & 1);
            break;

        case 2:
            sp6 = (tmp >> 0xC) & 0xF;
            break;

        case 3:
            sp6 = (tmp >> 8) & 0xF;
            break;

        case 4:
            sp6 = (tmp >> 4) & 0xF;
            break;

        case 5:
            sp6 = tmp & 0xF;
            break;
    }

    temp_a0 = ((((spA + sp6) * 2) + (i >> 2)) * arg1->level) / 100;
    if (arg0 == 1) {
        temp_a0 += arg1->level + 0xA;
    } else {
        temp_a0 += 5;
    }

    if (temp_a0 >= 0x3E8) {
        temp_a0 = 0x3E7;
    }

    switch (arg0) {
        case 1:
            arg1->maxHP = temp_a0;
            break;

        case 2:
            arg1->attack = temp_a0;
            break;

        case 3:
            arg1->defense = temp_a0;
            break;

        case 4:
            arg1->speed = temp_a0;
            break;

        case 5:
            arg1->special = temp_a0;
            break;
    }
}

void Battle_RecalculateStats(BattleMon* arg0) {
    Battle_CalculateStat(0, arg0);
    Battle_CalculateStat(1, arg0);
    Battle_CalculateStat(2, arg0);
    Battle_CalculateStat(3, arg0);
    Battle_CalculateStat(4, arg0);
}

void Battle_CalculateDamage(void) {
    gBattleDamage = 0;
    D_843C4DC6 = gBattleUser->cachedMove.power;
    if (D_843C4DC6) {
        if (gBattleUser->cachedMove.type < 0x14) {
            D_843C4DCA = gBattleTarget->defense;
            if (gBattleTarget->auxStatusFlags & 4) {
                D_843C4DCA *= 2;
            }

            if (gBattleCritFlag != 0) {
                D_843C4DCA = gBattleTarget->origDefense;
                D_843C4DC8 = gBattleUser->origAttack;
            } else {
                D_843C4DC8 = gBattleUser->attack;
            }
        } else {
            D_843C4DCA = gBattleTarget->special;
            if (gBattleTarget->auxStatusFlags & 2) {
                D_843C4DCA *= 2;
            }

            if (gBattleCritFlag != 0) {
                D_843C4DCA = gBattleTarget->origSpecial;
                D_843C4DC8 = gBattleUser->origSpecial;
            } else {
                D_843C4DC8 = gBattleUser->special;
            }
        }

        if (D_843C4DC8 & 0xFF00) {
            if (D_843C4DCA & 0xFF00) {
                D_843C4DC8 >>= 2;
                D_843C4DCA >>= 2;
                if (D_843C4DC8 == 0) {
                    D_843C4DC8 = 1;
                }
            }
        }

        D_843C4DCC = gBattleUser->level;
        if (gBattleCritFlag != 0) {
            D_843C4DCC *= 2;
        }
    }
}

s32 Battle_CalcDamage(void) {
    s32 temp_v0 = gBattleUser->cachedMove.effectId;

    if (temp_v0 == 7) {
        D_843C4DCA >>= 1;
        if (D_843C4DCA == 0) {
            D_843C4DCA = 1;
        }
    }

    if ((temp_v0 != 0x1D) && (temp_v0 != 0x1E)) {
        if (temp_v0 == 0x26) {
            return Battle_ExecuteMoveEffectAndCheckSuccess();
        }

        if (D_843C4DC6 == 0) {
            return 0;
        }
    }

    gBattleDamage += (((((D_843C4DCC * 2) / 5) + 2) * D_843C4DC6 * D_843C4DC8) / D_843C4DCA) / 50;
    if (gBattleDamage > 0x3E4) {
        gBattleDamage = 0x3E4;
    }
    gBattleDamage += 2;

    return 1;
}

void Battle_ApplyDamage(void) {
    if (gBattleTarget->volatileStatusFlags & 0x10) {
        if (gBattleScene.scene->unk_48 == 0x14) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_208, 0x2B);
            if (gBattleTarget->substituteHP < gBattleDamage) {
                gBattleTarget->volatileStatusFlags &= 0xFFEF;
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
                Battle_QueueMessage(gBattleMessageQueues->unk_208, 0x2C);

                if ((D_8438AC60[0] == 1) && (gBattleUser->cachedMove.effectId == 0x1D)) {
                    Battle_ExecuteMoveEffect();
                    gBattleUser->lockedEffectCounter = 1;
                }

                if ((D_8438AC60[0] != 1) || (gBattleUser->cachedMove.effectId != 7)) {
                    gBattleUser->cachedMove.effectId = 0;
                }

                Battle_SetRuntimeFlags(D_84390010[gBattleScene.scene->activeBattlerIndex], 0x1000);
            } else {
                gBattleTarget->substituteHP -= gBattleDamage;
            }
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_208, 0x2B);
            if (gBattleTarget->substituteHP < gBattleDamage) {
                gBattleTarget->volatileStatusFlags &= 0xFFEF;
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
                Battle_QueueMessage(gBattleMessageQueues->unk_208, 0x2C);

                if ((D_8438AC60[0] == 1) && (gBattleUser->cachedMove.effectId == 0x1D)) {
                    Battle_ExecuteMoveEffect();
                    gBattleUser->lockedEffectCounter = 1;
                }

                if ((D_8438AC60[0] != 1) || (gBattleUser->cachedMove.effectId != 7)) {
                    gBattleUser->cachedMove.effectId = 0;
                }

                Battle_SetRuntimeFlags(D_84390010[!gBattleScene.scene->activeBattlerIndex], 0x1000);
            } else {
                gBattleTarget->substituteHP -= gBattleDamage;
            }
        }
    } else if (D_84390288 == 0) {
        if (gBattleDamage < gBattleTarget->currentHP) {
            gBattleTarget->currentHP -= gBattleDamage;
        } else {
            gBattleDamage = gBattleTarget->currentHP;
            gBattleTarget->currentHP = 0;
        }
    }
}

void Battle_DecrementPP(s32 arg0) {
    BattleMonRuntime* temp_v0;
    BattlerState* temp_a1;
    BattleMon* temp_a2;

    temp_v0 = &D_84390010[arg0]->unk_654.monRuntime;
    temp_a1 = &D_84390010[arg0]->unk_654;
    temp_a2 = &D_84390010[arg0]->ownRoster->party[temp_a1->partyIndex];

    if ((temp_v0->currentMoveId != 0xA5) && !(temp_v0->lockedEffectFlags & 7) && !(temp_v0->volatileStatusFlags & 0x40)) {
        temp_v0->currentPP[temp_a1->committedMoveSlot]--;
        if (!(temp_v0->auxStatusFlags & 8)) {
            temp_a2->pp[temp_a1->committedMoveSlot] -= 1;
        }
    }
}

void Battle_ApplyAccuracyStatStages(void) {
    StatStageMultiplier* var_v0;
    s32 var_a2;
    s32 idx;
    s32 idx2;

    if (D_8438AC60[0] == 1) {
        var_v0 = D_8438ACD4;
    } else {
        var_v0 = gStatStageMultipliers;
    }

    idx = gBattleUser->statStages[4];
    idx2 = gBattleTarget->statStages[5];
    idx2 = 0xE - idx2;

    var_a2 = (gBattleUser->cachedMove.accuracy * var_v0[idx - 1].unk_00) / var_v0[idx - 1].unk_01;
    if (var_a2 == 0) {
        var_a2 = 1;
    }

    var_a2 = (var_a2 * var_v0[idx2 - 1].unk_00) / var_v0[idx2 - 1].unk_01;
    if (var_a2 == 0) {
        var_a2 = 1;
    }

    if (var_a2 >= 0x100) {
        var_a2 = 0xFF;
    }
    gBattleUser->cachedMove.accuracy = var_a2;
}

void Battle_FailMove(void) {
    gBattleDamage = 0;
    gBattleMoveFailed = 1;
    gBattleUser->lockedEffectFlags &= ~0x20;
}

void Battle_CheckMoveFailureConditions(void) {
    if (gBattleUser->cachedMove.effectId == 0x50) {
        if (!(gBattleUser->volatileStatusFlags & 0x20) && (D_8438AC60[0] == 1)) {
            gBattleUser->volatileStatusFlags |= 0x20;
        }
    }

    if ((gBattleUser->cachedMove.effectId == 8) && !(gBattleTarget->status & 7)) {
        Battle_FailMove();
        return;
    }

    if (gBattleTarget->volatileStatusFlags & 0x10) {
        if (gBattleUser->cachedMove.effectId == 17) {
            return;
        }

        if (gBattleUser->cachedMove.effectId == 3 || gBattleUser->cachedMove.effectId == 8 ||
            ((D_8438AC60[0] == 1) && (Battle_ValueInList(gBattleUser->cachedMove.effectId, D_8438ACB0, 5) != 0))) {
            Battle_FailMove();
            return;
        }
    }

    if (gBattleTarget->lockedEffectFlags & 0x40) {
        if (gBattleUser->cachedMove.effectId != 0x11) {
            Battle_FailMove();
        }
    } else {
        if ((((gBattleUser->cachedMove.effectId >= 0x12) && (gBattleUser->cachedMove.effectId < 0x1A)) ||
             ((gBattleUser->cachedMove.effectId >= 0x3A) && (gBattleUser->cachedMove.effectId < 0x42))) &&
            (gBattleTarget->volatileStatusFlags & 2)) {
            Battle_FailMove();
            return;
        }

        if (!(gBattleUser->volatileStatusFlags & 1)) {
            if ((D_8438AC60[0] != 1) || (gBattleUser->cachedMove.effectId != 0x11)) {
                Battle_ApplyAccuracyStatStages();
            }

            D_843C4DA2 = Battle_RandomAccuracy();
            if (D_843C4DA2 == 0xFF) {
                D_843C4DA2 = Battle_RandomAccuracy();
            }

            if (D_843C4DA2 >= gBattleUser->cachedMove.accuracy) {
                Battle_FailMove();
            }
        }
    }
}

void Battle_WriteBackRuntimeState(Battler* arg0) {
    BattleMon* ptr = &arg0->ownRoster->party[arg0->unk_654.partyIndex];

    ptr->status = arg0->unk_654.monRuntime.status;
}

void Battle_ApplyParalysisSpeedPenalty(Battler* arg0) {
    BattleMonRuntime* temp_v0 = &arg0->unk_654.monRuntime;

    if (arg0->unk_654.monRuntime.status & 0x40) {
        temp_v0->speed >>= 2;
        if (temp_v0->speed == 0) {
            temp_v0->speed = 1;
        }
    }
}

void Battle_ApplyBurnAttackPenalty(Battler* arg0) {
    BattleMonRuntime* temp_v0 = &arg0->unk_654.monRuntime;

    if (arg0->unk_654.monRuntime.status & 0x10) {
        temp_v0->attack >>= 1;
        if (temp_v0->attack == 0) {
            temp_v0->attack = 1;
        }
    }
}

#ifdef NON_MATCHING
void Battle_InitializeRuntimeMon(Battler* arg0) {
    BattlerState* sp28;
    BattleMonRuntime* sp24;
    BattleMon* sp34;
    s32 i;
    s32 j;

    sp28 = &arg0->unk_654;
    sp24 = &arg0->unk_654.monRuntime;
    sp34 = &arg0->sessionTeams->teams[sp28->sideIndex]->party[sp28->selectedPartyIndex];

    sp24->partySlotIndex = sp28->partyIndex;
    sp24->speciesId = sp34->species.dexId;
    sp24->currentHP = sp34->currentHP;
    sp24->status = sp34->status;
    sp24->unk_16[6] = sp34->type1;
    sp24->unk_16[7] = sp34->type2;
    sp24->unk_16[8] = sp34->catchRate;

    sp24->moveIds[0] = sp34->moves[0];
    sp24->moveIds[1] = sp34->moves[1];
    sp24->moveIds[2] = sp34->moves[2];
    sp24->moveIds[3] = sp34->moves[3];
    sp24->dvs = sp34->dvs;

    sp24->currentPP[0] = sp34->pp[0];
    sp24->currentPP[1] = sp34->pp[1];
    sp24->currentPP[2] = sp34->pp[2];
    sp24->currentPP[3] = sp34->pp[3];

    if ((sp34->moves[0] == 0) || (sp34->moves[0] >= 0xA6)) {
        sp24->currentPP[0] = 0;
    }

    if ((sp34->moves[1] == 0) || (sp34->moves[1] >= 0xA6)) {
        sp24->currentPP[1] = 0;
    }

    if ((sp34->moves[2] == 0) || (sp34->moves[2] >= 0xA6)) {
        sp24->currentPP[2] = 0;
    }

    if ((sp34->moves[3] == 0) || (sp34->moves[3] >= 0xA6)) {
        sp24->currentPP[3] = 0;
    }

    for (i = 0; i < 4; i++) {
        if (sp34->moves[i] == 0) {
            for (j = i; j < 4; j++) {
                sp34->moves[j] = sp24->currentPP[j] = 0;
                sp24->moveIds[j] = 0;
            }
            break;
        }
    }

    sp24->level = sp34->level;
    sp24->maxHP = sp34->maxHP;
    sp24->attack = sp34->attack;
    sp24->defense = sp34->defense;
    sp24->speed = sp34->speed;
    sp24->special = sp34->special;

    Battle_ApplyParalysisSpeedPenalty(arg0);
    Battle_ApplyBurnAttackPenalty(arg0);

    sp24->baseHP = gBaseStats[sp24->speciesId].baseHP;
    sp24->origAttack = sp34->attack;
    sp24->origDefense = sp34->defense;
    sp24->origSpeed = sp34->speed;
    sp24->origSpecial = sp34->special;

    // clang-format off
    for (i = 0; i < 8; i++) { sp24->statStages[i] = 7; }
    // clang-format on
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/62/fragment62_359F90/Battle_InitializeRuntimeMon.s")
#endif

s32 Battle_IsPartyMemberAlive(Battler* arg0) {
    if (arg0->ownRoster->party[arg0->unk_654.selectedPartyIndex - 1].currentHP != 0) {
        return 1;
    }
    return 0;
}

void func_84370E70(void) {
}

void func_84370E78(void) {
}

void Battle_LoadMoveData(void) {
    gBattleUser->cachedMove.moveId = gMoveData[gBattleUser->currentMoveId - 1].moveId;
    gBattleUser->cachedMove.effectId = gMoveData[gBattleUser->currentMoveId - 1].effectId;
    gBattleUser->cachedMove.power = gMoveData[gBattleUser->currentMoveId - 1].power;
    gBattleUser->cachedMove.type = gMoveData[gBattleUser->currentMoveId - 1].type;
    gBattleUser->cachedMove.accuracy = gMoveData[gBattleUser->currentMoveId - 1].accuracy;
    gBattleUser->cachedMove.basePP = gMoveData[gBattleUser->currentMoveId - 1].basePP;
}

void Battle_ApplyRageContinuation(void) {
    BattleMonRuntime* sp24;

    if ((gBattleTarget->volatileStatusFlags & 0x40) && ((gBattleTarget->statStages[0]) != 0xD)) {
        sp24 = gBattleUser;

        gBattleUser = gBattleTarget;
        gBattleUser->cachedMove.moveId = 0;
        gBattleUser->cachedMove.effectId = 10;

        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_348, 0x1F);
        Battle_ApplyRageStatBoost();

        gBattleUser->cachedMove.moveId = 0x63;
        gBattleUser->cachedMove.effectId = 0;

        gBattleUser = sp24;
    }
}

void Battle_QueueTypeEffectivenessMessage(void) {
    D_843C4DEC = D_843C4DB3 & 0x7F;
    if (D_843C4DEC >= 0xB) {
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 0x29);
    } else if (D_843C4DEC < 0xA) {
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 0x2A);
    }
}

void Battle_RecordCriticalHitAndQueueMessage(void) {
    BattleMonRuntime* sp1C = &D_84390010[!gBattleScene.scene->activeBattlerIndex]->unk_654.monRuntime;

    switch (gBattleCritFlag) {
        case 1:
            Battle_QueueMessage(gBattleMessageQueues->unk_148, 0x1D);
            gBattleScene.scene->unk_1A = 2;
            break;
        case 2:
            Battle_QueueMessage(gBattleMessageQueues->unk_148, 0x1E);
            gBattleScene.scene->unk_1A = 2;
            break;
    }
    sp1C->lastCritFlag = gBattleCritFlag;
    gBattleCritFlag = 0;
}

void Battle_ApplySpecialMoveDamage(void) {
    s32 var_s0;
    s32 temp_v0;

    switch (gBattleUser->cachedMove.effectId) {
        case 38:
            break;

        case 40:
            gBattleDamage = gBattleTarget->currentHP / 2;
            if (gBattleDamage == 0) {
                gBattleDamage = 1;
            }
            break;

        case 41:
            switch (gBattleUser->cachedMove.moveId) {
                case 0x45:
                case 0x65:
                    var_s0 = gBattleUser->level;
                    break;

                case 0x31:
                    var_s0 = 0x14;
                    break;

                case 0x52:
                    var_s0 = 0x28;
                    break;

                default:
                    var_s0 = gBattleUser->level;
                    var_s0 = (var_s0 >> 1) + var_s0;
                    do {
                        do {
                            temp_v0 = Battle_Random();
                        } while (temp_v0 == 0);
                    } while (temp_v0 >= var_s0);
                    var_s0 = temp_v0;
            }
            gBattleDamage = var_s0;
            break;

        default:
            if (gBattleUser->cachedMove.power != 0) {
                if (gBattleUser->lockedEffectFlags & 4) {
                    D_843C4DA0 += gBattleDamage;
                } else {
                    D_843C4DA0 = gBattleDamage;
                }
                break;
            }
            return;
    }

    if (gBattleDamage != 0) {
        Battle_ApplyDamage();
    }
}

void Battle_QueueMoveFailureAndApplyRecoil(void) {
    BattleMonRuntime* sp1C;

    if (D_843C4DB3 & 0x7F) {
        if (gBattleCritFlag == 0xFF) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_088, 0x19);
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_088, 0x1A);
        }
    } else {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 0x1B);
    }

    gBattleCritFlag = 0;

    if (gBattleUser->cachedMove.effectId == 0x2D) {
        gBattleScene.scene->unk_48 = 0x10;
        gBattleDamage >>= 3;
        if (gBattleDamage == 0) {
            gBattleDamage++;
        }

        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 0x1C);

        sp1C = gBattleUser;
        gBattleUser = gBattleTarget;
        gBattleTarget = sp1C;
        gBattleScene.scene->activeBattlerIndex = !gBattleScene.scene->activeBattlerIndex;
        Battle_ApplyDamage();
        gBattleScene.scene->activeBattlerIndex = !gBattleScene.scene->activeBattlerIndex;
        gBattleTarget = gBattleUser;
        gBattleUser = sp1C;
    }
}

void Battle_RestoreMovePP(void) {
    BattleMonRuntime* ptr;
    BattleMon* temp_a0;
    BattlerState* temp_a1;

    ptr = &D_84390010[gBattleScene.scene->activeBattlerIndex]->unk_654.monRuntime;
    temp_a0 =
        &D_84390010[gBattleScene.scene->activeBattlerIndex]->ownRoster->party[D_84390010[gBattleScene.scene->activeBattlerIndex]->unk_654.partyIndex];
    temp_a1 = &D_84390010[gBattleScene.scene->activeBattlerIndex]->unk_654;

    gBattleUser->currentPP[temp_a1->committedMoveSlot]++;
    if (!(ptr->auxStatusFlags & 8)) {
        temp_a0->pp[temp_a1->committedMoveSlot]++;
    }
}

s32 Battle_Effect_Metronome(void) {
    s32 temp_v0;

    Battle_RestoreMovePP();
    do {
        do {
            temp_v0 = Battle_Random();
        } while (temp_v0 == 0 || temp_v0 >= 0xA5);
    } while (temp_v0 == 0x76);
    gBattleUser->currentMoveId = temp_v0;
    Battle_LoadMoveData();
    return 1;
}

void Battle_ApplyPoisonOrBurnDamage(void) {
    u16 var_a0;

    if (gBattleUser->recoilPendingHP != 0xFFFF) {
        gBattleUser->poisonBurnPendingHP = gBattleUser->recoilPendingHP;
    } else {
        gBattleUser->poisonBurnPendingHP = gBattleUser->currentHP;
    }

    var_a0 = (gBattleUser->maxHP >> 4) & 0xFFFF;
    if (var_a0 == 0) {
        var_a0++;
    }

    if (gBattleUser->auxStatusFlags & 1) {
        gBattleUser->toxicCounter++;
        var_a0 *= gBattleUser->toxicCounter;
    }

    if (gBattleUser->poisonBurnPendingHP < var_a0) {
        gBattleUser->poisonBurnPendingHP = 0;
    } else {
        gBattleUser->poisonBurnPendingHP -= var_a0;
    }

    gBattleUser->leechSeedPendingHP = gBattleUser->poisonBurnPendingHP;
}

void Battle_ApplyLeechSeedDamage(void) {
    u16 var_a0;

    if (!(gBattleUser->status & 0x18)) {
        if (gBattleUser->recoilPendingHP != 0xFFFF) {
            gBattleUser->leechSeedPendingHP = gBattleUser->recoilPendingHP;
        } else {
            gBattleUser->leechSeedPendingHP = gBattleUser->currentHP;
        }
    }

    var_a0 = (gBattleUser->maxHP >> 4) & 0xFFFF;
    if (var_a0 == 0) {
        var_a0++;
    }

    if (gBattleUser->auxStatusFlags & 1) {
        gBattleUser->toxicCounter++;
        var_a0 *= gBattleUser->toxicCounter;
    }

    if (gBattleUser->leechSeedPendingHP < var_a0) {
        var_a0 = gBattleUser->leechSeedPendingHP;
        gBattleUser->leechSeedPendingHP = 0;
    } else {
        gBattleUser->leechSeedPendingHP -= var_a0;
    }

    gBattleTarget->leechSeedPendingHP = gBattleTarget->currentHP;
    gBattleTarget->leechSeedPendingHP += var_a0;

    if (gBattleTarget->maxHP < gBattleTarget->leechSeedPendingHP) {
        gBattleTarget->leechSeedPendingHP = gBattleTarget->maxHP;
    }
}

s32 Battle_ApplyResidualEffects(void) {
    if (gBattleTarget->currentHP == 0) {
        return 0;
    }

    if (gBattleUser->status & 0x18) {
        if (gBattleUser->status & 8) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_248, 0);
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_248, 1);
        }
        Battle_ApplyPoisonOrBurnDamage();
        gBattleTarget->poisonBurnPendingHP = gBattleTarget->currentHP;
    }

    if (gBattleUser->volatileStatusFlags & 0x80) {
        Battle_ApplyLeechSeedDamage();
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_288, 2);
    }

    if (gBattleUser->currentHP != 0) {
        return 0;
    }
    return 1;
}

void func_843718DC(void) {
}

s32 Battle_TryLoadFollowupMoveFromTarget(void) {
    if ((gBattleTarget->lastMoveUsedId == 0x77) || (gBattleTarget->lastMoveUsedId == 0)) {
        gBattleMessageQueues->unk_388[0] = 0;
        gBattleMessageQueues->unk_388[1] = 0;
        gBattleScene.scene->unk_48 = 0x1C;
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 0x20);
        return 0;
    }

    gBattleUser->currentMoveId = gBattleTarget->lastMoveUsedId;
    Battle_RestoreMovePP();
    Battle_LoadMoveData();
    return 1;
}

void Battle_ApplyTypeEffectAndSTAB(void) {
    u8 var_v0;
    u8 var_v1;
    u8 temp_a0;
    u8 temp_a1;
    u8 temp_a3;
    u8 temp_t1;
    TypeEffectivenessEntry* var_a3;

    var_v0 = 3;

    temp_a1 = gBattleUser->unk_16[6];
    temp_a3 = gBattleUser->unk_16[7];

    var_v1 = gBattleTarget->unk_16[6];
    temp_a0 = gBattleTarget->unk_16[7];

    temp_t1 = gBattleUser->cachedMove.type;

    if ((D_8438AC60[0] != 1) || (gBattleUser->cachedMove.moveId != 0xA5)) {
        if ((temp_t1 == temp_a1) || (temp_t1 == temp_a3)) {
            gBattleDamage += gBattleDamage >> 1;
            D_843C4DB3 |= 0x80;
        }

        var_a3 = &gTypeEffects[0];
        while (var_a3->unk_00 != 0xFF) {
            if ((var_a3->unk_00 == temp_t1) && ((var_a3->unk_01 == var_v1) || (var_a3->unk_01 == temp_a0))) {
                if (var_a3->unk_02 != 0) {
                    if (var_a3->unk_02 >= 0xB) {
                        var_v0++;
                    } else {
                        var_v0--;
                    }
                }

                gBattleDamage = (gBattleDamage * var_a3->unk_02) / 10;

                if (gBattleDamage == 0) {
                    gBattleMoveFailed = 1;
                }
            }
            var_a3++;
        }

        switch (var_v0) {
            case 1:
            case 2:
                D_843C4DB3 = (D_843C4DB3 & 0x80) + 5;
                break;

            case 3:
                break;

            case 4:
            case 5:
                D_843C4DB3 = (D_843C4DB3 & 0x80) + 0x14;
                break;
        }
    }
}

s32 Battle_Effect_Counter(void) {
    if (gBattleUser->currentMoveId != 0x44) {
        return 1;
    }

    gBattleMoveFailed = 1;

    if (gBattleTarget->currentMoveId == 0x44) {
        return 0;
    }

    if (gBattleTarget->cachedMove.power == 0) {
        return 0;
    }

    if ((gBattleTarget->cachedMove.type == 0) || (gBattleTarget->cachedMove.type == 1)) {
        if (gBattleDamage == 0) {
            return 0;
        }

        if (gBattleDamage >= 0x8000) {
            gBattleDamage = 0xFFFF;
        } else {
            gBattleDamage *= 2;
        }

        gBattleMoveFailed = 0;
        gBattleCritFlag = 0;
        Battle_CheckMoveFailureConditions();
        return 0;
    }
    return 0;
}

s32 Battle_CheckCriticalHit(void) {
    UNUSED s32 pad;
    s32 sp18;

    gBattleCritFlag = 0;
    if (D_8438AC60[0] == 1) {
        sp18 = (gBaseStats[gBattleUser->speciesId].baseSpeed + 0x4C) >> 2;
    } else {
        sp18 = gBaseStats[gBattleUser->speciesId].baseSpeed >> 1;
    }

    if (gBattleUser->cachedMove.power == 0) {
        return 0;
    }

    if (D_8438AC60[0] == 1) {
        if (gBattleUser->volatileStatusFlags & 4) {
            sp18 = (sp18 * 4) + 0xA0;
        } else {
            sp18 *= 2;
        }

        if (Battle_ValueInList(gBattleUser->cachedMove.moveId, gHighCritMoves, 4) != 0) {
            sp18 *= 4;
        } else {
            sp18 >>= 1;
        }
    } else {
        if (gBattleUser->volatileStatusFlags & 4) {
            sp18 >>= 1;
        } else {
            sp18 *= 2;
            if (sp18 >= 0x100) {
                sp18 = 0xFF;
            }
        }

        if (Battle_ValueInList(gBattleUser->cachedMove.moveId, gHighCritMoves, 4) != 0) {
            sp18 *= 4;
            if (sp18 >= 0x100) {
                sp18 = 0xFF;
            }
        } else {
            sp18 >>= 1;
        }
    }

    if (sp18 >= 0x100) {
        sp18 = 0xFF;
    }

    if (sp18 <= 0) {
        sp18 = 1;
    }

    D_843C4DA2 = Battle_Random();
    if (D_843C4DA2 >= sp18) {
        return 0;
    }
    gBattleCritFlag = 1;
    return 1;
}

s32 Battle_CheckMonCanAct(void) {
    BattleMonRuntime* sp2C;
    BattlerState* temp_v0;
    u16 sp26;
    u8 sp25;
    BattleMonRuntime* temp_v1;

    temp_v0 = &D_84390010[gBattleScene.scene->activeBattlerIndex]->unk_654;
    temp_v1 = &D_84390010[gBattleScene.scene->activeBattlerIndex]->unk_654.monRuntime;

    if (gBattleUser->status & 7) {
        gBattleUser->status = (gBattleUser->status & 7) - 1;
        if (temp_v1->unk_16[temp_v0->partyIndex + temp_v0->activeSideIndex * 3] != 0) {
            temp_v1->unk_16[temp_v0->partyIndex + temp_v0->activeSideIndex * 3]--;
        }

        if (gBattleUser->status != 0) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_088, 5);
            gBattleScene.scene->unk_48 = 0x11;
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_088, 6);
            gBattleScene.scene->unk_48 = 0x11;
        }

        gBattleUser->lastMoveUsedId = 0;
        return 0;
    }

    if (gBattleUser->status & 0x20) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 7);
        gBattleUser->lastMoveUsedId = 0;
        gBattleScene.scene->unk_48 = 0x11;
        return 0;
    }

    if (gBattleTarget->lockedEffectFlags & 0x20) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 8);
        gBattleScene.scene->unk_48 = 0x11;
        return 0;
    }

    if (gBattleUser->lockedEffectFlags & 8) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 9);
        gBattleUser->volatileStatusFlags &= 0xFFDF;
        gBattleUser->lockedEffectFlags &= 0xFFF7;
        gBattleScene.scene->unk_48 = 0x1C;
        return 0;
    }

    if (gBattleUser->volatileStatusFlags & 0x20) {
        gBattleUser->volatileStatusFlags &= ~0x20;
        if (D_8438AC60[0] == 1) {
            gBattleDamage = 0;
        }
        gBattleScene.scene->unk_48 = 0x11;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 0xA);
        return 0;
    }

    if (gBattleUser->disabledSlotAndTurns != 0) {
        gBattleUser->disabledSlotAndTurns--;
        if (!(gBattleUser->disabledSlotAndTurns & 0xF)) {
            gBattleUser->disabledSlotAndTurns = 0;
            gBattleUser->disabledMoveId = 0;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_388, 0xB);
        }
    }

    if (gBattleUser->lockedEffectFlags & 0x80) {
        gBattleUser->confusionCounter--;
        if (gBattleUser->confusionCounter == 0) {
            gBattleUser->lockedEffectFlags &= 0xFF7F;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_188, 0xC);
        } else {
            Battle_QueueMoveCategoryMessage();
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_188, 0xD);
            D_843C4DA2 = Battle_Random();
            if (D_843C4DA2 >= 0x80) {
                Battle_QueueMetronomeMoveMessage();
                gBattleScene.scene->unk_48 = 0x14;
                gBattleUser->lockedEffectFlags &= 0x80;
                Battle_QueueMessage(gBattleMessageQueues->unk_148, 0xE);

                sp26 = gBattleTarget->defense;
                gBattleTarget->defense = gBattleUser->defense;
                sp25 = gBattleUser->cachedMove.effectId;
                gBattleUser->cachedMove.effectId = 0;
                gBattleCritFlag = 0;
                gBattleUser->cachedMove.power = 0x28;
                gBattleUser->cachedMove.type = 0;
                Battle_CalculateDamage();
                Battle_CalcDamage();
                gBattleUser->cachedMove.effectId = sp25;
                gBattleTarget->defense = sp26;

                sp2C = gBattleUser;
                gBattleUser = gBattleTarget;
                gBattleTarget = sp2C;
                Battle_ApplyDamage();
                gBattleTarget = gBattleUser;
                gBattleUser = sp2C;
                goto label1;
            }
        }
    }

    if ((gBattleUser->disabledMoveId != 0) && (gBattleUser->disabledMoveId == gBattleUser->currentMoveId)) {
        gBattleUser->lockedEffectFlags &= 0xFFEF;
        Text_SetStringToken(0x1D, gMoveDisplayInfo[gBattleUser->currentMoveId - 1].unk_04);
        Battle_QueueMessage(gBattleMessageQueues->unk_088, 0xF);
        gBattleScene.scene->unk_48 = 0x11;
        return 0;
    }

    if (gBattleUser->status & 0x40) {
        D_843C4DA2 = Battle_Random();
        if (D_843C4DA2 < 0x3F) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_088, 0x10);
            gBattleScene.scene->unk_48 = 0x1F;
            gBattleDamage = 0;
        label1:
            gBattleUser->lockedEffectFlags &= 0xFF8C;
            return 0;
        }
    }

    if (gBattleUser->lockedEffectFlags & 1) {
        gBattleUser->cachedMove.moveId = 0;
        gBattleUser->effectAccumulator += gBattleDamage;
        gBattleUser->lockedEffectCounter -= 1;
        if (gBattleUser->lockedEffectCounter != 0) {
            gBattleScene.scene->unk_48 = 0x1B;
            return 0;
        }
        gBattleUser->lockedEffectFlags &= 0xFFFE;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_0C8, 0x11);
        gBattleUser->cachedMove.power = 1;

        gBattleDamage = gBattleUser->effectAccumulator * 2;
        if (!gBattleDamage) {
            gBattleMoveFailed = 1;
        }

        if ((D_8438AC60[0] == 1) && (gBattleTarget->lockedEffectFlags & 0x40)) {
            gBattleMoveFailed = 1;
        }

        gBattleUser->effectAccumulator = 0;
        gBattleUser->cachedMove.moveId = 0x75;
        return 5;
    }

    if (gBattleUser->lockedEffectFlags & 2) {
        gBattleUser->cachedMove.moveId = 0x25;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_0C8, 0x12);
        gBattleUser->lockedEffectCounter -= 1;
        if (gBattleUser->lockedEffectCounter != 0) {
            return 4;
        }

        Battle_QueueMessage(gBattleMessageQueues->unk_188, -0x45);
        gBattleUser->lockedEffectFlags &= 0xFFFD;
        gBattleUser->lockedEffectFlags |= 0x80;
        gBattleUser->confusionCounter = (Battle_Random() & 3) + 2;
        return 4;
    }

    if (gBattleUser->lockedEffectFlags & 0x20) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_0C8, 0x13);
        gBattleUser->lockedEffectCounter -= 1;
        return 6;
    }

    if (gBattleUser->volatileStatusFlags & 0x40) {
        gBattleUser->cachedMove.effectId = 0;
        return 3;
    }
    return 2;
}

#ifdef NON_MATCHING
void Battle_ExecuteAttackerTurn(void) {
    char sp2C[8];

    if (gBattleUser->currentMoveId != 0xFF) {
        gBattleMoveFailed = 0;
        D_843C4DB5 = 0;
        D_843C4DA9 = 0;
        D_843C4DB3 = 0xA;

        switch (Battle_CheckMonCanAct()) {
            case 2:
                Battle_LoadMoveData();
                if (gBattleUser->lockedEffectFlags & 0x10) {
                    gBattleUser->lockedEffectFlags &= ~0x50;
                    Battle_SetRuntimeFlags(D_84390010[gBattleScene.scene->activeBattlerIndex], 8);
                } else {
                block_5:
                    switch (gBattleUser->cachedMove.effectId) {
                        case 0x27:
                        case 0x2B:
                            if (D_8438AC60[0] == 1) {
                                gBattleDamage = 0;
                            }
                            Battle_ExecuteMoveEffect();
                            gBattleUser->lastMoveUsedId = gBattleUser->currentMoveId;
                            return;
                    }
                }

            case 3:
                Battle_QueueMoveCategoryUsedMessage();
                Battle_DecrementPP(gBattleScene.scene->activeBattlerIndex);

                if (Battle_ValueInList(gBattleUser->cachedMove.effectId, D_8438ADEC, 0x10) != 0) {
                    if (D_8438AC60[0] == 1) {
                        gBattleDamage = 0;
                    }
                    Battle_ExecuteMoveEffect();
                    return;
                }

                if (Battle_ValueInList(gBattleUser->cachedMove.effectId, D_8438ADFC, 1) != 0) {
                    Battle_ExecuteMoveEffect();
                }

            case 4:
                if (Battle_ValueInList(gBattleUser->cachedMove.effectId, D_8438AE00, 2) == 0) {
                    Battle_CheckCriticalHit();
                    if (Battle_Effect_Counter() == 0) {
                        goto case_5;
                    }

                    Battle_CalculateDamage();
                    if (Battle_CalcDamage() == 0) {
                        goto case_6;
                    }

                    Battle_ApplyTypeEffectAndSTAB();
                    Battle_ApplyDamageRandomFactor();
                }

                if (D_8438AC60[0] == 1) {
                    gBattleUser->cachedMove.accuracy = gMoveData[gBattleUser->currentMoveId - 1].accuracy;
                }
                Battle_CheckMoveFailureConditions();

            case 5:
            case_5:
                if (gBattleMoveFailed != 0) {
                    switch (gBattleUser->cachedMove.effectId) {
                        case 7:
                            break;
                    }
                    goto case_6;
                }

            case 6:
            case_6:
                switch (gBattleUser->cachedMove.effectId) {
                    case 9:
                        if (Battle_TryLoadFollowupMoveFromTarget() == 0) {
                            goto end;
                        }
                        D_843C4DB5 = 0;
                        goto block_5;

                    case 0x53:
                        Battle_Effect_Metronome();
                        goto block_5;
                }

                if (Battle_ValueInList(gBattleUser->cachedMove.effectId, D_8438AE04, 0x1B) != 0) {
                    Battle_ExecuteMoveEffect();
                    return;
                }

                if (gBattleMoveFailed != 0) {
                    Battle_QueueMoveFailureAndApplyRecoil();

                    if (gBattleUser->cachedMove.effectId != 7) {
                        goto end;
                    }
                } else {
                    Battle_ApplySpecialMoveDamage();
                    Battle_RecordCriticalHitAndQueueMessage();
                    Battle_QueueTypeEffectivenessMessage();
                    D_843C4DA9 = 1;
                }

                if (Battle_ValueInList(gBattleUser->cachedMove.effectId, D_8438AE20, 0xA) != 0) {
                    Battle_ExecuteMoveEffect();
                }

                if (gBattleTarget->currentHP == 0) {
                    if (D_8438AC60[0] == 1) {
                        switch (gBattleUser->cachedMove.effectId) {
                            case 0x50:
                                gBattleDamage = 0;
                                return;
                        }
                    }
                    return;
                }

                Battle_ApplyRageContinuation();

                if (gBattleUser->lockedEffectFlags & 4) {
                    gBattleUser->lockedEffectCounter -= 1;
                    if (gBattleUser->lockedEffectCounter != 0) {
                        goto case_6;
                    }
                    gBattleUser->lockedEffectFlags &= ~4;
                    sprintf(sp2C, "%d", gBattleUser->effectAccumulator);
                    Text_SetNumberToken(2, gBattleUser->effectAccumulator);
                    Battle_QueueMessage(gBattleMessageQueues->unk_3C8, 4);
                    gBattleScene.scene->unk_1A = 1;
                    gBattleUser->effectAccumulator = 0;
                    gBattleDamage = D_843C4DA0;
                }

                if (gBattleUser->cachedMove.effectId == 0) {
                    goto end;
                }

                if (Battle_ValueInList(gBattleUser->cachedMove.effectId, D_8438AE2C, 0xF) != 0) {
                    goto end;
                }

                Battle_ExecuteMoveEffect();
                goto end;
        }
    }
end:
    D_843C4DB7 = 0;
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/62/fragment62_359F90/Battle_ExecuteAttackerTurn.s")
#endif

void Battle_HandleFaint(Battler* arg0) {
    s32 sp1C = BattleScene_GetParticipantSideIndex(arg0);
    BattlerState* temp_a10;
    BattleMonRuntime* temp_a11;
    BattleMonRuntime* temp_a01;

    temp_a10 = &D_84390010[sp1C]->unk_654;
    temp_a11 = &D_84390010[sp1C]->unk_654.monRuntime;
    temp_a01 = &D_84390010[!sp1C]->unk_654.monRuntime;

    temp_a01->effectAccumulator = 0;
    temp_a01->lastMoveUsedId = 0;
    temp_a01->lockedEffectFlags &= ~4;

    temp_a11->unk_4A = 0;
    temp_a11->unk_4B = 0;
    temp_a11->lockedEffectFlags = 0;
    temp_a11->volatileStatusFlags &= 0x80;
    temp_a11->auxStatusFlags = 0;
    temp_a11->disabledSlotAndTurns = 0;
    temp_a11->disabledMoveId = 0;
    temp_a11->statChangeFlag = 0;
    temp_a11->lastMoveUsedId = 0;

    if (temp_a10->faintSequenceState != 0xF) {
        temp_a10->faintSequenceState = 0x11;
    }

    Battle_QueueDamageSummaryMessages(arg0);
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[sp1C]));
    Battle_QueueMessage(gBattleMessageQueues->unk_448[sp1C], 3);
}

void Battle_CheckSideDefeatAfterPoisonOrBurn(Battler* arg0) {
    BattlerState* ptr1 = &arg0->unk_654;
    BattleMon* temp_v0;
    s32 i;
    s32 var_s2 = 0;

    for (i = 0; i < BattleScene_GetSelectedSideTeamSize(arg0); i++) {
        if (i == ptr1->partyIndex) {
            var_s2 += arg0->unk_654.monRuntime.poisonBurnPendingHP;
        } else {
            temp_v0 = &arg0->ownRoster->party[i];
            var_s2 += temp_v0->currentHP;
        }
    }

    if (arg0->sessionTeams->playerCount == 2) {
        for (i = 0; i < BattleScene_GetOpposingSelectedSideTeamSize(arg0); i++) {
            temp_v0 = &arg0->sessionTeams->teams[!ptr1->activeSideIndex]->party[i];
            var_s2 += temp_v0->currentHP;
        }
    }

    if (var_s2 == 0) {
        Battle_QueueDamageSummaryMessages(arg0);
        ptr1->faintSequenceState = 0x12;
    } else {
        Battle_HandleFaint(arg0);
    }
}

void Battle_CheckSideDefeatAfterLeechSeed(Battler* arg0) {
    BattlerState* ptr1 = &arg0->unk_654;
    BattleMon* temp_v0;
    s32 i;
    s32 var_s2 = 0;

    for (i = 0; i < BattleScene_GetSelectedSideTeamSize(arg0); i++) {
        if (i == ptr1->partyIndex) {
            var_s2 += arg0->unk_654.monRuntime.leechSeedPendingHP;
        } else {
            temp_v0 = &arg0->ownRoster->party[i];
            var_s2 += temp_v0->currentHP;
        }
    }

    if (arg0->sessionTeams->playerCount == 2) {
        for (i = 0; i < BattleScene_GetOpposingSelectedSideTeamSize(arg0); i++) {
            temp_v0 = &arg0->sessionTeams->teams[!ptr1->activeSideIndex]->party[i];
            var_s2 += temp_v0->currentHP;
        }
    }

    if (var_s2 == 0) {
        Battle_QueueDamageSummaryMessages(arg0);
        ptr1->faintSequenceState = 0x12;
    } else {
        Battle_HandleFaint(arg0);
    }
}

void Battle_CheckSideDefeatAfterRecoil(Battler* arg0) {
    BattlerState* ptr1 = &arg0->unk_654;
    BattleMon* temp_v0;
    s32 i;
    s32 var_s2 = 0;

    for (i = 0; i < BattleScene_GetSelectedSideTeamSize(arg0); i++) {
        if (i == ptr1->partyIndex) {
            var_s2 += arg0->unk_654.monRuntime.recoilPendingHP;
        } else {
            temp_v0 = &arg0->ownRoster->party[i];
            var_s2 += temp_v0->currentHP;
        }
    }

    if (arg0->sessionTeams->playerCount == 2) {
        for (i = 0; i < BattleScene_GetOpposingSelectedSideTeamSize(arg0); i++) {
            temp_v0 = &arg0->sessionTeams->teams[!ptr1->activeSideIndex]->party[i];
            var_s2 += temp_v0->currentHP;
        }
    }

    if (var_s2 == 0) {
        Battle_QueueDamageSummaryMessages(arg0);
        ptr1->faintSequenceState = 0x12;
    } else {
        Battle_HandleFaint(arg0);
    }
}

void Battle_CheckSideDefeatAfterDamage(Battler* arg0) {
    BattlerState* ptr1 = &arg0->unk_654;
    BattleMon* temp_v0;
    s32 i;
    s32 var_s2 = 0;

    for (i = 0; i < BattleScene_GetSelectedSideTeamSize(arg0); i++) {
        if (i == ptr1->partyIndex) {
            var_s2 += arg0->unk_654.monRuntime.currentHP;
        } else {
            temp_v0 = &arg0->ownRoster->party[i];
            var_s2 += temp_v0->currentHP;
        }
    }

    if (arg0->sessionTeams->playerCount == 2) {
        for (i = 0; i < BattleScene_GetOpposingSelectedSideTeamSize(arg0); i++) {
            temp_v0 = &arg0->sessionTeams->teams[!ptr1->activeSideIndex]->party[i];
            var_s2 += temp_v0->currentHP;
        }
    }

    if (var_s2 == 0) {
        Battle_QueueDamageSummaryMessages(arg0);
        ptr1->faintSequenceState = 0x12;
    } else {
        Battle_HandleFaint(arg0);
    }
}

s32 Battle_CheckSideDefeatAfterResidualStatus(Battler* arg0) {
    BattlerState* ptr1 = &arg0->unk_654;
    BattleMon* temp_v0;
    s32 i;
    s32 var_s2;

    if (arg0->unk_654.monRuntime.status & 0x18) {
        var_s2 = 0;
        if (arg0->unk_654.monRuntime.poisonBurnPendingHP == 0) {
            for (i = 0; i < BattleScene_GetSelectedSideTeamSize(arg0); i++) {
                temp_v0 = &arg0->ownRoster->party[i];
                var_s2 += temp_v0->currentHP;
            }

            if (arg0->sessionTeams->playerCount == 2) {
                for (i = 0; i < BattleScene_GetOpposingSelectedSideTeamSize(arg0); i++) {
                    temp_v0 = &arg0->sessionTeams->teams[!ptr1->activeSideIndex]->party[i];
                    var_s2 += temp_v0->currentHP;
                }
            }

            if (var_s2 == 0) {
                ptr1->faintSequenceState = 0x12;
                return 1;
            }
            Battle_HandleFaint(arg0);
        }
        return 1;
    }
    return 0;
}

s32 Battle_CheckFaintAfterMove(void) {
    if (gBattleTarget->currentHP == 0) {
        Battle_CheckSideDefeatAfterDamage(D_84390010[!gBattleScene.scene->activeBattlerIndex]);
    }

    if (gBattleUser->currentHP == 0) {
        Battle_CheckSideDefeatAfterDamage(D_84390010[gBattleScene.scene->activeBattlerIndex]);
    }

    if ((gBattleUser->currentHP != 0) && (gBattleTarget->currentHP != 0) && (gBattleUser->recoilPendingHP == 0)) {
        Battle_CheckSideDefeatAfterRecoil(D_84390010[gBattleScene.scene->activeBattlerIndex]);
        return 0;
    } else if ((gBattleUser->currentHP != 0) && (gBattleTarget->currentHP != 0) && (gBattleUser->status & 0x18) &&
               (gBattleUser->poisonBurnPendingHP == 0)) {
        Battle_CheckSideDefeatAfterPoisonOrBurn(D_84390010[gBattleScene.scene->activeBattlerIndex]);
        return 0;
    } else if ((gBattleUser->currentHP != 0) && (gBattleTarget->currentHP != 0) && (gBattleUser->volatileStatusFlags & 0x80) &&
               (gBattleUser->leechSeedPendingHP == 0)) {
        Battle_CheckSideDefeatAfterLeechSeed(D_84390010[gBattleScene.scene->activeBattlerIndex]);
        return 0;
    }
}

void Battle_ResolveMonAction(void) {
    BattleMonRuntime* sp1C = &D_84390010[gBattleScene.scene->activeBattlerIndex]->unk_654.monRuntime;
    BattleMonRuntime* sp18 = &D_84390010[!gBattleScene.scene->activeBattlerIndex]->unk_654.monRuntime;
    BattlerState* ptr1 = &D_84390010[0]->unk_654;
    BattlerState* ptr2 = &D_84390010[1]->unk_654;

    ptr1->unk_2A = 0;
    ptr2->unk_2A = 0;

    D_843C4E44 = 1;
    D_843C4E45 = 1;

    gBattleScene.scene->unk_1A = 0;

    gBattleScene.scene->unk_4C = 0;
    gBattleScene.scene->unk_48 = 0;

    sp1C->recoilPendingHP = 0xFFFF;
    sp18->recoilPendingHP = 0xFFFF;

    D_843C4DA0 = 0;

    Battle_ClearAllMessageQueues();
    Battle_WriteBackRuntimeState(D_84390010[0]);
    Battle_WriteBackRuntimeState(D_84390010[1]);

    gBattleUser = sp1C;
    gBattleTarget = sp18;
    Battle_ExecuteAttackerTurn();
    Battle_ApplyResidualEffects();

    if (D_84390288 == 0) {
        Battle_CheckFaintAfterMove();
    }
}

void Battle_InitializeSwitchedInRuntimeMon(Battler* arg0) {
    BattlerState* sp24;
    BattlerState* sp20;
    BattleMonRuntime* ptr1;
    BattleMonRuntime* ptr2;

    ptr1 = &arg0->unk_654.monRuntime;

    sp24 = &D_84390010[0]->unk_654;
    sp20 = &D_84390010[1]->unk_654;

    ptr2 = &D_84390010[BattleScene_GetParticipantSideIndex(arg0) == 0]->unk_654.monRuntime;

    sp24->unk_2A = 0;
    sp20->unk_2A = 0;
    ptr1->recoilPendingHP = 0xFFFF;
    ptr2->recoilPendingHP = 0xFFFF;

    D_843C4E44 = 1;
    D_843C4E45 = 1;

    gBattleScene.scene->unk_48 = 0;
    gBattleScene.scene->unk_4C = 0;

    gBattleUser = ptr1;
    gBattleTarget = ptr2;

    Battle_QueueDamageSummaryMessages(arg0);
    Vec3f_SetComponentsDuplicate(&arg0->sway.basePosition, 0.0f, 0.0f, 0.0f);
    Vec3f_SetComponentsDuplicate(&arg0->sway.swayOffset, 0.0f, 0.0f, 0.0f);
    Battle_InitializeRuntimeMon(arg0);

    if (arg0 == D_84390010[0]) {
        Battle_ResetTransientRuntimeState(0, 1);
    } else {
        Battle_ResetTransientRuntimeState(1, 0);
    }
}
