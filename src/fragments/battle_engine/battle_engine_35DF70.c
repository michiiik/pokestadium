#include "battle_engine.h"
#include "src/text_system.h"

typedef void (*func_D_8438AE50)(void);

static const char D_8438DC80[] = "は\nこんらんした";
static const char D_8438DC90[] = "SP %d\n";
static const char D_8438DC98[] = "index %d  step %d\n";
static const char D_8438DCAC[] = "index %d  step %d\n";
static const char D_8438DCC0[] = "index %d\n";
static const char D_8438DCCC[] = "paramStep %d index %d  step %d\n";
static const char D_8438DCEC[] = "\n%%%%%%%%%%%%%%%%%%%%%%%\n";
static const char D_8438DD08[] = "INDEX %d   WARIKOMI %d\n";
static const char D_8438DD20[] = "%%%%%%%%%%%%%%%%%%%%%%%\n\n";
static const char D_8438DD3C[] = "FAIL\n";
static const char D_8438DD44[] = "index %d\n";
static const char D_8438DD50[] = "index %d\n";
static const char D_8438DD5C[] = "FAIL\n";
static const char D_8438DD64[] = "FAIL2\n";
static const char D_8438DD6C[] = "FAIL3\n";
static const char D_8438DD74[] = "offp->count1 %d\n";
static const char D_8438DD88[] = "\n\n";
static const char D_8438DD8C[] = "******** spAttackMain ********\n";
static const char D_8438DDAC[] = "!!!! SPATTACK Num %d!!!!\n";
static const char D_8438DDC8[] = "******** spAttackMainEnd ********\n";

static char* D_8438AE50 = D_8438DC80;
static func_D_8438AE50 gMoveEffectHandlers[] = {
    Battle_Effect_Sleep, Battle_Effect_Poison, Battle_Effect_DrainHP, Battle_Effect_SecondaryStatusChance, Battle_Effect_SecondaryStatusChance, Battle_Effect_SecondaryStatusChance, Battle_Effect_Explosion,
    Battle_Effect_DrainHP, NULL,          Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat,
    Battle_Effect_RaiseOwnStat, Battle_Effect_PayDay, NULL,          Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat,
    Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_Conversion, Battle_Effect_Haze, Battle_Effect_Bide, Battle_Effect_Thrash, Battle_Effect_ForceSwitchOrFlee,
    Battle_Effect_MultiHit, Battle_Effect_MultiHit, Battle_Effect_Flinch, Battle_Effect_Sleep, Battle_Effect_Poison, Battle_Effect_SecondaryStatusChance, Battle_Effect_SecondaryStatusChance,
    Battle_Effect_SecondaryStatusChance, Battle_Effect_Flinch, Battle_Effect_OHKO, Battle_Effect_TwoTurnMove, NULL,          NULL,          Battle_Effect_Trap,
    Battle_Effect_TwoTurnMove, Battle_Effect_MultiHit, NULL,          Battle_Effect_Mist, Battle_Effect_FocusEnergy, Battle_Effect_Recoil, Battle_Effect_Confuse,
    Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_RaiseOwnStat, Battle_Effect_Heal,
    Battle_Effect_Transform, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat,
    Battle_Effect_ReflectLightScreen, Battle_Effect_ReflectLightScreen, Battle_Effect_Poison, Battle_Effect_Paralyze, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat,
    Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_LowerStat, Battle_Effect_ConfuseChance, Battle_Effect_MultiHit,
    NULL,          Battle_Effect_Substitute, Battle_Effect_Recharge, Battle_Effect_Rage, Battle_Effect_Mimic, NULL,          Battle_Effect_LeechSeed,
    Battle_Effect_Splash, Battle_Effect_Disable, NULL,
};

u16 Battle_ApplyStatStageMultiplier(u16* arg0, u16 arg1, u8 arg2) {
    *arg0 = (arg1 * gStatStageMultipliers[arg2 - 1].unk_00) / gStatStageMultipliers[arg2 - 1].unk_01;

    if (*arg0 == 0) {
        *arg0 = 1;
    }

    if (*arg0 > 999) {
        *arg0 = 999;
    }

    return *arg0;
}

void Battle_Effect_Sleep(void) {
    s32 sp24;
    s32 var_v1;

    gBattleScene.scene->unk_48 = 0x16;
    sp24 = gBattleTarget->volatileStatusFlags & 0x20;
    gBattleTarget->volatileStatusFlags &= ~0x20;

    if (sp24 == 0) {
        if (gBattleTarget->status & 7) {
            D_843C4E44 = 0;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x80);
            return;
        }

        if (gBattleTarget->status == 0) {
            Battle_CheckMoveFailureConditions();
            if ((gBattleMoveFailed == 0) &&
                ((Battle_HasUsableMajorStatusTarget(D_84390010[!gBattleScene.scene->activeBattlerIndex]) == 0) || (D_8438AC60[0] == 2))) {
                goto block_9;
            }
        }
        gBattleDamage = 0;
        gBattleMoveFailed = 1;
        D_843C4E44 = 0;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x7F);
        return;
    }

block_9:
    if ((Battle_HasUsableMajorStatusTarget(D_84390010[!gBattleScene.scene->activeBattlerIndex]) != 0) && (D_8438AC60[0] != 2)) {
        D_843C4E44 = 0;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x7F);
        if ((sp24 != 0) && (D_8438AC60[0] == 1)) {
            gBattleTarget->volatileStatusFlags |= 0x20;
        }
    } else {
        do {
            if (D_8438AC60[0] == 2) {
                var_v1 = Battle_Random() & 7;
            } else {
                var_v1 = Battle_Random() & 3;
            }
        } while (var_v1 == 0);
        gBattleMoveFailed = 0;
        gBattleTarget->status = var_v1;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x7E);
    }
}

void Battle_Effect_Poison(void) {
    char* sp1C = gBattleMessageQueues->unk_088;

    if (!(gBattleTarget->volatileStatusFlags & 0x10) && (gBattleTarget->status == 0) && (gBattleTarget->unk_16[6] != 3) &&
        (gBattleTarget->unk_16[7] != 3)) {
        switch (gBattleUser->cachedMove.effectId) {
            case 2:
                if (gBattleScene.scene->unk_48 != 0x18) {
                    gBattleScene.scene->unk_48 = 0;
                    sp1C = gBattleMessageQueues->unk_208;
                } else {
                    sp1C = gBattleMessageQueues->unk_308;
                }

                if (Battle_Random() >= 0x34) {
                    return;
                }
                break;

            case 33:
                gBattleScene.scene->unk_48 = 0;
                sp1C = gBattleMessageQueues->unk_208;
                if (Battle_Random() >= 0x67) {
                    return;
                }
                break;

            default:
                gBattleScene.scene->unk_48 = 0x16;
                D_843C4DEC = 0xA;
                Battle_CheckMoveFailureConditions();
                if (gBattleMoveFailed != 0) {
                    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
                    Battle_QueueMessage(sp1C, -0x7D);
                    return;
                }
                break;
        }

        gBattleTarget->status |= 8;
        if (gBattleUser->cachedMove.moveId == 0x5C) {
            gBattleTarget->auxStatusFlags |= 1;
            gBattleTarget->toxicCounter = 0;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(sp1C, -0x7C);
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(sp1C, -0x7B);
        }
    } else if (gBattleUser->cachedMove.effectId == 0x42) {
        if (gBattleScene.scene->unk_48 != 0x18) {
            gBattleMoveFailed = 1;
            gBattleScene.scene->unk_48 = 0x16;
            D_843C4E44 = 0;
        }
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(sp1C, -0x7A);
    }
}

void Battle_Effect_DrainHP(void) {
    gBattleDamage /= 2;
    if (gBattleDamage == 0) {
        gBattleDamage = 1;
    }

    if (gBattleUser->maxHP < gBattleUser->currentHP + gBattleDamage) {
        gBattleUser->currentHP = gBattleUser->maxHP;
    } else {
        gBattleUser->currentHP += gBattleDamage;
    }

    if (gBattleUser->cachedMove.effectId == 8) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_208, -0x79);
    } else {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_208, -0x78);
    }
}

void Battle_Effect_Explosion(void) {
    gBattleScene.scene->unk_48 = 0x1E;
    gBattleUser->currentHP = 0;
    gBattleUser->status = 0;
    gBattleUser->volatileStatusFlags &= ~0x80;
}

void Battle_Effect_SecondaryStatusChance(void) {
    BattleMonRuntime* temp_a2 = &D_84390010[!gBattleScene.scene->activeBattlerIndex]->unk_654.monRuntime;
    BattlerState* temp_a0 = &D_84390010[!gBattleScene.scene->activeBattlerIndex]->unk_654;

    if (gBattleTarget->volatileStatusFlags & 0x10) {
        return;
    }

    if (gBattleTarget->status != 0) {
        if ((gBattleTarget->status & 0x20) && (gBattleUser->cachedMove.type == 0x14)) {
            gBattleTarget->status = 0;
            if (temp_a0->selectedMoveSlot != 4) {
                temp_a2->currentMoveId = temp_a2->moveIds[temp_a0->committedMoveSlot];
            } else {
                temp_a0->selectedMoveSlot = 0;
                temp_a0->committedMoveSlot = 0;
                temp_a2->currentMoveId = 0xA5;
            }
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x77);
        }
        return;
    }

    if ((gBattleTarget->unk_16[6] != (*gBattleUser).cachedMove.type) &&
        (gBattleTarget->unk_16[7] != (*gBattleUser).cachedMove.type)) {
        if ((gBattleUser->cachedMove.effectId < 7) || ((D_8438AC60[0] != 2) && (gBattleUser->cachedMove.moveId == 0x3B))) {
            if (Battle_Random() >= 0x1A) {
                return;
            }
        } else if (Battle_Random() >= 0x4D) {
            return;
        }

        switch (gBattleUser->cachedMove.effectId) {
            case 4:
            case 34:
                gBattleTarget->status = 0x10;
                Battle_ApplyBurnAttackPenalty(D_84390010[!gBattleScene.scene->activeBattlerIndex]);
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
                Battle_QueueMessage(gBattleMessageQueues->unk_208, -0x76);
                break;

            case 5:
            case 35:
                if (Battle_HasUsableSleepStatusTarget(D_84390010[!gBattleScene.scene->activeBattlerIndex]) == 0) {
                    gBattleTarget->volatileStatusFlags &= ~0x20;
                    gBattleTarget->status = 0x20;
                    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
                    Battle_QueueMessage(gBattleMessageQueues->unk_208, -0x75);
                }
                break;

            case 6:
            case 36:
                gBattleTarget->status = 0x40;
                Battle_ApplyParalysisSpeedPenalty(D_84390010[!gBattleScene.scene->activeBattlerIndex]);
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
                Battle_QueueMessage(gBattleMessageQueues->unk_208, -0x74);
                break;
        }
    }
}

void Battle_ApplyRageStatBoost(void) {
    s32 sp2C;
    s32 sp28;
    s32 sp24;
    s32 var_a3;
    u16* sp1C;
    u16* sp18;

    gBattleScene.scene->unk_4C = 0xA;

    var_a3 = 1;
    sp24 = gBattleUser->cachedMove.effectId - 0xA;

    if (sp24 >= 8) {
        var_a3 = 2;
        sp24 -= 0x28;
    }

    if (gBattleUser->statStages[sp24] != 0xD) {
        gBattleUser->statStages[sp24] += var_a3;
        if (gBattleUser->statStages[sp24] >= 0xE) {
            gBattleUser->statStages[var_a3] = 0xD;
        }

        sp28 = 0;
        sp2C = 0;

        if (sp24 < 4) {
            switch (sp24) {
                case 0:
                    sp18 = &gBattleUser->origAttack;
                    sp1C = &gBattleUser->attack;
                    sp2C = 1;
                    break;

                case 1:
                    sp18 = &gBattleUser->origDefense;
                    sp1C = &gBattleUser->defense;
                    break;

                case 2:
                    sp18 = &gBattleUser->origSpeed;
                    sp1C = &gBattleUser->speed;
                    sp28 = 1;
                    break;

                case 3:
                    sp18 = &gBattleUser->origSpecial;
                    sp1C = &gBattleUser->special;
                    break;
            }

            if (*sp1C != 0x3E7) {
                *sp1C = (*sp18 * gStatStageMultipliers[gBattleUser->statStages[sp24] - 1].unk_00) /
                        gStatStageMultipliers[gBattleUser->statStages[sp24] - 1].unk_01;
                if (*sp1C >= 0x3E8) {
                    *sp1C = 0x3E7;
                }
            } else {
                goto block_29;
            }
        }

        if (gBattleUser->cachedMove.moveId == 0x6B) {
            gBattleUser->statChangeFlag = 1;
        }

        if (var_a3 == 2) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_2C8, sp24 + 0x8D);
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_2C8, sp24 + 0x93);
        }

        if ((sp28 != 0) || (D_8438AC60[0] != 1)) {
            Battle_ApplyParalysisSpeedPenalty(D_84390010[!gBattleScene.scene->activeBattlerIndex]);
        }

        if ((sp2C != 0) || (D_8438AC60[0] != 1)) {
            Battle_ApplyBurnAttackPenalty(D_84390010[!gBattleScene.scene->activeBattlerIndex]);
        }
    } else {
    block_29:
        Battle_QueueMessage(gBattleMessageQueues->unk_2C8, -0x67);
    }
}

void Battle_Effect_RaiseOwnStat(void) {
    s32 sp2C;
    s32 sp28;
    s32 sp24;
    s32 var_a3;
    u16* sp1C;
    u16* sp18;

    if (gBattleUser->cachedMove.moveId == 0x6E) {
        gBattleScene.scene->unk_48 = 0x26;
    } else {
        gBattleScene.scene->unk_48 = 7;
    }

    var_a3 = 1;
    sp24 = gBattleUser->cachedMove.effectId - 0xA;
    if (sp24 >= 8) {
        var_a3 = 2;
        sp24 -= 0x28;
    }

    if (gBattleUser->statStages[sp24] != 0xD) {
        gBattleUser->statStages[sp24] += var_a3;
        if (gBattleUser->statStages[sp24] >= 0xE) {
            gBattleUser->statStages[sp24] = 0xD;
        }

        sp28 = 0;
        sp2C = 0;

        if (sp24 < 4) {
            switch (sp24) {
                case 0:
                    sp18 = &gBattleUser->origAttack;
                    sp1C = &gBattleUser->attack;
                    sp2C = 1;
                    break;

                case 1:
                    sp18 = &gBattleUser->origDefense;
                    sp1C = &gBattleUser->defense;
                    break;

                case 2:
                    sp18 = &gBattleUser->origSpeed;
                    sp1C = &gBattleUser->speed;
                    sp28 = 1;
                    break;

                case 3:
                    sp18 = &gBattleUser->origSpecial;
                    sp1C = &gBattleUser->special;
                    break;
            }

            if (*sp1C != 0x3E7) {
                *sp1C = (*sp18 * gStatStageMultipliers[gBattleUser->statStages[sp24] - 1].unk_00) /
                        gStatStageMultipliers[gBattleUser->statStages[sp24] - 1].unk_01;
                if (*sp1C >= 0x3E8) {
                    *sp1C = 0x3E7;
                }
            } else {
                goto block_32;
            }
        }

        if (gBattleUser->cachedMove.moveId == 0x6B) {
            gBattleUser->statChangeFlag = 1;
        }

        if (var_a3 == 2) {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_088, sp24 + 0x8D);
        } else {
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_088, sp24 + 0x93);
        }

        if ((sp28 != 0) || (D_8438AC60[0] != 1)) {
            Battle_ApplyParalysisSpeedPenalty(D_84390010[gBattleScene.scene->activeBattlerIndex]);
        }

        if ((sp2C != 0) || (D_8438AC60[0] != 1)) {
            Battle_ApplyBurnAttackPenalty(D_84390010[gBattleScene.scene->activeBattlerIndex]);
        }
    } else {
    block_32:
        D_843C4E45 = 0;
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x67);
    }
}

void Battle_Effect_LowerStat(void) {
    s32 sp2C;
    s32 sp28;
    s32 sp24;
    s32 sp20;
    u16* sp1C;
    u16* sp18;

    if (gBattleUser->cachedMove.effectId < 0x44) {
        gBattleScene.scene->unk_48 = 0x1A;
    } else {
        gBattleScene.scene->unk_1A = 3;
    }

    if (gBattleTarget->volatileStatusFlags & 0x10) {
        goto end;
    }

    sp24 = gBattleUser->cachedMove.effectId;
    sp20 = 1;

    if (sp24 >= 0x44) {
        if (Battle_Random() < 0x55) {
            sp24 -= 0x44;
            goto block_14;
        }
    } else {
        Battle_CheckMoveFailureConditions();

        if (gBattleMoveFailed != 0) {
            goto end;
        }

        if (gBattleTarget->lockedEffectFlags & 0x40) {
            goto end;
        }

        sp24 -= 0x12;
        if (sp24 >= 9) {
            sp20 = 2;
            sp24 -= 0x28;
        }

    block_14:
        if (gBattleTarget->statStages[sp24] != 1) {
            gBattleTarget->statStages[sp24] -= sp20;
            if (gBattleTarget->statStages[sp24] <= 0) {
                gBattleTarget->statStages[sp24] = 1;
            }

            sp28 = 0;
            sp2C = 0;

            if (sp24 < 4) {
                switch (sp24) {
                    case 0:
                        sp2C = 1;
                        sp18 = &gBattleTarget->origAttack;
                        sp1C = &gBattleTarget->attack;
                        break;

                    case 1:
                        sp18 = &gBattleTarget->origDefense;
                        sp1C = &gBattleTarget->defense;
                        break;

                    case 2:
                        sp28 = 1;
                        sp18 = &gBattleTarget->origSpeed;
                        sp1C = &gBattleTarget->speed;
                        break;

                    case 3:
                        sp18 = &gBattleTarget->origSpecial;
                        sp1C = &gBattleTarget->special;
                        break;
                }

                if (*sp1C != 1) {
                    *sp1C = (*sp18 * gStatStageMultipliers[gBattleTarget->statStages[sp24] - 1].unk_00) /
                            gStatStageMultipliers[gBattleTarget->statStages[sp24] - 1].unk_01;
                    if (*sp1C == 0) {
                        *sp1C = 1;
                    }
                } else {
                    goto block_39;
                }
            }

            if (sp20 == 2) {
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
                Battle_QueueMessage(gBattleMessageQueues->unk_208, sp24 + 0x9A);
            } else {
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
                Battle_QueueMessage(gBattleMessageQueues->unk_208, sp24 + 0xA0);
            }

            if ((sp28 != 0) || (D_8438AC60[0] != 1)) {
                Battle_ApplyParalysisSpeedPenalty(D_84390010[!gBattleScene.scene->activeBattlerIndex]);
            }

            if ((sp2C != 0) || (D_8438AC60[0] != 1)) {
                Battle_ApplyBurnAttackPenalty(D_84390010[!gBattleScene.scene->activeBattlerIndex]);
            }
            return;
        }
    }

block_39:
    if (gBattleUser->cachedMove.effectId < 0x44) {
        D_843C4E44 = 0;
        D_843C4E45 = 0;
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x5A);
        return;
    }

end:
    if (gBattleUser->cachedMove.effectId < 0x44) {
        D_843C4E44 = 0;
        D_843C4E45 = 0;
        if (D_843C4DA9 == 0) {
            Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x59);
        }
    } else {
        D_843C4E45 = 0;
    }
}

void Battle_Effect_Bide(void) {
    gBattleScene.scene->unk_48 = 0x1B;
    gBattleUser->cachedMove.effectId = 0;
    gBattleUser->lockedEffectFlags |= 1;
    gBattleUser->effectAccumulator = 0;
    gBattleUser->lockedEffectCounter = (Battle_Random() & 1) + 2;
}

void Battle_Effect_Thrash(void) {
    gBattleUser->cachedMove.effectId = 0;
    gBattleUser->lockedEffectFlags |= 2;
    gBattleUser->lockedEffectCounter = (Battle_Random() & 1) + 2;
}

void Battle_Effect_ForceSwitchOrFlee(void) {
    if (gBattleUser->cachedMove.moveId == 0x64) {
        gBattleScene.scene->unk_48 = 0x27;
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x53);
        return;
    }
    gBattleScene.scene->unk_48 = 8;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
    Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x52);
}

void Battle_Effect_MultiHit(void) {
    s32 var_v1;

    if (!(gBattleUser->lockedEffectFlags & 4)) {
        gBattleUser->lockedEffectFlags |= 4;

        switch (gBattleUser->cachedMove.effectId) {
            case 0x4D:
                gBattleUser->cachedMove.effectId = 2;

            case 0x2C:
                gBattleUser->effectAccumulator = 2;
                gBattleUser->lockedEffectCounter = gBattleUser->effectAccumulator;
                break;

            default:
                var_v1 = Battle_Random() & 3;
                if (var_v1 >= 2) {
                    var_v1 = Battle_Random() & 3;
                }
                gBattleUser->lockedEffectCounter = gBattleUser->effectAccumulator = var_v1 + 2;
                break;
        }

        D_8439022C = gBattleUser->effectAccumulator - 1;
        gBattleScene.scene->unk_48 = 0x18;
    }
}

void Battle_Effect_Flinch(void) {
    if (!(gBattleTarget->volatileStatusFlags & 0x10)) {
        if (D_8438AC60[0] == 1) {
            if (gBattleUser->cachedMove.effectId == 0x1F) {
                if (Battle_Random() >= 0x1A) {
                    return;
                }
            } else if (Battle_Random() >= 0x4D) {
                return;
            }
        } else {
            gBattleTarget->volatileStatusFlags &= ~0x20;
            if (gBattleUser->cachedMove.effectId == 0x1F) {
                if (Battle_Random() >= 0x1A) {
                    return;
                }
            } else if (Battle_Random() >= 0x4D) {
                return;
            }
        }
        gBattleTarget->lockedEffectFlags |= 8;
    }
}

void Battle_Effect_OHKO(void) {
    gBattleDamage = 0;
    gBattleCritFlag = 0xFF;
    if (gBattleUser->speed < gBattleTarget->speed) {
        gBattleMoveFailed = 1;
        return;
    }
    gBattleDamage = 0xFFFF;
    gBattleCritFlag = 2;
}

void Battle_Effect_TwoTurnMove(void) {
    gBattleScene.scene->unk_48 = 0xE;
    gBattleUser->lockedEffectFlags |= 0x10;

    if ((gBattleUser->cachedMove.effectId == 0x2B) || (gBattleUser->cachedMove.moveId == 0x5B)) {
        gBattleUser->lockedEffectFlags |= 0x40;
        if (gBattleUser->cachedMove.moveId == 0x5B) {
            gBattleScene.scene->unk_48 = 0xD;
        }

        if (gBattleUser->cachedMove.effectId == 0x2B) {
            gBattleScene.scene->unk_48 = 0xC;
        }
    }

    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));

    switch (gBattleUser->cachedMove.moveId) {
        case 0xD:
            Battle_QueueMessage(gBattleMessageQueues->unk_0C8, -0x51);
            break;

        case 0x4C:
            Battle_QueueMessage(gBattleMessageQueues->unk_0C8, -0x50);
            break;

        case 0x82:
            Battle_QueueMessage(gBattleMessageQueues->unk_0C8, -0x4F);
            break;

        case 0x8F:
            Battle_QueueMessage(gBattleMessageQueues->unk_0C8, -0x4E);
            break;

        case 0x13:
            Battle_QueueMessage(gBattleMessageQueues->unk_148, -0x4D);
            break;

        case 0x5B:
            Battle_QueueMessage(gBattleMessageQueues->unk_148, -0x4C);
            break;
    }
}

void Battle_Effect_Trap(void) {
    s32 var_v1;

    if (!(gBattleUser->lockedEffectFlags & 0x20) && ((D_8438AC60[0] != 1) || (gBattleUser->cachedMove.type != 0) ||
                                         ((gBattleTarget->unk_16[6] != 8) && (gBattleTarget->unk_16[7] != 8)))) {
        gBattleTarget->volatileStatusFlags &= 0xFFDF;
        gBattleUser->lockedEffectFlags |= 0x20;
        var_v1 = Battle_Random() & 3;
        if (var_v1 >= 2) {
            var_v1 = Battle_Random() & 3;
        }
        gBattleUser->lockedEffectCounter = var_v1 + 1;
    }
}

void Battle_Effect_Mist(void) {
    if (gBattleUser->volatileStatusFlags & 2) {
        gBattleScene.scene->unk_48 = 7;
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x4B);
        return;
    }
    gBattleScene.scene->unk_48 = 7;
    gBattleUser->volatileStatusFlags |= 2;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
    Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x4A);
}

void Battle_Effect_FocusEnergy(void) {
    gBattleScene.scene->unk_48 = 7;
    if (gBattleUser->volatileStatusFlags & 4) {
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x49);
        return;
    }
    gBattleUser->volatileStatusFlags |= 4;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
    Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x48);
}

void Battle_Effect_Recoil(void) {
    s32 var_v1;

    gBattleUser->recoilPendingHP = gBattleUser->currentHP;

    if (gBattleUser->cachedMove.moveId == 0xA5) {
        var_v1 = gBattleDamage >> 1;
    } else {
        var_v1 = gBattleDamage >> 2;
    }

    if (var_v1 == 0) {
        var_v1 += 1;
    }

    if (gBattleUser->recoilPendingHP < var_v1) {
        gBattleUser->recoilPendingHP = 0;
    } else {
        gBattleUser->recoilPendingHP -= var_v1;
    }

    gBattleTarget->recoilPendingHP = gBattleTarget->currentHP;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
    Battle_QueueMessage(gBattleMessageQueues->unk_408, -0x47);
    gBattleScene.scene->unk_48 = 9;
}

void Battle_Effect_ConfuseChance(void) {
    if (!(gBattleTarget->volatileStatusFlags & 0x10) && (Battle_Random() < 0x1A) && !(gBattleTarget->lockedEffectFlags & 0x80)) {
        gBattleScene.scene->unk_1A = 3;
        D_843C4E45 = 0;
        gBattleTarget->lockedEffectFlags |= 0x80;
        gBattleTarget->confusionCounter = (Battle_Random() & 3) + 2;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_208, -0x46);
    }
}

void Battle_Effect_Confuse(void) {
    gBattleScene.scene->unk_48 = 0x16;
    D_843C4E44 = 0;

    if (!(gBattleTarget->volatileStatusFlags & 0x10)) {
        Battle_CheckMoveFailureConditions();
        if ((gBattleMoveFailed == 0) && !(gBattleTarget->lockedEffectFlags & 0x80)) {
            D_843C4E44 = 1;
            gBattleTarget->lockedEffectFlags |= 0x80;
            gBattleTarget->confusionCounter = (Battle_Random() & 3) + 2;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x45);
            return;
        }
    }

    if (D_843C4DA9 == 0) {
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x44);
    }
}

void Battle_Effect_Paralyze(void) {
    gBattleScene.scene->unk_48 = 0x16;
    if (gBattleTarget->status == 0) {
        if ((gBattleUser->cachedMove.type != 0x17) || ((gBattleTarget->unk_16[6] != 4) && (gBattleTarget->unk_16[7] != 4))) {
            Battle_CheckMoveFailureConditions();
            if (gBattleMoveFailed == 0) {
                gBattleTarget->status |= 0x40;
                Battle_ApplyParalysisSpeedPenalty(D_84390010[!gBattleScene.scene->activeBattlerIndex]);
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
                Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x43);
                return;
            }
        } else {
            gBattleScene.scene->unk_48 = 8;
            D_843C4E44 = 0;
            Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x42);
            return;
        }
    }

    gBattleScene.scene->unk_48 = 8;
    D_843C4E44 = 0;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
    Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x41);
}

void Battle_Effect_Substitute(void) {
    s32 temp_v1;

    gBattleScene.scene->unk_48 = 0x1C;
    if (gBattleUser->volatileStatusFlags & 0x10) {
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x2A);
        return;
    }

    temp_v1 = gBattleUser->maxHP >> 2;
    if (temp_v1 >= gBattleUser->currentHP) {
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x29);
        return;
    }

    gBattleScene.scene->unk_48 = 0x19;
    gBattleUser->substituteHP = temp_v1;
    gBattleUser->currentHP -= temp_v1;
    gBattleUser->volatileStatusFlags |= 0x10;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
    Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x28);
}

void Battle_Effect_Recharge(void) {
    gBattleUser->volatileStatusFlags |= 0x20;
}

void Battle_Effect_Rage(void) {
    gBattleUser->volatileStatusFlags |= 0x40;
}

void Battle_Effect_Mimic(void) {
    BattlerState* sp24 = &D_84390010[gBattleScene.scene->activeBattlerIndex]->unk_654;
    s32 var_t0;
    u8 temp_v0;

    gBattleScene.scene->unk_48 = 7;
    Battle_CheckMoveFailureConditions();

    if ((gBattleMoveFailed == 0) && !(gBattleTarget->lockedEffectFlags & 0x40)) {
        D_843C4DAA = 1;
        do {
            var_t0 = BattleAnim_RandomRange(4);
            if (var_t0 >= 4) {
                var_t0 = 3;
            }
            temp_v0 = gBattleTarget->moveIds[var_t0];
        } while (temp_v0 == 0);

        gBattleUser->moveIds[sp24->committedMoveSlot] = temp_v0;
        Text_SetStringToken(0x1E, Text_GetString(NULL, 0, D_843900B8, gBattleTarget->moveIds[var_t0] - 1));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x40);
        return;
    }

    Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x3F);
}

void Battle_Effect_LeechSeed(void) {
    Battle_CheckMoveFailureConditions();

    if ((gBattleMoveFailed == 0) && (gBattleTarget->unk_16[6] != 0x16) && (gBattleTarget->unk_16[7] != 0x16)) {
        if (!(gBattleTarget->volatileStatusFlags & 0x80)) {
            gBattleTarget->volatileStatusFlags |= 0x80;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x3E);
            return;
        }
    }

    gBattleScene.scene->unk_48 = 8;
    Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
    Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x3D);
}

void Battle_Effect_Splash(void) {
    gBattleScene.scene->unk_48 = 7;
    Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x3C);
}

void Battle_Effect_Disable(void) {
    s32 sp24;
    s32 temp_v1;

    Battle_CheckMoveFailureConditions();

    if (gBattleMoveFailed == 0) {
        gBattleScene.scene->unk_48 = 0x20;
        if (gBattleTarget->disabledSlotAndTurns == 0) {
            do {
                sp24 = Battle_Random() & 3;
            } while (gBattleTarget->moveIds[sp24] == 0);
            temp_v1 = (Battle_Random() & 7) + 1;
            gBattleTarget->disabledSlotAndTurns = (sp24 * 0x10) + temp_v1;
            gBattleTarget->disabledMoveId = gBattleTarget->moveIds[sp24];
            Text_SetStringToken(0x1E, Text_GetString(NULL, 0, D_843900B8, gBattleTarget->disabledMoveId - 1));
            Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x3B);
            return;
        }
    }

    gBattleScene.scene->unk_48 = 8;
    Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x3A);
}

void Battle_Effect_PayDay(void) {
    Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x39);
}

void Battle_Effect_Conversion(void) {
    gBattleScene.scene->unk_48 = 7;
    if (!(gBattleTarget->lockedEffectFlags & 0x40)) {
        gBattleUser->unk_16[6] = gBattleTarget->unk_16[6];
        gBattleUser->unk_16[7] = gBattleTarget->unk_16[7];
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x38);
    } else {
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x37);
    }
}

void Battle_Effect_Haze(void) {
    s32 i;
    BattleMonRuntime* temp_t0 = &D_84390010[0]->unk_654.monRuntime;
    BattleMonRuntime* temp_a3 = &D_84390010[1]->unk_654.monRuntime;

    gBattleScene.scene->unk_48 = 0x1D;

    for (i = 0; i < 8; i++) {
        temp_t0->statStages[i] = 7;
        temp_a3->statStages[i] = 7;
    }

    temp_t0->attack = temp_t0->origAttack;
    temp_t0->defense = temp_t0->origDefense;
    temp_t0->speed = temp_t0->origSpeed;
    temp_t0->special = temp_t0->origSpecial;
    temp_a3->attack = temp_a3->origAttack;
    temp_a3->defense = temp_a3->origDefense;
    temp_a3->speed = temp_a3->origSpeed;
    temp_a3->special = temp_a3->origSpecial;

    if ((gBattleTarget->status & 0x27) && !(gBattleTarget->lockedEffectFlags & 0x10)) {
        gBattleTarget->currentMoveId = 0xFE;
    }

    gBattleTarget->status = 0;
    // clang-format off
    if (D_8438AC60[0] == 1) { gBattleUser->status = 0; }
    // clang-format on

    temp_t0->disabledSlotAndTurns = temp_a3->disabledSlotAndTurns = 0;
    temp_t0->disabledMoveId = temp_a3->disabledMoveId = 0;
    temp_t0->lockedEffectFlags &= ~0x80;
    temp_t0->volatileStatusFlags &= 0x78;
    temp_t0->auxStatusFlags &= 8;
    temp_a3->lockedEffectFlags &= ~0x80;
    temp_a3->volatileStatusFlags &= 0x78;
    temp_a3->auxStatusFlags &= 8;
    gBattleUser->statChangeFlag = 0;
    gBattleTarget->statChangeFlag = 0;

    Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x36);
}

void Battle_Effect_Heal(void) {
    u8 temp_v0;
    BattlerState* temp_a1;
    BattleMonRuntime* temp_a2;

    temp_a1 = &D_84390010[gBattleScene.scene->activeBattlerIndex]->unk_654;
    temp_a2 = &D_84390010[gBattleScene.scene->activeBattlerIndex]->unk_654.monRuntime;

    gBattleScene.scene->unk_48 = 0x15;
    if (gBattleUser->currentHP != gBattleUser->maxHP) {
        if (gBattleUser->cachedMove.moveId == 0x9C) {
            temp_v0 = gBattleUser->status;

            gBattleUser->status = 2;
            temp_a2->unk_16[temp_a1->partyIndex + temp_a1->activeSideIndex * 3] = 2;

            if (temp_v0 != 0) {
                if (D_8438AC60[0] == 1) {
                    gBattleUser->auxStatusFlags &= ~1;
                    Battle_ApplyStatStageMultiplier(&gBattleUser->attack, gBattleUser->origAttack, gBattleUser->statStages[0]);
                    Battle_ApplyStatStageMultiplier(&gBattleUser->speed, gBattleUser->origSpeed, gBattleUser->statStages[2]);
                }
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
                Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x35);
            } else {
                Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
                Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x34);
            }
            gBattleUser->currentHP += gBattleUser->maxHP;
        } else {
            gBattleUser->currentHP += gBattleUser->maxHP / 2;
        }

        if (gBattleUser->maxHP < gBattleUser->currentHP) {
            gBattleUser->currentHP = gBattleUser->maxHP;
        }

        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_148, -0x33);
        return;
    }

    gBattleScene.scene->unk_48 = 0x1C;
    gBattleMoveFailed = 1;
    Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x32);
}

void Battle_Effect_Transform(void) {
    s32 i;
    BattlerState* sp20;
    BattlerState* sp1C;
    BattleMonRuntime* ptr;

    sp20 = &D_84390010[gBattleScene.scene->activeBattlerIndex]->unk_654;
    sp1C = &D_84390010[!gBattleScene.scene->activeBattlerIndex]->unk_654;

    if ((D_8438AC60[0] == 1) && ((((gBattleTarget->auxStatusFlags & 8) == 0) && (gBattleTarget->speciesId == 0x84)) ||
                                 (((gBattleTarget->auxStatusFlags & 8) != 0) && (sp1C->unk_BE == 0x84)))) {
        gBattleScene.scene->unk_48 = 0x1C;
        Text_SetStringToken(0x1B, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x2F);
        return;
    }

    ptr = gBattleUser;
    if (!(ptr->lockedEffectFlags & 0x40) &&
        ((D_8438AC60[0] != 1) || (gBattleTarget->moveIds[0] != 0x90) || (gBattleTarget->moveIds[1] != 0))) {
        gBattleScene.scene->unk_48 = 0xF;
        Text_SetStringToken(0x1B, Battle_GetActiveMonNickname(D_84390010[!gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_148, -0x31);
        gBattleUser->auxStatusFlags |= 8;

        if (gBattleTarget->auxStatusFlags & 8) {
            sp20->unk_BE = sp1C->unk_BE;
        } else {
            sp20->unk_BE = gBattleTarget->speciesId;
        }

        gBattleUser->unk_16[6] = gBattleTarget->unk_16[6];
        gBattleUser->unk_16[7] = gBattleTarget->unk_16[7];
        gBattleUser->moveIds[0] = gBattleTarget->moveIds[0];
        gBattleUser->moveIds[1] = gBattleTarget->moveIds[1];
        gBattleUser->moveIds[2] = gBattleTarget->moveIds[2];
        gBattleUser->moveIds[3] = gBattleTarget->moveIds[3];
        gBattleUser->dvs = gBattleTarget->dvs;
        gBattleUser->attack = gBattleTarget->attack;
        gBattleUser->defense = gBattleTarget->defense;
        gBattleUser->speed = gBattleTarget->speed;
        gBattleUser->special = gBattleTarget->special;

        for (i = 0; i < 4; i++) {
            if (gBattleTarget->moveIds[i] == 0) {
                break;
            }
            gBattleUser->currentPP[i] = 5;
        }

        gBattleUser->origAttack = gBattleTarget->origAttack;
        gBattleUser->origDefense = gBattleTarget->origDefense;
        gBattleUser->origSpeed = gBattleTarget->origSpeed;
        gBattleUser->origSpecial = gBattleTarget->origSpecial;

        for (i = 0; i < 8; i++) {
            gBattleUser->statStages[i] = gBattleTarget->statStages[i];
        }
        return;
    }

    gBattleScene.scene->unk_48 = 0x1C;
    Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x30);
}

void Battle_Effect_ReflectLightScreen(void) {
    gBattleScene.scene->unk_48 = 7;
    if (gBattleUser->cachedMove.effectId == 0x40) {
        if (!(gBattleUser->auxStatusFlags & 2)) {
            gBattleUser->auxStatusFlags |= 2;
            Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
            Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x2E);
        } else {
            Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x2D);
        }
    } else if (!(gBattleUser->auxStatusFlags & 4)) {
        gBattleUser->auxStatusFlags |= 4;
        Text_SetStringToken(0x19, Battle_GetActiveMonNickname(D_84390010[gBattleScene.scene->activeBattlerIndex]));
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x2C);
    } else {
        Battle_QueueMessage(gBattleMessageQueues->unk_088, -0x2B);
    }
}

void Battle_RunMoveEffect(void) {
    BattleMonRuntime* ptr = &D_84390010[gBattleScene.scene->activeBattlerIndex]->unk_654.monRuntime;
    s32 idx = ptr->cachedMove.effectId;

    if (gMoveEffectHandlers[idx - 1] != NULL) {
        gMoveEffectHandlers[idx - 1]();
    }
}

void Battle_ExecuteMoveEffect(void) {
    Battle_RunMoveEffect();
}
