#include "minigame_actor_framework.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/geo_render.h"
#include "src/memory.h"

static MiniActor* D_87906120;

void miniRankingActorsInit(void) {
    s32 i;
    MiniActor* ptr;

    D_87906120 = main_pool_alloc(sizeof(MiniActor) * 3, 0);
    ptr = D_87906120;

    for (i = 0; i < 3; i++, ptr++) {
        ModelRenderer_AttachDisplayObject(&ptr->unk_000);
        ptr->unk_168 = Model_LoadByArchiveIndex(0xAB);
        Model_InitDisplayObject(&ptr->unk_000, 0, ptr->unk_23C, ptr->unk_168->unk_08->unk_00[0]);
        ModelAnim_SetAnimation(&ptr->unk_000, 0);
        ptr->unk_000.unk_000.unk_01 &= ~1;
        miniActorAllToZero(ptr);
    }
}

void miniRankingActorSpawn(MiniActor* arg0) {
    if (arg0->mainState == 1) {
        arg0->unk_000.unk_000.unk_01 |= 1;
        miniChangeActorAnim(arg0, 0, -1, 0);
        arg0->mainState++;
    }
}

void miniRankingActorsUpdate(void) {
    s32 i;
    MiniActor* ptr = D_87906120;

    Particle_UpdateFrameCounters();

    for (i = 0; i < 3; i++, ptr++) {
        miniRankingActorSpawn(ptr);
        MiniActor_UpdateAnimation(ptr);
        miniActorUpdateTransform(ptr);
        MiniActor_SyncModelTransform(ptr);
    }
}

void miniRankingActorSetup(MiniActor* arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, s16 arg5, s16 arg6, s16 arg7, s16 arg8) {
    arg0->scale.x = arg1;
    arg0->scale.y = arg1;
    arg0->scale.z = arg1;

    arg0->position_1.x = arg2;
    arg0->position_1.y = arg3;
    arg0->position_1.z = arg4;
    arg0->xRot_1 = arg5;
    arg0->yRot_1 = arg6;
    arg0->zRot_1 = arg7;
    arg0->collidingActorId = arg8;
    arg0->mainState = 1;
}

void miniRankingActorSetup1st(f32 arg0, f32 arg1, f32 arg2, f32 arg3, s16 arg4, s16 arg5, s16 arg6) {
    MiniActor* sp34 = D_87906120;

    miniRankingActorSetup(sp34, arg0, arg1, arg2, arg3, arg4, arg5, arg6, 0);
    sp34->unk_000.unk_000.unk_14 = 0;
}

void miniRankingActorSetup2nd(f32 arg0, f32 arg1, f32 arg2, f32 arg3, s16 arg4, s16 arg5, s16 arg6) {
    MiniActor* sp34 = &D_87906120[1];

    miniRankingActorSetup(sp34, arg0, arg1, arg2, arg3, arg4, arg5, arg6, 1);
    sp34->unk_000.unk_000.unk_14 = 1;
}

void miniRankingActorSetup3rd(f32 arg0, f32 arg1, f32 arg2, f32 arg3, s16 arg4, s16 arg5, s16 arg6) {
    MiniActor* sp34 = &D_87906120[2];

    miniRankingActorSetup(sp34, arg0, arg1, arg2, arg3, arg4, arg5, arg6, 2);
    sp34->unk_000.unk_000.unk_14 = 2;
}

#ifdef NON_MATCHING
Gfx* MiniFx_BuildNodeColorRampDisplayList(Gfx* arg0, arg1_func_87903D64_014* arg1) {
    s32 sp38;
    s32 sp34;
    s32 sp30;

    unk_D_86002F34_000_014_004* sp2C;
    unk_D_86002F34_000_014_004* sp28;
    unk_D_86002F34_000_014_010* sp24;
    unk_D_86002F34_000_014_010* sp20;

    s32 temp_a2;
    UNUSED s32 temp_t1;
    UNUSED s32 temp_t1_2;
    UNUSED s32 temp_t2;
    UNUSED s32 temp_t2_2;
    UNUSED s32 temp_t2_3;
    UNUSED s32 temp_t4;
    s32 var_a0;
    s32 var_v1;
    u32 temp_a3;
    u32 var_v0;
    UNUSED u8* temp_t1_3;
    UNUSED u8* temp_v1;
    unk_D_86002F34_000_014_004* var_a1;
    unk_D_86002F34_000_014_004* var_a2;
    unk_D_86002F34_000_014_010* var_a3;
    unk_D_86002F34_000_014_010* var_t0;
    u32 tmp1;
    u32 tmp2;
    u32 tmp3;
    u32 tmp4;
    u32 tmp5;
    u32 tmp6;
    u32 tmp7;
    u32 tmp8;
    u32 tmp9;

    temp_a2 = D_8006F09C->unk_000.unk_14;
    temp_a3 = (D_8006F09C->unk_040.unk_08 >> 0x10) + 1;

    switch (arg1->unk_00) {
        default:
            var_a0 = sp34;
            var_v1 = sp38;
            var_v0 = sp30;
            break;

        case 1:
            if (temp_a3 < 0xF) {
                var_v1 = 0;
                var_v0 = (temp_a3 << 8) / 15;
                var_a0 = 1;
            } else if (temp_a3 < 0x1D) {
                var_v1 = 1;
                var_v0 = ((temp_a3 << 8) - 0xF00) / 14;
                var_a0 = 2;
            } else {
                var_v1 = 2;
                var_a0 = 2;
                var_v0 = 0;
            }
            break;

        case 2:
            if (temp_a3 < 5) {
                var_v1 = 0;
                var_v0 = (temp_a3 << 8) / 5;
                var_a0 = 1;
            } else if (temp_a3 < 0xF) {
                var_v1 = 1;
                var_v0 = ((temp_a3 << 8) - 0x500) / 10;
                var_a0 = 2;
            } else if (temp_a3 < 0x1D) {
                var_v1 = 2;
                var_v0 = ((temp_a3 << 8) - 0xF00) / 14;
                var_a0 = 3;
            } else {
                var_v1 = 3;
                var_a0 = 3;
                var_v0 = 0;
            }
            break;
    }

    switch (temp_a2) {
        default:
            var_t0 = sp20;
            var_a3 = sp24;
            var_a2 = sp28;
            var_a1 = sp2C;
            break;

        case 0:
            var_a3 = &arg1->unk_10[var_v1];
            var_t0 = &arg1->unk_10[var_a0];
            var_a1 = &arg1->unk_04[var_v1];
            var_a2 = &arg1->unk_04[var_a0];
            break;

        case 1:
            var_a3 = &arg1->unk_14[var_v1];
            var_t0 = &arg1->unk_14[var_a0];
            var_a1 = &arg1->unk_08[var_v1];
            var_a2 = &arg1->unk_08[var_a0];
            break;

        case 2:
            var_a3 = &arg1->unk_18[var_v1];
            var_t0 = &arg1->unk_18[var_a0];
            var_a1 = &arg1->unk_0C[var_v1];
            var_a2 = &arg1->unk_0C[var_a0];
            break;
    }

    gDPPipeSync(arg0++);

    tmp5 = ((var_a1->unk_00[4] * (0x100 - var_v0)) + (var_a2->unk_00[4] * var_v0)) >> 8;

    tmp1 = ((var_a1->unk_00[0] * (0x100 - var_v0)) + (var_a2->unk_00[0] * var_v0));
    tmp1 >>= 8;
    tmp2 = ((var_a1->unk_00[1] * (0x100 - var_v0)) + (var_a2->unk_00[1] * var_v0));
    tmp2 >>= 8;
    tmp3 = ((var_a1->unk_00[2] * (0x100 - var_v0)) + (var_a2->unk_00[2] * var_v0));
    tmp3 >>= 8;
    tmp4 = ((var_a1->unk_00[3] * (0x100 - var_v0)) + (var_a2->unk_00[3] * var_v0));
    tmp4 >>= 8;

    tmp6 = ((var_a3->unk_00[0] * (0x100 - var_v0)) + (var_t0->unk_00[0] * var_v0));
    tmp6 >>= 8;
    tmp7 = ((var_a3->unk_00[1] * (0x100 - var_v0)) + (var_t0->unk_00[1] * var_v0));
    tmp7 >>= 8;
    tmp8 = ((var_a3->unk_00[2] * (0x100 - var_v0)) + (var_t0->unk_00[2] * var_v0));
    tmp8 >>= 8;
    tmp9 = ((var_a3->unk_00[3] * (0x100 - var_v0)) + (var_t0->unk_00[3] * var_v0));
    tmp9 >>= 8;

    gDPSetPrimColor(arg0++, 0, tmp5, tmp1, tmp2, tmp3, tmp4);

    gDPSetEnvColor(arg0++, tmp1, tmp2, tmp3, tmp4);

    return arg0;
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/minigame_actor_framework/minigame_actor_framework_11BF90/MiniFx_BuildNodeColorRampDisplayList.s")
#endif

void MiniFx_GraphNodeBuildColorRampList(s32 arg0, arg1_func_87903D64* arg1) {
    if (arg0 == 5) {
        arg1_func_87903D64_014* tmp = arg1->unk_14;

        gDisplayListHead = MiniFx_BuildNodeColorRampDisplayList(gDisplayListHead, tmp);
    }
}
