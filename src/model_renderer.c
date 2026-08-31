#include "global.h"
#include "model_renderer.h"
#include "geo_node.h"
#include "model_animation.h"
#include "matrix.h"
#include "model_animation_events.h"
#include "battle_hud.h"
#include "gb_data.h"
#include "display_object_textures.h"
#include "game_state.h"
#include "dp_intro.h"
#include "hal_libc.h"
#include "src/geo_render.h"

struct unk_D_86002F58_004_000_000 D_800AC840;
struct unk_D_86002F58_004_000_000 D_800AC858;

void ModelRenderer_InitDisplayRoots(void) {
    GeoNode_CreateContainer(0, &D_800AC840);
    GeoNode_CreateContainer(0, &D_800AC858);
}

void ModelRenderer_AttachDisplayObject(unk_D_86002F58_004_000* arg0) {
    GeoNode_CreateModelPart(NULL, arg0, 0, &D_8006F050, &D_8006F05C, &D_8006F064);
    GraphNode_AppendChild(&D_800AC840, &arg0->unk_000);
    arg0->unk_0A6 = 0;
    arg0->unk_000.unk_01 &= ~1;
}

void ModelRenderer_AttachSecondaryDisplayObject(unk_D_86002F58_004_000* arg0) {
    GeoNode_CreateModelPart(NULL, arg0, 0, &D_8006F050, &D_8006F05C, &D_8006F064);
    GraphNode_AppendChild(&D_800AC858, &arg0->unk_000);
    arg0->unk_000.unk_01 &= ~1;
}

void Model_InitDisplayObject(unk_D_86002F58_004_000* arg0, s16 arg1, s16 arg2, unk_D_86002F58_004_000_004* arg3) {
    GraphNode_AppendChild(&arg0->unk_000, arg3);
    MtxF_Identity(&arg0->unk_060);
    arg0->unk_018 = arg1;
    arg0->unk_01A = arg2;

    arg0->unk_01D = 0xFF;
    arg0->unk_01C = 0;

    arg0->unk_000.unk_01 |= 0x1;

    arg0->unk_000.unk_02 &= ~0x4;
    arg0->unk_000.unk_02 |= 0x20;
    arg0->unk_000.unk_02 |= 0x40;
    arg0->unk_000.unk_02 &= ~0x2;

    arg0->unk_0A0.rgba = 0xFFFFFF00;
    arg0->unk_03C.rgba = 0xFFFFFF00;
    arg0->unk_040.unk_00 = -1;
    arg0->unk_040.unk_04 = NULL;
    arg0->unk_054.unk_00 = -1;
    arg0->unk_054.unk_04 = NULL;

    arg0->unk_000.unk_02 &= ~0x8;

    if (arg2 > 0) {
        ModelAnim_StartDisplayObjectAnimation(arg0);
    }
}

void ModelRenderer_ClearDisplayObject(unk_D_86002F58_004_000* arg0) {
    arg0->unk_000.unk_0C = NULL;
    arg0->unk_000.unk_01 &= ~1;
}

s32 ModelAnim_SetAnimation(unk_D_86002F58_004_000* arg0, s32 arg1) {
    unk_D_86002F58_004_000_00C* tmp = arg0->unk_000.unk_0C;
    unk_D_86002F58_004_000_00C_028* temp_v0 = tmp->unk_28(0, 0);

    if (arg1 == -1) {
        ModelAnim_ClearTransformChannel(arg0);
    } else if (arg1 < temp_v0->unk_04) {
        ModelAnim_BindTransformCurve(arg0, arg1, temp_v0->unk_0C[arg1], 0x10000);
        return 1;
    }

    return 0;
}

s32 ModelAnim_SetEventTrack(unk_D_86002F58_004_000* arg0, s32 arg1) {
    unk_D_86002F58_004_000_00C* temp_v0 = arg0->unk_000.unk_0C;
    unk_D_86002F58_004_000_00C_028* temp_v1 = temp_v0->unk_28(0, 0);

    if (arg1 == -1) {
        ModelAnim_ClearEventTrack(arg0);
    } else if (arg1 < temp_v1->unk_05) {
        ModelAnim_BindEventTrack(arg0, arg1, temp_v1->unk_10[arg1]);
        return 1;
    }

    return 0;
}

void Model_SetMaterialColor(unk_D_86002F58_004_000* arg0, u8 arg1, u8 arg2, u8 arg3, u8 arg4) {
    Color_SetRGBA(&arg0->unk_0A0, arg1, arg2, arg3, arg4);
}

void Model_SetMaterialAlpha(unk_D_86002F58_004_000* arg0, u8 arg1) {
    arg0->unk_01D = arg1;
}

void Model_SetMaterialTextureMode(unk_D_86002F58_004_000* arg0, u8 arg1) {
    arg0->unk_01C = arg1;
}

unk_D_86002F58_004_000_00C_028* ModelAnim_GetAnimationRecord(unk_D_86002F58_004_000* arg0, s32 arg1) {
    unk_D_86002F58_004_000_00C_028* var_v1 = NULL;

    if (arg0 == NULL) {
        arg0 = D_8006F09C;
    }

    if (arg0 != NULL) {
        unk_D_86002F58_004_000_00C* tmp = arg0->unk_000.unk_0C;

        var_v1 = tmp->unk_28(arg1, 0);
    }

    return var_v1;
}

arg1_func_80010CA8* Model_ComputeSizeVariant(arg1_func_80010CA8* arg0, BattleMon* arg1) {
    UNUSED s32 pad;
    char sp38[0xB];
    arg1_func_80010CA8 sp34;
    unk_D_8006FF00* sp30;
    UNUSED s32 pad2;
    u8 var_a0;
    s8* var_v1;

    sp30 = &D_8006FF00[arg1->unk_00.unk_00 - 1];
    sp34.raw = 0;

    Text_CopySpeciesName(sp38, arg1->unk_00.unk_00);

    if (HAL_Strcmp(arg1->unk_30, sp38) != 0) {
        var_a0 = ((arg1->unk_0E >> 8) & 0xFF) + (arg1->unk_0E & 0xFF);

        if (1) {}

        var_v1 = arg1->unk_30;
        while (*var_v1) {
            var_a0 += *var_v1++ & 0xFF;
            var_a0 += 0;
        }

        var_v1 = arg1->unk_3B;
        while (*var_v1) {
            var_a0 += *var_v1++ & 0xFF;
            var_a0 += 0;
        }

        sp34.unk_00 = ((((sp30->unk_12 - sp30->unk_10) << 6) * var_a0) / 255) + (sp30->unk_10 << 6);
    }

    *arg0 = sp34;
    return arg0;
}

void Model_ComputeSizeVariantFromRecord(arg1_func_80010CA8* arg0, unk_func_8001C014* arg1, u16 arg2) {
    BattleMon stack;

    stack.unk_00.unk_00 = arg2;
    stack.unk_0E = arg1->unk_02;

    _bcopy(arg1->unk_04, stack.unk_30, 0x10);
    _bcopy(arg1->unk_14, stack.unk_3B, 0x10);
    Model_ComputeSizeVariant(arg0, &stack);
}

void Trainer_RequestPokeIcon(BattleSessionTeams* arg0) {
    arg1_func_80010CA8 sp2C;
    s16 sp2A;
    TeamRoster* temp_a1;

    sp2A = arg0->unk_02;
    sp2C.raw = 0;
    if (!(arg0->unk_00 & 0x40) && (arg0->unk_00 & 0x80)) {
        temp_a1 = arg0->unk_04;
        if ((temp_a1 != NULL) && (sp2A == arg0->unk_04->unk_000)) {
            Model_ComputeSizeVariant(&sp2C, (BattleMon*)arg0->unk_04);
            if ((sp2A == 0x19) && (arg0->unk_04->unk_01C[0].unk_30[6] & 0x80)) {
                sp2A = 0x99;
            }
        }
        if (!(arg0->unk_10->unk_00 & 4)) {
            while (Display_IsFrameReady() == 0) {}
        }
        PokeIcon_RequestFrameLoad(arg0->unk_10, (u16) sp2A, sp2C);
        arg0->unk_00 |= 0x40;
        arg0->unk_00 &= ~0x10;

        if (arg0->unk_00 & 0x20) {
            PokeIcon_WaitFrameLoad(arg0->unk_10);
            arg0->unk_14 = arg0->unk_10->unk_24;
            arg0->unk_00 &= ~0xE0;
        }
    }
}

void PokeIcon_PollFrameLoadForObject(unk_func_8001C248* arg0) {
    if (PokeIcon_PollFrameLoad(arg0->unk_000.unk_10)) {
        arg0->unk_000.unk_14 = arg0->unk_000.unk_10->unk_24;
        arg0->unk_000.unk_00 &= ~0xE0;
        arg0->unk_000.unk_00 |= 0x10;
    }
}

void PokeIcon_WaitFrameLoadForObject(unk_func_8001C248* arg0) {
    if (arg0->unk_000.unk_00 & 0x40 && PokeIcon_WaitFrameLoad(arg0->unk_000.unk_10) != 0) {
        arg0->unk_000.unk_14 = arg0->unk_000.unk_10->unk_24;
        arg0->unk_000.unk_00 &= ~0xE0;
        arg0->unk_000.unk_00 |= 0x10;
    }
}

void PokeIcon_RequestBackgroundForObject(unk_func_8001C248* arg0) {
    if (!(arg0->unk_01D & 0x40) && (arg0->unk_01D & 0x80)) {
        PokeIcon_RequestBackgroundLoad(arg0->unk_000.unk_10, arg0->unk_020, arg0->unk_024);
        arg0->unk_01D |= 0x40;
        arg0->unk_01D &= ~0x10;
        if (arg0->unk_01D & 0x20) {
            PokeIcon_WaitBackgroundLoad(arg0->unk_000.unk_10);
            arg0->unk_024 = arg0->unk_000.unk_10->unk_28;
            arg0->unk_01D &= ~0xE0;
        }
    }
}

void PokeIcon_PollBackgroundForObject(unk_func_8001C248* arg0) {
    if (PokeIcon_PollBackgroundLoad(arg0->unk_000.unk_10) != 0) {
        arg0->unk_024 = arg0->unk_000.unk_10->unk_28;
        arg0->unk_01D &= ~0xE0;
        arg0->unk_01D |= 0x10;
    }
}
