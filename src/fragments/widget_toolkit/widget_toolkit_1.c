#include "global.h"
#include "widget_toolkit.h"
#include "src/gfx_buffer.h"
#include "src/geo_node.h"
#include "src/model_renderer.h"
#include "src/math_util.h"

static Gfx gFragment2_1EmptyDL[] = {
    gsSPEndDisplayList(),
};

u32 gFragment2_1GeoLayout[] = {
    0x0B00001E, 0x00000000, 0x014000F0, 0x00000000,  0x000016A8, 0x00000190, 0x05000000, 0x0D000000, 0x05000000,
    0x0F000002, 0x05000000, 0x0A000000, &gGraphRoot1, 0x06000000, 0x06000000, 0x06000000, 0x04000000, 0x00000000,
};

static Vtx gFragment2_1QuadVertices[] = {
    VTX(-32, 32, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, 16, 0, 0, 512, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 16, 0, 2048, 512, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, 32, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, 0, 0, 0, 512, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 0, 0, 2048, 512, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(32, 16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),       VTX(-32, -16, 0, 0, 512, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -16, 0, 2048, 512, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 0, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, -16, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),     VTX(-32, -32, 0, 0, 512, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, -32, 0, 2048, 512, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, -16, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};

static u32* gFragment2_1TexturePtrs[] = { &D_300C000, &D_3000000, 0x3004000, &D_3008000 };

static s16 gFragment2_1RemainingSteps;
static s16 gFragment2_1StepTimer;
static s16 gFragment2_1Phase;
static s16 gFragment2_1StartFlag;
static DisplayObject gFragment2_1GraphNode;
static SimpleGfxNode* gFragment2_1DisplayNode;

void Widget_CountdownComputeScale(s16 arg0, s16 arg1, f32* arg2, f32* arg3) {
    f32 var_fv0;
    s32 temp_ft1;
    s32 var_v0;

    if (arg1 >= 0x12) {
        var_v0 = 0;
    } else if (arg1 >= 8) {
        var_v0 = 1;
    } else if (arg1 > 0) {
        var_v0 = 2;
    } else {
        var_v0 = 3;
    }

    *arg3 = 0.0f;
    *arg2 = 0.0f;

    switch (var_v0) {
        case 0:
            var_fv0 = 0.5f;
            temp_ft1 = (1.0f - ((0x1B - arg1) / 10.0f)) * (0, 81920.0f);
            if (temp_ft1 <= 0x10000) {
                if (temp_ft1 > 0x10000) {
                    var_fv0 *= .5f;
                } else {
                    var_fv0 *= 0.25f;
                }
            }

            var_fv0 *= ABS(SINS((temp_ft1 & 0xFFFF)));

            *arg3 = 1.0f - var_fv0;
            *arg2 = 1.0f - var_fv0;
            break;

        case 1:
            *arg3 = 1.0f;
            *arg2 = 1.0f;
            break;

        case 2:
            if (arg0 == 0) {
                *arg2 = arg1 / 7.0f;
                if (0.001f < *arg2) {
                    *arg3 = 1.0f / *arg2;
                } else {
                    *arg2 = 0.0f;
                }
            }
            break;
    }
}

void Widget_CountdownInit(void) {
    MainPoolState* sp1C;

    gFragment2_1Phase = 0;
    gFragment2_1RemainingSteps = 4;
    gFragment2_1StepTimer = 0;
    gFragment2_1StartFlag = 0;

    ModelRenderer_AttachSecondaryDisplayObject(&gFragment2_1GraphNode);
    sp1C = MainPool_AllocState(main_pool_get_available(), 0);
    gFragment2_1DisplayNode = GeoNode_CreateDisplayList(sp1C, NULL, 4, gFragment2_1EmptyDL);
    gFragment2_1GraphNode.node.renderFlags |= 8;

    GraphNode_AppendChild(&gFragment2_1GraphNode.node, &gFragment2_1DisplayNode->display_node_header);
    MainPool_FinalizeAllocation(sp1C);
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/widget_toolkit/widget_toolkit_1/Widget_CountdownUpdate.s")
#else
void Widget_CountdownUpdate(void) {
    s32 i;
    Gfx* temp_v0;
    s32 var_s2;
    u32* temp_s1;
    u32* img;

    var_s2 = 0;
    switch (gFragment2_1Phase) {
        case 0:
            if (gFragment2_1StartFlag != 0) {
                gFragment2_1StartFlag = 0;
                gFragment2_1Phase = 1;
                var_s2 = 1;
                gFragment2_1GraphNode.node.flags |= 1;
                gFragment2_1GraphNode.scale.x = 0.5f;
                gFragment2_1GraphNode.scale.y = 0.5f;
            }
            break;

        case 1:
            gFragment2_1StepTimer -= 1;
            if (gFragment2_1StepTimer <= 0) {
                gFragment2_1StepTimer = 0x1B;
                gFragment2_1RemainingSteps -= 1;
            }

            if ((gFragment2_1RemainingSteps >= 0) && (gFragment2_1RemainingSteps < 4)) {
                var_s2 = 1;
                Widget_CountdownComputeScale(gFragment2_1RemainingSteps, gFragment2_1StepTimer, &gFragment2_1GraphNode.scale.x, &gFragment2_1GraphNode.scale.y);
            } else {
                gFragment2_1DisplayNode->gfx = gFragment2_1EmptyDL;
            }

            if (gFragment2_1RemainingSteps < 0) {
                gFragment2_1Phase = 2;
                gFragment2_1StepTimer = 0x1B;
                gFragment2_1GraphNode.node.flags &= ~1;
            }
            break;

        case 2:
            if (gFragment2_1StepTimer == 0x1B) {
                gFragment2_1StepTimer = 0;
            }
            break;
    }

    if ((var_s2 != 0) && (gFragment2_1RemainingSteps < 4)) {
        temp_s1 = gFragment2_1TexturePtrs[gFragment2_1RemainingSteps];
        temp_v0 = Gfx_AllocDisplayList(sizeof(Gfx) * 48);
        gFragment2_1DisplayNode->gfx = temp_v0;

        gSPSetGeometryMode(&temp_v0[0], G_CULL_BACK);
        gSPTexture(&temp_v0[1], 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
        gDPPipeSync(&temp_v0[2]);
        gDPSetCombineMode(&temp_v0[3], G_CC_DECALRGBA, G_CC_PASS2);
        gSPVertex(&temp_v0[4], gFragment2_1QuadVertices, 16, 0);

        temp_v0 += 5;
        img = temp_s1;

        for (i = 0; i < 4; i++, img += 0x400) {
            gDPLoadTextureTile(temp_v0++, img, G_IM_FMT_RGBA, G_IM_SIZ_32b, 64, 0, 0, 0, 63, 15, 0,
                               G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                               G_TX_NOLOD, G_TX_NOLOD);
            gSP2Triangles(temp_v0++, (i * 4) + 0, (i * 4) + 1, (i * 4) + 2, 0, (i * 4) + 0, (i * 4) + 2, (i * 4) + 3,
                          0);
        }

        gSPEndDisplayList(temp_v0++);
    }
}
#endif

void Widget_CountdownStart(s16 arg0) {
    gFragment2_1StartFlag = arg0;
}

s32 Widget_CountdownGetState(void) {
    s16 var_v1 = -1;

    if (gFragment2_1Phase == 2) {
        var_v1 = -2;
    } else if ((gFragment2_1StepTimer == 0x1B) && (gFragment2_1Phase == 1)) {
        var_v1 = gFragment2_1RemainingSteps;
    }

    return var_v1;
}
