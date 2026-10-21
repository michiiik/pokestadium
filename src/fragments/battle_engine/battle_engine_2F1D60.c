#include "battle_engine.h"
#include "src/model_animation.h"
#include "src/model_renderer.h"
#include "src/model_renderer_callbacks.h"
#include "src/matrix.h"

/*
    Handles movement and display of Pokemon's thumbnail
*/

// seems to actually be idle state trigger after moves
void BattleScene_TriggerOwnerModelAnimation(ActivePokemon* active_pokemon) {
    ModelAnim_SetFrame(&active_pokemon->presentationState.displayObject, active_pokemon->presentationState.idleTimer);
    ModelAnim_SetAnimation(&active_pokemon->presentationState.displayObject, active_pokemon->presentationState.idleAnimation);
    BattleScene_SetFlagToUpdatePokemonThumbnail(active_pokemon->presentationState.thumbnailCamera);
}

void BattleScene_ClearParticipantOwnerIdleFlags(ActivePokemon* active_pokemon) {
    active_pokemon->presentationState.isIdle = 0;
}

void BattleScene_SetParticipantOwnerIdleFlags(ActivePokemon* active_pokemon) {
    active_pokemon->presentationState.isIdle = 1;
}

void BattleScene_ClearBothOwnerIdleFlags(void) {
    sActivePokemon[0]->presentationState.isIdle = 0;
    sActivePokemon[1]->presentationState.isIdle = 0;
}

void BattleScene_SetBothOwnerIdleFlags(void) {
    sActivePokemon[0]->presentationState.isIdle = 1;
    sActivePokemon[1]->presentationState.isIdle = 1;
}

void BattleScene_SetFlagToUpdatePokemonThumbnail(CameraNode* camera) {
    camera->node.flags |= 0x10;
}

void BattleScene_LoadOwnerCameraFramingConstants(ActivePokemon* active_pokemon) {
    s32 side;

    if (active_pokemon == sActivePokemon[0]) {
        side = 0;
    } else {
        side = 1;
    }

    active_pokemon->presentationState.thumbnailCamera->viewport.width = D_843847BC[side].width;
    active_pokemon->presentationState.thumbnailCamera->viewport.x = D_843847BC[side].x;
    active_pokemon->presentationState.thumbnailCamera->viewport.y = D_843847BC[side].y;

    active_pokemon->presentationState.side1ThumnailInPlace = D_843847BC[side].unk_14;

    active_pokemon->presentationState.thumbnailCamera->perspective.nearPlane = D_843847BC[side].nearPlane;
    active_pokemon->presentationState.thumbnailCamera->perspective.farPlane = D_843847BC[side].farPlane;
    active_pokemon->presentationState.thumbnailCamera->perspective.verticalFieldOfView = D_843847BC[side].verticalFieldOfView;
}

void BattleScene_SetupOwnerCameraFraming(ActivePokemon* side1_pokemon, ActivePokemon* side2_pokemon) {
    if (sBattleScene.unk_00->unk_1C == 1) {
        BattleScene_SetBothOwnerIdleFlags();
    } else {
        BattleScene_ClearBothOwnerIdleFlags();
    }
    BattleScene_LoadOwnerCameraFramingConstants(side1_pokemon);
    BattleScene_LoadOwnerCameraFramingConstants(side2_pokemon);
}

void BattleScene_OwnerCameraLifecycleNop(void) {
}

void BattleScene_ApplyOwnerCameraFraming(ActivePokemon* active_pokemon) {
    s32 side;
    CameraNode* camera;
    f32 tmp_z;
    f32 tmp_y;

    camera = active_pokemon->presentationState.thumbnailCamera;
    side = BattleScene_GetParticipantSideIndex(active_pokemon);

    active_pokemon->presentationState.rotationX = sSidePresentationTransform[side]->unk_0C;
    if (sSidePresentationTransform[side]->unk_10 == 0xFF) {
        active_pokemon->presentationState.rotationY = sSidePresentationTransform[side]->unk_0E * active_pokemon->presentationState.side1ThumnailInPlace;
        tmp_z = sSidePresentationTransform[side]->coordinates.z * active_pokemon->presentationState.side1ThumnailInPlace;
    } else {
        active_pokemon->presentationState.rotationY = sSidePresentationTransform[side]->unk_0E;
        tmp_z = sSidePresentationTransform[side]->coordinates.z;
    }

    active_pokemon->presentationState.z = tmp_z;
    active_pokemon->presentationState.x = sSidePresentationTransform[side]->coordinates.x;
    active_pokemon->presentationState.idleTimer = sSidePresentationTransform[side]->idleTimer;
    tmp_y = sSidePresentationTransform[side]->coordinates.y;
    active_pokemon->presentationState.y = tmp_y;

    BattleScene_SetCameraAtAndEyeFromAngles(camera, active_pokemon->presentationState.displayObject.position.x + active_pokemon->presentationState.z, active_pokemon->presentationState.displayObject.position.y + tmp_y,
                  active_pokemon->presentationState.displayObject.position.z, active_pokemon->presentationState.x, active_pokemon->presentationState.rotationX, active_pokemon->presentationState.rotationY);
}

void BattleScene_TickOwnerCameraFraming(ActivePokemon* active_pokemon) {
    BattleScene_ApplyOwnerCameraFraming(active_pokemon);
}

void BattleScene_InitializeOwnerCameraFraming(CameraNode* side1_camera_node, ActivePokemon* side1_pokemon, CameraNode* side2_camera_node, ActivePokemon* side2_pokemon) {
    side1_pokemon->presentationState.thumbnailCamera = side1_camera_node;
    side2_pokemon->presentationState.thumbnailCamera = side2_camera_node;
    BattleScene_SetupOwnerCameraFraming(side1_pokemon, side2_pokemon);
    BattleScene_TickOwnerCameraFraming(side1_pokemon);
    BattleScene_TickOwnerCameraFraming(side2_pokemon);
}

void BattleScene_TickOwnerSlideIn(ActivePokemon* arg0, s32 arg1) {
    s16 var_a0;
    CameraNode* temp_v0;

    arg0->presentationState.thumbnailCamera->node.flags |= 1;
    arg0->presentationState.displayObject.node.flags |= 1;

    if (arg0->presentationState.side1ThumnailInPlace == 1) {
        if ((arg0->presentationState.thumbnailCamera->viewport.x == -0x48) && !(arg0->state.flags & 0x800)) {
            BattleScene_TriggerOwnerModelAnimation(arg0);
        }

        if ((ModelRenderer_IsReady(0) != 0) && (arg0->presentationState.side2ThumbnailInPlace == 0)) {
            arg0->presentationState.side2ThumbnailInPlace = 2;
            ModelRenderer_OnSlotReady(0);
        }
        arg0->presentationState.thumbnailCamera->viewport.x = Math_StepToS32(arg0->presentationState.thumbnailCamera->viewport.x, 0x17, arg1, arg1);
    } else {
        if ((arg0->presentationState.thumbnailCamera->viewport.x == 0x15F) && !(arg0->state.flags & 0x800)) {
            BattleScene_TriggerOwnerModelAnimation(arg0);
        }

        arg0->presentationState.thumbnailCamera->viewport.x = Math_StepToS32(arg0->presentationState.thumbnailCamera->viewport.x, 0x103, arg1, arg1);

        if ((ModelRenderer_IsReady(1) != 0) && (arg0->presentationState.side2ThumbnailInPlace == 0)) {
            arg0->presentationState.side2ThumbnailInPlace = 2;
            ModelRenderer_OnSlotReady(1);
        }
    }
}

void BattleScene_TickOwnerSlideOut(ActivePokemon* active_pokemon, s32 delta) {
    if (active_pokemon->presentationState.side1ThumnailInPlace == 1) {
        active_pokemon->presentationState.side2ThumbnailInPlace = 0;
        active_pokemon->presentationState.thumbnailCamera->viewport.x = Math_StepToS32(active_pokemon->presentationState.thumbnailCamera->viewport.x, -72, delta, delta);
        if (active_pokemon->presentationState.thumbnailCamera->viewport.x == -72) {
            active_pokemon->presentationState.thumbnailCamera->node.flags &= ~1;
            active_pokemon->presentationState.displayObject.node.flags &= ~1;
        }
    } else {
        active_pokemon->presentationState.side2ThumbnailInPlace = 0;
        active_pokemon->presentationState.thumbnailCamera->viewport.x = Math_StepToS32(active_pokemon->presentationState.thumbnailCamera->viewport.x, 351, delta, delta);
        if (active_pokemon->presentationState.thumbnailCamera->viewport.x == 351) {
            active_pokemon->presentationState.thumbnailCamera->node.flags &= ~1;
            active_pokemon->presentationState.displayObject.node.flags &= ~1;
        }
    }
}

void BattleScene_TickOwnerSlideAnimation(ActivePokemon* active_pokemon) {
    switch (active_pokemon->presentationState.isIdle) {
        case 0:
            BattleScene_TickOwnerSlideOut(active_pokemon, 9);
            break;

        case 1:
            BattleScene_TickOwnerSlideIn(active_pokemon, 9);
            break;
    }
}

void BattleScene_TickBothOwnerSlideAnimations(ActivePokemon* arg0, ActivePokemon* arg1) {
    if (gSessionContext.mode != SESSION_MODE_ATTRACT_DEMO_BATTLE) {
        BattleScene_TickOwnerSlideAnimation(arg0);
        BattleScene_TickOwnerSlideAnimation(arg1);
    }
}

s32 BattleScene_DispatchOwnerCameraLifecycle(s32 arg0, CameraNode* side1_camera_node, CameraNode* side2_camera_node) {
    switch (arg0) {
        case 0:
            BattleScene_OwnerCameraLifecycleNop();
            break;

        case 1:
        case 2:
            BattleScene_TickBothOwnerSlideAnimations(sActivePokemon[0], sActivePokemon[1]);
            break;

        case 3:
            BattleScene_InitializeOwnerCameraFraming(side1_camera_node, sActivePokemon[0], side2_camera_node, sActivePokemon[1]);
            break;
    }
    return 0;
}
