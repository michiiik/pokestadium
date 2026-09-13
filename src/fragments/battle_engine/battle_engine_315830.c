#include "battle_engine.h"
#include "src/model_animation.h"
#include "src/model_animation_events.h"
#include "src/model_renderer.h"

void BattleAnim_StartFreezeStatusAnimation(Battler* arg0) {
    UNUSED s32 pad[2];
    BattleMon* ptr;
    BattlerState* sp2C;
    BattleMonRuntime* sp28;
    MovePresentationCue* sp20;
    s32 idx = BattleScene_GetParticipantSideIndex(arg0);

    sp28 = &arg0->unk_654.monRuntime;
    sp2C = &arg0->unk_654;
    ptr = &arg0->ownRoster->party[sp2C->partyIndex];
    sp20 = &D_84384570[idx]->unk_A80;

    if ((ptr->status & 0x20) && (sp2C->battleStateFlags & 2)) {
        arg0->model.unk_000.unk_02 &= ~0x20;
    }

    if ((sp28->status & 0x20) && (ptr->status & 0x20) && !(sp2C->battleStateFlags & 2) &&
        ((arg0->model.unk_040.unk_08 >> 0x10) >= 3)) {
        ModelAnim_SetAnimation(&arg0->model, sp20->animationId);
        ModelAnim_SetFrame(&arg0->model, 4);
        ModelAnim_SetEventTrack(&arg0->model, sp20->eventTrackId);
        ModelAnim_SetEventFrame(&arg0->model, 3);
        arg0->model.unk_000.unk_02 &= ~0x20;
        Battle_SetRuntimeFlags(arg0, 2);
        BattleAnim_QueueEffectList(1, arg0);
        BattleAnim_PlayBattleSequenceById(arg0, 0x13);
    }
}

void BattleAnim_RestoreBaseModelAnimation(Battler* arg0) {
    MovePresentationCue* ptr = &D_84384570[BattleScene_GetParticipantSideIndex(arg0)]->unk_A50;

    ModelAnim_SetAnimation(&arg0->model, ptr->animationId);
}

void BattleAnim_ClearFreezeStatusAnimation(Battler* arg0) {
    if (arg0->unk_654.battleStateFlags & 2) {
        BattleAnim_StopOwnerCategoryParticles(1, arg0);
        arg0->model.unk_000.unk_02 |= 0x20;
        Battle_ClearRuntimeFlags(arg0, 2);
        BattleAnim_RestoreBaseModelAnimation(arg0);
    }
}

void BattleAnim_ResetFreezeStatusAndRestoreAnimation(Battler* arg0) {
    BattleMon* ptr = &arg0->ownRoster->party[arg0->unk_654.partyIndex];

    if (arg0->unk_654.battleStateFlags & 2) {
        ptr->status = 0;
        BattleAnim_StopOwnerCategoryParticles(1, arg0);
        arg0->model.unk_000.unk_02 |= 0x20;
        Battle_ClearRuntimeFlags(arg0, 2);
        BattleAnim_RestoreBaseModelAnimation(arg0);
    }
}

void BattleAnim_ClearFreezeStatusAnimationIfThawed(Battler* arg0) {
    if ((arg0->unk_654.battleStateFlags & 2) && !(arg0->unk_654.monRuntime.status & 0x20)) {
        BattleAnim_StopOwnerCategoryParticles(1, arg0);
        arg0->model.unk_000.unk_02 |= 0x20;
        Battle_ClearRuntimeFlags(arg0, 2);
        BattleAnim_RestoreBaseModelAnimation(arg0);
    }
}

void BattleAnim_UpdateSleepStatusAnimation(Battler* arg0) {
    BattlerState* sp24 = &arg0->unk_654;
    BattleMonRuntime* sp28 = &sp24->monRuntime;
    BattleMon* sp2C = &arg0->ownRoster->party[arg0->unk_654.partyIndex];

    if ((gBattleScene.scene->unk_38 != 0xB) || (gBattleScene.scene->scenePhase >= 6)) {
        if ((sp2C->status & 7) && (sp28->status & 7) && !(arg0->unk_654.battleStateFlags & 1)) {
            BattleAnim_SetModelEventTrackAndState(arg0, 0xAF, 2);
            BattleAnim_QueueEffectList(3, arg0);
            BattleAnim_PlayBattleSequenceById(arg0, 0x32);
            Battle_SetRuntimeFlags(arg0, 1);
        }

        if (!(sp2C->status & 7) && (sp24->battleStateFlags & 1)) {
            if (D_84384570[BattleScene_GetParticipantSideIndex(arg0)]->unk_B81 == 0xFF) {
                sp24->unk_36 = 0;
                BattleAnim_SetModelEventTrackFromRow(arg0, 0xA5);
            } else {
                BattleAnim_SetModelEventTrackAndState(arg0, 0xB8, 1);
            }
            BattleAnim_StopOwnerCategoryParticles(3, arg0);
            Battle_ClearRuntimeFlags(arg0, 1);
        }
    }
}

void BattleAnim_EndSleepStatusAnimation(Battler* arg0) {
    BattleMon* ptr = &arg0->ownRoster->party[arg0->unk_654.partyIndex];

    if (arg0->unk_654.battleStateFlags & 1) {
        ptr->status = 0;
        if (D_84384570[BattleScene_GetParticipantSideIndex(arg0)]->unk_B81 == 0xFF) {
            arg0->unk_654.unk_36 = 0;
            BattleAnim_SetModelEventTrackFromRow(arg0, 0xA5);
        } else {
            BattleAnim_SetModelEventTrackAndState(arg0, 0xB8, 1);
        }
        BattleAnim_StopOwnerCategoryParticles(3, arg0);
        Battle_ClearRuntimeFlags(arg0, 1);
    }
}

void BattleAnim_ClearSleepStatusAnimation(Battler* arg0) {
    if (arg0->unk_654.battleStateFlags & 1) {
        BattleAnim_StopOwnerCategoryParticles(3, arg0);
        Battle_ClearRuntimeFlags(arg0, 1);
    }
}
