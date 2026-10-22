#include "gallery_camera.h"
#include "src/matrix.h"
#include "src/stage_loader.h"
#include "src/fragments/minigame_actor_framework/minigame_actor_framework.h"

static s16 gCameraRotationY;
static s16 gCameraRotationX;
static f32 gCameraDistance;
static s16 gCameraRotationXTemp;
static s16 gCameraRotationYTemp;
static f32 gHorizontalDist;
static f32 gGalleryCameraTimer;
static Vec3f gCameraInitialPos;

f32 GalleryCamera_Distance3D(Vec3f arg0, Vec3f arg1) {
    return sqrtf(SQ(arg1.x - arg0.x) + SQ(arg1.y - arg0.y) + SQ(arg1.z - arg0.z));
}

f32 GalleryCamera_ComputeZoomDistance(f32 arg0) {
    f32 temp_fv1 = (gCameraConfig.unk_00 + gCameraZoomOffset) * arg0;

    if (temp_fv1 > 400.0f) {
        return 400.0f;
    }
    return temp_fv1;
}

f32 GalleryCamera_DistanceXZ(Vec3f arg0, Vec3f arg1) {
    return sqrtf(SQ(arg1.x - arg0.x) + SQ(arg1.z - arg0.z));
}

void GalleryCamera_NullUpdate(void) {
}

void GalleryCamera_InitFreeView(void) {
    gMiniCameraPtr = gMiniCameraStruct->node.children;
    gCameraRotationY = 0;
    gCameraRotationX = 0;
    gCameraDistance = GalleryCamera_ComputeZoomDistance(4.0f);

    if (gCameraConfig.unk_08 != 0.0f) {
        gMiniCameraPtr->view.targetPosition.y = gCameraConfig.unk_08;
    } else {
        gMiniCameraPtr->view.targetPosition.y = gCameraConfig.unk_04 + gCameraConfig.unk_0C;
    }

    Camera_ComputeEyeFromAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, gCameraDistance, gCameraRotationY, gCameraRotationX);

    if (gMiniCameraPtr->view.eyePosition.y < 12.0f) {
        gMiniCameraPtr->view.eyePosition.y = 12.5f;
        Vec3f_CalculateDistanceAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, &gCameraDistance, &gCameraRotationY, &gCameraRotationX);
    }

    gMiniCameraPtr->perspective.nearPlane = 20.0f;
    gMiniCameraPtr->perspective.farPlane = 10000.0f;
    gMiniCameraPtr->perspective.verticalFieldOfView = 40.0f;
    gCameraParams->unk_008 = 40.0f;

    gGalleryCameraTimer = 0.0f;

    Vec3f_SetComponentsDuplicate(&gCameraInitialPos, gMiniCameraPtr->view.targetPosition.x, gMiniCameraPtr->view.targetPosition.y, gMiniCameraPtr->view.targetPosition.z);
}

void GalleryCamera_DebugAdjustZoomOffset(void) {
    f32 temp_fv1;

    if (gPlayer2Controller->stickMag != 0.0f) {
        if (gPlayer2Controller->stickY >= 0.0f) {
            gCameraZoomOffset += gPlayer2Controller->stickMag / 400.0f;
        } else {
            gCameraZoomOffset -= gPlayer2Controller->stickMag / 400.0f;
        }

        temp_fv1 = 10.0f - gCameraConfig.unk_00;
        if (gCameraZoomOffset < temp_fv1) {
            gCameraZoomOffset = temp_fv1;
        }

        if (gCameraZoomOffset > 60.0f) {
            gCameraZoomOffset = 60.0f;
        }
    }

    HAL_Printf(0x14, 0x14, "OFFSET %6.2f", gCameraZoomOffset);
}

void GalleryCamera_ResetFreeView(void) {
    gCameraRotationY = 0;
    gCameraRotationX = 0;
    gCameraDistance = GalleryCamera_ComputeZoomDistance(4.0f);
    gMiniCameraPtr->view.targetPosition.x = 0.0f;
    gMiniCameraPtr->view.targetPosition.z = 0.0f;

    if (gCameraConfig.unk_08 != 0) {
        gMiniCameraPtr->view.targetPosition.y = gCameraConfig.unk_08;
    } else {
        gMiniCameraPtr->view.targetPosition.y = gCameraConfig.unk_04 + gCameraConfig.unk_0C;
    }

    Camera_ComputeEyeFromAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, gCameraDistance, gCameraRotationY, gCameraRotationX);

    if (gMiniCameraPtr->view.eyePosition.y < 12.0f) {
        gMiniCameraPtr->view.eyePosition.y = 12.5f;
        Vec3f_CalculateDistanceAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, &gCameraDistance, &gCameraRotationY, &gCameraRotationX);
    }

    gMiniCameraPtr->perspective.nearPlane = 20.0f;
    gMiniCameraPtr->perspective.farPlane = 10000.0f;
    gMiniCameraPtr->perspective.verticalFieldOfView = 40.0f;
    gCameraParams->unk_008 = 40.0f;
    gGalleryCameraTimer = 0.0f;

    Vec3f_SetComponentsDuplicate(&gCameraInitialPos, gMiniCameraPtr->view.targetPosition.x, gMiniCameraPtr->view.targetPosition.y, gMiniCameraPtr->view.targetPosition.z);
}

s32 GalleryCamera_UpdateZoomFov(void) {
    if (gControllerInput.buttonPressed & 0x10) {
        if (gCameraParams->unk_008 == 25.0f) {
            gCameraParams->unk_008 = 40.0f;
        } else {
            gCameraParams->unk_008 = 25.0f;
        }
    }

    gMiniCameraPtr->perspective.verticalFieldOfView = Math_StepToF(gMiniCameraPtr->perspective.verticalFieldOfView, gCameraParams->unk_008, 1.5f, 1.5f);

    if (gMiniCameraPtr->perspective.verticalFieldOfView == gCameraParams->unk_008) {
        return 0;
    }
    return 1;
}

void GalleryCamera_RotateVecByMatrix(Vec3f* arg0, MtxF* arg1) {
    f32 x = (arg1->mf[0][0] * arg0->x) + (arg1->mf[0][1] * arg0->y) + (arg1->mf[0][2] * arg0->z);
    f32 y = (arg1->mf[1][0] * arg0->x) + (arg1->mf[1][1] * arg0->y) + (arg1->mf[1][2] * arg0->z);
    f32 z = (arg1->mf[2][0] * arg0->x) + (arg1->mf[2][1] * arg0->y) + (arg1->mf[2][2] * arg0->z);

    arg0->x = x;
    arg0->y = y;
    arg0->z = z;
}

void GalleryCamera_OrbitAtPoint(s16 arg0) {
    MtxF sp70;
    Vec3s sp68;
    Vec3f sp5C;
    Vec3f sp50;
    UNUSED s32 pad[3];
    Vec3f sp38;
    UNUSED s32 pad2;

    Vec3f_SetComponentsDuplicate(&sp38, 0, 0.0f, 0.0f);
    Vec3f_CalculateDistanceAngles(&sp38, &gMiniCameraPtr->view.eyePosition, &gHorizontalDist, &gCameraRotationXTemp, &gCameraRotationYTemp);
    Vec3f_SetComponentsDuplicate(&sp50, gMiniCameraPtr->view.targetPosition.x, 0.0f, gMiniCameraPtr->view.targetPosition.z);
    Vec3s_SetComponents(&sp68, 0, gCameraRotationYTemp + arg0, 0);
    Vec3f_SetComponentsDuplicate(&sp5C, gCameraParams->unk_004 * 4.0f, 0.0f, 0.0f);
    MtxF_SetRotationAndTransformTranslation(&sp70, &gMiniCameraPtr->view.targetPosition, &sp68);
    GalleryCamera_RotateVecByMatrix(&sp5C, &sp70);

    sp50.x += sp5C.x;
    sp50.z += sp5C.z;

    gMiniCameraPtr->view.targetPosition.x = sp50.x;
    gMiniCameraPtr->view.targetPosition.z = sp50.z;

    Vec3f_CalculateDistanceAngles(&sp38, &gMiniCameraPtr->view.targetPosition, &gHorizontalDist, &gCameraRotationXTemp, &gCameraRotationYTemp);
    gHorizontalDist = GalleryCamera_DistanceXZ(sp38, gMiniCameraPtr->view.targetPosition);
    if ((GalleryCamera_ComputeZoomDistance(1.9f) - 1.0f) <= gHorizontalDist) {
        Camera_ComputeEyeFromAngles(&sp38, &sp50, GalleryCamera_ComputeZoomDistance(1.9f) - 1.0f, 0, gCameraRotationYTemp);
        gMiniCameraPtr->view.targetPosition.x = sp50.x;
        gMiniCameraPtr->view.targetPosition.z = sp50.z;
    }
    Vec3f_CalculateDistanceAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, &gCameraDistance, &gCameraRotationY, &gCameraRotationX);
}

void GalleryCamera_HandleDPadInput(void) {
    if (((gControllerInput.buttonDown & 8) | (gControllerInput.buttonDown & 4) | (gControllerInput.buttonDown & 2) |
         (gControllerInput.buttonDown & 1)) != 0) {
        gCameraParams->unk_004 = Math_StepToF(gCameraParams->unk_004, 1.0f, 0.2f, 0.2f);
    } else {
        gCameraParams->unk_004 = Math_StepToF(gCameraParams->unk_004, 0.0f, 0.2f, 0.2f);
    }

    if ((gControllerInput.buttonDown & 8) && (gMiniCameraPtr->view.targetPosition.y < 300.0f)) {
        gMiniCameraPtr->view.targetPosition.y += 4.0f * gCameraParams->unk_004;
        Vec3f_CalculateDistanceAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, &gCameraDistance, &gCameraRotationY, &gCameraRotationX);
        if (gCameraRotationY < -0x38E3) {
            gMiniCameraPtr->view.targetPosition.y -= 4.0f * gCameraParams->unk_004;
        }
        Vec3f_CalculateDistanceAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, &gCameraDistance, &gCameraRotationY, &gCameraRotationX);
    } else if ((gControllerInput.buttonDown & 4) && (gMiniCameraPtr->view.targetPosition.y > 5.0f)) {
        gMiniCameraPtr->view.targetPosition.y -= 4.0f * gCameraParams->unk_004;
        Vec3f_CalculateDistanceAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, &gCameraDistance, &gCameraRotationY, &gCameraRotationX);
        if (gCameraRotationY >= 0x38E4) {
            gMiniCameraPtr->view.targetPosition.y += 4.0f * gCameraParams->unk_004;
        }
        Vec3f_CalculateDistanceAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, &gCameraDistance, &gCameraRotationY, &gCameraRotationX);
    }

    if (gMiniCameraPtr->view.targetPosition.y > 200.0f) {
        gMiniCameraPtr->view.targetPosition.y = 200.0f;
        Vec3f_CalculateDistanceAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, &gCameraDistance, &gCameraRotationY, &gCameraRotationX);
    }

    if (gMiniCameraPtr->view.targetPosition.y < 5.0f) {
        gMiniCameraPtr->view.targetPosition.y = 5.0f;
        Vec3f_CalculateDistanceAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, &gCameraDistance, &gCameraRotationY, &gCameraRotationX);
    }

    if (gControllerInput.buttonDown & 1) {
        GalleryCamera_OrbitAtPoint(0);
    } else if (gControllerInput.buttonDown & 2) {
        GalleryCamera_OrbitAtPoint(-0x8000);
    }
}

void GalleryCamera_ClampToBoundaryCircle(Vec3f* arg0, Vec3f* arg1, Vec3f arg2, f32 arg3, s32 arg4) {
    Vec3f sp94;
    Vec3f sp88;
    Vec3f sp7C;
    Vec3f sp70;
    f32 sp6C;
    f32 sp68;
    f32 tmp;
    f32 sp3C;
    f32 sp38;
    f32 sp34;
    f32 sp54;

    sp70 = gCameraClampCenter;

    if (arg0->x == arg1->x) {
        sp7C.x = arg0->x;
        if (arg0->z < 0.0) {
            sp7C.z = -arg3;
        } else {
            sp7C.z = arg3;
        }
    } else {
        sp6C = (arg1->z - arg0->z) / (arg1->x - arg0->x);
        sp68 = arg1->z - (arg1->x * sp6C);

        sp54 = (((SQ(arg2.x) + SQ(sp68)) - ((2 * arg2.z) * sp68)) + SQ(arg2.z)) - SQ(arg3);

        sp38 = SQ(sp6C) + 1.0f;
        sp3C = (((2 * sp6C) * sp68) - (2 * arg2.x)) - ((2 * arg2.z) * sp6C);

        if (SQ(sp3C) < ((4 * sp38) * sp54)) {
            Vec3f_SetComponentsDuplicate(arg1, arg0->x, arg0->y, arg0->z);
            return;
        }

        sp34 = SQ(sp3C) - ((4 * sp38) * sp54);

        sp94.x = (-sp3C - sqrtf(sp34)) / (2 * sp38);
        sp94.y = 0.0f;
        sp94.z = (sp6C * sp94.x) + sp68;

        sp88.x = (sqrtf(sp34) + -sp3C) / (2 * sp38);
        sp88.y = 0.0f;
        sp88.z = (sp6C * sp88.x) + sp68;

        if (GalleryCamera_DistanceXZ(*arg0, sp94) > GalleryCamera_DistanceXZ(*arg0, sp88)) {
            sp7C.x = sp88.x;
            sp7C.z = sp88.z;
        } else {
            sp7C.x = sp94.x;
            sp7C.z = sp94.z;
        }
    }

    sp6C = arg0->y - arg1->y;
    if (sp6C == 0.0f) {
        sp7C.y = arg1->y;
    } else {
        if (arg1->x == arg0->x) {
            tmp = (sp7C.z - arg0->z) / (arg1->z - arg0->z);
        } else {
            tmp = (sp7C.x - arg0->x) / (arg1->x - arg0->x);
        }

        if (arg4 == 1) {
            sp7C.y = ((arg1->y - arg0->y) * tmp) + arg0->y;
        } else {
            sp7C.y = ((arg0->y - arg1->y) * tmp) + arg1->y;
        }
    }

    Vec3f_SetComponentsDuplicate(arg0, arg1->x, arg1->y, arg1->z);
    Vec3f_SetComponentsDuplicate(arg1, sp7C.x, sp7C.y, sp7C.z);
    Vec3f_CalculateDistanceAngles(&gMiniCameraPtr->view.targetPosition, arg1, &gCameraDistance, &gCameraRotationY, &gCameraRotationX);
}

void GalleryCamera_ClampAtToBoundaryCircle(Vec3f* arg0, Vec3f* arg1, Vec3f arg2, f32 arg3) {
    Vec3f sp84;
    Vec3f sp78;
    Vec3f tmp;
    Vec3f sp60;
    f32 sp5C;
    f32 sp58;
    f32 sp40;
    f32 sp3C;
    f32 sp38;
    f32 sp2C;
    f32 sp44;

    sp60 = gCameraClampCenterMode;
    sp2C = arg1->x;
    tmp.z = arg3;
    if (gMiniCameraPtr->view.targetPosition.x == sp2C) {
        tmp.x = gMiniCameraPtr->view.targetPosition.x;
    } else {
        sp5C = (arg1->z - gMiniCameraPtr->view.targetPosition.z) / (arg1->x - gMiniCameraPtr->view.targetPosition.x);
        sp58 = arg1->z - (arg1->x * sp5C);

        sp44 = (((SQ(arg2.x) + SQ(sp58)) - ((2 * arg2.z) * sp58)) + SQ(arg2.z)) - SQ(arg3);

        sp3C = SQ(sp5C) + 1.0f;
        sp38 = ((2 * sp5C * sp58) - (2 * arg2.x)) - ((2 * arg2.z) * sp5C);

        sp2C = SQ(sp38) - ((4 * sp3C) * sp44);

        sp84.x = (-sp38 - sqrtf(sp2C)) / (2 * sp3C);
        sp84.y = 0.0f;
        sp84.z = (sp5C * sp84.x) + sp58;

        sp78.x = (sqrtf(sp2C) + -sp38) / (2 * sp3C);
        sp78.y = 0.0f;
        sp78.z = (sp5C * sp78.x) + sp58;

        if (GalleryCamera_DistanceXZ(*arg1, sp84) > GalleryCamera_DistanceXZ(*arg1, sp78)) {
            tmp.x = sp78.x;
            tmp.z = sp78.z;
        } else {
            tmp.x = sp84.x;
            tmp.z = sp84.z;
        }
    }

    tmp.y = arg0->y;

    Vec3f_SetComponentsDuplicate(arg1, tmp.x, tmp.y, tmp.z);
    Vec3f_CalculateDistanceAngles(&gMiniCameraPtr->view.targetPosition, arg1, &gCameraDistance, &gCameraRotationY, &gCameraRotationX);
}

void GalleryCamera_ClampHeight(Vec3f* arg0, Vec3f* arg1, f32 arg2, f32 arg3, s32 arg4) {
    f32 temp_fa0;

    arg2 += arg3;

    if (arg4 == 1) {
        temp_fa0 = (arg2 - arg0->y) / (arg1->y - arg0->y);
        if (arg1->x == arg0->x) {
            arg1->x = arg0->x;
        } else {
            arg1->x = ((arg1->x - arg0->x) * temp_fa0) + arg0->x;
        }

        if (arg1->z == arg0->z) {
            arg1->z = arg0->z;
        } else {
            arg1->z = ((arg1->z - arg0->z) * temp_fa0) + arg0->z;
        }
    }

    arg1->y = arg2;
    Vec3f_CalculateDistanceAngles(&gMiniCameraPtr->view.targetPosition, arg1, &gCameraDistance, &gCameraRotationY, &gCameraRotationX);
}

void GalleryCamera_ClampEyeToPanBounds(Vec3f* arg0, Vec3f* arg1) {
    UNUSED s32 pad[2];
    Vec3f sp3C = gCameraConstraintCenter;
    f32 sp38;

    if (arg1->y < 12.0f) {
        GalleryCamera_ClampHeight(arg0, arg1, 12.0f, 0.05f, 0);
    }

    if (arg1->y > 350.0f) {
        GalleryCamera_ClampHeight(arg0, arg1, 350.0f, 0.0f, 0);
    }

    sp38 = GalleryCamera_ComputeZoomDistance(1.9f);
    if (GalleryCamera_DistanceXZ(sp3C, *arg1) < sp38) {
        GalleryCamera_ClampToBoundaryCircle(arg0, arg1, sp3C, GalleryCamera_ComputeZoomDistance(1.9f) + 0.05f, 1);
    }

    if (GalleryCamera_DistanceXZ(sp3C, *arg1) > 400.0f) {
        GalleryCamera_ClampToBoundaryCircle(arg0, arg1, sp3C, 399.0f, 1);
    }

    if (GalleryCamera_DistanceXZ(gMiniCameraPtr->view.targetPosition, *arg1) < 30.0f) {
        GalleryCamera_ClampToBoundaryCircle(arg0, arg1, gMiniCameraPtr->view.targetPosition, 30.1f, 1);
    }
}

void GalleryCamera_ClampOrbitBounds(Vec3f* arg0, Vec3f* arg1, s32 arg2) {
    UNUSED s32 pad[2];
    Vec3f sp3C = gCameraConstraintCenterMode;
    f32 sp38;

    if (arg2 == 0) {
        if (arg1->y < 12.0f) {
            GalleryCamera_ClampHeight(arg0, arg1, 12.0f, 0.05f, 1);
        }

        if (arg1->y > 350.0f) {
            GalleryCamera_ClampHeight(arg0, arg1, 350.0f, 0.0f, 1);
        }
    } else {
        if (arg1->y < 12.0f) {
            GalleryCamera_ClampHeight(arg0, arg1, 12.0f, 0.05f, 0);
        }

        if (arg1->y > 350.0f) {
            GalleryCamera_ClampHeight(arg0, arg1, 350.0f, 0.0f, 0);
        }
    }

    if (arg2 == 0) {
        sp38 = GalleryCamera_ComputeZoomDistance(1.9f);
        if (GalleryCamera_DistanceXZ(sp3C, *arg1) < sp38) {
            GalleryCamera_ClampToBoundaryCircle(arg0, arg1, sp3C, GalleryCamera_ComputeZoomDistance(1.9f) + 0.05f, 0);
        }

        if (GalleryCamera_DistanceXZ(sp3C, *arg1) > 400.0f) {
            GalleryCamera_ClampToBoundaryCircle(arg0, arg1, sp3C, 399.0f, 0);
        }

        if (GalleryCamera_DistanceXZ(gMiniCameraPtr->view.targetPosition, *arg1) < 30.0f) {
            GalleryCamera_ClampToBoundaryCircle(arg0, arg1, gMiniCameraPtr->view.targetPosition, 30.1f, 0);
        }
    } else {
        sp38 = GalleryCamera_ComputeZoomDistance(1.9f);
        if (GalleryCamera_DistanceXZ(sp3C, *arg1) < sp38) {
            GalleryCamera_ClampAtToBoundaryCircle(arg0, arg1, sp3C, GalleryCamera_ComputeZoomDistance(1.9f) + 0.05f);
        }

        if (GalleryCamera_DistanceXZ(sp3C, *arg1) > 400.0f) {
            GalleryCamera_ClampAtToBoundaryCircle(arg0, arg1, sp3C, 399.0f);
        }

        if (GalleryCamera_DistanceXZ(gMiniCameraPtr->view.targetPosition, *arg1) < 30.0f) {
            GalleryCamera_ClampAtToBoundaryCircle(arg0, arg1, gMiniCameraPtr->view.targetPosition, 30.1f);
        }
    }
}

void GalleryCamera_HandleFreeMoveInput(void) {
    Vec3f sp44;
    Vec3f sp38;
    UNUSED s32 pad;
    f32 mag;

    if (gControllerInput.stickMag != 0.0f) {
        Vec3f_SetComponentsDuplicate(&sp44, gMiniCameraPtr->view.eyePosition.x, gMiniCameraPtr->view.eyePosition.y, gMiniCameraPtr->view.eyePosition.z);
        if (gControllerInput.stickY >= 0.0f) {
            mag = -(gControllerInput.stickMag / 6.0f);
            Camera_ComputeEyeFromAngles(&gMiniCameraPtr->view.eyePosition, &sp38, mag, gCameraRotationY, gCameraRotationX);
        } else {
            mag = gControllerInput.stickMag / 6.0f;
            Camera_ComputeEyeFromAngles(&gMiniCameraPtr->view.eyePosition, &sp38, mag, gCameraRotationY, gCameraRotationX);
        }
        gCameraDistance = GalleryCamera_Distance3D(gMiniCameraPtr->view.targetPosition, sp38);
        GalleryCamera_ClampEyeToPanBounds(&sp44, &sp38);
        Vec3f_SetComponentsDuplicate(&gMiniCameraPtr->view.eyePosition, sp38.x, sp38.y, sp38.z);
    }

    if (gControllerInput.buttonDown & 0x200) {
        gCameraRotationX -= 0x100;
    }

    if (gControllerInput.buttonDown & 0x100) {
        gCameraRotationX += 0x100;
    }

    if (gControllerInput.buttonDown & 0x300) {
        Vec3f_SetComponentsDuplicate(&sp44, gMiniCameraPtr->view.eyePosition.x, gMiniCameraPtr->view.eyePosition.y, gMiniCameraPtr->view.eyePosition.z);
        Camera_ComputeEyeFromAngles(&gMiniCameraPtr->view.targetPosition, &sp38, gCameraDistance, gCameraRotationY, gCameraRotationX);
        GalleryCamera_ClampOrbitBounds(&sp44, &sp38, 1);
        Vec3f_SetComponentsDuplicate(&gMiniCameraPtr->view.eyePosition, sp38.x, sp38.y, sp38.z);
    }

    if (gControllerInput.buttonDown & 0x800) {
        gCameraRotationY += 0x100;
    }

    if (gControllerInput.buttonDown & 0x400) {
        gCameraRotationY -= 0x100;
    }

    if (gControllerInput.buttonDown & 0xC00) {
        Vec3f_SetComponentsDuplicate(&sp44, gMiniCameraPtr->view.eyePosition.x, gMiniCameraPtr->view.eyePosition.y, gMiniCameraPtr->view.eyePosition.z);
        Camera_ComputeEyeFromAngles(&gMiniCameraPtr->view.targetPosition, &sp38, gCameraDistance, gCameraRotationY, gCameraRotationX);
        GalleryCamera_ClampOrbitBounds(&sp44, &sp38, 0);
        Vec3f_SetComponentsDuplicate(&gMiniCameraPtr->view.eyePosition, sp38.x, sp38.y, sp38.z);
    }
}
