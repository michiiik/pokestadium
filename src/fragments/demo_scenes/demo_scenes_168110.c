#include "demo_scenes.h"
#include "src/matrix.h"

typedef struct CameraFollowModelIds {
    u8 unk_00[1];
} CameraFollowModelIds; // size = ??

static CameraFollowModelIds gCameraFollowModelIds = { 0x8E };

s16 Intro_GetTrackedModelFacingOffset(void) {
    return gCameraTarget->unk_004.rotation.y + 0x8000;
}

s32 Util_ByteInArray(u8 arg0, u8* arg1, s32 arg2) {
    while (arg2--) {
        if (arg0 == *arg1) {
            return 1;
        }
        arg1++;
    }
    return 0;
}

void Intro_ComputeFollowPoint(unk_D_86B0FA78* arg0, Vec3f* arg1) {
    f32 sp24 = 0.0f;
    CameraFollowModelIds sp20 = gCameraFollowModelIds;

    if (Util_ByteInArray(gCameraTarget->unk_174, sp20.unk_00, 1) != 0) {
        sp24 = gMonsterStageData.unk_04 / 1.5f;
    }

    if (gMonsterStageData.unk_08 != 0) {
        arg1->x = arg0->unk_004.position.x;
        arg1->y = arg0->unk_004.position.y + sp24;
        arg1->z = arg0->unk_004.position.z;
    } else {
        arg1->x = arg0->unk_004.position.x;
        arg1->y = arg0->unk_004.position.y + gMonsterStageData.unk_04;
        arg1->z = arg0->unk_004.position.z;
    }

    if (arg1->y < 0) {
        arg1->y = gMonsterStageData.unk_04;
    }
}

void Intro_UpdateOrbitCamera(void) {
    Camera_ComputeEyeFromAngles(&gCameraState->unk_04, &gCameraState->unk_00->view.targetPosition, gCameraState->unk_1C, gCameraState->unk_10,
                  Intro_GetTrackedModelFacingOffset() + gCameraState->unk_12);
    Camera_ComputeEyeFromAngles(&gCameraState->unk_04, &gCameraState->unk_00->view.eyePosition, gCameraState->unk_20, gCameraState->unk_16,
                  Intro_GetTrackedModelFacingOffset() + gCameraState->unk_18);

    gCameraState->unk_00->view.targetPosition.y += gMonsterStageData.unk_0C;
    if (gCameraState->unk_00->view.eyePosition.y <= 10.0f) {
        gCameraState->unk_00->view.eyePosition.y = 10.0f;
    }
}

f32 Intro_ClampOrbitDistance(f32 arg0, f32 arg1) {
    f32 temp_fv1 = gMonsterStageData.unk_00 * arg0;

    if (arg1 < temp_fv1) {
        return arg1;
    }
    return temp_fv1;
}

void IntroCamKeyframe_SetAngles(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    gCameraState->unk_10 = arg0;
    gCameraState->unk_12 = arg1;
    gCameraState->unk_24 = arg2;
    gCameraState->unk_26 = arg3;
}

void IntroCamKeyframe_SetDistance(f32 arg0, f32 arg1) {
    gCameraState->unk_1C = arg0;
    gCameraState->unk_30 = arg1;
}

void IntroCamKeyframe_SetAtAngles(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    gCameraState->unk_16 = arg0;
    gCameraState->unk_18 = arg1;
    gCameraState->unk_2A = arg2;
    gCameraState->unk_2C = arg3;
}

void IntroCamKeyframe_SetAtDistance(f32 arg0, f32 arg1) {
    gCameraState->unk_20 = arg0;
    gCameraState->unk_34 = arg1;
}

void IntroCamKeyframe_SetTransitionSpeed(f32 arg0, f32 arg1) {
    gCameraState->unk_38 = arg0;
    gCameraState->unk_3C = arg1;
}

void Intro_LoadCameraKeyframe(DioramaAnimData* arg0, CameraNode* arg1) {
    f32 sp34;
    f32 sp30;
    f32 sp2C;
    f32 sp28;

    gCameraState->unk_44 = 0.0f;
    if (arg0->unk_30 != 200.0f) {
        arg1->perspective.verticalFieldOfView = gCameraState->unk_40 = arg0->unk_30;
    } else {
        gCameraState->unk_40 = arg0->unk_30;
    }

    sp34 = Intro_ClampOrbitDistance(arg0->unk_0C, 500.0f);
    sp30 = Intro_ClampOrbitDistance(arg0->unk_20, 500.0f);
    sp2C = Intro_ClampOrbitDistance(arg0->unk_10, 500.0f);
    sp28 = Intro_ClampOrbitDistance(arg0->unk_24, 500.0f);

    IntroCamKeyframe_SetAngles(arg0->unk_00, Intro_GetTrackedModelFacingOffset() + arg0->unk_02, arg0->unk_14, Intro_GetTrackedModelFacingOffset() + arg0->unk_16);
    IntroCamKeyframe_SetDistance(sp34, sp30);

    IntroCamKeyframe_SetAtAngles(arg0->unk_06, Intro_GetTrackedModelFacingOffset() + arg0->unk_08, arg0->unk_1A, Intro_GetTrackedModelFacingOffset() + arg0->unk_1C);
    IntroCamKeyframe_SetAtDistance(sp2C, sp28);
    IntroCamKeyframe_SetTransitionSpeed(arg0->unk_28, arg0->unk_2C);
}

s32 Intro_UpdateCameraKeyframe(CameraNode* arg0) {
    Vec3f sp5C;
    Vec3f sp50;
    Vec3f sp44;
    Vec3f sp38;
    f32 sp34;
    f32 sp30;
    s16 sp2E;
    s16 sp2C;

    if (gCameraState->unk_40 != 200.0f) {
        arg0->perspective.verticalFieldOfView = gCameraState->unk_40;
    } else {
        Math_EaseTowardF(&arg0->perspective.verticalFieldOfView, 30.0f, 0.06f);
    }

    Math_EaseTowardF(&gCameraState->unk_44, gCameraState->unk_38, gCameraState->unk_3C);
    Camera_ComputeEyeFromAngles(&gCameraState->unk_04, &sp50, gCameraState->unk_34, gCameraState->unk_2A,
                  Intro_GetTrackedModelFacingOffset() + gCameraState->unk_2C);

    if (sp50.y <= 10.0f) {
        sp50.y = 10.0f;
    }

    Vec3f_Copy(&sp44, &arg0->view.eyePosition);
    Vec3f_CalculateDistanceAngles(&sp44, &sp50, &sp34, &sp2E, &sp2C);
    Camera_ComputeEyeFromAngles(&sp44, &arg0->view.eyePosition, gCameraState->unk_44 * sp34, sp2E, sp2C);
    Camera_ComputeEyeFromAngles(&gCameraState->unk_04, &sp38, gCameraState->unk_30, gCameraState->unk_24,
                  Intro_GetTrackedModelFacingOffset() + gCameraState->unk_26);

    sp38.y += gMonsterStageData.unk_0C;

    Vec3f_Copy(&sp5C, &arg0->view.targetPosition);
    Vec3f_CalculateDistanceAngles(&sp5C, &sp38, &sp30, &sp2E, &sp2C);
    Camera_ComputeEyeFromAngles(&sp5C, &arg0->view.targetPosition, gCameraState->unk_44 * sp30, sp2E, sp2C);

    if ((sp34 <= 1.0f) && (sp30 <= 1.0f)) {
        return 1;
    }
    return 0;
}
