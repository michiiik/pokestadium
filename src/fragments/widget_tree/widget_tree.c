#include "widget_tree.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/gb_data.h"
#include "src/table_view.h"
#include "src/text_system.h"
#include "src/gb_save.h"
#include "src/audio_sfx.h"
#include "src/memmap.h"
#include "src/memory.h"
#include "src/pokemon_stats.h"

static Color_RGBA8 D_8850CF80 = { 0xF0, 0xF0, 0xF0, 0xFF };

void WidgetTree_DrawTiledTexture(unk_func_88500020* arg0, s32 arg1, s32 arg2) {
    s32 temp_lo = 0x1000 / (arg0->unk_00 * 2);
    s32 var_t3 = arg0->unk_04;
    u8* var_s2 = arg0->unk_0C;
    s32 var_s3 = arg2;
    s32 var_t0;

    while (var_t3 > 0) {
        if (temp_lo < var_t3) {
            var_t0 = temp_lo;
        } else {
            var_t0 = var_t3;
        }

        gDPLoadTextureBlock(gDisplayListHead++, var_s2, G_IM_FMT_RGBA, G_IM_SIZ_16b, arg0->unk_00, var_t0, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

        gSPTextureRectangle(gDisplayListHead++, arg1 << 2, var_s3 << 2, ((arg0->unk_08 + arg1) - 1) << 2,
                            ((var_s3 + var_t0) - 1) << 2, G_TX_RENDERTILE, 0, 0, 0x1000, 0x0400);

        var_s3 += var_t0;
        var_s2 += var_t0 * arg0->unk_00 * 2;
        var_t3 -= var_t0;
    }
}

void WidgetTree_DrawTiledTextureFmt(unk_func_88500020* arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 temp_lo = 0x1000 / (arg0->unk_00 * 2);
    s32 var_t0;
    s32 var_t3 = arg0->unk_04;
    u8* var_s2 = arg0->unk_0C;
    s32 var_s3 = arg3;

    while (var_t3 > 0) {
        if (temp_lo < var_t3) {
            var_t0 = temp_lo;
        } else {
            var_t0 = var_t3;
        }

        gDPLoadTextureBlock(gDisplayListHead++, var_s2, arg1, G_IM_SIZ_16b, arg0->unk_00, var_t0, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, arg2 << 2, var_s3 << 2, (arg0->unk_08 + arg2) << 2,
                            (var_s3 + var_t0) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

        var_s3 += var_t0;
        var_s2 += var_t0 * arg0->unk_00 * 2;
        var_t3 -= var_t0;
    }
}

void WidgetTree_InitNode(WidgetLinkHeader* arg0, s32 arg1) {
    arg0->recordSize = arg1;
    arg0->firstChild = NULL;
    arg0->nextSibling = NULL;
    arg0->destructor = NULL;
}

void WidgetTree_PrependChild(WidgetLinkHeader* arg0, WidgetLinkHeader* arg1) {
    arg1->nextSibling = arg0->firstChild;
    arg0->firstChild = arg1;
}

void WidgetTree_AppendChild(WidgetLinkHeader* arg0, WidgetLinkHeader* arg1) {
    WidgetLinkHeader* var_a0;
    WidgetLinkHeader* var_v1;

    if (arg0->firstChild == NULL) {
        arg0->firstChild = arg1;
        return;
    }

    var_v1 = arg0->firstChild;
    var_a0 = arg0->firstChild->nextSibling;

    while (var_a0 != NULL) {
        var_v1 = var_a0;
        var_a0 = var_a0->nextSibling;
    }

    var_v1->nextSibling = arg1;
}

void WidgetTree_FreeChildren(WidgetLinkHeader* arg0, MemoryPool* arg1) {
    WidgetLinkHeader* temp_s1;
    WidgetLinkHeader* var_s0 = arg0->firstChild;

    while (var_s0 != NULL) {
        WidgetTree_FreeChildren(var_s0, arg1);
        if (var_s0->destructor != NULL) {
            var_s0->destructor(var_s0, arg1);
        }
        temp_s1 = var_s0->nextSibling;
        mem_pool_free(arg1, var_s0);
        var_s0 = temp_s1;
    }
    arg0->firstChild = NULL;
}

void WidgetTree_FreeTree(WidgetLinkHeader* arg0, MemoryPool* arg1) {
    WidgetLinkHeader* temp_s1;
    WidgetLinkHeader* var_s0 = arg0->firstChild;

    while (var_s0 != NULL) {
        temp_s1 = var_s0->nextSibling;
        WidgetTree_FreeTree(var_s0, arg1);
        var_s0 = temp_s1;
    }

    if (arg0->destructor != NULL) {
        arg0->destructor(arg0, arg1);
    }
    mem_pool_free(arg1, arg0);
}

void WidgetTree_InitWidget(WidgetNode* arg0, s32 arg1) {
    WidgetTree_InitNode(&arg0->link, arg1);
    arg0->position.x = 0;
    arg0->position.y = 0;
    arg0->size.x = 0x10;
    arg0->size.y = 0x10;
    arg0->drawCallback = NULL;
    arg0->updateCallback = NULL;
    arg0->inputCallback = NULL;
    arg0->setStateCallback = WidgetTree_SetStateDefault;
    arg0->flags = 1;
    arg0->state = 0;
}

void WidgetTree_Draw(WidgetNode* arg0, s32 arg1, s32 arg2) {
    s32 var_v1 = 0;
    WidgetLinkHeader* var_s0;

    if (arg0->flags & 1) {
        arg1 += arg0->position.x;
        arg2 += arg0->position.y;

        if (arg0->drawCallback != NULL) {
            var_v1 = arg0->drawCallback(arg0, arg1, arg2);
        }

        if (var_v1 == 0) {
            var_s0 = arg0->link.firstChild;
            while (var_s0 != NULL) {
                WidgetTree_Draw(var_s0, arg1, arg2);
                var_s0 = var_s0->nextSibling;
            }
        }
    }
}

void WidgetTree_Update(WidgetNode* arg0) {
    s32 var_v1 = 0;
    WidgetLinkHeader* var_s0;

    if (arg0->flags & 1) {
        if (arg0->updateCallback != NULL) {
            var_v1 = arg0->updateCallback(arg0);
        }

        if (var_v1 == 0) {
            var_s0 = arg0->link.firstChild;
            while (var_s0 != NULL) {
                WidgetTree_Update(var_s0);
                var_s0 = var_s0->nextSibling;
            }
        }
    }
}

void WidgetTree_SetStateDefault(WidgetNode* arg0, s32 arg1) {
    WidgetNode* var_s0 = arg0->link.firstChild;

    arg0->state = arg1;

    while (var_s0 != NULL) {
        var_s0->setStateCallback(var_s0, arg1);
        var_s0 = var_s0->link.nextSibling;
    }
}

void WidgetTree_InitPagedContainer(unk_func_88500994* arg0, s32 arg1, s32 arg2) {
    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_88500994));
    arg0->unk_00.drawCallback = WidgetTree_DrawCurrentPage;
    arg0->unk_00.position.x = arg1;
    arg0->unk_00.position.y = arg2;
    arg0->unk_2C = 0;
    arg0->unk_30 = 0;
}

s32 WidgetTree_DrawCurrentPage(unk_func_88500994* arg0, s32 arg1, s32 arg2) {
    s32 i;
    WidgetLinkHeader* var_a0;

    if (arg0->unk_30 > 0) {
        var_a0 = arg0->unk_00.link.firstChild;

        for (i = 0; i < arg0->unk_2C; i++) {
            var_a0 = var_a0->nextSibling;
        }
        WidgetTree_Draw(var_a0, arg1, arg2);
    }
    return 1;
}

void WidgetTree_AddPage(unk_func_88500994* arg0, WidgetNode* arg1) {
    WidgetTree_AppendChild(&arg0->unk_00.link, &arg1->link);
    arg0->unk_30++;
}

void WidgetTree_SelectPage(unk_func_88500994* arg0, s32 arg1) {
    arg0->unk_2C = arg1;
}

#ifdef NON_MATCHING
void WidgetTree_DrawTileGrid(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 sp88;
    s32 sp34;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);

    gDPSetScissorFrac(gDisplayListHead++, G_SC_NON_INTERLACE, arg0 * 4.0f, arg1 * 4.0f, (arg0 + arg2) * 4.0f,
                      (arg1 + arg3) * 4.0f);

    sp88 = arg1 + arg3;
    sp34 = arg0 + arg2;

    while (arg1 < sp88) {
        while (arg0 < sp34) {
            gDPLoadTextureBlock(gDisplayListHead++, arg4, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 16, 0,
                                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(gDisplayListHead++, arg0 << 2, arg1 << 2, (arg0 + 0x20) << 2, (arg1 + 0x20) << 2,
                                G_TX_RENDERTILE, 0, 0, 0x0200, 0x0200);

            arg0 += 0x20;
            arg4 += 0x200;
        }

        arg1 += 0x20;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 640, 480);
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/widget_tree/widget_tree/WidgetTree_DrawTileGrid.s")
#endif

void WidgetTree_InitTiledTextureRegion(unk_func_88500E34* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, u8* arg5) {
    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_88500E34));
    arg0->unk_00.drawCallback = WidgetTree_DrawTiledTextureRegion;
    arg0->unk_00.position.x = arg1;
    arg0->unk_00.position.y = arg2;
    arg0->unk_00.size.x = arg3;
    arg0->unk_00.size.y = arg4;
    arg0->unk_2C = arg5;
}

s32 WidgetTree_DrawTiledTextureRegion(unk_func_88500E34* arg0, s32 arg1, s32 arg2) {
    WidgetTree_DrawTileGrid(arg1, arg2, arg0->unk_00.size.x, arg0->unk_00.size.y, arg0->unk_2C);
    return 0;
}

void WidgetTree_InitTintedTexture(unk_func_88500EE4* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, u8* arg5, Color_RGBA8 arg6,
                   Color_RGBA8 arg7) {
    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_88500EE4));
    arg0->unk_00.drawCallback = WidgetTree_DrawTintedTexture;
    arg0->unk_00.position.x = arg1;
    arg0->unk_00.position.y = arg2;
    arg0->unk_00.size.x = arg3;
    arg0->unk_00.size.y = arg4;
    arg0->unk_2C = arg5;
    arg0->unk_30 = arg6;
    arg0->unk_34 = arg7;
}

s32 WidgetTree_DrawTintedTexture(unk_func_88500EE4* arg0, s32 arg1, s32 arg2) {
    s32 h = 0x80;
    s32 w = 0x40;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineLERP(gDisplayListHead++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, 1, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, 1);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, arg0->unk_30.r, arg0->unk_30.g, arg0->unk_30.b, arg0->unk_30.a);
    gDPSetEnvColor(gDisplayListHead++, arg0->unk_34.r, arg0->unk_34.g, arg0->unk_34.b, arg0->unk_34.a);

    gDPLoadTextureBlock_4b(gDisplayListHead++, arg0->unk_2C, G_IM_FMT_I, h, w, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 7, 6, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(gDisplayListHead++, arg1 << 2, arg2 << 2, ((arg0->unk_00.size.x + arg1) - 1) << 2,
                        ((arg2 + arg0->unk_00.size.y) - 1) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    return 0;
}

void WidgetTree_InitSolidColor(unk_func_885012A4* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, Color_RGBA8 arg5) {
    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_885012A4));
    arg0->unk_00.drawCallback = WidgetTree_DrawSolidColor;
    arg0->unk_00.position.x = arg1;
    arg0->unk_00.position.y = arg2;
    arg0->unk_00.size.x = arg3;
    arg0->unk_00.size.y = arg4;
    arg0->unk_2C = arg5;
}

s32 WidgetTree_DrawSolidColor(unk_func_885012A4* arg0, s32 arg1, s32 arg2) {
    s32 temp_t1;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_FILL);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);

    // clang-format off
    temp_t1 = ((arg0->unk_2C.r << 8) & 0xF800) | ((arg0->unk_2C.g << 3) & 0x7C0) | ((arg0->unk_2C.b >> 2) & 0x3E) | 1; gDPPipeSync(gDisplayListHead++);
    // clang-format on
    gDPSetFillColor(gDisplayListHead++, (temp_t1 << 0x10) | temp_t1);
    gDPFillRectangle(gDisplayListHead++, arg1, arg2, (arg0->unk_00.size.x + arg1) - 1,
                     (arg2 + arg0->unk_00.size.y) - 1);

    return 0;
}

void WidgetTree_InitPatternTexture(unk_func_8850143C* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, Color_RGBA8 arg5,
                   Color_RGBA8 arg6) {
    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_8850143C));
    arg0->unk_00.drawCallback = WidgetTree_DrawPatternTexture;
    arg0->unk_00.position.x = arg1;
    arg0->unk_00.position.y = arg2;
    arg0->unk_00.size.x = arg3;
    arg0->unk_00.size.y = arg4;
    arg0->unk_2C = arg5;
    arg0->unk_30 = arg6;
}

s32 WidgetTree_DrawPatternTexture(unk_func_8850143C* arg0, s32 arg1, s32 arg2) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);

    if (arg0->unk_2C.a == 0xFF) {
        gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    } else {
        gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    }

    gDPSetCombineLERP(gDisplayListHead++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, PRIMITIVE, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, PRIMITIVE);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_BILERP);

    gDPSetPrimColor(gDisplayListHead++, 0, 0, arg0->unk_2C.r, arg0->unk_2C.g, arg0->unk_2C.b, (0, arg0->unk_2C.a));
    gDPSetEnvColor(gDisplayListHead++, arg0->unk_30.r, arg0->unk_30.g, arg0->unk_30.b, (0, arg0->unk_30.a));

    gDPLoadTextureBlock(gDisplayListHead++, D_4000008, G_IM_FMT_I, G_IM_SIZ_8b, 64, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, arg1 << 2, arg2 << 2, (arg0->unk_00.size.x + arg1) << 2,
                        (arg2 + arg0->unk_00.size.y) << 2, G_TX_RENDERTILE, 0, 0,
                        0x10000 / arg0->unk_00.size.x, 0x10000 / arg0->unk_00.size.y);

    return 0;
}

void WidgetTree_InitCornerFrame(unk_func_8850182C* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, Color_RGBA8 arg5) {
    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_8850182C));
    arg0->unk_00.drawCallback = WidgetTree_DrawCornerFrame;
    arg0->unk_00.position.x = arg1;
    arg0->unk_00.position.y = arg2;
    arg0->unk_00.size.x = arg3;
    arg0->unk_00.size.y = arg4;
    arg0->unk_2C = arg5;
}

s32 WidgetTree_DrawCornerFrame(unk_func_8850182C* arg0, s32 arg1, s32 arg2) {
    s32 w = 16;
    s32 h = 8;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, arg0->unk_2C.r, arg0->unk_2C.g, arg0->unk_2C.b, arg0->unk_2C.a);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_4007750, G_IM_FMT_I, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(gDisplayListHead++, arg1 << 2, arg2 << 2, ((arg0->unk_00.size.x + arg1) - w) << 2,
                        (arg2 + h) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_4007798, G_IM_FMT_I, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(gDisplayListHead++, ((arg0->unk_00.size.x + arg1) - w) << 2, arg2 << 2,
                        (arg0->unk_00.size.x + arg1) << 2, ((arg2 + arg0->unk_00.size.y) - h) << 2,
                        G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_40077E0, G_IM_FMT_I, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(gDisplayListHead++, (arg1 + w) << 2, ((arg2 + arg0->unk_00.size.y) - h) << 2,
                        (arg0->unk_00.size.x + arg1) << 2, (arg2 + arg0->unk_00.size.y) << 2,
                        G_TX_RENDERTILE, (arg0->unk_00.size.x * -0x20) + 0x400, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_4007828, G_IM_FMT_I, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(gDisplayListHead++, arg1 << 2, (arg2 + h) << 2, (arg1 + w) << 2,
                        (arg2 + arg0->unk_00.size.y) << 2, G_TX_RENDERTILE, 0,
                        (arg0->unk_00.size.y * -0x20) + 0x200, 0x0400, 0x0400);

    if ((arg0->unk_00.size.x > 0x20) && (arg0->unk_00.size.y > 0x10)) {
        gDPPipeSync(gDisplayListHead++);

        gDPSetCycleType(gDisplayListHead++, G_CYC_FILL);
        gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);

        {
            // clang-format off
            s32 temp_a3 = ((arg0->unk_2C.r << 8) & 0xF800) | ((arg0->unk_2C.g << 3) & 0x7C0) | ((arg0->unk_2C.b >> 2) & 0x3E) | 1; gDPPipeSync(gDisplayListHead++);
            // clang-format on
            gDPSetFillColor(gDisplayListHead++, (temp_a3 << 0x10) | temp_a3);
        }

        gDPFillRectangle(gDisplayListHead++, arg1 + w, arg2 + h, ((arg1 + w) + arg0->unk_00.size.x) - 0x21,
                         ((arg2 + h) + arg0->unk_00.size.y) - 0x11);
    }

    return 0;
}

void WidgetTree_InitAnimatedPanel(WidgetAnimatedPanel* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    WidgetTree_InitWidget(&arg0->node, sizeof(WidgetAnimatedPanel));

    arg0->node.drawCallback = WidgetTree_DrawAnimatedPanel;
    arg0->node.updateCallback = WidgetTree_UpdateAnimatedPanel;
    arg0->node.inputCallback = WidgetTree_GetAnimatedPanelInputState;

    arg0->node.position.x = arg1;
    arg0->node.position.y = arg2;
    arg0->node.size.x = arg3;
    arg0->node.size.y = arg4;
    arg0->animState = 0;
    arg0->animFrame = -1;
    arg0->node.flags |= 0x400;
    arg0->node.flags &= ~1;
}

s32 WidgetTree_UpdateAnimatedPanel(WidgetAnimatedPanel* arg0) {
    arg0->animFrame++;
    if (arg0->animFrame < 8) {
        arg0->animState = 1;
        if (arg0->animFrame == 1) {
            Audio_PlaySoundEffectById(4);
        }
    } else if (arg0->animFrame >= 0x14) {
        arg0->animFrame = -1;
        arg0->animState = 0;
        if (arg0->node.flags & 0x400) {
            arg0->node.flags &= ~1;
        }
    } else if (arg0->animFrame >= 0xC) {
        arg0->animState = 4;
    } else {
        arg0->animState = 2;
        if (arg0->animFrame == 0xB) {
            arg0->animFrame--;
        } else if (arg0->animFrame < 0xA) {
            arg0->animState |= 8;
        }
    }

    return (arg0->animState & 2) ? 0 : 1;
}

s32 WidgetTree_DrawAnimatedPanel(WidgetAnimatedPanel* arg0, s32 arg1, s32 arg2) {
    s32 sp114;
    s32 sp110;
    s32 sp10C;
    s32 sp108;
    s32 val = 0x10;
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_a2;
    s32 temp_t0;
    s32 temp_t2;
    s32 var_v0;
    s32 var_v1_2;
    UNUSED s32 pad;

    if (0) {}

    temp_a1 = arg2 - 7;

    if ((arg0->animState & 0xD) || ((arg0->animState & 2) && (arg0->node.flags & 0x200))) {
        temp_a0 = arg1 - 7;

        var_v0 = arg0->node.size.x + 0xE;
        var_v1_2 = arg0->node.size.y + 0xE;

        if (var_v0 < val) {
            var_v0 = val;
        }

        if (var_v1_2 < val) {
            var_v1_2 = val;
        }

        if (arg0->animState & 4) {
            temp_a2 = 0x13 - arg0->animFrame;

            sp10C = ((var_v0 - val) * temp_a2) / 8;
            sp108 = ((var_v1_2 - val) * temp_a2) / 8;

            sp114 = (((var_v0 - val) - sp10C) / 2) + temp_a0;
            sp110 = (((var_v1_2 - val) - sp108) / 2) + temp_a1;

            // clang-format off
            sp10C += val; sp108 += val;
            // clang-format on
        } else if (arg0->animState & 1) {
            temp_t0 = arg0->animFrame;

            sp10C = ((var_v0 - val) * temp_t0) / 8;
            sp108 = ((var_v1_2 - val) * temp_t0) / 8;

            sp114 = (((var_v0 - val) - sp10C) / 2) + temp_a0;
            sp110 = (((var_v1_2 - val) - sp108) / 2) + temp_a1;

            // clang-format off
            sp10C += val; sp108 += val;
            // clang-format on
        } else {
            sp114 = temp_a0;
            sp110 = temp_a1;
            sp10C = var_v0;
            sp108 = var_v1_2;
        }

        gDPPipeSync(gDisplayListHead++);
        gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
        gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
        gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
        gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);

        gDPLoadTextureBlock(gDisplayListHead++, D_4006F40, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, sp114 << 2, sp110 << 2, ((sp114 + sp10C) - 8) << 2, (sp110 + 8) << 2,
                            G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

        gDPLoadTextureBlock(gDisplayListHead++, D_4006F88, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, ((sp114 + sp10C) - 8) << 2, sp110 << 2, (sp114 + sp10C) << 2,
                            ((sp110 + sp108) - 8) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

        gDPLoadTextureBlock(gDisplayListHead++, D_4006FD0, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, (sp114 + 8) << 2, ((sp110 + sp108) - 8) << 2, (sp114 + sp10C) << 2,
                            (sp110 + sp108) << 2, G_TX_RENDERTILE, (sp10C * -0x20) + 0x200, 0, 0x0400, 0x0400);

        gDPLoadTextureBlock(gDisplayListHead++, D_4007018, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, sp114 << 2, (sp110 + 8) << 2, (sp114 + 8) << 2, (sp110 + sp108) << 2,
                            G_TX_RENDERTILE, 0, (sp108 * -0x20) + 0x200, 0x0400, 0x0400);
    }

    return (arg0->animState & 2) ? 0 : 1;
}

s32 WidgetTree_GetAnimatedPanelInputState(WidgetAnimatedPanel* arg0, UNUSED Controller* arg1) {
    s32 ret;

    if (arg0->animState & 1) {
        ret = 0x10;
    } else if (arg0->animState & 4) {
        ret = 0x20;
    } else if (arg0->animState & 2) {
        if ((arg0->animFrame == 0xA) && (arg0->node.flags & 0x100)) {
            arg0->animFrame = 0xB;
            ret = 0x20;
        } else {
            ret = 4;
        }
    } else {
        ret = 2;
    }

    return ret;
}

void WidgetTree_OpenAnimatedPanel(WidgetAnimatedPanel* arg0) {
    if ((arg0->animFrame >= 0xB) && (arg0->node.flags & 1)) {
        do {
            Ui_SendMessageAndPollInput(NULL);
        } while (arg0->node.flags & 1);
    }
    arg0->node.flags |= 1;
}

void WidgetTree_InitHorizontalFrame(WidgetNode* arg0, s32 arg1, s32 arg2, s32 arg3) {
    WidgetTree_InitWidget(arg0, sizeof(WidgetNode));
    arg0->drawCallback = WidgetTree_DrawHorizontalFrame;
    arg0->position.x = arg1;
    arg0->position.y = arg2;
    arg0->size.x = arg3;
    arg0->size.y = 8;
}

s32 WidgetTree_DrawHorizontalFrame(WidgetNode* arg0, s32 arg1, s32 arg2) {
    s32 sp84;
    s32 sp80;
    s32 sp7C;

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    // clang-format off
    sp84 = arg1 - 7; sp80 = arg2 - 1; 
    sp7C = arg0->size.x + 0xE;
    // clang-format on

    gDPLoadTextureBlock(gDisplayListHead++, D_4007060, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, sp84 << 2, sp80 << 2, ((sp84 + sp7C) - 8) << 2, (sp80 + 8) << 2,
                        G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_40070A8, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, ((sp84 + sp7C) - 8) << 2, sp80 << 2, (sp84 + sp7C) << 2, (sp80 + 8) << 2,
                        G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    return 0;
}

void WidgetTree_InitDelayedWidget(WidgetDelayedNode* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    WidgetTree_InitWidget(&arg0->node, sizeof(WidgetDelayedNode));
    arg0->node.drawCallback = WidgetTree_DrawDelayedWidget;
    arg0->node.updateCallback = WidgetTree_UpdateDelayedWidget;
    arg0->node.position.x = arg1;
    arg0->node.position.y = arg2;
    arg0->node.size.x = arg3;
    arg0->node.size.y = arg4;
    arg0->delayCounter = -1;
}

s32 WidgetTree_DrawDelayedWidget(WidgetDelayedNode* arg0, s32 arg1, s32 arg2) {
    return (arg0->delayCounter < 3) ? 0 : 1;
}

s32 WidgetTree_UpdateDelayedWidget(WidgetDelayedNode* arg0) {
    s32 var_v1 = 1;

    if (arg0->delayCounter < 0) {
        var_v1 = 0;
    } else if (arg0->delayCounter < 3) {
        arg0->delayCounter++;
        var_v1 = 0;
    }
    return var_v1;
}

void WidgetTree_InitVisibilityGate(WidgetNode* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    WidgetTree_InitWidget(arg0, sizeof(WidgetNode));
    arg0->drawCallback = WidgetTree_DrawVisibilityGate;
    arg0->updateCallback = WidgetTree_ClearVisibilityGateFlags;
    arg0->position.x = arg1;
    arg0->position.y = arg2;
    arg0->size.x = arg3;
    arg0->size.y = arg4;
}

s32 WidgetTree_ClearVisibilityGateFlags(WidgetNode* arg0) {
    arg0->flags &= ~0x300;
    return 0;
}

s32 WidgetTree_DrawVisibilityGate(WidgetNode* arg0, s32 arg1, s32 arg2) {
    return ((arg0->flags & 0x100) || !(arg0->flags & 0x200)) ? 0 : 1;
}

void WidgetTree_InitVisibilityGateBridge(unk_func_88503298* arg0, WidgetAnimatedPanel* arg1, WidgetNode* arg2) {
    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_88503298));
    arg0->unk_00.updateCallback = WidgetTree_UpdateVisibilityGateBridge;
    arg0->unk_2C = arg1;
    arg0->unk_30 = arg2;
}

s32 WidgetTree_UpdateVisibilityGateBridge(unk_func_88503298* arg0) {
    if (!(arg0->unk_2C->node.flags & 1)) {
        return 0;
    }

    if (arg0->unk_2C->animState & 2) {
        arg0->unk_30->unk_00.flags |= 0x200;
    } else {
        arg0->unk_30->unk_00.flags |= 0x100;
    }

    return 0;
}

void WidgetTree_InitAnimatedFrameVariantA(WidgetAnimatedFrame* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, Color_RGBA8 arg5) {
    WidgetTree_InitWidget(&arg0->node, sizeof(WidgetAnimatedFrame));
    arg0->node.drawCallback = WidgetTree_DrawAnimatedFrameVariantA;
    arg0->node.updateCallback = WidgetTree_UpdateAnimatedFrameVariantA;
    arg0->node.setStateCallback = WidgetTree_SetAnimatedFrameVariantAState;
    arg0->node.position.x = arg1;
    arg0->node.position.y = arg2;
    arg0->node.size.x = arg3;
    arg0->node.size.y = arg4;
    arg0->color = arg5;
    arg0->pulseFrame = 0;
}

s32 WidgetTree_UpdateAnimatedFrameVariantA(WidgetAnimatedFrame* arg0) {
    arg0->pulseFrame++;
    arg0->pulseFrame &= 7;
    return 0;
}

s32 WidgetTree_DrawAnimatedFrameVariantA(WidgetAnimatedFrame* arg0, s32 arg1, s32 arg2) {
    s32 sp104;
    s32 sp100;
    s32 spFC;
    s32 spF8;
    Color_RGBA8 spF4;
    s32 var_v0;

    // clang-format off
    sp104 = arg1 - 3; sp100 = arg2 - 3;
    // clang-format on

    spFC = arg0->node.size.x + 6;
    spF8 = arg0->node.size.y + 6;
    spF4 = arg0->color;

    if (!(arg0->node.state & 1) && ((arg0->node.flags & 0x100) || !(arg0->node.state & 0x100))) {
        return 0;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);

    if (arg0->node.state & 0x100) {
        spF4.r /= 2;
        spF4.g /= 2;
        spF4.b /= 2;
        var_v0 = 1;
    } else {
        var_v0 = (arg0->pulseFrame - 1) & 7;
        if (var_v0 >= 4) {
            var_v0 = 7 - var_v0;
        }
    }

    gDPSetPrimColor(gDisplayListHead++, 0, 0, spF4.r, spF4.g, spF4.b, spF4.a);

    gDPLoadTextureBlock(gDisplayListHead++, D_40070F0, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, (sp104 + var_v0) << 2, (sp100 + var_v0) << 2,
                        ((((sp104 + var_v0) + spFC) - (var_v0 << 1)) - 8) << 2, ((sp100 + var_v0) + 8) << 2,
                        G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007138, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, ((((sp104 + var_v0) + spFC) - (var_v0 << 1)) - 8) << 2,
                        (sp100 + var_v0) << 2, (((sp104 + var_v0) + spFC) - (var_v0 << 1)) << 2,
                        ((((sp100 + var_v0) + spF8) - (var_v0 << 1)) - 8) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007180, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(
        gDisplayListHead++, ((sp104 + var_v0) + 8) << 2, ((((sp100 + var_v0) + spF8) - (var_v0 << 1)) - 8) << 2,
        (((sp104 + var_v0) + spFC) - (var_v0 << 1)) << 2, (((sp100 + var_v0) + spF8) - (var_v0 << 1)) << 2,
        G_TX_RENDERTILE, ((spFC - (var_v0 << 1)) * -0x20) + 0x200, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_40071C8, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, (sp104 + var_v0) << 2, ((sp100 + var_v0) + 8) << 2,
                        ((sp104 + var_v0) + 8) << 2, (((sp100 + var_v0) + spF8) - (var_v0 << 1)) << 2, G_TX_RENDERTILE,
                        0, ((spF8 - (var_v0 << 1)) * -0x20) + 0x200, 0x0400, 0x0400);

    return 0;
}

void WidgetTree_SetAnimatedFrameVariantAState(WidgetAnimatedFrame* arg0, s32 arg1) {
    arg0->node.state = arg1;
    arg0->pulseFrame = 0;
}

void WidgetTree_InitAnimatedFrameVariantB(WidgetAnimatedFrame* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, Color_RGBA8 arg5) {
    WidgetTree_InitWidget(&arg0->node, sizeof(WidgetAnimatedFrame));
    arg0->node.drawCallback = WidgetTree_DrawAnimatedFrameVariantB;
    arg0->node.updateCallback = WidgetTree_UpdateAnimatedFrameVariantB;
    arg0->node.setStateCallback = WidgetTree_SetAnimatedFrameVariantBState;
    arg0->node.position.x = arg1;
    arg0->node.position.y = arg2;
    arg0->node.size.x = arg3;
    arg0->node.size.y = arg4;
    arg0->color = arg5;
    arg0->pulseFrame = 0;
}

s32 WidgetTree_UpdateAnimatedFrameVariantB(WidgetAnimatedFrame* arg0) {
    arg0->pulseFrame++;
    arg0->pulseFrame &= 7;
    return 0;
}

s32 WidgetTree_DrawAnimatedFrameVariantB(WidgetAnimatedFrame* arg0, s32 arg1, s32 arg2) {
    s32 sp10C;
    s32 sp108;
    s32 sp104;
    s32 sp100;
    Color_RGBA8 spFC;
    s32 var_v0;
    s32 w = 8;
    s32 h = 8;

    sp10C = arg1;
    sp108 = arg2;
    sp104 = arg0->node.size.x;
    sp100 = arg0->node.size.y;
    spFC = arg0->color;

    if (!(arg0->node.state & 1) && ((arg0->node.flags & 0x100) || !(arg0->node.state & 0x100))) {
        return 0;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);

    if (arg0->node.state & 0x100) {
        spFC.r /= 2;
        spFC.g /= 2;
        spFC.b /= 2;
        var_v0 = 1;
    } else {
        var_v0 = (arg0->pulseFrame - 1) & 7;
        if (var_v0 >= 4) {
            var_v0 = 7 - var_v0;
        }
    }

    sp10C += var_v0;
    sp108 += var_v0;
    sp104 -= (var_v0 << 1);
    sp100 -= (var_v0 << 1);

    gDPSetPrimColor(gDisplayListHead++, 0, 0, var_v0 = spFC.r, spFC.g, spFC.b, spFC.a);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007BF0, G_IM_FMT_IA, G_IM_SIZ_8b, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, sp10C << 2, sp108 << 2, ((sp10C + sp104) - w) << 2, (sp108 + h) << 2,
                        G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007BF0, G_IM_FMT_IA, G_IM_SIZ_8b, w, h, 0, G_TX_MIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_CLAMP, 3, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, ((sp10C + sp104) - w) << 2, sp108 << 2, (sp10C + sp104) << 2,
                        ((sp108 + sp100) - h) << 2, G_TX_RENDERTILE, 0x0100, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007BF0, G_IM_FMT_IA, G_IM_SIZ_8b, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, 3, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, (sp10C + w) << 2, ((sp108 + sp100) - h) << 2, (sp10C + sp104) << 2,
                        (sp108 + sp100) << 2, G_TX_RENDERTILE, (sp104 - 9) << 5, 0x0100, -0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007BF0, G_IM_FMT_IA, G_IM_SIZ_8b, w, h, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, sp10C << 2, (sp108 + h) << 2, (sp10C + w) << 2, (sp108 + sp100) << 2,
                        G_TX_RENDERTILE, 0, (sp100 - 9) << 5, 0x0400, -0x0400);

    return 0;
}

void WidgetTree_SetAnimatedFrameVariantBState(WidgetAnimatedFrame* arg0, s32 arg1) {
    arg0->node.state = arg1;
    arg0->pulseFrame = 0;
}

void WidgetTree_InitAnimatedFrameVariantC(WidgetAnimatedFrame* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, Color_RGBA8 arg5) {
    WidgetTree_InitWidget(&arg0->node, sizeof(WidgetAnimatedFrame));
    arg0->node.drawCallback = WidgetTree_DrawAnimatedFrameVariantC;
    arg0->node.updateCallback = WidgetTree_UpdateAnimatedFrameVariantC;
    arg0->node.setStateCallback = WidgetTree_SetAnimatedFrameVariantCState;
    arg0->node.position.x = arg1;
    arg0->node.position.y = arg2;
    arg0->node.size.x = arg3;
    arg0->node.size.y = arg4;
    arg0->color = arg5;
    arg0->pulseFrame = 0;
}

s32 WidgetTree_UpdateAnimatedFrameVariantC(WidgetAnimatedFrame* arg0) {
    arg0->pulseFrame++;
    arg0->pulseFrame &= 7;
    return 0;
}

s32 WidgetTree_DrawAnimatedFrameVariantC(WidgetAnimatedFrame* arg0, s32 arg1, s32 arg2) {
    s32 spFC;
    s32 spF8;
    s32 spF4;
    s32 spF0;
    Color_RGBA8 spEC;
    s32 var_v0;
    s32 w = 16;
    s32 h = 16;

    spFC = arg1;
    spF8 = arg2;
    spF4 = arg0->node.size.x + 2;
    spF0 = arg0->node.size.y + 2;
    spEC = arg0->color;

    if (!(arg0->node.state & 1) && !(arg0->node.state & 0x100)) {
        return 0;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);

    if (arg0->node.state & 0x100) {
        spEC.r = (spEC.r * 0x46) / 100;
        spEC.g = (spEC.g * 0x46) / 100;
        spEC.b = (spEC.b * 0x46) / 100;
        var_v0 = 1;
    } else {
        var_v0 = (arg0->pulseFrame - 1) & 7;
        if (var_v0 >= 4) {
            var_v0 = 7 - var_v0;
        }
    }

    spFC += var_v0;
    spF8 += var_v0;
    spF4 -= var_v0 << 1;
    spF0 -= var_v0 << 1;

    gDPSetPrimColor(gDisplayListHead++, 0, 0, spEC.r, spEC.g, spEC.b, spEC.a);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007210, G_IM_FMT_IA, G_IM_SIZ_8b, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, spFC << 2, spF8 << 2, (spFC + w) << 2, (spF8 + h) << 2, G_TX_RENDERTILE, 0,
                        0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007318, G_IM_FMT_IA, G_IM_SIZ_8b, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, ((spFC + spF4) - w) << 2, spF8 << 2, (spFC + spF4) << 2, (spF8 + h) << 2,
                        G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007420, G_IM_FMT_IA, G_IM_SIZ_8b, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, ((spFC + spF4) - w) << 2, ((spF8 + spF0) - h) << 2, (spFC + spF4) << 2,
                        (spF8 + spF0) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007528, G_IM_FMT_IA, G_IM_SIZ_8b, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, spFC << 2, ((spF8 + spF0) - h) << 2, (spFC + w) << 2, (spF8 + spF0) << 2,
                        G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    return 0;
}

void WidgetTree_SetAnimatedFrameVariantCState(WidgetAnimatedFrame* arg0, s32 arg1) {
    arg0->node.state = arg1;
    arg0->pulseFrame = 0;
}

void WidgetTree_InitDirectionalIndicator(unk_func_8820E99C_030_044* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_8820E99C_030_044));
    arg0->unk_00.drawCallback = WidgetTree_DrawDirectionalIndicator;
    arg0->unk_00.updateCallback = WidgetTree_UpdateDirectionalIndicator;
    arg0->unk_00.setStateCallback = WidgetTree_SetDirectionalIndicatorState;
    arg0->unk_00.position.x = arg1;
    arg0->unk_00.position.y = arg2;
    arg0->unk_00.size.x = arg3;
    arg0->unk_00.size.y = arg4;
    arg0->unk_30 = 0;
}

s32 WidgetTree_UpdateDirectionalIndicator(unk_func_8820E99C_030_044* arg0) {
    arg0->unk_30++;
    arg0->unk_30 &= 7;
    return 0;
}

s32 WidgetTree_DrawDirectionalIndicator(unk_func_8820E99C_030_044* arg0, s32 arg1, s32 arg2) {
    UNUSED s32 pad[2];
    s32 var_t4;
    s32 sp80 = ((arg0->unk_00.size.x - 0xE) / 2) + arg1;

    if (!(arg0->unk_00.state & 1) && ((arg0->unk_00.flags & 0x100) || !(arg0->unk_00.state & 0x100))) {
        return 0;
    }

    if (arg0->unk_00.state & 0x100) {
        var_t4 = 2;
    } else if (gPlayer1Controller->buttonDown & 0xC00) {
        var_t4 = 2;
    } else {
        var_t4 = (arg0->unk_30 - 1) & 7;
        if (var_t4 >= 4) {
            var_t4 = 7 - var_t4;
        }
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_COPY);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetAlphaCompare(gDisplayListHead++, G_AC_THRESHOLD);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);

    gDPLoadTextureBlock(gDisplayListHead++, (gPlayer1Controller->buttonDown & 0x800) ? D_4001428 : D_4001218,
                        G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, sp80 << 2, ((arg2 - var_t4) - 8) << 2, (sp80 + 0xD) << 2,
                        ((arg2 - var_t4) - 1) << 2, G_TX_RENDERTILE, 0, 0, 0x1000, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, (gPlayer1Controller->buttonDown & 0x400) ? D_4001530 : D_4001320,
                        G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 8, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, sp80 << 2, (arg2 + arg0->unk_00.size.y + var_t4) << 2,
                        (sp80 + 0xD) << 2, (arg2 + arg0->unk_00.size.y + var_t4 + 7) << 2, G_TX_RENDERTILE, 0, 0,
                        0x1000, 0x0400);

    gDPPipeSync(gDisplayListHead++);
    gDPSetAlphaCompare(gDisplayListHead++, G_AC_NONE);

    return 0;
}

void WidgetTree_SetDirectionalIndicatorState(unk_func_8820E99C_030_044* arg0, s32 arg1) {
    arg0->unk_00.state = arg1;
    arg0->unk_30 = 0;
}

void WidgetTree_InitAnimatedPanelVariantB(WidgetAnimatedPanelVariantB* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, Color_RGBA8 arg5) {
    WidgetTree_InitWidget(&arg0->node, sizeof(WidgetAnimatedPanelVariantB));
    arg0->node.drawCallback = WidgetTree_DrawAnimatedPanelVariantB;
    arg0->node.updateCallback = WidgetTree_UpdateAnimatedPanelVariantB;
    arg0->node.position.x = arg1;
    arg0->node.position.y = arg2;
    arg0->node.size.x = arg3;
    arg0->node.size.y = arg4;
    arg0->color = arg5;
    arg0->animFrame = 0;
    arg0->animState = 0;
}

s32 WidgetTree_UpdateAnimatedPanelVariantB(WidgetAnimatedPanelVariantB* arg0) {
    if (arg0->animFrame >= 0xB) {
        arg0->animFrame = 0;
        arg0->animState = 0;
        arg0->node.flags &= ~1;
    } else if (arg0->animFrame >= 7) {
        arg0->animState = 4;
        arg0->animFrame++;
    } else if (arg0->animFrame < 4) {
        if (arg0->animFrame == 0) {
            Audio_PlaySoundEffectById(4);
        }
        arg0->animState = 1;
        arg0->animFrame++;
    } else {
        arg0->animState = 2;
        if (arg0->animFrame < 6) {
            arg0->animState |= 8;
            arg0->animFrame++;
        }
    }

    return (arg0->animState & 2) ? 0 : 1;
}

s32 WidgetTree_DrawAnimatedPanelVariantB(WidgetAnimatedPanelVariantB* arg0, s32 arg1, s32 arg2) {
    UNUSED s32 pad[2];
    s32 sp114;
    s32 sp110;
    UNUSED s32 pad2[4];
    s16 var_v0;
    s16 var_v1_2;
    s32 temp_a0;
    s32 var_s0;
    s32 var_t4;
    s32 val = 0x10;

    if (arg0->animState & 0xF) {
        var_v0 = arg0->node.size.x;
        var_v1_2 = arg0->node.size.y;

        if (var_v0 < val) {
            var_v0 = val;
        }

        if (var_v1_2 < val) {
            do {
                var_v1_2 = val;
            } while (0);
        }

        if (arg0->animState & 4) {
            temp_a0 = 0xB - arg0->animFrame;

            sp110 = ((var_v1_2 - val) * temp_a0) / 4;
            sp114 = ((var_v0 - val) * temp_a0) / 4;
            var_s0 = (((var_v0 - val) - sp114) / 2) + arg1;
            var_t4 = (((var_v1_2 - val) - sp110) / 2) + arg2;

            // clang-format off
            sp114 += val; sp110 += val;
            // clang-format on
        } else if (arg0->animState & 1) {
            temp_a0 = arg0->animFrame - 1;

            sp110 = ((var_v1_2 - val) * temp_a0) / 4;
            sp114 = ((var_v0 - val) * temp_a0) / 4;
            var_s0 = (((var_v0 - val) - sp114) / 2) + arg1;
            var_t4 = (((var_v1_2 - val) - sp110) / 2) + arg2;

            // clang-format off
            sp114 += val; sp110 += val;
            // clang-format on
        } else {
            var_s0 = arg1;
            var_t4 = arg2;
            sp114 = var_v0;
            sp110 = var_v1_2;
        }

        gDPPipeSync(gDisplayListHead++);
        gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
        gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
        gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
        gDPSetCombineMode(gDisplayListHead++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(gDisplayListHead++, 0, 0, arg0->color.r, arg0->color.g, arg0->color.b, arg0->color.a);

        gDPLoadTextureBlock(gDisplayListHead++, D_4007630, G_IM_FMT_I, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, var_s0 << 2, var_t4 << 2, ((var_s0 + sp114) - 8) << 2,
                            ((var_t4 + sp110) - 8) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

        gDPLoadTextureBlock(gDisplayListHead++, D_4007678, G_IM_FMT_I, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, ((var_s0 + sp114) - 8) << 2, var_t4 << 2, (var_s0 + sp114) << 2,
                            ((var_t4 + sp110) - 8) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

        gDPLoadTextureBlock(gDisplayListHead++, D_40076C0, G_IM_FMT_I, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, (var_s0 + 8) << 2, ((var_t4 + sp110) - 8) << 2, (var_s0 + sp114) << 2,
                            (var_t4 + sp110) << 2, G_TX_RENDERTILE, (sp114 * -0x20) + 0x200, 0, 0x0400, 0x0400);

        gDPLoadTextureBlock(gDisplayListHead++, D_4007708, G_IM_FMT_I, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, var_s0 << 2, ((var_t4 + sp110) - 8) << 2, (var_s0 + 8) << 2,
                            (var_t4 + sp110) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);
    }

    return (arg0->animState & 2) ? 0 : 1;
}

void WidgetTree_InitTextList(WidgetTextList* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, Color_RGBA8 arg6,
                   FontContext* arg7) {
    WidgetTree_InitWidget(&arg0->node, sizeof(WidgetTextList));
    arg0->node.drawCallback = WidgetTree_DrawTextList;
    arg0->node.updateCallback = WidgetTree_UpdateTextList;
    arg0->node.position.x = arg1;
    arg0->node.position.y = arg2;
    arg0->fontId = arg3;
    arg0->charSpacing = arg4;
    arg0->lineHeight = arg5;
    arg0->color = arg6;
    arg0->font = arg7;
    WidgetTree_SetTextList(arg0, NULL);
}

s32 WidgetTree_UpdateTextList(WidgetTextList* arg0) {
    if ((((arg0->revealedCharCount * 0xF) / 5) + 1) <= arg0->charCount) {
        arg0->revealedCharCount++;
    }
    return 0;
}

s32 WidgetTree_DrawTextList(WidgetTextList* arg0, s32 arg1, s32 arg2) {
    s32 temp_fp;
    s32 var_s2;
    s32 var_s3;
    s32 var_s5;
    s32 var_v1;
    u8* var_s4;
    s32 temp_v0_3;
    u8 var_s0;

    if (arg0->text == NULL) {
        return 0;
    }

    var_s2 = arg1;
    var_s5 = arg2;
    var_s4 = &arg0->text->node.link.recordSize;
    temp_fp = ((arg0->revealedCharCount * 0xF) / 5) + 1;

    Font_BeginTranslucentTextRendering();
    Font_SetActive(arg0->fontId, arg0->charSpacing);
    Gfx_SetEnvColor(arg0->color.r, arg0->color.g, arg0->color.b, arg0->color.a);
    var_s3 = 0;
    if (temp_fp > 0) {
        var_s0 = *var_s4++;

        while (var_s0 != 0) {
            if (var_s0 == (0, 0xA)) {
                var_s2 = arg1;
                var_s5 += arg0->lineHeight;
            } else {
                Font_DrawCharAt(var_s2, var_s5, var_s0);
                temp_v0_3 = Font_GetGlyphAdvance(&arg0->font->unk_00[arg0->font->unk_50], temp_v0_3 = var_s0);
                if (arg0->font->unk_50 >= 2) {
                    var_v1 = temp_v0_3 - 2;
                } else {
                    var_v1 = temp_v0_3 - 1;
                }
                var_s2 += var_v1 + arg0->charSpacing;
            }

            var_s3++;
            if (var_s3 >= temp_fp) {
                break;
            }

            var_s0 = *var_s4++;
        }
    }

    Font_EndTexturedTextRendering();
    return 0;
}

void WidgetTree_SetTextList(WidgetTextList* arg0, u8* arg1) {
    s32 var_v0;
    s32 val;
    u8* arg;

    arg0->text = arg1;
    arg0->revealedCharCount = 0;

    if (arg0->text == NULL) {
        arg0->charCount = 0;
        return;
    }

    var_v0 = 0;
    arg = arg1;
    val = *arg++;
    while (val != 0) {
        var_v0++;
        val = *arg++;
    }
    arg0->charCount = var_v0;
}

void WidgetTree_InitPagedGrid(unk_func_88200FA0_030_030* arg0, s32 arg1, s32 arg2, unk_func_88200FA0_030_030_040 arg3, s32 arg4,
                   s32 arg5, s32 arg6, s32 arg7, MemoryPool* arg8) {
    static Color_RGBA8 D_8850CF84 = { 255, 255, 0, 255 };

    s32 var_v0;
    WidgetAnimatedFrame* temp_v0;

    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_88200FA0_030_030));

    arg0->unk_00.drawCallback = WidgetTree_DrawPagedGrid;
    arg0->unk_00.inputCallback = WidgetTree_PagedGridHandleInput;
    arg0->unk_00.setStateCallback = WidgetTree_SetPagedGridState;

    arg0->unk_00.position.x = arg1;
    arg0->unk_00.position.y = arg2;
    arg0->unk_00.size.x = arg4 * arg7;
    arg0->unk_00.size.y = arg5 * arg6;

    arg0->unk_3C = arg4;
    arg0->unk_3E = arg5;
    arg0->unk_40 = arg3;
    arg0->unk_48 = arg7;
    arg0->unk_4A = arg6;

    arg0->unk_2C = NULL;
    arg0->unk_30 = 0;
    arg0->unk_34 = NULL;

    // clang-format off
    arg0->unk_4C = 0; arg0->unk_4E = 0;
    // clang-format on

    if (arg7 == 1) {
        arg0->unk_50 = 0;
    } else {
        // clang-format off
        arg0->unk_50 = -3;    }    arg0->unk_52 = -3;
    // clang-format on

    arg0->unk_44 = mem_pool_alloc(arg8, sizeof(WidgetAnimatedFrame));

    if (arg7 == 1) {
        var_v0 = 0;
    } else {
        var_v0 = 3;
    }

    WidgetTree_InitAnimatedFrameVariantC(arg0->unk_44, 0, 0, (var_v0 << 1) + arg4, arg5 + 6, D_8850CF84);
    arg0->unk_44->node.flags &= ~1;
    WidgetTree_AppendChild(&arg0->unk_00.link, &arg0->unk_44->node.link);
    ((func88506BFC)Memmap_GetFragmentVaddr(WidgetTree_SetPagedGridSelection))(arg0, 0);
}

void WidgetTree_AllocateEntryFlags(unk_func_88200FA0_030_030* arg0, s32 arg1, MemoryPool* arg2) {
    if (arg0->unk_34 != NULL) {
        mem_pool_free(arg2, arg0->unk_34);
    }
    arg0->unk_34 = mem_pool_alloc(arg2, arg1 * 2);
}

void WidgetTree_ClearEntryFlags(unk_func_88200FA0_030_030* arg0) {
    s32 i;

    for (i = 0; i < arg0->unk_2C->capacity; i++) {
        arg0->unk_34[i] = 0;
    }
}

void WidgetTree_SetPagedGridState(unk_func_88200FA0_030_030* arg0, s32 arg1) {
    arg0->unk_00.state = arg1;
    arg0->unk_44->node.setStateCallback(&arg0->unk_44->node, arg1);
    arg0->unk_44->node.flags &= ~1;
    if (arg1 & 0x101) {
        arg0->unk_44->node.flags |= 1;
    }
}

void WidgetTree_BindPagedGridData(unk_func_88200FA0_030_030_1CEA00* arg0, unk_func_8830867C_02C_0CC* arg1) {
    arg0->unk_2C = arg1;
    arg0->unk_30 = 0;
    ((func88506BFC)Memmap_GetFragmentVaddr(WidgetTree_SetPagedGridSelection))(arg0, 0);
    WidgetTree_ClearEntryFlags(arg0);
}

void WidgetTree_BindPagedGridPage(unk_func_88200FA0_030_030* arg0, unk_func_88200FA0_030_038* arg1, MemoryPool* arg2) {
    WidgetTree_AllocateEntryFlags(arg0, arg1->capacity, arg2);
    WidgetTree_BindPagedGridData(arg0, arg1);
}

void WidgetTree_BindPagedGridStridedData(unk_func_88200FA0_030_030* arg0, unk_func_88201DA0_038* arg1) {
    arg0->unk_2C = arg1;
    arg0->unk_30 = 1;
    ((func88506BFC)Memmap_GetFragmentVaddr(WidgetTree_SetPagedGridSelection))(arg0, 0);
    WidgetTree_ClearEntryFlags(arg0);
}

void WidgetTree_BindPagedGridStridedPage(unk_func_88200FA0_030_030* arg0, unk_func_8820BE14_06C* arg1, MemoryPool* arg2) {
    WidgetTree_AllocateEntryFlags(arg0, arg1->capacity, arg2);
    WidgetTree_BindPagedGridStridedData(arg0, arg1);
}

s32 WidgetTree_DrawPagedGrid(unk_func_88200FA0_030_030* arg0, s32 arg1, s32 arg2) {
    s32 i;
    s16 temp_v1;
    s32 temp_lo;
    s32 var_s3;
    s32 var_s5;
    s32 var_v0;
    void* var_s2;
    void* var_a2;

    temp_lo = arg0->unk_4A * arg0->unk_48;
    var_s2 = NULL;
    var_s5 = 0;
    var_s3 = 0;
    if (arg0->unk_2C != NULL) {
        var_s2 = arg0->unk_2C->data;
        if (arg0->unk_30 != 0) {
            var_s5 = arg0->unk_2C->rowStride;
        }
        var_s3 = arg0->unk_2C->count;
    }

    for (i = 0; i < temp_lo; i++) {
        var_a2 = NULL;
        var_v0 = 0;
        temp_v1 = arg0->unk_48;
        if (i < var_s3) {
            if (arg0->unk_30 != 0) {
                var_a2 = &((s8*)var_s2)[var_s5 * i];
            } else {
                var_a2 = ((s32*)var_s2)[i];
            }

            var_v0 = arg0->unk_34[i];
            if (i == arg0->unk_38) {
                var_v0 |= 1;
            }
        }
        arg0->unk_40(((i % temp_v1) * arg0->unk_3C) + arg1, ((i / temp_v1) * arg0->unk_3E) + arg2, var_a2, i, var_v0,
                     arg0);
    }

    return 0;
}

#ifdef NON_MATCHING
s32 WidgetTree_PagedGridHandleInput(unk_func_88200FA0_030_030* arg0, Controller* arg1) {
    s32 i;
    s32 j;
    s32 sp5C;
    s32 sp58;
    s32 sp54;
    s32 sp50;
    s32 sp4C;
    s32 sp48;
    s32 sp24;
    s32 var_a3;
    s32 var_t1_2;
    s32 var_v0_2;
    s32 temp_v0_3;
    s32 temp_v1;
    s32 var_t1;
    s32 var_t4;
    s32 var_v0;

    var_t4 = 0;
    sp5C = 0;
    if ((arg0->unk_2C == NULL) || (arg0->unk_2C->count == 0)) {
        return 0;
    }
    sp48 = ((arg0->unk_2C->count + arg0->unk_48) - 1) / arg0->unk_48;
    sp58 = arg0->unk_38 % arg0->unk_48;
    sp54 = arg0->unk_38 / arg0->unk_48;
    sp24 = Input_GetRepeatedDPad() & 0xFFFF;

    // if (0) {}

    if (sp24 == 0x800) {
        var_t1 = sp54;

        for (i = 0; i < sp48 - 1; i++) {
            var_a3 = arg0->unk_4C;

            if (var_t1 == 0) {
                var_v0 = sp48;
            } else {
                var_v0 = var_t1;
            }

            var_t1 = var_v0 - 1;
            for (j = 0; j < arg0->unk_48; j++) {
                temp_v0_3 = (arg0->unk_48 * var_t1) + var_a3;
                if ((temp_v0_3 < arg0->unk_2C->count) && !(arg0->unk_34[temp_v0_3] & 4)) {
                    var_t4 = 1;
                    break;
                }

                if (var_a3 == 0) {
                    var_v0_2 = arg0->unk_48;
                } else {
                    var_v0_2 = var_a3;
                }
                var_a3 = var_v0_2 - 1;
            }

            if (var_t4 != 0) {
                break;
            }
        }

        if ((arg0->unk_00.flags & 0x200) && ((var_t4 == 0) || (sp54 < var_t1))) {
            sp5C = 8;
            var_t4 = 0;
        }
        sp4C = var_t1;
        sp50 = var_a3;
    } else if (sp24 == 0x400) {
        var_t1 = sp54;

        for (i = 0; i < sp48 - 1; i++) {
            var_a3 = arg0->unk_4C;

            if (var_t1 == sp48 - 1) {
                var_t1 = 0;
            } else {
                var_t1++;
            }

            for (j = 0; j < arg0->unk_48; j++) {
                temp_v0_3 = (arg0->unk_48 * var_t1) + var_a3;
                if ((temp_v0_3 < arg0->unk_2C->count) && !(arg0->unk_34[temp_v0_3] & 4)) {
                    var_t4 = 1;
                    break;
                }

                if (var_a3 == 0) {
                    var_v0_2 = arg0->unk_48;
                } else {
                    var_v0_2 = var_a3;
                }
                var_a3 = var_v0_2 - 1;
            }

            if (var_t4 != 0) {
                break;
            }
        }

        if ((arg0->unk_00.flags & 0x200) && ((var_t4 == 0) || (var_t1 < sp54))) {
            sp5C = 8;
            var_t4 = 0;
        }
        sp4C = var_t1;
        sp50 = var_a3;
    } else if (sp24 == 0x100) {
        var_t1 = sp54;
        var_a3 = sp58;

        for (i = 0; i < arg0->unk_48 - 1; i++) {
            var_t1_2 = arg0->unk_4E;

            if (var_a3 == arg0->unk_48 - 1) {
                var_a3 = 0;
            } else {
                var_a3++;
            }

            for (j = 0; j < sp48; j++) {
                temp_v0_3 = (arg0->unk_48 * var_t1_2) + var_a3;
                if ((temp_v0_3 < arg0->unk_2C->count) && !(arg0->unk_34[temp_v0_3] & 4)) {
                    var_t4 = 1;
                    break;
                }

                if (var_t1_2 == 0) {
                    var_v0_2 = sp48;
                } else {
                    var_v0_2 = var_t1_2;
                }
                var_t1_2 = var_v0_2 - 1;
            }

            if (var_t4 != 0) {
                break;
            }
        }

        if ((arg0->unk_00.flags & 0x100) && ((var_t4 == 0) || (var_a3 < sp58))) {
            sp5C = 8;
            var_t4 = 0;
        }

        sp50 = var_a3;
        sp4C = var_t1_2;
    } else if (sp24 == 0x200) {
        var_t1 = sp54;
        var_a3 = sp58;

        for (i = 0; i < arg0->unk_48 - 1; i++) {
            var_t1_2 = arg0->unk_4E;

            if (var_a3 == 0) {
                var_v0 = arg0->unk_48;
            } else {
                var_v0 = var_a3;
            }

            var_a3 = var_v0 - 1;
            for (j = 0; j < sp48; j++) {
                temp_v0_3 = (arg0->unk_48 * var_t1_2) + var_a3;
                if ((temp_v0_3 < arg0->unk_2C->count) && !(arg0->unk_34[temp_v0_3] & 4)) {
                    var_t4 = 1;
                    break;
                }

                if (var_t1_2 == 0) {
                    var_v0_2 = sp48;
                } else {
                    var_v0_2 = var_t1_2;
                }
                var_t1_2 = var_v0_2 - 1;
            }

            if (var_t4 != 0) {
                break;
            }
        }

        if ((arg0->unk_00.flags & 0x100) && ((var_t4 == 0) || (sp58 < var_a3))) {
            sp5C = 8;
            var_t4 = 0;
        }

        sp4C = var_t1_2;
        sp50 = var_a3;
    }

    if (var_t4 != 0) {
        arg0->unk_38 = (arg0->unk_48 * sp4C) + sp50;
        arg0->unk_44->node.position.x = ((arg0->unk_38 % arg0->unk_48) * arg0->unk_3C) + arg0->unk_50;
        arg0->unk_44->node.position.y = ((arg0->unk_38 / arg0->unk_48) * arg0->unk_3E) + arg0->unk_52;
        if ((sp24 == 0x800) || (sp24 == 0x400)) {
            arg0->unk_4E = sp4C;
        } else {
            arg0->unk_4C = sp50;
        }
        sp5C = 0x80000009;
    }
    return sp5C;
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/widget_tree/widget_tree/WidgetTree_PagedGridHandleInput.s")
#endif

void WidgetTree_SetPagedGridSelection(unk_func_88200FA0_030_030* arg0, s32 arg1) {
    arg0->unk_38 = arg1;
    arg0->unk_44->node.position.x = ((arg1 % arg0->unk_48) * arg0->unk_3C) + arg0->unk_50;
    arg0->unk_44->node.position.y = ((arg0->unk_38 / arg0->unk_48) * arg0->unk_3E) + arg0->unk_52;
    arg0->unk_4C = arg0->unk_38 % arg0->unk_48;
    arg0->unk_4E = arg0->unk_38 / arg0->unk_48;
}

s32 WidgetTree_FindSelectableGridEntry(unk_func_88200FA0_030_030* arg0) {
    s32 var_a1 = arg0->unk_38;

    if (arg0->unk_38 < arg0->unk_2C->count) {
        while (arg0->unk_34[var_a1] & 4) {
            var_a1++;
            if (var_a1 >= arg0->unk_2C->count) {
                break;
            }
        }
    }

    if (var_a1 < arg0->unk_2C->count) {
        WidgetTree_SetPagedGridSelection(arg0, var_a1);
        return var_a1;
    }

    var_a1 = arg0->unk_38 - 1;
    if (var_a1 >= 0) {
        while (arg0->unk_34[var_a1] & 4) {
            var_a1--;
            if (var_a1 < 0) {
                break;
            }
        }
    }

    if (var_a1 >= 0) {
        WidgetTree_SetPagedGridSelection(arg0, var_a1);
        return var_a1;
    }

    return -1;
}

void WidgetTree_InitScrollableGrid(unk_func_8821421C_038_034* arg0, s32 arg1, s32 arg2, unk_func_88200FA0_030_030_040 arg3, s32 arg4,
                   s32 arg5, s32 arg6, MemoryPool* arg7) {
    WidgetTree_InitPagedGrid(&arg0->unk_00, arg1, arg2, arg3, arg4, arg5, arg6, 1, arg7);
    arg0->unk_00.unk_00.updateCallback = WidgetTree_UpdateScrollableGridScroll;
    arg0->unk_00.unk_00.drawCallback = WidgetTree_DrawScrollableGrid;
    arg0->unk_00.unk_00.inputCallback = WidgetTree_HandleScrollableGridInput;
    arg0->unk_00.unk_4A = arg6;
    arg0->unk_54 = arg0->unk_56 = 0;
    arg0->unk_64 = (arg0->unk_00.unk_3E * 4) / 10;
    arg0->unk_68 = (arg0->unk_00.unk_3E * arg0->unk_00.unk_4A * 2) / 10;
    arg0->unk_00.unk_38 = 0;
    arg0->unk_5C = 0;
    arg0->unk_60 = 0;
    arg0->unk_58 = 0;
    arg0->unk_6C = 8;
}

void WidgetTree_BindScrollableGridData(unk_func_8820BE14_02C_038* arg0, unk_func_88200FA0_030_038* arg1) {
    arg0->unk_00.unk_2C = arg1;
    arg0->unk_00.unk_30 = 0;
    arg0->unk_00.unk_38 = 0;
    arg0->unk_5C = 0;
    arg0->unk_60 = 0;
    arg0->unk_58 = 0;
    arg0->unk_6C = 0;
    WidgetTree_ClearEntryFlags(&arg0->unk_00);
}

void WidgetTree_BindScrollableGridPage(unk_func_8830867C_02C_034* arg0, unk_func_88200FA0_030_038* arg1, MemoryPool* arg2) {
    WidgetTree_AllocateEntryFlags(arg0, arg1->capacity, arg2);
    WidgetTree_BindScrollableGridData(arg0, arg1);
}

void WidgetTree_BindScrollableGridStridedData(unk_func_8821421C_038_034* arg0, unk_func_8821421C_038_050** arg1) {
    arg0->unk_00.unk_2C = arg1;
    arg0->unk_00.unk_30 = 1;
    arg0->unk_00.unk_38 = 0;
    arg0->unk_5C = 0;
    arg0->unk_60 = 0;
    arg0->unk_58 = 0;
    arg0->unk_6C = 0;
    WidgetTree_ClearEntryFlags(&arg0->unk_00);
}

void WidgetTree_BindScrollableGridStridedPage(unk_func_8821421C_038_034* arg0, unk_func_8830867C_03C_0D0* arg1, MemoryPool* arg2) {
    WidgetTree_AllocateEntryFlags(&arg0->unk_00, arg1->unk_04, arg2);
    WidgetTree_BindScrollableGridStridedData(arg0, arg1);
}

void WidgetTree_ClampScrollableGridTop(unk_func_8820BE14_02C_038* arg0) {
    if (arg0->unk_5C < arg0->unk_58) {
        arg0->unk_58 = arg0->unk_5C;
    }
}

void WidgetTree_ClampScrollableGridBottom(unk_func_8820BE14_02C_038* arg0) {
    s32 temp_v0 = (arg0->unk_5C - (arg0->unk_00.unk_3E * arg0->unk_00.unk_4A)) + 1;

    if (arg0->unk_58 < temp_v0) {
        arg0->unk_58 = temp_v0;
    }
}

s32 WidgetTree_UpdateScrollableGridScroll(unk_func_8821421C_038_034* arg0) {
    s32 sp24;
    s32 temp_a2_2;
    s32 sp1C;
    s32 temp_a2_4;
    s32 temp_a3;
    s32 temp_a3_2;
    s32 var_a3;
    s32 var_v1;

    if (arg0->unk_56 & 0x800) {
        if (!(arg0->unk_54 & 0x800)) {
            sp24 = arg0->unk_00.unk_38 * arg0->unk_00.unk_3E;
            temp_a3 = arg0->unk_5C - sp24;
            arg0->unk_5C -= (temp_a3 < arg0->unk_64) ? temp_a3 : arg0->unk_64;
            WidgetTree_ClampScrollableGridTop(arg0);
            if (sp24 == arg0->unk_5C) {
                arg0->unk_56 &= ~0x800;
            }
        }
    } else if (arg0->unk_56 & 0x400) {
        if (!(arg0->unk_54 & 0x400)) {
            sp1C = ((arg0->unk_00.unk_38 + 1) * arg0->unk_00.unk_3E) - 1;
            temp_a3_2 = sp1C - arg0->unk_5C;
            arg0->unk_5C += (temp_a3_2 < arg0->unk_64) ? temp_a3_2 : arg0->unk_64;
            WidgetTree_ClampScrollableGridBottom(arg0);
            if (sp1C == arg0->unk_5C) {
                arg0->unk_56 &= ~0x400;
                arg0->unk_5C = (arg0->unk_5C - arg0->unk_00.unk_3E) + 1;
            }
        }
    } else if (arg0->unk_56 & 0x200) {
        if (!(arg0->unk_54 & 0x200)) {
            temp_a2_2 = arg0->unk_5C - arg0->unk_60;
            var_v1 = (temp_a2_2 < arg0->unk_68) ? temp_a2_2 : arg0->unk_68;
            arg0->unk_5C -= var_v1;
            arg0->unk_58 -= (arg0->unk_58 < var_v1) ? arg0->unk_58 : var_v1;
            arg0->unk_00.unk_38 = arg0->unk_5C / arg0->unk_00.unk_3E;
            if (arg0->unk_60 == arg0->unk_5C) {
                arg0->unk_56 &= ~0x200;
            }
        }
    } else if ((arg0->unk_56 & 0x100) && !(arg0->unk_54 & 0x100)) {
        s32 tmp = arg0->unk_00.unk_3E * arg0->unk_00.unk_2C->count;
        s32 tmp2 = arg0->unk_00.unk_3E * arg0->unk_00.unk_4A;

        temp_a2_4 = arg0->unk_60 - arg0->unk_5C;

        var_a3 = (temp_a2_4 < arg0->unk_68) ? temp_a2_4 : arg0->unk_68;
        arg0->unk_5C += var_a3;

        var_v1 = (tmp - arg0->unk_58) - tmp2;

        if (var_v1 <= 0) {
            var_v1 = 0;
        }

        arg0->unk_58 += (var_v1 < var_a3) ? var_v1 : var_a3;
        arg0->unk_00.unk_38 = arg0->unk_5C / arg0->unk_00.unk_3E;
        if (arg0->unk_60 == arg0->unk_5C) {
            arg0->unk_56 &= ~0x100;
        }
    }

    arg0->unk_54 = 0;
    var_v1 = arg0->unk_5C - arg0->unk_58;
    if (arg0->unk_56 & 0x400) {
        var_v1 = (var_v1 - arg0->unk_00.unk_3E) + 1;
    }
    arg0->unk_00.unk_44->node.position.x = arg0->unk_00.unk_50;
    arg0->unk_00.unk_44->node.position.y = arg0->unk_00.unk_52 + var_v1;
    return 0;
}

#ifdef NON_MATCHING
s32 WidgetTree_DrawScrollableGrid(unk_func_8821421C_038_034* arg0, s32 arg1, s32 arg2) {
    s16 temp_v0_2;
    s32 temp_a0;
    s32 temp_lo;
    s32 temp_s5;
    s32 var_s0;
    s32 var_s2;
    s32 var_s4;
    s32 var_s6;
    s32 var_v0;
    unk_func_8820BE14_06C* temp_v0;
    void* var_s3;
    void* var_a2;

    temp_lo = arg0->unk_00.unk_3E * arg0->unk_00.unk_4A;
    var_s3 = NULL;
    var_s6 = 0;
    var_s4 = 0;

    if (arg0->unk_00.unk_2C != NULL) {
        var_s3 = arg0->unk_00.unk_2C->data;
        if (arg0->unk_00.unk_30 != 0) {
            var_s6 = arg0->unk_00.unk_2C->rowStride;
        }
        var_s4 = arg0->unk_00.unk_2C->count;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetScissorFrac(gDisplayListHead++, G_SC_NON_INTERLACE, 0, (arg2 + temp_lo) * 4.0f, 0x0A00, arg2 * 4.0f);

    var_s0 = arg0->unk_58 / arg0->unk_00.unk_3E;
    var_s2 = arg2 - (arg0->unk_58 % arg0->unk_00.unk_3E);
    temp_s5 = (((arg0->unk_58 + temp_lo) - 1) / arg0->unk_00.unk_3E) + 1;

    while (var_s0 < temp_s5) {
        var_a2 = NULL;
        var_v0 = 0;
        if (var_s0 < var_s4) {
            if (arg0->unk_00.unk_30 != 0) {
                var_a2 = &((s8*)var_s3)[var_s6 * var_s0];
            } else {
                var_a2 = ((s32*)var_s3)[var_s0];
            }

            var_v0 = arg0->unk_00.unk_34[var_s0];
            if (var_s0 == arg0->unk_00.unk_38) {
                var_v0 |= 1;
            }
        }
        arg0->unk_00.unk_40(arg1, var_s2, var_a2, var_s0, var_v0, arg0);
        var_s2 += arg0->unk_00.unk_3E;
        var_s0++;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, 640, 480);
    return 0;
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/widget_tree/widget_tree/WidgetTree_DrawScrollableGrid.s")
#endif

s32 WidgetTree_HandleScrollableGridInput(unk_func_8821421C_038_034* arg0, Controller* arg1) {
    s32 temp_t0;
    s32 temp_t3;
    s32 var_t2;
    s32 var_t4;

    temp_t3 = arg0->unk_60;
    temp_t0 = arg0->unk_00.unk_38;
    var_t2 = 0;
    var_t4 = 0;

    // clang-format off
    if (arg1->buttonPressed & 0x800) {
        arg0->unk_6C = 8; arg0->unk_54 = 0x800;
    } else if (arg1->buttonPressed & 0x400) {
        arg0->unk_6C = 8; arg0->unk_54 = 0x400;
    } else if (arg1->buttonPressed & 0x200) {
        arg0->unk_6C = 8; arg0->unk_54 = 0x200;
    } else if (arg1->buttonPressed & 0x100) {
        arg0->unk_6C = 8; arg0->unk_54 = 0x100;
    } else if (arg0->unk_6C > 0) {
        arg0->unk_6C--;
    } else {
        arg0->unk_54 = arg1->buttonDown;
    }
    // clang-format on

    if ((arg0->unk_00.unk_2C == NULL) || !(arg0->unk_00.unk_2C->count)) {
        return (arg1->buttonPressed & 0xF00) ? 8 : 0;
    }

    if ((arg0->unk_54 & 0x800) && ((arg0->unk_56 & ~0x800) == 0)) {
        if ((arg0->unk_56 == 0) && (arg0->unk_5C == 0)) {
            var_t4 = 1;
        }
        arg0->unk_56 |= 0x800;
        arg0->unk_5C -= (arg0->unk_5C < arg0->unk_64) ? arg0->unk_5C : arg0->unk_64;
        WidgetTree_ClampScrollableGridTop(arg0);
        arg0->unk_00.unk_38 = arg0->unk_5C / arg0->unk_00.unk_3E;
        if (temp_t0 != arg0->unk_00.unk_38) {
            var_t2 = 0x80000000;
        }
    } else if ((arg0->unk_54 & 0x400) && ((arg0->unk_56 & ~0x400) == 0)) {
        s32 temp_a3_2;
        s32 temp_lo_2 = arg0->unk_00.unk_3E * arg0->unk_00.unk_2C->count;

        if (arg0->unk_56 == 0) {
            arg0->unk_5C = (arg0->unk_5C + arg0->unk_00.unk_3E) - 1;
            if (temp_lo_2 == (arg0->unk_5C + 1)) {
                var_t4 = 1;
            }
        }
        temp_a3_2 = (temp_lo_2 - arg0->unk_5C) - 1;
        arg0->unk_56 |= 0x400;
        arg0->unk_5C += (temp_a3_2 < arg0->unk_64) ? temp_a3_2 : arg0->unk_64;
        WidgetTree_ClampScrollableGridBottom(arg0);
        arg0->unk_00.unk_38 = arg0->unk_5C / arg0->unk_00.unk_3E;
        if (temp_t0 != arg0->unk_00.unk_38) {
            var_t2 = 0x80000000;
        }
    } else if ((arg0->unk_54 & 0x200) && ((arg0->unk_56 & ~0x200) == 0)) {
        s32 temp_lo_2 = arg0->unk_00.unk_3E * arg0->unk_00.unk_4A;

        if (arg0->unk_56 == 0) {
            arg0->unk_60 = arg0->unk_5C;
            if (arg0->unk_60 > 0) {
                var_t2 = 0x80000000;
            }
        }
        arg0->unk_56 |= 0x200;
        arg0->unk_5C -= (arg0->unk_5C < arg0->unk_68) ? arg0->unk_5C : arg0->unk_68;
        if (arg0->unk_5C < arg0->unk_60) {
            arg0->unk_60 -= (temp_lo_2 < arg0->unk_60) ? temp_lo_2 : arg0->unk_60;
        }
        arg0->unk_00.unk_38 = arg0->unk_5C / arg0->unk_00.unk_3E;
        arg0->unk_58 -= (arg0->unk_58 < arg0->unk_68) ? arg0->unk_58 : arg0->unk_68;
        if (temp_t3 != arg0->unk_60) {
            var_t2 |= 0x80000000;
        }
    } else if (arg0->unk_54 & 0x100) {
        if (!(arg0->unk_56 & ~0x100)) {
            s32 temp_lo_5 = arg0->unk_00.unk_3E * arg0->unk_00.unk_2C->count;
            s32 temp_lo_6 = arg0->unk_00.unk_3E * arg0->unk_00.unk_4A;
            s32 temp_a2 = (temp_lo_5 - arg0->unk_5C) - arg0->unk_00.unk_3E;
            s32 var_a2;

            if (arg0->unk_56 == 0) {
                arg0->unk_60 = arg0->unk_5C;
                if (temp_a2 > 0) {
                    var_t2 = 0x80000000;
                }
            }
            arg0->unk_56 |= 0x100;
            arg0->unk_5C += (temp_a2 < arg0->unk_68) ? temp_a2 : arg0->unk_68;
            if (arg0->unk_60 < arg0->unk_5C) {
                var_a2 = (temp_lo_5 - arg0->unk_60) - arg0->unk_00.unk_3E;
                arg0->unk_60 += (temp_lo_6 < var_a2) ? temp_lo_6 : var_a2;
            }
            arg0->unk_00.unk_38 = arg0->unk_5C / arg0->unk_00.unk_3E;
            var_a2 = (temp_lo_5 - arg0->unk_58) - temp_lo_6;
            if (var_a2 <= 0) {
                var_a2 = 0;
            }

            arg0->unk_58 += (var_a2 < arg0->unk_68) ? var_a2 : arg0->unk_68;
            if (temp_t3 != arg0->unk_60) {
                var_t2 |= 0x80000000;
            }
        }
    }

    if (arg0->unk_56 == 0) {
        var_t2 = 0;
    } else {
        var_t2 |= 8;
        if (var_t4 == 0) {
            var_t2 |= 1;
        }
    }
    return var_t2;
}

void WidgetTree_SetScrollableGridIndex(unk_func_8820BE14_02C_038* arg0, s32 arg1) {
    s32 temp_v1;
    s32 var_a3;

    arg0->unk_6C = 0;
    arg0->unk_5C = arg0->unk_00.unk_3E * arg1;
    arg0->unk_00.unk_38 = arg1;

    if (arg0->unk_5C < arg0->unk_58) {
        arg0->unk_58 = arg0->unk_5C;
        return;
    }

    temp_v1 = (arg0->unk_5C + arg0->unk_00.unk_3E) - (arg0->unk_00.unk_4A * arg0->unk_00.unk_3E);
    if (arg0->unk_58 < temp_v1) {
        arg0->unk_58 = temp_v1;
        return;
    }

    if (arg0->unk_00.unk_4A >= arg0->unk_00.unk_2C->count) {
        var_a3 = 0;
    } else {
        var_a3 = (arg0->unk_00.unk_2C->count - arg0->unk_00.unk_4A) * arg0->unk_00.unk_3E;
    }

    if (var_a3 < arg0->unk_58) {
        arg0->unk_58 = var_a3;
    }
}

void WidgetTree_SetScrollableGridIndexPreserveScroll(unk_func_8820BE14_02C_038* arg0, s32 arg1) {
    s32 temp_a2;
    s32 temp_v0;
    s32 var_a1;
    s32 var_v0;

    arg0->unk_6C = 0;
    arg0->unk_00.unk_38 = arg1;
    temp_v0 = arg0->unk_5C - arg0->unk_58;
    arg0->unk_5C = arg0->unk_00.unk_3E * arg1;

    temp_a2 = arg0->unk_00.unk_2C->count - arg0->unk_00.unk_4A;

    var_a1 = temp_a2 = (temp_a2 > 0) ? temp_a2 : 0;
    temp_a2 *= arg0->unk_00.unk_3E;

    var_v0 = (arg0->unk_5C - temp_v0 > 0) ? arg0->unk_5C - temp_v0 : 0;

    if (var_v0 < temp_a2) {
        arg0->unk_58 = var_v0;
    } else {
        arg0->unk_58 = temp_a2;
    }
}

void WidgetTree_SetPagedGridSelectionWithOffset(unk_func_8830867C_02C_034* arg0, s32 arg1, s32 arg2) {
    s32 temp_a2;
    s32 temp_a2_2;
    s32 temp_lo;
    s32 temp_v1;
    s32 var_v0_2;

    if (arg0->unk_00.unk_2C->count != 0) {
        arg0->unk_6C = 0;
        temp_v1 = arg0->unk_00.unk_2C->count - 1;
        if (arg1 >= temp_v1) {
            arg1 = temp_v1;
        }
        arg0->unk_00.unk_38 = arg1;
        arg0->unk_5C = arg0->unk_00.unk_3E * arg1;

        temp_a2 = arg0->unk_00.unk_2C->count - arg0->unk_00.unk_4A;
        temp_lo = ((temp_a2 > 0) ? temp_a2 : 0) * arg0->unk_00.unk_3E;

        temp_a2_2 = arg0->unk_5C - (arg2 * arg0->unk_00.unk_3E);
        if (temp_a2_2 > 0) {
            var_v0_2 = temp_a2_2;
        } else {
            var_v0_2 = 0;
        }

        if (var_v0_2 < temp_lo) {
            arg0->unk_58 = var_v0_2;
        } else {
            arg0->unk_58 = temp_lo;
        }
    }
}

void WidgetTree_SetScrollableGridEntryText(unk_func_8820BE14_02C_038* arg0, unk_func_88205880_00D0* arg1) {
    PointerList_Insert(arg0->unk_00.unk_2C, arg1, arg0->unk_00.unk_38);
}

void WidgetTree_RemoveScrollableGridEntry(unk_func_8820BE14_02C_038* arg0) {
    s32 var_a1;

    PointerList_Remove(arg0->unk_00.unk_2C, arg0->unk_00.unk_38);
    var_a1 = arg0->unk_00.unk_38;
    if ((var_a1 >= arg0->unk_00.unk_2C->count) && (var_a1 > 0)) {
        var_a1 = arg0->unk_00.unk_2C->count - 1;
        arg0->unk_00.unk_38 = var_a1;
    }
    WidgetTree_SetScrollableGridIndexPreserveScroll(arg0, var_a1);
}

void WidgetTree_InitScrollableGridScrollbar(unk_func_88507D4C* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, unk_func_8820BE14_02C_038* arg5) {
    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_88507D4C));
    arg0->unk_00.drawCallback = WidgetTree_DrawScrollableGridScrollbar;
    arg0->unk_00.position.x = arg1;
    arg0->unk_00.position.y = arg2;
    arg0->unk_00.size.x = arg3;
    arg0->unk_00.size.y = 0x10;
    arg0->unk_30 = arg4;
    arg0->unk_2C = arg5;
}

s32 WidgetTree_DrawScrollableGridScrollbar(unk_func_88507D4C* arg0, s32 arg1, s32 arg2) {
    s32 sp12C;
    s32 temp_s0;
    s32 w = 8;
    s32 h = 16;

    sp12C = arg0->unk_2C->unk_00.unk_3E * arg0->unk_2C->unk_00.unk_4A;
    temp_s0 = ((arg0->unk_00.size.x / 2) + arg1) - 9;

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    gDPLoadTextureBlock(gDisplayListHead++, D_40022E0, G_IM_FMT_I, G_IM_SIZ_8b, w, h, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, arg1 << 2, arg2 << 2, (arg0->unk_00.size.x + arg1) << 2,
                        (arg2 + 0x10) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_COPY);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetAlphaCompare(gDisplayListHead++, G_AC_THRESHOLD);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);

    if (arg0->unk_30 == 0) {
        gDPLoadTextureBlock(gDisplayListHead++, (arg0->unk_2C->unk_58 > 0) ? D_4002368 : D_4002D88, G_IM_FMT_RGBA,
                            G_IM_SIZ_16b, 20, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, temp_s0 << 2, arg2 << 2, (temp_s0 + 0x11) << 2, (arg2 + 0xF) << 2,
                            G_TX_RENDERTILE, 0, 0, 0x1000, 0x0400);

        gDPLoadTextureBlock(gDisplayListHead++, (arg0->unk_2C->unk_58 > 0) ? D_4002878 : D_4003298, G_IM_FMT_RGBA,
                            G_IM_SIZ_16b, 20, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, (temp_s0 - 0x1C) << 2, arg2 << 2, (temp_s0 - 9) << 2, (arg2 + 0xF) << 2,
                            G_TX_RENDERTILE, 0, 0, 0x1000, 0x0400);
    } else {
        s32 var_t5 = 0;

        if ((arg0->unk_2C->unk_00.unk_2C != NULL) &&
            ((arg0->unk_2C->unk_58 + sp12C) < (arg0->unk_2C->unk_00.unk_3E * arg0->unk_2C->unk_00.unk_2C->count))) {
            var_t5 = 1;
        }

        gDPLoadTextureBlock(gDisplayListHead++, (var_t5 != 0) ? D_40025F0 : D_4003010, G_IM_FMT_RGBA, G_IM_SIZ_16b, 20,
                            16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, temp_s0 << 2, arg2 << 2, (temp_s0 + 0x11) << 2, (arg2 + 0xF) << 2,
                            G_TX_RENDERTILE, 0, 0, 0x1000, 0x0400);

        gDPLoadTextureBlock(gDisplayListHead++, (var_t5 != 0) ? D_4002B00 : D_4003520, G_IM_FMT_RGBA, G_IM_SIZ_16b, 20,
                            16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, (temp_s0 + 0x1A) << 2, arg2 << 2, (temp_s0 + 0x2D) << 2,
                            (arg2 + 0xF) << 2, G_TX_RENDERTILE, 0, 0, 0x1000, 0x0400);
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetAlphaCompare(gDisplayListHead++, G_AC_NONE);
    return 0;
}

void WidgetTree_InitTextLabel(unk_func_8850878C* arg0, s32 arg1, s32 arg2, char* arg3, s32 arg4) {
    Color_RGBA8 D_8850CF88 = { 240, 240, 240, 255 };

    ((func885007CC)Memmap_GetFragmentVaddr(WidgetTree_InitWidget))(arg0, sizeof(unk_func_8850878C));
    arg0->unk_00.drawCallback = WidgetTree_DrawTextLabel;
    arg0->unk_00.position.x = arg1;
    arg0->unk_00.position.y = arg2;
    arg0->unk_40 = arg3;
    arg0->unk_2C = arg4;
    arg0->unk_30 = D_8850CF88;
    arg0->unk_38 = 0;
    arg0->unk_3C = 0x28;
    arg0->unk_44 = 0;
}

s32 WidgetTree_DrawTextLabel(unk_func_8850878C* arg0, s32 arg1, s32 arg2) {
    if (arg0->unk_40 == NULL) {
        return 0;
    }
    Font_BeginTranslucentTextRendering();
    Font_SetActive(arg0->unk_2C, arg0->unk_38);
    Gfx_SetEnvColor(arg0->unk_30.r, arg0->unk_30.g, arg0->unk_30.b, arg0->unk_30.a);
    if (arg0->unk_44 != 0) {
        Font_EnableTwoCycleTexturing();
        Gfx_SetPrimColor(arg0->unk_34.r, arg0->unk_34.g, arg0->unk_34.b, arg0->unk_34.a);
    }
    Font_SetLineHeight(arg0->unk_3C);
    Font_Printf(arg1, arg2, arg0->unk_40);
    if (arg0->unk_44 != 0) {
        Font_DisableTwoCycleTexturing();
    }
    Font_EndTexturedTextRendering();
    return 0;
}

void WidgetTree_InitSlideTransition(unk_func_885088F4* arg0, s32 arg1, s32 arg2, s32 arg3) {
    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_885088F4));
    arg0->unk_00.updateCallback = WidgetTree_UpdateSlideTransition;
    arg0->unk_00.drawCallback = WidgetTree_DrawSlideTransition;
    arg0->unk_00.position.x = 0;
    arg0->unk_00.size.x = 0x280;
    arg0->unk_00.size.y = arg1;
    arg0->unk_2C = -1;
    arg0->unk_00.position.y = -arg0->unk_00.size.y;
    arg0->unk_30 = 0;
    arg0->unk_34 = arg2;
    arg0->unk_38 = arg3;
}

s32 WidgetTree_UpdateSlideTransition(unk_func_885088F4* arg0) {
    arg0->unk_2C++;
    if (arg0->unk_2C < 6) {
        arg0->unk_30 = 1;
    } else if (arg0->unk_2C >= 0x10) {
        arg0->unk_2C = -1;
        arg0->unk_00.position.y = -arg0->unk_00.size.y;
        arg0->unk_30 = 0;
        arg0->unk_00.flags &= ~1;
    } else if (arg0->unk_2C >= 0xA) {
        arg0->unk_30 = 4;
    } else {
        arg0->unk_30 = 2;
        if (arg0->unk_2C == 9) {
            arg0->unk_2C--;
        } else if (arg0->unk_2C < 8) {
            arg0->unk_30 |= 8;
        }
    }

    if (arg0->unk_30 & 4) {
        s32 tmp = 0xF - arg0->unk_2C;

        arg0->unk_00.position.y = ((arg0->unk_00.size.y * tmp) / 6) - arg0->unk_00.size.y;
    } else if (arg0->unk_30 & 1) {
        s32 tmp = arg0->unk_2C;

        arg0->unk_00.position.y = ((arg0->unk_00.size.y * tmp) / 6) - arg0->unk_00.size.y;
    } else {
        arg0->unk_00.position.y = 0;
    }
    return 0;
}

s32 WidgetTree_DrawSlideTransition(unk_func_885088F4* arg0, s32 arg1, s32 arg2) {
    static Color_RGBA8 D_8850CF8C = { 0x38, 0x38, 0x68, 0xFF };

    u32 color = RGBA5551(D_8850CF8C.r, D_8850CF8C.g, D_8850CF8C.b, 1);

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_FILL);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetFillColor(gDisplayListHead++, (color << 0x10) | color);

    gDPFillRectangle(gDisplayListHead++, CLAMP_MAX(arg1, 0), CLAMP_MAX(arg2, 0), CLAMP_MAX(arg1 + 0x27F, 0),
                     CLAMP_MAX((arg0->unk_00.size.y + arg2) - 0x16, 0));

    gDPFillRectangle(gDisplayListHead++, CLAMP_MAX(arg1, 0), CLAMP_MAX((arg0->unk_00.size.y + arg2) - 0x15, 0),
                     CLAMP_MAX((arg0->unk_34 + arg1) - 1, 0), CLAMP_MAX((arg0->unk_00.size.y + arg2) - 6, 0));

    gDPFillRectangle(gDisplayListHead++, CLAMP_MAX((arg1 - arg0->unk_38) + 0x280, 0),
                     CLAMP_MAX((arg0->unk_00.size.y + arg2) - 0x15, 0), CLAMP_MAX(arg1 + 0x27F, 0),
                     CLAMP_MAX((arg0->unk_00.size.y + arg2) - 6, 0));

    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, D_8850CF8C.r, D_8850CF8C.g, D_8850CF8C.b, D_8850CF8C.a);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007948, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 5, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPScisTextureRectangle(gDisplayListHead++, arg1 << 2, ((arg0->unk_00.size.y + arg2) - 5) << 2,
                            (arg0->unk_34 + arg1) << 2, (arg0->unk_00.size.y + arg2) << 2, G_TX_RENDERTILE, 0, 0,
                            0x400, 0x400);

    gSPScisTextureRectangle(gDisplayListHead++, ((arg1 - arg0->unk_38) + 0x280) << 2,
                            ((arg0->unk_00.size.y + arg2) - 5) << 2, (arg1 + 0x280) << 2,
                            (arg0->unk_00.size.y + arg2) << 2, G_TX_RENDERTILE, 0, 0, 0x400, 0x400);

    gSPScisTextureRectangle(gDisplayListHead++, (arg0->unk_34 + arg1 + 0x10) << 2,
                            ((arg0->unk_00.size.y + arg2) - 0x15) << 2, ((arg1 - arg0->unk_38) + 0x270) << 2,
                            ((arg0->unk_00.size.y + arg2) - 0x10) << 2, G_TX_RENDERTILE, 0, 0, 0x400, 0x400);

    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(gDisplayListHead++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007978, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 21, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPScisTextureRectangle(gDisplayListHead++, (arg0->unk_34 + arg1) << 2,
                            ((arg0->unk_00.size.y + arg2) - 0x15) << 2, ((arg1 - arg0->unk_38) + 0x270) << 2,
                            (arg0->unk_00.size.y + arg2) << 2, G_TX_RENDERTILE, 0, 0, 0x400, 0x400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007978, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 21, 0, G_TX_MIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPScisTextureRectangle(gDisplayListHead++, ((arg1 - arg0->unk_38) + 0x270) << 2,
                            ((arg0->unk_00.size.y + arg2) - 0x15) << 2, ((arg1 - arg0->unk_38) + 0x280) << 2,
                            (arg0->unk_00.size.y + arg2) << 2, G_TX_RENDERTILE, 0x200, 0, 0x400, 0x400);

    return 0;
}

void WidgetTree_InitSaveSlotLabel(unk_func_88509A2C* arg0, s32 arg1, s32 arg2, s32 arg3, char* arg4) {
    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_88509A2C));
    arg0->unk_00.drawCallback = WidgetTree_DrawSaveSlotLabel;
    arg0->unk_00.position.x = arg1;
    arg0->unk_00.position.y = arg2;
    // clang-format off
    arg0->unk_00.size.x = 0x124; arg0->unk_00.size.y = 0x18;
    // clang-format on
    arg0->unk_2C = arg3;

    if (arg3 != -1) {
        if (GbSave_GetSaveState(arg3) != 0) {
            arg0->unk_30 = 1;
        } else {
            arg0->unk_30 = 0;
            arg0->unk_38 = GbSave_GetTrainerId(arg3);
            GbSave_GetPlayerName(arg3, arg0->unk_40);
        }
        arg0->unk_34 = GbSave_GameToCategoryIndex(GbSave_GetPortGame(arg3));
    }
    arg0->unk_3C = arg4;
}

s32 WidgetTree_DrawSaveSlotLabel(unk_func_88509A2C* arg0, s32 arg1, s32 arg2) {
    static Gfx* D_8850CF90[] = { D_04005C48, D_04005F70, D_04006298, D_040065C0 };
    static Color_RGBA8 D_8850CFA0 = { 0xF0, 0xF0, 0xF0, 0xFF };

    char* var_a2;

    if (arg0->unk_2C != -1) {
        Font_BeginTranslucentTextRendering();
        Gfx_SetEnvColor(D_8850CFA0.r, D_8850CFA0.g, D_8850CFA0.b, D_8850CFA0.a);
        Font_SetActive(0x10, 0);
        Font_Printf(arg1 + 0x1A, arg2, "%dP", arg0->unk_2C + 1);
        if (arg0->unk_30 == 0) {
            var_a2 = arg0->unk_40;
        } else {
            var_a2 = "?????";
        }
        Font_Printf(arg1 + 0x48, arg2, var_a2);
        Font_SetActive(4, 0);
        if (arg0->unk_30 == 0) {
            Font_Printf(arg1 + 0xBB, arg2 + 8, "%s %05d", arg0->unk_3C, arg0->unk_38);
        } else {
            Font_Printf(arg1 + 0xBB, arg2 + 8, "%s ?????", arg0->unk_3C);
        }
        Font_EndTexturedTextRendering();

        gDPPipeSync(gDisplayListHead++);
        gDPSetCycleType(gDisplayListHead++, G_CYC_COPY);
        gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);
        gDPSetAlphaCompare(gDisplayListHead++, G_AC_THRESHOLD);
        gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);

        gDPLoadTextureBlock(gDisplayListHead++, D_8850CF90[arg0->unk_34], G_IM_FMT_RGBA, G_IM_SIZ_16b, 20, 20, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        gSPTextureRectangle(gDisplayListHead++, arg1 << 2, (arg2 + 4) << 2, (arg1 + 0x13) << 2, (arg2 + 0x17) << 2,
                            G_TX_RENDERTILE, 0, 0, 0x1000, 0x0400);

        gDPPipeSync(gDisplayListHead++);
        gDPSetAlphaCompare(gDisplayListHead++, G_AC_NONE);
    }
    return 0;
}

void WidgetTree_InitTextMessage(unk_func_88509E34* arg0, s32 arg1, s32 arg2, char* arg3) {
    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_88509E34));
    arg0->unk_00.unk_00.drawCallback = WidgetTree_DrawTextMessage;
    arg0->unk_00.unk_00.position.x = arg1;
    arg0->unk_00.unk_00.position.y = arg2;
    arg0->unk_00.unk_2C = arg3;
}

s32 WidgetTree_DrawTextMessage(unk_func_88509F0C* arg0, s32 arg1, s32 arg2) {
    static Color_RGBA8 D_8850CFA4 = { 0xF0, 0xF0, 0xF0, 0xFF };

    Font_BeginTranslucentTextRendering();
    Font_SetActive(8, 0);
    Gfx_SetEnvColor(D_8850CFA4.r, D_8850CFA4.g, D_8850CFA4.b, D_8850CFA4.a);
    Font_Printf(arg1 + 0x1AC, arg2 + 0x26, arg0->unk_2C);
    Font_EndTexturedTextRendering();
    return 0;
}

void WidgetTree_InitTextMessageBuffer(unk_func_88509F0C* arg0) {
    ((func885007CC)Memmap_GetFragmentVaddr(WidgetTree_InitWidget))(arg0, sizeof(unk_func_88509F0C));
    arg0->unk_2C = 0;
}

void WidgetTree_InitMessagePanel(unk_func_88509F48* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, MemoryPool* arg5,
                   FontContext* arg6) {
    static Color_RGBA8 D_8850CFA8 = { 0x64, 0x1E, 0x1E, 0xFF };

    ((func885007CC)Memmap_GetFragmentVaddr(WidgetTree_InitWidget))(arg0, sizeof(unk_func_88509F48));
    arg0->unk_00.inputCallback = WidgetTree_HandleMessagePanelInput;
    arg0->unk_00.position.x = arg1;
    arg0->unk_00.position.y = arg2;
    arg0->unk_00.size.x = arg3;
    arg0->unk_00.size.y = arg4;

    arg0->unk_2C = mem_pool_alloc(arg5, sizeof(WidgetAnimatedPanel));
    ((func88502274)Memmap_GetFragmentVaddr(WidgetTree_InitAnimatedPanel))(arg0->unk_2C, 0, 0, 0x10, 0x10);
    arg0->unk_2C->node.flags |= 0x200;
    arg0->unk_2C->node.flags |= 0x400;
    arg0->unk_2C->node.flags &= ~1;
    ((func8850068C)Memmap_GetFragmentVaddr(WidgetTree_AppendChild))(arg0, arg0->unk_2C);

    arg0->unk_30 = mem_pool_alloc(arg5, sizeof(unk_func_885012A4));
    ((func885012A4)Memmap_GetFragmentVaddr(WidgetTree_InitSolidColor))(arg0->unk_30, 0, 0, 0x10, 0x10, D_8850CFA8);
    ((func8850068C)Memmap_GetFragmentVaddr(WidgetTree_AppendChild))(arg0->unk_2C, arg0->unk_30);
    arg0->unk_34 = mem_pool_alloc(arg5, sizeof(unk_func_8850878C));
    ((func8850878C)Memmap_GetFragmentVaddr(WidgetTree_InitTextLabel))(arg0->unk_34, 0, 0, "-----", 0x10);
    ((func8850068C)Memmap_GetFragmentVaddr(WidgetTree_AppendChild))(arg0->unk_30, arg0->unk_34);
    arg0->unk_34->unk_00.flags &= ~1;
    arg0->unk_38 = arg6;
}

void WidgetTree_SetMessagePanelText(unk_func_88509F48* arg0, unk_func_8850878C* arg1, s32 arg2, s32 arg3) {
    s16 temp_v1;

    *arg0->unk_34 = *arg1;
    arg0->unk_34->unk_00.flags |= 1;
    Font_SetActive(arg1->unk_2C, arg1->unk_38);
    Font_SetLineHeight(arg1->unk_3C);

    if (arg3 == 0) {
        arg3 = Font_MeasureTextExtent(0, 0, arg1->unk_40);
    }

    if (arg2 == 0) {
        arg2 = Text_CountLines(arg1->unk_40);
    }

    temp_v1 = arg0->unk_38->unk_00[arg0->unk_38->unk_50].unk_02;

    arg0->unk_2C->node.size.x = temp_v1 + arg3;

    arg0->unk_2C->node.size.y = (((arg1->unk_3C * arg2) + temp_v1) - arg1->unk_3C) + temp_v1;
    arg0->unk_2C->node.position.x = (arg0->unk_00.size.x - arg0->unk_2C->node.size.x) / 2;
    arg0->unk_2C->node.position.y = (arg0->unk_00.size.y - arg0->unk_2C->node.size.y) / 2;

    arg0->unk_30->unk_00.size = arg0->unk_2C->node.size;

    arg0->unk_34->unk_00.position.x = temp_v1 / 2;
    arg0->unk_34->unk_00.position.y = temp_v1 / 2;
}

s32 WidgetTree_HandleMessagePanelInput(unk_func_88509F48* arg0, Controller* arg1) {
    s32 var_v1 = 0;

    if (arg0->unk_2C->animState & 2) {
        if (arg1->buttonPressed & 0xC000) {
            var_v1 = 0x80000002;
        }
    } else {
        var_v1 = 1;
    }

    return var_v1;
}

void WidgetTree_OpenMessagePanel(unk_func_88509F48* arg0) {
    WidgetTree_OpenAnimatedPanel(arg0->unk_2C);
}

void WidgetTree_ResetMessagePanelAnimation(unk_func_88509F48* arg0) {
    arg0->unk_2C->animFrame = 0xB;
}

void WidgetTree_RunMessagePanel(unk_func_88509F48* arg0, Controller* arg1, s32 arg2) {
    s32 temp_v0;
    s32 var_s0 = 0;

    WidgetTree_OpenMessagePanel(arg0);

    while (var_s0 == 0) {
        Ui_SendMessageAndPollInput(var_s0);

        temp_v0 = arg0->unk_00.inputCallback(arg0, arg1);
        if (!(temp_v0 & 1) && (temp_v0 & 2)) {
            var_s0 = 1;
        }

        if (arg2 != 0) {
            ((func8850BC94)Memmap_GetFragmentVaddr(Ui_PlayInputActionSound))(temp_v0);
        }
    }

    WidgetTree_ResetMessagePanelAnimation(arg0);
}

void WidgetTree_RunMessagePanelWithSound(unk_func_88509F48* arg0, Controller* arg1) {
    WidgetTree_RunMessagePanel(arg0, arg1, 1);
}

void WidgetTree_RunMessagePanelSilent(unk_func_88509F48* arg0, Controller* arg1) {
    WidgetTree_RunMessagePanel(arg0, arg1, 0);
}

void WidgetTree_InitBorderFrame(WidgetNode* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    ((func885007CC)Memmap_GetFragmentVaddr(WidgetTree_InitWidget))(arg0, sizeof(WidgetNode));
    arg0->drawCallback = WidgetTree_DrawBorderFrame;
    arg0->position.x = arg1;
    arg0->position.y = arg2;
    arg0->size.x = arg3;
    arg0->size.y = arg4;
}

s32 WidgetTree_DrawBorderFrame(WidgetNode* arg0, s32 arg1, s32 arg2) {
    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    gDPLoadTextureBlock(gDisplayListHead++, D_4001638, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, arg1 << 2, arg2 << 2, ((arg0->size.x + arg1) - 8) << 2,
                        (arg2 + 8) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4001710, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, ((arg0->size.x + arg1) - 8) << 2, arg2 << 2,
                        (arg0->size.x + arg1) << 2, ((arg2 + arg0->size.y) - 8) << 2, G_TX_RENDERTILE, 0,
                        0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_40016C8, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, (arg1 + 8) << 2, ((arg2 + arg0->size.y) - 8) << 2,
                        (arg0->size.x + arg1) << 2, (arg2 + arg0->size.y) << 2, G_TX_RENDERTILE,
                        (arg0->size.x * -0x20) + 0x200, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4001680, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, arg1 << 2, (arg2 + 8) << 2, (arg1 + 8) << 2,
                        (arg2 + arg0->size.y) << 2, G_TX_RENDERTILE, 0, (arg0->size.y * -0x20) + 0x200,
                        0x0400, 0x0400);
    return 0;
}

void WidgetTree_InitInsetBorderFrame(WidgetNode* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    ((func885007CC)Memmap_GetFragmentVaddr(WidgetTree_InitWidget))(arg0, sizeof(WidgetNode));
    arg0->drawCallback = WidgetTree_DrawInsetBorderFrame;
    arg0->position.x = arg1;
    arg0->position.y = arg2;
    arg0->size.x = arg3;
    arg0->size.y = arg4;
}

s32 WidgetTree_DrawInsetBorderFrame(WidgetNode* arg0, s32 arg1, s32 arg2) {
    s32 spE4;
    s32 spE0;
    s32 spDC;
    s32 spD8;

    // clang-format off
    spE4 = arg1 - 2; spE0 = arg2 - 2;
    // clang-format on
    spDC = arg0->size.x + 4;
    spD8 = arg0->size.y + 4;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    gDPLoadTextureBlock(gDisplayListHead++, D_40068E8, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, spE4 << 2, spE0 << 2, ((spE4 + spDC) - 8) << 2, (spE0 + 8) << 2,
                        G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_40069C0, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, ((spE4 + spDC) - 8) << 2, spE0 << 2, (spE4 + spDC) << 2,
                        ((spE0 + spD8) - 8) << 2, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4006978, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, (spE4 + 8) << 2, ((spE0 + spD8) - 8) << 2, (spE4 + spDC) << 2,
                        (spE0 + spD8) << 2, G_TX_RENDERTILE, (spDC * -0x20) + 0x200, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4006930, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, spE4 << 2, (spE0 + 8) << 2, (spE4 + 8) << 2, (spE0 + spD8) << 2,
                        G_TX_RENDERTILE, 0, (spD8 * -0x20) + 0x200, 0x0400, 0x0400);
    return 0;
}

void WidgetTree_InitDashedBorderFrame(unk_func_8850B254* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, Color_RGBA8 arg5,
                   Color_RGBA8 arg6) {
    WidgetTree_InitWidget(&arg0->unk_00, sizeof(unk_func_8850B254));
    arg0->unk_00.drawCallback = WidgetTree_DrawDashedBorderFrame;
    arg0->unk_00.position.x = arg1;
    arg0->unk_00.position.y = arg2;
    arg0->unk_00.size.x = arg3;
    arg0->unk_00.size.y = arg4;
    arg0->unk_2C = arg5;
    arg0->unk_30 = arg6;
}

#ifdef NON_MATCHING
s32 WidgetTree_DrawDashedBorderFrame(unk_func_8850B254* arg0, s32 arg1, s32 arg2) {
    s32 h = 8;
    s32 w = 8;

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCombineMode(gDisplayListHead++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, arg0->unk_2C.r, arg0->unk_2C.g, arg0->unk_2C.b, arg0->unk_2C.a);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007AD0, G_IM_FMT_IA, G_IM_SIZ_8b, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, (arg1 - 7) << 2, (arg2 - 7) << 2,
                        ((arg1 - 1) + arg0->unk_00.size.x) << 2, (arg2 + 1) << 2, G_TX_RENDERTILE, 0, 0, 0x0400,
                        0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007B18, G_IM_FMT_IA, G_IM_SIZ_8b, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, ((arg1 - 1) + arg0->unk_00.size.x) << 2, (arg2 - 7) << 2,
                        ((arg1 + 7) + arg0->unk_00.size.x) << 2, ((arg2 - 1) + arg0->unk_00.size.y) << 2,
                        G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007B60, G_IM_FMT_IA, G_IM_SIZ_8b, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, (arg1 + 1) << 2, ((arg2 - 1) + arg0->unk_00.size.y) << 2,
                        ((arg1 + 7) + arg0->unk_00.size.x) << 2, ((arg2 + 7) + arg0->unk_00.size.y) << 2,
                        G_TX_RENDERTILE, (arg0->unk_00.size.x * -0x20) + 0x40, 0, 0x0400, 0x0400);

    gDPLoadTextureBlock(gDisplayListHead++, D_4007BA8, G_IM_FMT_IA, G_IM_SIZ_8b, w, h, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSPTextureRectangle(gDisplayListHead++, (arg1 - 7) << 2, (arg2 + 1) << 2, (arg1 + 1) << 2,
                        ((arg2 + 7) + arg0->unk_00.size.y) << 2, G_TX_RENDERTILE, 0,
                        (arg0->unk_00.size.y * -0x20) + 0x40, 0x0400, 0x0400);

    gDPPipeSync(gDisplayListHead++);

    gDPSetCycleType(gDisplayListHead++, G_CYC_FILL);
    gDPSetRenderMode(gDisplayListHead++, G_RM_NOOP, G_RM_NOOP2);

    {
        // clang-format off
        s32 color = ((arg0->unk_30.r << 8) & 0xF800) | ((arg0->unk_30.g << 3) & 0x7C0) | ((arg0->unk_30.b >> 2) & 0x3E) | 1;gDPPipeSync(gDisplayListHead++);
        // clang-format on
        gDPSetFillColor(gDisplayListHead++, (color << 0x10) | color);
    }
    gDPFillRectangle(gDisplayListHead++, arg1, arg2, (arg0->unk_00.size.x + arg1) - 1,
                     (arg2 + arg0->unk_00.size.y) - 1);

    return 0;
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/widget_tree/widget_tree/WidgetTree_DrawDashedBorderFrame.s")
#endif

void Ui_PlayInputActionSound(s32 arg0) {
    u32 var_a1 = 0;

    if (arg0 & 0x80000000) {
        arg0 &= 0x7FFFFFFF;
        arg0 &= 0x99E;

        switch (arg0) {
            case 0x2:
                var_a1 = 3;
                break;

            case 0x4:
                var_a1 = 2;
                break;

            case 0x8:
                var_a1 = 1;
                break;

            case 0x800:
                var_a1 = 0x18;
                break;

            case 0x80:
                var_a1 = 0x29;
                break;

            case 0x100:
                var_a1 = 8;
                break;
        }

        if (var_a1 != 0) {
            Audio_PlaySoundEffectById(var_a1);
        }
    }
}

void WidgetTree_InitScrollableGridInputProxy(unk_func_8850BD40* arg0, unk_func_8821421C_038_034* arg1) {
    WidgetTree_InitNode(arg0, sizeof(unk_func_8850BD40));
    arg0->unk_10 = WidgetTree_HandleScrollableGridInputProxy;
    arg0->unk_14 = WidgetTree_SetScrollableGridInputProxyState;
    arg0->unk_1C = arg1;
}

s32 WidgetTree_HandleScrollableGridInputProxy(unk_func_8850BD40* arg0, Controller* arg1) {
    s32 var_v1 = 0;

    if (arg0->unk_1C->unk_00.unk_00.inputCallback != NULL) {
        var_v1 = arg0->unk_1C->unk_00.unk_00.inputCallback(arg0->unk_1C, arg1);
    }
    return var_v1;
}

void WidgetTree_SetScrollableGridInputProxyState(unk_func_8850BD40* arg0, s32 arg1) {
    arg0->unk_1C->unk_00.unk_00.setStateCallback(&arg0->unk_1C->unk_00.unk_00, arg1);
}

void WidgetTree_InitVerticalMenu(unk_func_8830867C_02C_048_000* arg0) {
    WidgetTree_InitNode(arg0, sizeof(unk_func_8830867C_02C_048_000));
    arg0->unk_10 = WidgetTree_HandleVerticalMenuInput;
    arg0->unk_14 = WidgetTree_SetVerticalMenuState;
    arg0->unk_18 = 0;
    arg0->unk_1C = 0;
    arg0->unk_20 = 0;
}

s32 WidgetTree_HandleVerticalMenuInput(unk_func_8830867C_02C_048_000* arg0, Controller* arg1) {
    s32 ret = 0;

    if (arg0->unk_20 > 0) {
        unk_func_8830867C_02C_048_000* var_a0 = arg0->unk_00.firstChild;
        s32 i;

        for (i = 0; i < arg0->unk_1C; i++) {
            var_a0 = var_a0->unk_00.nextSibling;
        }
        ret = var_a0->unk_10(var_a0, arg1);
    }

    if (ret & 1) {
        return ret;
    }

    if (ret == 0) {
        if (arg1->buttonPressed & 0x4000) {
            ret = 0x80000002;
        } else if (arg1->buttonPressed & 0x8000) {
            ret = 0x80000004;
        }
    }

    if (ret & 2) {
        if (arg0->unk_1C > 0) {
            arg0->unk_1C--;
            WidgetTree_UpdateVerticalMenuItems(arg0);
            ret |= 1;
        }
    } else if (ret & 4) {
        if (arg0->unk_1C < (arg0->unk_20 - 1)) {
            arg0->unk_1C++;
            WidgetTree_UpdateVerticalMenuItems(arg0);
            ret |= 1;
        }
    }

    return ret;
}

void WidgetTree_SetVerticalMenuState(unk_func_8830867C_02C_048_000* arg0, s32 arg1) {
    arg0->unk_18 = arg1;
    WidgetTree_UpdateVerticalMenuItems(arg0);
}

void WidgetTree_AppendVerticalMenuItem(unk_func_8830867C_02C_048_000* arg0, WidgetLinkHeader* arg1) {
    WidgetTree_AppendChild(&arg0->unk_00, arg1);
    arg0->unk_20++;
}

void WidgetTree_UpdateVerticalMenuItems(unk_func_8830867C_02C_048_000* arg0) {
    s32 temp_s3;
    s32 temp_s4;
    s32 var_v0;
    s32 var_v1;
    unk_func_8830867C_02C_048_000* var_s1;
    s32 i;

    var_s1 = arg0->unk_00.firstChild;
    temp_s4 = arg0->unk_18 & 0xFF;
    temp_s3 = arg0->unk_18 & 0xFF00;

    for (i = 0; i < arg0->unk_20; i++) {
        var_v0 = 0;
        var_v1 = temp_s3;
        if (i < arg0->unk_1C) {
            var_v1 |= 0x100;
        } else if (i == arg0->unk_1C) {
            var_v0 = 1;
        }

        var_s1->unk_14(var_s1, (var_v0 & temp_s4) | var_v1);
        var_s1 = var_s1->unk_00.nextSibling;
    }
}

void WidgetTree_SetVerticalMenuSelection(unk_func_8830867C_02C_048_000* arg0, s32 arg1) {
    arg0->unk_1C = arg1;
    WidgetTree_UpdateVerticalMenuItems(arg0);
}

void WidgetTree_InitHorizontalMenu(unk_func_8830867C_02C_048_000* arg0) {
    WidgetTree_InitNode(arg0, sizeof(unk_func_8830867C_02C_048_000));
    arg0->unk_10 = WidgetTree_HandleHorizontalMenuInput;
    arg0->unk_14 = WidgetTree_SetHorizontalMenuState;
    arg0->unk_18 = 0;
    arg0->unk_1C = 0;
    arg0->unk_20 = 0;
}

s32 WidgetTree_HandleHorizontalMenuInput(unk_func_8830867C_02C_048_000* arg0, Controller* arg1) {
    s32 var_a3 = 0;

    if (arg0->unk_20 > 0) {
        unk_func_8830867C_02C_048_000* var_a0 = arg0->unk_00.firstChild;
        s32 i;

        for (i = 0; i < arg0->unk_1C; i++) {
            var_a0 = var_a0->unk_00.nextSibling;
        }
        var_a3 = var_a0->unk_10(var_a0, arg1);
    }

    if (var_a3 == 0) {
        s32 var_v0_2 = arg0->unk_1C;

        if (arg1->buttonPressed & 0x200) {
            if (var_v0_2 == 0) {
                var_v0_2 = arg0->unk_20 - 1;
            } else {
                var_v0_2--;
            }
        } else if (arg1->buttonPressed & 0x100) {
            if (var_v0_2 + 1 == arg0->unk_20) {
                var_v0_2 = 0;
            } else {
                var_v0_2++;
            }
        }

        if (var_v0_2 != arg0->unk_1C) {
            arg0->unk_1C = var_v0_2;
            WidgetTree_UpdateHorizontalMenuItems(arg0);
            var_a3 = 0x80000008;
        }
    }
    return var_a3;
}

void WidgetTree_SetHorizontalMenuState(unk_func_8830867C_02C_048_000* arg0, s32 arg1) {
    arg0->unk_18 = arg1;
    WidgetTree_UpdateHorizontalMenuItems(arg0);
}

void WidgetTree_AppendHorizontalMenuItem(unk_func_8830867C_02C_048_000* arg0, WidgetLinkHeader* arg1) {
    WidgetTree_AppendChild(&arg0->unk_00, arg1);
    arg0->unk_20++;
}

void WidgetTree_UpdateHorizontalMenuItems(unk_func_8830867C_02C_048_000* arg0) {
    unk_func_8830867C_02C_048_000* var_s0 = arg0->unk_00.firstChild;
    s32 i;

    for (i = 0; i < arg0->unk_20; i++) {
        s32 var_a1 = 0;

        if (i == arg0->unk_1C) {
            var_a1 = arg0->unk_18;
        }
        var_s0->unk_14(var_s0, var_a1);
        var_s0 = var_s0->unk_00.nextSibling;
    }
}

void WidgetTree_InitGridMenu(WidgetGridMenu* arg0, s32 arg1, s32 arg2, MemoryPool* arg3) {
    s32 i;

    WidgetTree_InitNode(arg0, sizeof(WidgetGridMenu));
    arg0->inputCallback = WidgetTree_HandleGridMenuInput;
    arg0->setStateCallback = WidgetTree_SetGridMenuState;
    arg0->items = mem_pool_alloc(arg3, (arg2 * sizeof(WidgetNode*)) * arg1);

    for (i = 0; i < arg2 * arg1; i++) {
        arg0->items[i] = NULL;
    }

    arg0->itemState = 0;
    arg0->selectedIndex = 0;
    arg0->wrapFlags = 0;
    arg0->rowCount = arg1;
    arg0->columnCount = arg2;
    arg0->cursor = NULL;
    arg0->selectedRow = 0;
    arg0->selectedColumn = 0;
    arg0->cursorOffsetX = -3;
    arg0->cursorOffsetY = -3;
    arg0->cursorSizeOffsetX = 6;
    arg0->cursorSizeOffsetY = 6;
}

s32 WidgetTree_HandleGridMenuInput(WidgetGridMenu* arg0, Controller* arg1) {
    s32 sp5C;
    UNUSED s32 pad1;
    s32 sp54;
    s32 sp50;
    s32 sp4C;
    s32 sp48;
    UNUSED s32 pad2;
    s32 i;
    s32 j;
    s32 var_t4;
    s32 sp24;

    sp5C = 0;
    var_t4 = 0;
    if (arg0->items[arg0->selectedIndex] != NULL) {
        if (arg0->items[arg0->selectedIndex]->inputCallback != NULL) {
            sp5C = arg0->items[arg0->selectedIndex]->inputCallback(arg0->items[arg0->selectedIndex], arg1);
        }
    }

    if (sp5C != 0) {
        return sp5C;
    }

    sp54 = arg0->selectedIndex % arg0->columnCount;
    sp50 = arg0->selectedIndex / arg0->columnCount;
    sp24 = Input_GetRepeatedDPad() & 0xFFFF;

    if (sp24 == 0x800) {
        for (i = 0, sp48 = sp50; i < arg0->rowCount - 1; i++) {
            sp48 = ((sp48 == 0) ? arg0->rowCount : sp48) - 1;

            for (j = 0, sp4C = arg0->selectedColumn; j < arg0->columnCount;) {
                if ((arg0->items[sp48 * arg0->columnCount + sp4C] != NULL) &&
                    !(arg0->items[sp48 * arg0->columnCount + sp4C]->flags & 2)) {
                    var_t4 = 1;
                    break;
                }

                j++;
                sp4C = ((sp4C == 0) ? arg0->columnCount : sp4C) - 1;
            }

            if (var_t4 != 0) {
                break;
            }
        }

        if ((arg0->wrapFlags & 0x200) && ((var_t4 == 0) || (sp50 < sp48))) {
            sp5C = 8;
            var_t4 = 0;
        }
    } else if (sp24 == 0x400) {
        for (i = 0, sp48 = sp50; i < arg0->rowCount - 1; i++) {
            if (sp48 == arg0->rowCount - 1) {
                sp48 = 0;
            } else {
                sp48++;
            }

            for (j = 0, sp4C = arg0->selectedColumn; j < arg0->columnCount;) {
                if ((arg0->items[sp48 * arg0->columnCount + sp4C] != NULL) &&
                    !(arg0->items[sp48 * arg0->columnCount + sp4C]->flags & 2)) {
                    var_t4 = 1;
                    break;
                }

                j++;
                sp4C = ((sp4C == 0) ? arg0->columnCount : sp4C) - 1;
            }

            if (var_t4 != 0) {
                break;
            }
        }

        if ((arg0->wrapFlags & 0x200) && ((var_t4 == 0) || (sp48 < sp50))) {
            sp5C = 8;
            var_t4 = 0;
        }
    } else if (sp24 == 0x100) {
        for (j = 0, sp4C = sp54; j < arg0->columnCount - 1; j++) {
            if (sp4C == arg0->columnCount - 1) {
                sp4C = 0;
            } else {
                sp4C++;
            }

            for (i = 0, sp48 = arg0->selectedRow; i < arg0->rowCount;) {
                if ((arg0->items[sp48 * arg0->columnCount + sp4C] != NULL) &&
                    !(arg0->items[sp48 * arg0->columnCount + sp4C]->flags & 2)) {
                    var_t4 = 1;
                    break;
                }

                i++;
                sp48 = ((sp48 == 0) ? arg0->rowCount : sp48) - 1;
            }

            if (var_t4 != 0) {
                break;
            }
        }

        if ((arg0->wrapFlags & 0x100) && ((var_t4 == 0) || (sp4C < sp54))) {
            sp5C = 8;
            var_t4 = 0;
        }
    } else if (sp24 == 0x200) {
        for (j = 0, sp4C = sp54; j < arg0->columnCount - 1; j++) {
            sp4C = ((sp4C == 0) ? arg0->columnCount : sp4C) - 1;

            for (i = 0, sp48 = arg0->selectedRow; i < arg0->rowCount;) {
                if ((arg0->items[sp48 * arg0->columnCount + sp4C] != NULL) &&
                    !(arg0->items[sp48 * arg0->columnCount + sp4C]->flags & 2)) {
                    var_t4 = 1;
                    break;
                }

                i++;
                sp48 = ((sp48 == 0) ? arg0->rowCount : sp48) - 1;
            }

            if (var_t4 != 0) {
                break;
            }
        }

        if ((arg0->wrapFlags & 0x100) && ((var_t4 == 0) || (sp54 < sp4C))) {
            sp5C = 8;
            var_t4 = 0;
        }
    }

    if (var_t4 != 0) {
        arg0->selectedIndex = (arg0->columnCount * sp48) + sp4C;
        if ((sp24 == 0x800) || (sp24 == 0x400)) {
            arg0->selectedRow = sp48;
        } else {
            arg0->selectedColumn = sp4C;
        }
        WidgetTree_UpdateGridMenuItems(arg0);
        sp5C = 0x80000009;
    }
    return sp5C;
}

void WidgetTree_SetGridMenuState(WidgetGridMenu* arg0, s32 arg1) {
    arg0->itemState = arg1;
    WidgetTree_UpdateGridMenuItems(arg0);
}

void WidgetTree_UpdateGridMenuItems(WidgetGridMenu* arg0) {
    s32 i;

    if (arg0->cursor != NULL) {
        arg0->cursor->node.flags &= 0xFFFE;
    }

    for (i = 0; i < arg0->columnCount * arg0->rowCount; i++) {
        WidgetNode* temp_s1 = arg0->items[i];

        if (temp_s1 != NULL) {
            s32 var_s2 = 0;

            if (i == arg0->selectedIndex) {
                var_s2 = arg0->itemState;
                if (arg0->cursor != NULL) {
                    arg0->cursor->node.position = temp_s1->position;
                    arg0->cursor->node.size = temp_s1->size;
                    arg0->cursor->node.position.x = temp_s1->position.x + arg0->cursorOffsetX;
                    arg0->cursor->node.position.y = temp_s1->position.y + arg0->cursorOffsetY;
                    arg0->cursor->node.size.x = temp_s1->size.x + arg0->cursorSizeOffsetX;
                    arg0->cursor->node.size.y = temp_s1->size.y + arg0->cursorSizeOffsetY;
                    arg0->cursor->node.setStateCallback(&arg0->cursor->node, var_s2);
                    if (var_s2 & 0x101) {
                        arg0->cursor->node.flags |= 1;
                    }
                }
            }
            temp_s1->setStateCallback(temp_s1, var_s2);
        }
    }
}

void WidgetTree_SelectFirstEnabledGridItem(WidgetGridMenu* arg0) {
    s32 i;

    for (i = 0; i < arg0->columnCount * arg0->rowCount; i++) {
        if ((arg0->items[i] != NULL) && !(arg0->items[i]->flags & 2)) {
            arg0->selectedIndex = i;
            arg0->selectedColumn = i % arg0->columnCount;
            arg0->selectedRow = i / arg0->columnCount;
            break;
        }
    }
}

void WidgetTree_SetGridMenuSelection(WidgetGridMenu* arg0, s32 arg1) {
    arg0->selectedIndex = arg1;
    arg0->selectedColumn = arg1 % arg0->columnCount;
    arg0->selectedRow = arg1 / arg0->columnCount;
    WidgetTree_UpdateGridMenuItems(arg0);
}

void WidgetTree_InitChildSelectionProxy(unk_func_8830867C_02C_048_000* arg0) {
    WidgetTree_InitNode(&arg0->unk_00, sizeof(unk_func_8830867C_02C_048_000));
    arg0->unk_10 = WidgetTree_HandleChildSelectionProxyInput;
    arg0->unk_14 = WidgetTree_SetChildSelectionProxyState;
    arg0->unk_18 = 0;
    arg0->unk_1C = 0;
    arg0->unk_20 = 0;
}

s32 WidgetTree_HandleChildSelectionProxyInput(unk_func_8830867C_02C_048_000* arg0, Controller* arg1) {
    s32 i;
    s32 var_v1 = 0;

    if (arg0->unk_20 > 0) {
        unk_func_8830867C_02C_048_000* var_a0 = arg0->unk_00.firstChild;

        for (i = 0; i < arg0->unk_1C; i++) {
            var_a0 = var_a0->unk_00.nextSibling;
        }
        var_v1 = var_a0->unk_10(var_a0, arg1);
    }
    return var_v1;
}

void WidgetTree_SetChildSelectionProxyState(unk_func_8830867C_02C_048_000* arg0, s32 arg1) {
    arg0->unk_18 = arg1;
    WidgetTree_UpdateChildSelectionProxyItems(arg0);
}

void WidgetTree_AppendChildSelectionProxyItem(unk_func_8830867C_02C_048_000* arg0, WidgetNode* arg1) {
    WidgetTree_AppendChild(&arg0->unk_00.recordSize, &arg1->link);
    arg0->unk_20++;
}

void WidgetTree_UpdateChildSelectionProxyItems(unk_func_8830867C_02C_048_000* arg0) {
    s32 var_a1;
    s32 i;
    unk_func_8830867C_02C_048_000* var_s0 = arg0->unk_00.firstChild;

    for (i = 0; i < arg0->unk_20; i++) {
        if (i == arg0->unk_1C) {
            var_a1 = arg0->unk_18;
        } else {
            var_a1 = 0;
        }
        var_s0->unk_14(var_s0, var_a1);
        var_s0 = var_s0->unk_00.nextSibling;
    }
}

void WidgetTree_SetChildSelectionProxySelection(unk_func_8830867C_02C_048* arg0, s32 arg1) {
    arg0->unk_00.unk_1C = arg1;
    WidgetTree_UpdateChildSelectionProxyItems(&arg0->unk_00);
}

void WidgetTree_InitChildWidgetGroup(unk_func_8850CD44* arg0, s32 arg1, MemoryPool* arg2) {
    s32 i;

    ((func88500668)Memmap_GetFragmentVaddr(WidgetTree_InitNode))(arg0, sizeof(unk_func_8850CD44));
    arg0->unk_10 = WidgetTree_HandleChildWidgetGroupInput;
    arg0->unk_14 = WidgetTree_SetChildWidgetGroupState;
    arg0->unk_18 = 0;
    arg0->unk_20 = arg1;
    arg0->unk_1C = mem_pool_alloc(arg2, arg1 * 4);

    for (i = 0; i < arg1; i++) {
        arg0->unk_1C[i] = NULL;
    }
}

s32 WidgetTree_HandleChildWidgetGroupInput(unk_func_8850CD44* arg0, Controller* arg1) {
    s32 i;

    for (i = 0; i < arg0->unk_20; i++) {
        if (arg0->unk_1C[i]->unk_00.flags & 0x100) {
            return 0x40;
        }
    }
    return 4;
}

void WidgetTree_SetChildWidgetGroupState(unk_func_8850CD44* arg0, s32 arg1) {
    s32 i;

    for (i = 0; i < arg0->unk_20; i++) {
        arg0->unk_1C[i]->unk_00.setStateCallback(arg0->unk_1C[i], arg1);
    }
}

void WidgetTree_RunChildWidgetGroup(unk_func_8850CD44* arg0, Controller* arg1) {
    s32 temp_v0;
    s32 var_s0 = 0;

    while (var_s0 == 0) {
        Ui_SendMessageAndPollInput(var_s0);
        temp_v0 = arg0->unk_10(arg0, arg1);
        if (!(temp_v0 & 1) && (temp_v0 & 4)) {
            var_s0 = 1;
        }
    }
}
