#include "battle_engine.h"
#include "src/gallery.h"
#include "src/gfx_buffer.h"

#pragma REWRITE_FUNCTION_STACK_OFFSET(func_84367660, 0x2CC, 0x80, 0x8C)
#pragma REWRITE_FUNCTION_STACK_OFFSET(func_84367660, 0x304, 0x7C, 0x88)
#pragma REWRITE_FUNCTION_STACK_OFFSET(func_84367660, 0x338, 0x78, 0x84)
#pragma REWRITE_FUNCTION_STACK_OFFSET(func_84367660, 0x36C, 0x74, 0x80)
#pragma REWRITE_FUNCTION_STACK_OFFSET(func_84367660, 0x3A0, 0x70, 0x7C)
#pragma REWRITE_FUNCTION_STACK_OFFSET(func_84367660, 0x3D4, 0x6C, 0x78)
#pragma REWRITE_FUNCTION_STACK_OFFSET(func_84367660, 0x42C, 0x80, 0x8C)
#pragma REWRITE_FUNCTION_STACK_OFFSET(func_84367660, 0x444, 0x7C, 0x88)
#pragma REWRITE_FUNCTION_STACK_OFFSET(func_84367660, 0x45C, 0x78, 0x84)
#pragma REWRITE_FUNCTION_STACK_OFFSET(func_84367660, 0x474, 0x74, 0x80)
#pragma REWRITE_FUNCTION_STACK_OFFSET(func_84367660, 0x49C, 0x70, 0x7C)
#pragma REWRITE_FUNCTION_STACK_OFFSET(func_84367660, 0x4B4, 0x6C, 0x78)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x144, ft, 18, 16)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x174, ft, 18, 16)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x174, fs, 18, 16)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x21C, ft, 18, 16)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x234, ft, 18, 16)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x234, fd, 16, 12)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x23C, fd, 12, 16)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x244, fs, 12, 16)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x248, ft, 16, 12)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x248, fs, 16, 12)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x278, fs, 12, 16)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x334, fs, 16, 12)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x3D8, fs, 16, 12)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x460, ft, 16, 12)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x4B8, ft, 16, 12)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x4C8, ft, 12, 16)
#pragma REWRITE_FUNCTION_COP1_REGISTER(func_84367660, 0x4F8, ft, 12, 16)

static Gfx D_84389780[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH | G_CLIPPING | 0x0040F9FA),
    gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPPipeSync(),
    gsDPLoadTextureBlock_4b(D_8438A648, G_IM_FMT_I, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5,
                            5, G_TX_NOLOD, G_TX_NOLOD),
    gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_XLU_SURF2),
    gsDPSetCombineMode(G_CC_BLENDIA, G_CC_BLENDIA),
    gsDPSetAlphaDither(G_AD_NOISE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsSPEndDisplayList(),
};
static Vtx D_84389810[] = {
    VTX(0, 20, 0, 1024, 2048, 0xFF, 0xFF, 0xFF, 0x00), VTX(30, 20, 0, 1024, 2048, 0xFF, 0xFF, 0xFF, 0x00),
    VTX(-20, 10, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0x00),  VTX(50, 10, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0x00),
    VTX(0, 0, 0, 1024, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(30, 0, 0, 1024, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-20, -10, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0x00), VTX(50, -10, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0x00),
    VTX(0, -20, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0x00),   VTX(30, -20, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0x00),
};

Gfx* func_84367660(Gfx* arg0, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, f32 arg4, u8 arg5, u8 arg6, u8 arg7, u8 arg8) {
    s32 i;
    Vtx* sp1E8;
    s32 pad[2];
    Mtx* sp1DC;
    volatile s32 pad2[42];
    f32 sp130;
    f32 sp12C;
    f32 sp128;
    f32 sp8C;
    f32 sp88;
    f32 sp84;
    f32 sp80;
    f32 sp7C;
    f32 sp78;
    volatile s32 pad4[7];
    Vtx* var_a1;
    Vtx* temp_v1;
    f32 a1_x;
    f32 a1_y;
    f32 a1_z;
    f32 a2_x;
    f32 a2_y;
    f32 a2_z;
    f32 a3_x;
    f32 a3_y;
    f32 a3_z;
    f32 tmp1;
    f32 tmp2;
    f32 tmp3;

    sp1DC = Gfx_AllocDisplayList(sizeof(Mtx) * 1);
    sp1E8 = Gfx_AllocDisplayList(sizeof(Vtx) * 10);

    temp_v1 = sp1E8;
    var_a1 = &D_84389810[0];
    for (i = 0; i < 10; i++) {
        *temp_v1 = *var_a1++;
        if ((i == 4) || (i == 5)) {
            temp_v1->v.cn[0] = arg5;
            temp_v1->v.cn[1] = arg6;
            temp_v1->v.cn[2] = arg7;
            temp_v1->v.cn[3] = arg8;
        }
        temp_v1++;
    }

    a1_x = arg1->x;
    a1_y = arg1->y;
    a1_z = arg1->z;
    a2_x = arg2->x;
    a2_y = arg2->y;
    a2_z = arg2->z;
    a3_x = arg3->x;
    a3_y = arg3->y;
    a3_z = arg3->z;

    Vec3f_CrossProductComponents(a2_x - a1_x, a2_y - a1_y, a2_z - a1_z, a1_x - a3_x, a1_y - a3_y, a1_z - a3_z, &sp130, &sp12C, &sp128);
    if ((SQ(sp130) + SQ(sp12C) + SQ(sp128)) > 0.0f) {
        Vec3f_NormalizeComponents(&sp130, &sp12C, &sp128);
    }

    gSPDisplayList(arg0++, D_84389780);
    gDPPipeSync(arg0++);
    gDPSetEnvColor(arg0++, arg5, arg6, arg7, arg8);

    a3_x = arg4 * sp130;
    a3_y = arg4 * sp12C;
    a3_z = arg4 * sp128;

    temp_v1 = sp1E8;

    if (a3_z) {}

    tmp1 = a2_y - a3_y * 2.0f;

    temp_v1->v.ob[0] = a3_x * 2.0f + a1_x;
    temp_v1->v.ob[1] = a3_y * 2.0f + a1_y;
    temp_v1->v.ob[2] = a3_z * 2.0f + a1_z;
    temp_v1++;

    temp_v1->v.ob[0] = a3_x * 2.0f + a2_x;
    temp_v1->v.ob[1] = a3_y * 2.0f + a2_y;
    temp_v1->v.ob[2] = a3_z * 2.0f + a2_z;
    temp_v1++;

    sp8C = (-a2_x * 0.2f) + (a1_x * 1.2f);
    sp88 = (-a2_y * 0.2f) + (a1_y * 1.2f);
    sp84 = (-a2_z * 0.2f) + (a1_z * 1.2f);

    sp80 = (a2_x * 1.2f) - (a1_x * 0.2f);
    sp7C = (a2_y * 1.2f) - (a1_y * 0.2f);
    sp78 = (a2_z * 1.2f) - (a1_z * 0.2f);

    temp_v1->v.ob[0] = a3_x + sp8C;
    temp_v1->v.ob[1] = a3_y + sp88;
    temp_v1->v.ob[2] = a3_z + sp84;
    temp_v1++;

    temp_v1->v.ob[0] = a3_x + sp80;
    temp_v1->v.ob[1] = a3_y + sp7C;
    temp_v1->v.ob[2] = a3_z + sp78;
    temp_v1++;

    temp_v1->v.ob[0] = a1_x;
    temp_v1->v.ob[1] = a1_y;
    temp_v1->v.ob[2] = a1_z;
    temp_v1++;

    temp_v1->v.ob[0] = a2_x;
    temp_v1->v.ob[1] = a2_y;
    temp_v1->v.ob[2] = a2_z;
    temp_v1++;

    temp_v1->v.ob[0] = sp8C - a3_x;
    temp_v1->v.ob[1] = sp88 - a3_y;
    temp_v1->v.ob[2] = sp84 - a3_z;
    temp_v1++;

    temp_v1->v.ob[0] = sp80 - a3_x;
    temp_v1->v.ob[1] = sp7C - a3_y;
    temp_v1->v.ob[2] = sp78 - a3_z;
    temp_v1++;

    temp_v1->v.ob[0] = a1_x - a3_x * 2.0f;
    temp_v1->v.ob[1] = a1_y - a3_y * 2.0f;
    temp_v1->v.ob[2] = a1_z - a3_z * 2.0f;
    temp_v1++;

    temp_v1->v.ob[0] = a2_x - a3_x * 2.0f;
    temp_v1->v.ob[1] = tmp1;
    temp_v1->v.ob[2] = a2_z - a3_z * 2.0f;
    temp_v1++;

    guTranslate(sp1DC, 0.0f, 0.0f, 0.0f);

    gSPMatrix(arg0++, sp1DC, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    gSPVertex(arg0++, sp1E8, 10, 0);
    gSP2Triangles(arg0++, 0, 4, 2, 0, 2, 4, 6, 0);
    gSP2Triangles(arg0++, 0, 1, 4, 0, 1, 5, 4, 0);
    gSP2Triangles(arg0++, 1, 3, 5, 0, 3, 7, 5, 0);
    gSP2Triangles(arg0++, 6, 4, 8, 0, 4, 5, 8, 0);
    gSP2Triangles(arg0++, 8, 5, 9, 0, 5, 7, 9, 0);
    gSPPopMatrix(arg0++, G_MTX_MODELVIEW);

    return arg0;
}
