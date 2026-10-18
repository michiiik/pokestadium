#include "src/util.h"
#include <stdlib.h>
#include "minigame_actor_framework.h"
#include "src/display_object_textures.h"
#include "src/model_renderer.h"
#include "src/geo_render.h"
#include "src/memory.h"

// PORT: the MiniFx geo callbacks are named by ROM model layouts (through fragment 31's jump table): their callback data
// is big-endian with 32-bit addresses; converted once (cached by source address).
extern int Port_IsRawData(const void* p);

static u32 Port_MiniFxBE32(const u8* p) {
    return ((u32)p[0] << 24) | ((u32)p[1] << 16) | ((u32)p[2] << 8) | p[3];
}

static arg1_func_87903D64_014* Port_MiniFxColorRamp(void* arg) {
    static struct {
        const u8* src;
        arg1_func_87903D64_014* out;
    } sCache[16];
    static s32 sCount;
    const u8* p = (const u8*)Util_ConvertAddrToVirtAddr((uintptr_t)arg);
    arg1_func_87903D64_014* out;
    s32 i;

    if (p == NULL || !Port_IsRawData(p)) {
        return (arg1_func_87903D64_014*)p;
    }
    for (i = 0; i < sCount; i++) {
        if (sCache[i].src == p) {
            return sCache[i].out;
        }
    }
    out = calloc(1, sizeof(*out));
    out->unk_00 = (s32)Port_MiniFxBE32(p);
    // colour arrays (5-byte prim and 4-byte env entries) are bytes: used in place
    out->unk_04 = (void*)Util_ConvertAddrToVirtAddr(Port_MiniFxBE32(p + 0x04));
    out->unk_08 = (void*)Util_ConvertAddrToVirtAddr(Port_MiniFxBE32(p + 0x08));
    out->unk_0C = (void*)Util_ConvertAddrToVirtAddr(Port_MiniFxBE32(p + 0x0C));
    out->unk_10 = (void*)Util_ConvertAddrToVirtAddr(Port_MiniFxBE32(p + 0x10));
    out->unk_14 = (void*)Util_ConvertAddrToVirtAddr(Port_MiniFxBE32(p + 0x14));
    out->unk_18 = (void*)Util_ConvertAddrToVirtAddr(Port_MiniFxBE32(p + 0x18));
    if (sCount < 16) {
        sCache[sCount].src = p;
        sCache[sCount].out = out;
        sCount++;
    }
    return out;
}

static MiniActor* D_87906120;

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/minigame_actor_framework/minigame_actor_framework_11BF90/miniRankingActorsInit.s")
#else
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
        ptr->unk_000.node.flags &= ~1;
        miniActorAllToZero(ptr);
    }
}
#endif

void miniRankingActorSpawn(MiniActor* arg0) {
    if (arg0->mainState == 1) {
        arg0->unk_000.node.flags |= 1;
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
    sp34->unk_000.node.callbackArg = 0;
}

void miniRankingActorSetup2nd(f32 arg0, f32 arg1, f32 arg2, f32 arg3, s16 arg4, s16 arg5, s16 arg6) {
    MiniActor* sp34 = &D_87906120[1];

    miniRankingActorSetup(sp34, arg0, arg1, arg2, arg3, arg4, arg5, arg6, 1);
    sp34->unk_000.node.callbackArg = 1;
}

void miniRankingActorSetup3rd(f32 arg0, f32 arg1, f32 arg2, f32 arg3, s16 arg4, s16 arg5, s16 arg6) {
    MiniActor* sp34 = &D_87906120[2];

    miniRankingActorSetup(sp34, arg0, arg1, arg2, arg3, arg4, arg5, arg6, 2);
    sp34->unk_000.node.callbackArg = 2;
}

Gfx* MiniFx_BuildNodeColorRampDisplayList(Gfx* gfx, arg1_func_87903D64_014* arg1) {
    u8 tmp1;
    u8 tmp2;
    u8 tmp3;
    u8 tmp4;
    u8 tmp5;
    u8 tmp6;
    u8 tmp7;
    u8 tmp8;
    u8 tmp9;
    u32 sp38;
    u32 sp34;
    u32 sp30;
    unk_D_86002F34_000_014_004* sp2C;
    unk_D_86002F34_000_014_004* sp28;
    unk_D_86002F34_000_014_010* sp24;
    unk_D_86002F34_000_014_010* sp20;
    u32 pad;
    u32 temp_a2;
    u32 temp_a3;

    temp_a2 = D_8006F09C->node.callbackArg;
    temp_a3 = (D_8006F09C->transformAnim.frameFixed >> 0x10) + 1;

    pad = arg1->unk_00;
    switch (pad) {
        case 1:
            if (temp_a3 < 0xF) {
                sp38 = 0;
                sp34 = 1;
                sp30 = (temp_a3 << 8) / 15;
            } else if (temp_a3 < 0x1D) {
                sp38 = 1;
                sp34 = 2;
                sp30 = ((temp_a3 << 8) - 0xF00) / 14;
            } else {
                sp38 = 2;
                sp34 = 2;
                sp30 = 0;
            }
            break;

        case 2:
            if (temp_a3 < 5) {
                sp38 = 0;
                sp34 = 1;
                sp30 = (temp_a3 << 8) / 5;
            } else if (temp_a3 < 0xF) {
                sp38 = 1;
                sp34 = 2;
                sp30 = ((temp_a3 << 8) - 0x500) / 10;
            } else if (temp_a3 < 0x1D) {
                sp38 = 2;
                sp34 = 3;
                sp30 = ((temp_a3 << 8) - 0xF00) / 14;
            } else {
                sp38 = 3;
                sp34 = 3;
                sp30 = 0;
            }
            break;
    }

    switch (temp_a2) {
        case 0:
            sp2C = &arg1->unk_04[sp38];
            sp28 = &arg1->unk_04[sp34];
            sp24 = &arg1->unk_10[sp38];
            sp20 = &arg1->unk_10[sp34];
            break;

        case 1:
            sp2C = &arg1->unk_08[sp38];
            sp28 = &arg1->unk_08[sp34];
            sp24 = &arg1->unk_14[sp38];
            sp20 = &arg1->unk_14[sp34];
            break;

        case 2:
            sp2C = &arg1->unk_0C[sp38];
            sp28 = &arg1->unk_0C[sp34];
            sp24 = &arg1->unk_18[sp38];
            sp20 = &arg1->unk_18[sp34];
            break;
    }



    tmp1 = ((sp28->unk_00[0] * sp30) + (sp2C->unk_00[0] * (0x100 - sp30))) >> 8;
    tmp2 = ((sp28->unk_00[1] * sp30) + (sp2C->unk_00[1] * (0x100 - sp30))) >> 8;
    tmp3 = ((sp28->unk_00[2] * sp30) + (sp2C->unk_00[2] * (0x100 - sp30))) >> 8;
    tmp4 = ((sp28->unk_00[3] * sp30) + (sp2C->unk_00[3] * (0x100 - sp30))) >> 8;
    tmp5 = ((sp28->unk_00[4] * sp30) + (sp2C->unk_00[4] * (0x100 - sp30))) >> 8;

    tmp6 = ((sp20->unk_00[0] * sp30) + (sp24->unk_00[0] * (0x100 - sp30))) >> 8;
    tmp7 = ((sp20->unk_00[1] * sp30) + (sp24->unk_00[1] * (0x100 - sp30))) >> 8;
    tmp8 = ((sp20->unk_00[2] * sp30) + (sp24->unk_00[2] * (0x100 - sp30))) >> 8;
    tmp9 = ((sp20->unk_00[3] * sp30) + (sp24->unk_00[3] * (0x100 - sp30))) >> 8;

    gDPPipeSync(gfx++);
    gDPSetPrimColor(gfx++, 0, tmp5, tmp1, tmp2, tmp3, tmp4);
    gDPSetEnvColor(gfx++, tmp6, tmp7, tmp8, tmp9);

    return gfx;
}

void MiniFx_GraphNodeBuildColorRampList(s32 arg0, arg1_func_87903D64* arg1) {
    if (arg0 == 5) {
        // PORT: the data is the node's callbackArg (the flat view's 0x14 is the N64 offset)
        arg1_func_87903D64_014* tmp = Port_MiniFxColorRamp(((GraphNode*)arg1)->callbackArg);

        gDisplayListHead = MiniFx_BuildNodeColorRampDisplayList(gDisplayListHead, tmp);
    }
}
