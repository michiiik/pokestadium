#include "minigame_actor_framework.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/matrix.h"

s16 miniInputLockTimer;
s16 gMiniGameIdleTimer;
s16 miniShowHUB;
s16 miniDifficulty;
s16 gMiniGameResetOnlyFlag; //  unused?
s16 gMetapodAudioMuted;
s32 pad_D_8790604C; //	unused?
CameraNode* gMiniCameraStruct;
CameraNode* gMiniCameraPtr;
s32 pad_D_87906058[2]; //	unused?
s16 miniCameraXRot;
s16 miniCameraYRot;
s16 miniCameraDistance;
s16 miniCameraFov;
s16 miniCameraNear;
s16 miniCameraFar;
Vec3s miniCameraCoords;
s16 gMiniWinCamPhase;
s16 pad_D_87906074;
s16 gMiniCameraXRotBackup;
s16 gMiniCameraYRotBackup;
f32 gMiniCameraDistanceBackup;
f32 gMiniCameraFovBackup;
s32 pad_D_87906084;
Vec3f gMiniCameraCoordsBackup;
s16 gMiniCamRotXDelta;
s16 gMiniCamRotYDelta;
s16 gMiniCamDistDelta;
s16 gMiniCamFovDelta;
Vec3s gMiniCamPosDelta;
s16 gMiniCamBaseRotX;
s16 gMiniCamBaseRotY;
s16 gMiniCamBaseDist;
s16 gMiniCamBaseFov;
Vec3s gMiniCamBasePos;
s32 pad_D_879060B4;
s32 pad_D_879060B8;
Controller* miniControllerPtr;
s16 gEkansGameCountdownFrames;
s16 miniEkansCountdown;
s8 gMiniHudColors[4];
s16 miniHudTransparency;
s16 gMiniDebugCameraMode;
s32 pad_D_879060D0[4];
Vec3f gMiniDebugBasePos;
Vec3f pad_D_879060F0;
Vec3f gMiniDebugCamVelocity;
Vec3f gMiniDebugGravityVector;

void miniApplyCameraDebugOffset(void) {
    miniCameraXRot = gMiniCamBaseRotX + gMiniCamRotXDelta;
    miniCameraYRot = gMiniCamBaseRotY + gMiniCamRotYDelta;

    miniCameraCoords.x = gMiniCamBasePos.x + gMiniCamPosDelta.x;
    miniCameraCoords.y = gMiniCamBasePos.y + gMiniCamPosDelta.y;
    miniCameraCoords.z = gMiniCamBasePos.z + gMiniCamPosDelta.z;

    miniCameraDistance = gMiniCamBaseDist + gMiniCamDistDelta;
    miniCameraFov = gMiniCamBaseFov + gMiniCamFovDelta;

    if (miniCameraDistance < 0x32) {
        miniCameraDistance = 0x32;
        gMiniCamDistDelta = miniCameraDistance - gMiniCamBaseDist;
    }

    if (miniCameraFov < 0xA) {
        miniCameraFov = 0xA;
        gMiniCamFovDelta = miniCameraFov - gMiniCamBaseFov;
    }
}

void miniUpdateCamera(void) {
    gMiniCameraPtr->perspective.verticalFieldOfView = miniCameraFov;
    gMiniCameraPtr->perspective.nearPlane = miniCameraNear;
    gMiniCameraPtr->perspective.farPlane = miniCameraFar;

    gMiniCameraPtr->view.targetPosition.x = miniCameraCoords.x;
    gMiniCameraPtr->view.targetPosition.y = miniCameraCoords.y;
    gMiniCameraPtr->view.targetPosition.z = miniCameraCoords.z;

    Camera_ComputeEyeFromAngles(&gMiniCameraPtr->view.targetPosition, &gMiniCameraPtr->view.eyePosition, miniCameraDistance, miniCameraXRot, miniCameraYRot);
}

s32 minigameDebuggModeControl(void) {
    s16 sp6;

    if (miniDebugMode == 0) {
        return;
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_L)) {
        gMiniDebugCameraTranslateMode++;
        if (gMiniDebugCameraTranslateMode == 1) {
            gMiniDebugCameraTranslateMode = 0;
        }
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_R)) {
        gMiniDebugCameraTranslateMode--;
        if (gMiniDebugCameraTranslateMode < 0) {
            gMiniDebugCameraTranslateMode = 0;
        }
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_START)) {
        gMiniCamPosDelta.x = 0;
        gMiniCamPosDelta.y = 0;
        gMiniCamPosDelta.z = 0;

        gMiniCamRotXDelta = 0;
        gMiniCamRotYDelta = 0;
        gMiniCamDistDelta = 0;
        gMiniCamFovDelta = 0;
    }

    if (gMiniDebugCameraTranslateMode == 0) {
        if (BTN_IS_DOWN(gPlayer1Controller, 0xCF00)) {
            if (BTN_IS_DOWN(gPlayer1Controller, BTN_DUP)) {
                sp6 = miniCameraYRot + 0x8000;
            }

            if (BTN_IS_DOWN(gPlayer1Controller, BTN_DDOWN)) {
                sp6 = miniCameraYRot;
            }

            if (BTN_IS_DOWN(gPlayer1Controller, BTN_DLEFT)) {
                sp6 = miniCameraYRot + 0xC000;
            }

            if (BTN_IS_DOWN(gPlayer1Controller, BTN_DRIGHT)) {
                sp6 = miniCameraYRot + 0x4000;
            }

            if (BTN_IS_DOWN(gPlayer1Controller, BTN_A)) {
                sp6 = miniCameraYRot + 0x8000;
            }

            if (BTN_IS_DOWN(gPlayer1Controller, BTN_B)) {
                sp6 = miniCameraYRot;
            }

            gMiniCamPosDelta.x += SINS(sp6) * 16.0f;
            gMiniCamPosDelta.z += COSS(sp6) * 16.0f;
        }

        gMiniCamRotYDelta += gPlayer1Controller->stickX * 4.0f;
        gMiniCamRotXDelta += gPlayer1Controller->stickY * 4.0f;

        if (BTN_IS_DOWN(gPlayer1Controller, BTN_CUP)) {
            gMiniCamPosDelta.y += 4;
        }

        if (BTN_IS_DOWN(gPlayer1Controller, BTN_CDOWN)) {
            gMiniCamPosDelta.y -= 4;
        }

        if (BTN_IS_DOWN(gPlayer1Controller, BTN_CRIGHT)) {
            gMiniCamDistDelta += 0xA;
        }

        if (BTN_IS_DOWN(gPlayer1Controller, BTN_CLEFT)) {
            gMiniCamDistDelta -= 0xA;
        }
    }
}

void showDebuggCameraInfo(void) {
    u8 sp24[3] = { 0, 0, 30 };  // D_87903DE0
    u8 sp20[3] = { 0, 0, 250 }; // D_87903DE4

    Ui_DrawBorderedPanel(0x14, 0x14, 0x122, 0x3C, 0x141);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, -2);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);

    if (gMiniDebugCameraTranslateMode == 0) {
        Font_Printf(0x19, 0x1C, "TRANSRATE");
    }

    Font_Printf(0x19, 0x28, "CAMERA X:%d", miniCameraCoords.x);
    Font_Printf(0x19, 0x34, "CAMERA Y:%d", miniCameraCoords.y);
    Font_Printf(0x19, 0x40, "CAMERA Z:%d", miniCameraCoords.z);
    Font_Printf(0xA7, 0x28, "CAMERA D:%d", miniCameraDistance);
    Font_Printf(0xA7, 0x34, "CAMERA F:%d", miniCameraFov);
    Font_Printf(0xA7, 0x40, "X:%04hX Y:%04hX", miniCameraXRot, miniCameraYRot);
    Font_EndTexturedTextRendering();
}

void showDebuggJoystickInfo(void) {
    u8 sp2C[3] = { 0, 0, 30 };  // D_87903DE8
    u8 sp28[3] = { 0, 0, 250 }; // D_87903DEC

    Ui_DrawBorderedPanel(0x78, 0x80, 0xB4, 0x64, 0x141);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, -2);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    Font_Printf(0x82, 0x8C, "FORCE   :%2.2f", gPlayer1Controller->stickMag);
    Font_Printf(0x82, 0x98, "LEVEL X :%2.2f", gPlayer1Controller->stickX);
    Font_Printf(0x82, 0xA4, "LEVEL Y :%2.2f", gPlayer1Controller->stickY);
    Font_Printf(0x82, 0xB0, "STICK X :%d", gPlayer1Controller->rawStickX);
    Font_Printf(0x82, 0xBC, "STICK Y :%d", gPlayer1Controller->rawStickY);
    Font_Printf(0x82, 0xC8, "ANGLE   :%04hx", gPlayer1Controller->angle);
    Font_EndTexturedTextRendering();
}

void minigameDebuggVectorEditor(void) {
    UNUSED s32 pad;
    u8 sp30[3] = { 0, 0, 30 };  // D_87903DF0
    u8 sp2C[3] = { 0, 0, 250 }; // D_87903DF4

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_CUP)) {
        gMiniDebugCameraMode--;
        if (gMiniDebugCameraMode < 0) {
            gMiniDebugCameraMode = 4;
        }
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_CDOWN)) {
        gMiniDebugCameraMode++;
        if (gMiniDebugCameraMode >= 5) {
            gMiniDebugCameraMode = 0;
        }
    }

    switch (gMiniDebugCameraMode) {
        case 0:
            if (BTN_IS_PRESSED(gPlayer1Controller, BTN_CLEFT)) {
                gMiniDebugCamVelocity.x -= 0.1f;
            }
            if (BTN_IS_PRESSED(gPlayer1Controller, BTN_CRIGHT)) {
                gMiniDebugCamVelocity.x += 0.1f;
            }
            break;

        case 1:
            if (BTN_IS_PRESSED(gPlayer1Controller, BTN_CLEFT)) {
                gMiniDebugCamVelocity.y -= 0.1;
            }
            if (BTN_IS_PRESSED(gPlayer1Controller, BTN_CRIGHT)) {
                gMiniDebugCamVelocity.y += 0.1;
            }
            break;

        case 2:
            if (BTN_IS_PRESSED(gPlayer1Controller, BTN_CLEFT)) {
                gMiniDebugBasePos.x -= 1.0;
            }
            if (BTN_IS_PRESSED(gPlayer1Controller, BTN_CRIGHT)) {
                gMiniDebugBasePos.x += 1.0;
            }
            break;

        case 3:
            if (BTN_IS_PRESSED(gPlayer1Controller, BTN_CLEFT)) {
                gMiniDebugBasePos.y -= 1.0;
            }
            if (BTN_IS_PRESSED(gPlayer1Controller, BTN_CRIGHT)) {
                gMiniDebugBasePos.y += 1.0;
            }
            break;

        case 4:
            if (BTN_IS_PRESSED(gPlayer1Controller, BTN_CLEFT)) {
                gMiniDebugGravityVector.y -= 0.02;
            }
            if (BTN_IS_PRESSED(gPlayer1Controller, BTN_CRIGHT)) {
                gMiniDebugGravityVector.y += 0.02;
            }
            break;
    }

    Ui_DrawBorderedPanel(0x78, 0x80, 0xB4, 0x64, 0x141);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(2, -2);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    Font_Printf(0x7C, 0x8C + (gMiniDebugCameraMode * 0xC), "@");

    Font_Printf(0x82, 0x8C, "VX:%2.2f", gMiniDebugCamVelocity.x);
    Font_Printf(0x82, 0x98, "VY:%2.2f", gMiniDebugCamVelocity.y);
    Font_Printf(0x82, 0xA4, "BX:%2.2f", gMiniDebugBasePos.x);
    Font_Printf(0x82, 0xB0, "BY:%2.2f", gMiniDebugBasePos.y);
    Font_Printf(0x82, 0xBC, "GV:%2.2f", gMiniDebugGravityVector.y);
    Font_Printf(0x82, 0xC8, "PW:%2.2f", gMiniDebugGravityVector.x);

    Font_EndTexturedTextRendering();
}
