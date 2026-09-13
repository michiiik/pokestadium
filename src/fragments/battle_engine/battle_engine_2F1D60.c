#include "battle_engine.h"
#include "src/model_animation.h"
#include "src/model_renderer.h"
#include "src/model_renderer_callbacks.h"
#include "src/matrix.h"

void BattleScene_TriggerOwnerModelAnimation(Battler* arg0) {
    ModelAnim_SetFrame(&arg0->presentation.model, arg0->presentation.animationFrame);
    ModelAnim_SetAnimation(&arg0->presentation.model, arg0->presentation.animationId);
    BattleScene_SetOwnerPoseFlag(arg0->presentation.layout);
}

void BattleScene_ClearParticipantOwnerActiveFlag(Battler* arg0) {
    arg0->presentation.visible = 0;
}

void BattleScene_SetParticipantOwnerActiveFlag(Battler* arg0) {
    arg0->presentation.visible = 1;
}

void BattleScene_ClearBothOwnerActiveFlags(void) {
    D_84390010[0]->presentation.visible = 0;
    D_84390010[1]->presentation.visible = 0;
}

void BattleScene_SetBothOwnerActiveFlags(void) {
    D_84390010[0]->presentation.visible = 1;
    D_84390010[1]->presentation.visible = 1;
}

void BattleScene_SetOwnerPoseFlag(PresentationLayout* arg0) {
    arg0->unk_00.unk_01 |= 0x10;
}

void BattleScene_LoadOwnerCameraFramingConstants(Battler* arg0) {
    s32 var_v0;

    if (arg0 == D_84390010[0]) {
        var_v0 = 0;
    } else {
        var_v0 = 1;
    }

    arg0->presentation.layout->unk_20 = D_843847BC[var_v0].unk_04;
    arg0->presentation.layout->hudX = D_843847BC[var_v0].unk_00;
    arg0->presentation.layout->hudY = D_843847BC[var_v0].unk_02;

    arg0->presentation.cameraConfigIndex = D_843847BC[var_v0].unk_14;

    arg0->presentation.layout->unk_34 = D_843847BC[var_v0].unk_0C;
    arg0->presentation.layout->unk_38 = D_843847BC[var_v0].unk_10;
    arg0->presentation.layout->unk_2C = D_843847BC[var_v0].unk_08;
}

void BattleScene_SetupOwnerCameraFraming(Battler* arg0, Battler* arg1) {
    if (gBattleScene.scene->unk_1C == 1) {
        BattleScene_SetBothOwnerActiveFlags();
    } else {
        BattleScene_ClearBothOwnerActiveFlags();
    }
    BattleScene_LoadOwnerCameraFramingConstants(arg0);
    BattleScene_LoadOwnerCameraFramingConstants(arg1);
}

void func_84307630(void) {
}

void BattleScene_ApplyOwnerCameraFraming(Battler* arg0) {
    s32 temp_v0;
    PresentationLayout* sp30;
    f32 tmp;
    f32 tmp2;

    sp30 = arg0->presentation.layout;
    temp_v0 = BattleScene_GetParticipantSideIndex(arg0);

    arg0->presentation.cameraYaw = D_84384578[temp_v0]->rotationOffset;
    if (D_84384578[temp_v0]->chainIndex == 0xFF) {
        arg0->presentation.cameraPitch = D_84384578[temp_v0]->rotationScale * arg0->presentation.cameraConfigIndex;
        tmp = D_84384578[temp_v0]->positionOffset.z * arg0->presentation.cameraConfigIndex;
    } else {
        arg0->presentation.cameraPitch = D_84384578[temp_v0]->rotationScale;
        tmp = D_84384578[temp_v0]->positionOffset.z;
    }

    arg0->presentation.cameraOffsetX = tmp;
    arg0->presentation.cameraDistance = D_84384578[temp_v0]->positionOffset.x;
    arg0->presentation.animationFrame = D_84384578[temp_v0]->animationFrame;
    tmp2 = D_84384578[temp_v0]->positionOffset.y;
    arg0->presentation.cameraOffsetZ = tmp2;

    BattleScene_SetCameraAtAndEyeFromAngles(sp30, arg0->presentation.model.unk_024.x + arg0->presentation.cameraOffsetX, arg0->presentation.model.unk_024.y + tmp2,
                  arg0->presentation.model.unk_024.z, arg0->presentation.cameraDistance, arg0->presentation.cameraYaw, arg0->presentation.cameraPitch);
}

void BattleScene_TickOwnerCameraFraming(Battler* arg0) {
    BattleScene_ApplyOwnerCameraFraming(arg0);
}

void BattleScene_InitializeOwnerCameraFraming(unk_D_86002F34_00C* arg0, Battler* arg1, unk_D_86002F34_00C* arg2, Battler* arg3) {
    arg1->presentation.layout = arg0;
    arg3->presentation.layout = arg2;
    BattleScene_SetupOwnerCameraFraming(arg1, arg3);
    BattleScene_TickOwnerCameraFraming(arg1);
    BattleScene_TickOwnerCameraFraming(arg3);
}

void BattleScene_TickOwnerSlideIn(Battler* arg0, s32 arg1) {
    s16 var_a0;
    PresentationLayout* temp_v0;

    arg0->presentation.layout->unk_00.unk_01 |= 1;
    arg0->presentation.model.unk_000.unk_01 |= 1;

    if (arg0->presentation.cameraConfigIndex == 1) {
        if ((arg0->presentation.layout->hudX == -0x48) && !(arg0->unk_654.battleStateFlags & 0x800)) {
            BattleScene_TriggerOwnerModelAnimation(arg0);
        }

        if ((ModelRenderer_IsReady(0) != 0) && (arg0->presentation.modelReadyState == 0)) {
            arg0->presentation.modelReadyState = 2;
            ModelRenderer_OnSlotReady(0);
        }
        arg0->presentation.layout->hudX = Math_StepToS32(arg0->presentation.layout->hudX, 0x17, arg1, arg1);
    } else {
        if ((arg0->presentation.layout->hudX == 0x15F) && !(arg0->unk_654.battleStateFlags & 0x800)) {
            BattleScene_TriggerOwnerModelAnimation(arg0);
        }

        arg0->presentation.layout->hudX = Math_StepToS32(arg0->presentation.layout->hudX, 0x103, arg1, arg1);

        if ((ModelRenderer_IsReady(1) != 0) && (arg0->presentation.modelReadyState == 0)) {
            arg0->presentation.modelReadyState = 2;
            ModelRenderer_OnSlotReady(1);
        }
    }
}

void BattleScene_TickOwnerSlideOut(Battler* arg0, s32 arg1) {
    if (arg0->presentation.cameraConfigIndex == 1) {
        arg0->presentation.modelReadyState = 0;
        arg0->presentation.layout->hudX = Math_StepToS32(arg0->presentation.layout->hudX, -0x48, arg1, arg1);
        if (arg0->presentation.layout->hudX == -0x48) {
            arg0->presentation.layout->unk_00.unk_01 &= ~1;
            arg0->presentation.model.unk_000.unk_01 &= ~1;
        }
    } else {
        arg0->presentation.modelReadyState = 0;
        arg0->presentation.layout->hudX = Math_StepToS32(arg0->presentation.layout->hudX, 0x15F, arg1, arg1);
        if (arg0->presentation.layout->hudX == 0x15F) {
            arg0->presentation.layout->unk_00.unk_01 &= ~1;
            arg0->presentation.model.unk_000.unk_01 &= ~1;
        }
    }
}

void BattleScene_TickOwnerSlideAnimation(Battler* arg0) {
    switch (arg0->presentation.visible) {
        case 0:
            BattleScene_TickOwnerSlideOut(arg0, 9);
            break;

        case 1:
            BattleScene_TickOwnerSlideIn(arg0, 9);
            break;
    }
}

void BattleScene_TickBothOwnerSlideAnimations(Battler* arg0, Battler* arg1) {
    if (D_800AE540.sessionMode != 0x10) {
        BattleScene_TickOwnerSlideAnimation(arg0);
        BattleScene_TickOwnerSlideAnimation(arg1);
    }
}

s32 BattleScene_DispatchOwnerCameraLifecycle(s32 arg0, unk_D_86002F34_00C* arg1, unk_D_86002F34_00C* arg2) {
    switch (arg0) {
        case 0:
            func_84307630();
            break;

        case 1:
        case 2:
            BattleScene_TickBothOwnerSlideAnimations(D_84390010[0], D_84390010[1]);
            break;

        case 3:
            BattleScene_InitializeOwnerCameraFraming(arg1, D_84390010[0], arg2, D_84390010[1]);
            break;
    }
    return 0;
}
