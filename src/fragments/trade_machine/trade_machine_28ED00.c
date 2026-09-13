#include "trade_machine.h"
#include "src/geo_render.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/game_state.h"
#include "src/text_system.h"
#include "src/audio_sfx.h"
#include "src/matrix.h"
#include "src/geo_layout.h"
#include "src/math_util.h"
#include "src/memmap.h"
#include "src/memory.h"

s32 Trade_DrawBox3DModel(s32 arg0, GraphNode* arg1);

static unk_D_82F20A40 D_82F20A40;
static unk_D_82F20A88 D_82F20A88[4];
static GraphNode* D_82F210C8;
static GraphNode* D_82F210CC;
static s16 D_82F210D0;
static s16 D_82F210D2;
static s16 D_82F210D4;
static s16 D_82F210D8[2];

static Vtx D_82F13CB0[] = {
    VTX(-32, 64, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, 32, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 32, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF), VTX(32, 64, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-32, 32, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-32, 0, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(32, 0, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(32, 32, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-16, 39, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),      VTX(-16, 15, 0, 0, 768, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(16, 15, 0, 1024, 768, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(16, 39, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};

static Gfx D_82F13D70[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineLERP(0, 0, 0, TEXEL0, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(D_82F13CB0, 12, 0),
    gsDPLoadTextureBlock(0x0F000000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsDPLoadTextureBlock(0x0F001000, G_IM_FMT_RGBA, G_IM_SIZ_16b, 64, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsDPLoadTextureBlock(0x0E000000, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 24, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                         G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSPEndDisplayList(),
};

static u32 D_82F13E58[] = {
    0x17000000, 0x00000000, 0x00000000,    0x00000000, D_82F13CB0, 0x05000000, 0x22050000,
    0x00000000, 0x08000000, Trade_DrawBox3DModel, D_82F13D70, 0x06000000, 0x01000000,
};

static u32 D_82F13E8C[] = {
    0x0C000000, 0x05000000, 0x0B00002D,  0x00000000, 0x028001E0, 0x00000000, 0xFDBD0000, 0x00000243,
    0x05000000, 0x0D000000, 0x05000000,  0x14000000, 0x002D013B, 0xFFFFFF32, 0x16FFFFFF, 0x0F000002,
    0x05000000, 0x0A000000, &D_800AC840, 0x06000000, 0x06000000, 0x06000000, 0x06000000, 0x01000000,
};
static s16 D_82F13EEC[] = {
    0x007E,
    0x013C,
    0x0184,
    0x0000,
};
static s16 D_82F13EF4[] = {
    0x007E,
    0x0120,
    0x0184,
    0x0038,
};
static s16 D_82F13EFC[] = {
    0x00A1,
    0x013C,
    0x013E,
    0x0000,
};
static s16 D_82F13F04[] = {
    0x00A1,
    0x0120,
    0x013E,
    0x0038,
};
static s16 D_82F13F0C[] = {
    0x007D,
    0x0149,
    0x0186,
    0x0000,
};
static s16 D_82F13F14[] = {
    0x007D,
    0x0120,
    0x0186,
    0x0052,
};
static s16 D_82F13F1C[] = {
    0x00CC,
    0x013C,
    0x00E8,
    0x0000,
};
static s16 D_82F13F24[] = {
    0x00CC,
    0x0120,
    0x00E8,
    0x0038,
};
static u8* D_82F13F2C[] = {
    D_302FAB0,
    D_302FDB0,
    D_30300B0,
    D_30303B0,
};

s32 Trade_DrawBox3DModel(s32 arg0, GraphNode* arg1) {
    if (arg0 == 5) {
        s32 idx = D_8006F09C->unk_000.unk_14;

        gDPPipeSync(gDisplayListHead++);
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, D_8006F09C->materialAlpha);

        if (D_82F20A88[idx].unk_024 != NULL) {
            gSPSegment(gDisplayListHead++, 0x0F, Memmap_GetSegmentVaddr(D_82F20A88[idx].unk_024));
            gSPSegment(gDisplayListHead++, 0x0E, Memmap_GetSegmentVaddr(D_82F13F2C[D_82F20A88[idx].unk_002]));
            gSPDisplayList(gDisplayListHead++, arg1->unk_14);
        }
        GeoRender_ApplyMaterialState();
    }
    return 0;
}

s16 Trade_GetBoxSlotType(unk_D_82F14450* arg0) {
    s16 ret;

    switch (arg0->unk_00) {
        case 0:
            if (arg0->unk_04 == 0) {
                ret = 4;
            } else if (arg0->unk_03 == 0) {
                ret = 3;
            } else {
                ret = 0;
            }
            break;

        case 2:
            ret = 1;
            break;

        case 3:
            ret = 2;
            break;

        default:
            ret = -1;
            break;
    }

    return ret;
}

void Trade_SetupBoxSlots(s16 arg0) {
    s16 temp_v0;
    unk_D_82F20A88* var_s0;
    s32 i;

    var_s0 = &D_82F20A88[0];
    for (i = 0; i < 4; i++, var_s0++) {
        var_s0->animState = 0;
        var_s0->unk_002 = -1;
        var_s0->slotType = 0;
        var_s0->unk_024 = NULL;
    }

    D_82F210D0 = 1;

    for (i = 0; i < 4; i++) {
        temp_v0 = Trade_GetBoxSlotType(&D_82F14450[i]);
        if (temp_v0 == -1) {
            continue;
        }

        if (i != D_82F210D8[0]) {
            var_s0 = &D_82F20A88[D_82F210D0++];
        } else {
            var_s0 = &D_82F20A88[0];
        }

        var_s0->unk_002 = i;
        var_s0->slotType = temp_v0;

        switch (var_s0->slotType) {
            case 0:
            case 2:
            case 3:
            case 4:
                switch (D_82F14450[i].unk_01) {
                    case 1:
                        var_s0->unk_024 = D_30306B0;
                        break;

                    case 2:
                        var_s0->unk_024 = D_30326B0;
                        break;

                    case 3:
                        var_s0->unk_024 = D_30346B0;
                        break;

                    case 7:
                        var_s0->unk_024 = D_30366B0;
                        break;
                }
                break;

            case 1:
                var_s0->unk_024 = D_302BE68;
                break;
        }
    }

    var_s0 = &D_82F20A88[0];
    for (i = 0; i < D_82F210D0; i++, var_s0++) {
        unk_D_86002F58_004_000* ptr = &var_s0->model;

        ModelRenderer_AttachDisplayObject(ptr);
        Model_InitDisplayObject(ptr, 0, 0, D_82F210CC);
        ptr->unk_000.unk_14 = i;
        ptr->unk_000.unk_01 &= ~1;
    }
}

void Trade_SetBoxSlotAnimState(unk_D_82F20A88* arg0, s16 arg1) {
    unk_D_86002F58_004_000* temp_v0 = &arg0->model;

    arg0->animState = arg1;

    switch (arg0->animState) {
        case 0:
            temp_v0->unk_000.unk_01 &= ~1;
            break;

        case 1:
            if ((((s32)arg0 - (s32)D_82F20A88) / 400) == 0) {
                arg0->screenX = 0xF8;
                arg0->screenY = 0xB8;
            }

            arg0->timer = 0;
            arg0->rotationA = 0;
            arg0->rotationB = 0;
            arg0->unk_012 = 0;

            if (arg0->slotType != 0) {
                arg0->alpha = 0x80;
            } else {
                arg0->alpha = 0xFF;
            }

            arg0->scaleX = 1.0f;
            arg0->scaleY = 1.0f;
            arg0->scaleZ = 1.0f;
            temp_v0->unk_000.unk_01 |= 1;
            break;

        case 2:
            arg0->screenX = ((((s32)arg0 - (s32)D_82F20A88) / 400) * 0x88) + 0x30;
            arg0->screenY = 0x17C;
            arg0->targetX = arg0->screenX;
            arg0->timer = 0xA;
            arg0->rotationA = 0;
            arg0->rotationB = 0;
            arg0->unk_012 = 0;

            if (arg0->slotType != 0) {
                arg0->alpha = 0x80;
            } else {
                arg0->alpha = 0xFF;
            }

            arg0->scaleY = 1.0f;
            arg0->scaleZ = 1.0f;
            arg0->scaleX = 0.0f;
            temp_v0->unk_000.unk_01 |= 1;
            break;

        case 4:
            arg0->timer = 0;
            arg0->targetX = arg0->screenX;

            if ((((s32)arg0 - (s32)D_82F20A88) / 400) & 1) {
                arg0->rotationA = -0x8000;
            } else {
                arg0->rotationA = 0;
            }

            arg0->rotationB = arg0->rotationA;
            arg0->scaleX = 1.0f;
            break;

        case 3:
            arg0->screenX = ((((s32)arg0 - (s32)D_82F20A88) / 400) * 0x88) + 0x30;
            arg0->screenY = 0x17C;
            arg0->timer = 0xA;
            arg0->rotationA = 0;
            arg0->rotationB = 0;
            arg0->unk_012 = 0;
            arg0->scaleY = 1.0f;
            arg0->scaleZ = 1.0f;
            break;

        case 5:
            arg0->timer = 0xA;
            break;

        case 6:
            arg0->timer = 0xA;
            break;
    }
}

void Trade_UpdateBoxSlotFlyIn(unk_D_82F20A88* arg0) {
    arg0->timer--;
    arg0->scaleX = (0xA - arg0->timer) / 10.0f;
    if (arg0->timer <= 0) {
        Trade_SetBoxSlotAnimState(arg0, 4);
    }
}

void Trade_UpdateBoxSlotBob(unk_D_82F20A88* arg0) {
    static s16 D_82F13F38[] = { 0x5C0, 0x680, 0x580 };

    s32 temp_lo = arg0 - &D_82F20A88[0];
    s16 var_a2;
    s16 var_a3;
    f32 var_fv0;
    f32 var_fv1;

    if (temp_lo - 1 == D_82F210D2) {
        var_fv0 = 1.5f;
        var_a2 = D_82F13F38[temp_lo - 1] * 2;
        var_fv1 = 0.06f;
        var_a3 = 0x3000;
    } else {
        var_a2 = D_82F13F38[temp_lo - 1];
        var_fv1 = 0.02f;
        var_a3 = 0x2000;
        var_fv0 = 1.0f;
    }

    arg0->screenX = arg0->targetX - (SINS(arg0->rotationA) * (6.0f * var_fv0));

    arg0->unk_012 = SINS(arg0->rotationA) * (1536.0f * var_fv0);
    arg0->scaleY = (SINS(arg0->rotationB) * var_fv1) + 1.0f;
    arg0->scaleZ = (COSS(arg0->rotationB) * var_fv1) + 1.0f;

    arg0->rotationA += var_a2;
    arg0->rotationB += var_a3 + ((MathUtil_Random16() & 0xF) * 0x10);
}

void Trade_UpdateBoxSlotBobIdle(unk_D_82F20A88* arg0) {
    Trade_UpdateBoxSlotBob(arg0);
}

void Trade_UpdateBoxSlotFlyOut(unk_D_82F20A88* arg0) {
    arg0->timer -= 1;
    arg0->screenX = 0x208 - (((0x208 - arg0->targetX) * arg0->timer) / 10);
    arg0->screenY = 0xB8 - ((arg0->timer * -0xC4) / 10);
    if (arg0->timer <= 0) {
        Trade_SetBoxSlotAnimState(arg0, 1);
        D_82F210D8[1] = arg0->unk_002;
    }
}

void Trade_UpdateBoxSlotFadeOut(unk_D_82F20A88* arg0) {
    arg0->timer--;
    arg0->alpha = (arg0->timer * 0xFF) / 10;
    if (arg0->timer <= 0) {
        Trade_SetBoxSlotAnimState(arg0, 0);
        D_82F210D8[1] = -1;
    }
}

void Trade_UpdateBoxSlotShrink(unk_D_82F20A88* arg0) {
    Trade_UpdateBoxSlotBob(arg0);
    arg0->timer--;
    arg0->scaleX = arg0->timer / 10.0f;
    if (arg0->timer <= 0) {
        Trade_SetBoxSlotAnimState(arg0, 0);
    }
}

void Trade_UpdateBoxSlotTransforms(void) {
    s32 i;
    unk_D_82F20A88* var_s0 = &D_82F20A88[0];

    for (i = 0; i < D_82F210D0; i++, var_s0++) {
        switch (var_s0->animState) {
            case 0:
            case 1:
                break;

            case 2:
                Trade_UpdateBoxSlotFlyIn(var_s0);
                break;

            case 4:
                Trade_UpdateBoxSlotBobIdle(var_s0);
                break;

            case 3:
                Trade_UpdateBoxSlotFlyOut(var_s0);
                break;

            case 6:
                Trade_UpdateBoxSlotShrink(var_s0);
                break;

            case 5:
                Trade_UpdateBoxSlotFadeOut(var_s0);
                break;
        }

        if (var_s0->animState != 0) {
            unk_D_86002F58_004_000* ptr = &var_s0->model;

            Vec3f_SetComponentsDuplicate(&ptr->unk_024, var_s0->screenX - 320.0f, 240.0f - var_s0->screenY, -579.0f);
            ptr->unk_01E.z = var_s0->unk_012;
            ptr->materialAlpha = var_s0->alpha;
            ptr->unk_030.x = var_s0->scaleY * var_s0->scaleX;
            ptr->unk_030.y = var_s0->scaleZ * var_s0->scaleX;
        }
    }
}

void Trade_ResetBoxMachineFlow(void) {
    unk_D_82F20A40* ptr = &D_82F20A40;
    s32 i;

    ptr->flowState = 1;
    ptr->timer = 0xA;
    ptr->selectedPanelType = 0;
    ptr->topBarRect.x = 0x40;
    ptr->topBarRect.y = 0x8C;
    ptr->topBarRect.width = 0x200;
    ptr->topBarRect.height = 0;

    for (i = 0; i < 3; i++) {
        ptr->panelRects[i].x = 0x7C + i * 0x88;
        ptr->panelRects[i].y = 0x10C;
        ptr->panelRects[i].width = 0x78;
        ptr->panelRects[i].height = 0;
    }

    ptr->titleRect.x = 0x38;
    ptr->titleRect.y = 0x1A4;
    ptr->titleRect.width = 0x210;
    ptr->titleRect.height = 0;
    ptr->confirmRect.x = 0xCC;
    ptr->confirmRect.y = 0x130;
    ptr->confirmRect.width = 0xE8;
    ptr->confirmRect.height = 0;
    ptr->bannerRect.x = 0;
    ptr->bannerRect.y = 0;
    ptr->bannerRect.width = 0;
    ptr->bannerRect.height = 0;

    D_82F210D8[1] = -1;
    D_82F210D2 = -1;

    if (D_82F20A88[0].slotType == 0) {
        D_82F210D2 = 0;
    }
}

s32 Trade_GetSelectedBox(s16 arg0) {
    return D_82F210D8[arg0];
}

s32 Trade_GetBoxMachineFlowState(void) {
    return D_82F20A40.flowState;
}

void Trade_UpdateBoxMachineFlowOpen(unk_D_82F20A40* arg0, s32 arg1) {
    s32 i;

    arg0->timer--;
    if (arg1 != 0) {
        arg0->topBarRect.x = 0x40 - ((arg0->timer * 0) / 10);
        arg0->topBarRect.y = 0x48 - ((arg0->timer * -0x44) / 10);
        arg0->topBarRect.width = 0x200 - ((arg0->timer * 0) / 10);
        arg0->topBarRect.height = 0x88 - ((arg0->timer * 0x88) / 10);
    }

    for (i = 0; i < 3; i++) {
        arg0->panelRects[i].x = ((i * 0x88) - ((arg0->timer * 0) / 10)) + 0x7C;
        arg0->panelRects[i].y = 0xF0 - ((arg0->timer * -0x1C) / 10);
        arg0->panelRects[i].width = 0x78 - ((arg0->timer * 0) / 10);
        arg0->panelRects[i].height = 0x38 - ((arg0->timer * 0x38) / 10);
    }

    if (arg1 != 0) {
        arg0->titleRect.x = 0x38 - ((arg0->timer * 0) / 10);
        arg0->titleRect.y = 0x190 - ((arg0->timer * -0x14) / 10);
        arg0->titleRect.width = 0x210 - ((arg0->timer * 0) / 10);
        arg0->titleRect.height = 0x28 - ((arg0->timer * 0x28) / 10);
    }

    if (arg0->timer <= 0) {
        arg0->flowState = 2;
        arg0->timer = 0;
        Trade_SetBoxSlotAnimState(D_82F20A88, 1);

        for (i = 0; i < 3; i++) {
            if (D_82F20A88[i + 1].unk_002 != -1) {
                Trade_SetBoxSlotAnimState(&D_82F20A88[i + 1], 2);
            }
        }
    }
}

void Trade_UpdateBoxMachineFlowSelect(unk_D_82F20A40* arg0) {
    s32 i;
    s16 tmp;

    MathUtil_Random16();

    tmp = D_82F210D2;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DLEFT) && (D_82F210D2 != -1) && (D_82F210D2 > 0)) {
        D_82F210D2 -= 1;
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DRIGHT) && (D_82F210D2 != -1) && (D_82F210D2 < (D_82F210D0 - 2))) {
        D_82F210D2 += 1;
    }

    if (D_82F210D2 != tmp) {
        Audio_PlaySoundEffectById(0x25);
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A) && (D_82F210D2 != -1)) {
        if (D_82F20A88[D_82F210D2 + 1].slotType == 0) {
            arg0->flowState = 6;
            arg0->timer = 0xA;

            for (i = 0; i < 3; i++) {
                if (D_82F20A88[i + 1].unk_002 != -1) {
                    if (i != D_82F210D2) {
                        Trade_SetBoxSlotAnimState(&D_82F20A88[i + 1], 6);
                    } else {
                        Trade_SetBoxSlotAnimState(&D_82F20A88[i + 1], 3);
                    }
                }
            }
            Audio_PlaySoundEffectById(2);
        } else {
            arg0->flowState = 3;
            arg0->timer = 0xA;
            arg0->selectedPanelType = D_82F20A88[D_82F210D2 + 1].slotType;

            switch (arg0->selectedPanelType) {
                case 1:
                    arg0->targetRectA = &D_82F13EEC;
                    arg0->targetRectB = &D_82F13EF4;
                    break;

                case 2:
                    arg0->targetRectA = &D_82F13EFC;
                    arg0->targetRectB = &D_82F13F04;
                    break;

                case 3:
                    arg0->targetRectA = &D_82F13F0C;
                    arg0->targetRectB = &D_82F13F14;
                    break;

                case 4:
                    arg0->targetRectA = &D_82F13F1C;
                    arg0->targetRectB = &D_82F13F24;
                    break;

                default:
                    arg0->targetRectB = NULL;
                    arg0->targetRectA = NULL;
                    break;
            }
        }
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        arg0->flowState = 0xB;
        arg0->timer = 0xA;

        for (i = 0; i < 4; i++) {
            Trade_SetBoxSlotAnimState(&D_82F20A88[i], 0);
        }

        Audio_PlaySoundEffectById(3);
    }
}

void Trade_UpdateBoxMachineFlowFrameMove(unk_D_82F20A40* arg0) {
    arg0->timer--;
    arg0->bannerRect.x = arg0->targetRectB->x - (((arg0->targetRectB->x - arg0->targetRectA->x) * arg0->timer) / 10);
    arg0->bannerRect.y = arg0->targetRectB->y - (((arg0->targetRectB->y - arg0->targetRectA->y) * arg0->timer) / 10);
    arg0->bannerRect.width = arg0->targetRectB->width - (((arg0->targetRectB->width - arg0->targetRectA->width) * arg0->timer) / 10);
    arg0->bannerRect.height = arg0->targetRectB->height - (((arg0->targetRectB->height - arg0->targetRectA->height) * arg0->timer) / 10);
    if (arg0->timer <= 0) {
        arg0->flowState = 4;
        arg0->timer = 0;
    }
}

void Trade_UpdateBoxMachineFlowConfirmWait(unk_D_82F20A40* arg0) {
    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A | BTN_B)) {
        arg0->flowState = 5;
        arg0->timer = 0xA;
    }
}

void Trade_UpdateBoxMachineFlowFrameReturn(unk_D_82F20A40* arg0) {
    arg0->timer--;
    arg0->bannerRect.x = arg0->targetRectA->x - (((arg0->targetRectA->x - arg0->targetRectB->x) * arg0->timer) / 10);
    arg0->bannerRect.y = arg0->targetRectA->y - (((arg0->targetRectA->y - arg0->targetRectB->y) * arg0->timer) / 10);
    arg0->bannerRect.width = arg0->targetRectA->width - (((arg0->targetRectA->width - arg0->targetRectB->width) * arg0->timer) / 10);
    arg0->bannerRect.height = arg0->targetRectA->height - (((arg0->targetRectA->height - arg0->targetRectB->height) * arg0->timer) / 10);
    if (arg0->timer <= 0) {
        arg0->flowState = 2;
        arg0->timer = 0;
        arg0->selectedPanelType = 0;
        arg0->targetRectB = NULL;
        arg0->targetRectA = NULL;
    }
}

void Trade_UpdateBoxMachineSlots(unk_D_82F20A40* arg0, s16 arg1, s16 arg2) {
    TradeRect* ptr;
    s32 i;
    s32 tmp;

    for (i = 0; i < 3; i++) {
        ptr = &arg0->panelRects[i];

        if ((ptr->width == 0x78) && (ptr->height == 0)) {
            continue;
        }

        tmp = (ptr->x * 0) / arg2;

        ptr->x = (0x7C + (i * 0x88)) - (tmp);
        ptr->y = 0x10C - ((arg1 * 0x1C) / arg2);
        ptr->width = 0x78 - (tmp);
        ptr->height = -((arg1 * -0x38) / arg2);
    }
}

void Trade_UpdateBoxMachineFlowPanelsFull(unk_D_82F20A40* arg0) {
    s32 i;

    arg0->timer--;
    Trade_UpdateBoxMachineSlots(arg0, arg0->timer, 0xA);
    if (arg0->timer <= 0) {
        arg0->flowState = 7;
        arg0->timer = 0xA;

        for (i = 0; i < 3; i++) {
            arg0->panelRects[i].x = 0x7C + i * 0x88;
            arg0->panelRects[i].y = 0x10C;
            arg0->panelRects[i].width = 0x78;
            arg0->panelRects[i].height = 0;
        }
    }
}

void Trade_UpdateBoxMachineFlowConfirmOpen(unk_D_82F20A40* arg0) {
    arg0->timer--;
    arg0->confirmRect.x = 0xCC - (arg0->timer * 0) / 10;
    arg0->confirmRect.y = 0xF8 - ((arg0->timer * -0x38) / 10);
    arg0->confirmRect.width = 0xE8 - (arg0->timer * 0) / 10;
    arg0->confirmRect.height = 0x70 - ((arg0->timer * 0x70) / 10);
    if (arg0->timer <= 0) {
        arg0->flowState = 8;
        arg0->timer = 0;
        D_82F210D4 = 0;
    }
}

void Trade_UpdateBoxMachineFlowConfirmInput(unk_D_82F20A40* arg0) {
    s16 tmp;
    s32 var_a2;

    var_a2 = 0;
    tmp = D_82F210D4;

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP) && (D_82F210D4 > 0)) {
        D_82F210D4 -= 1;
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN) && (D_82F210D4 <= 0)) {
        D_82F210D4 += 1;
    }

    if (D_82F210D4 != tmp) {
        Audio_PlaySoundEffectById(1);
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        var_a2 = 1;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B)) {
        var_a2 = 1;
        D_82F210D4 = 1;
    }

    if (var_a2 != 0) {
        if (D_82F210D4 == 0) {
            arg0->flowState = 0xB;
            arg0->timer = 0xA;
            Trade_SetBoxSlotAnimState(D_82F20A88, 0);
            Trade_SetBoxSlotAnimState(&D_82F20A88[D_82F210D2 + 1], 0);
            Audio_PlaySoundEffectById(0x1B);
        } else {
            arg0->flowState = 9;
            arg0->timer = 0xA;
            Trade_SetBoxSlotAnimState(&D_82F20A88[D_82F210D2 + 1], 5);
            Audio_PlaySoundEffectById(3);
        }
    }
}

void Trade_UpdateBoxMachineRect(TradeRect* arg0, s16 arg1, s16 arg2) {
    if ((arg0->width != 0xE8) || (arg0->height != 0)) {
        s32 tmp = (arg0->x * 0) / arg2;

        arg0->x = 0xCC - tmp;
        arg0->y = 0x130 - ((arg1 * 0x38) / arg2);
        arg0->width = 0xE8 - tmp;
        arg0->height = -((arg1 * -0x70) / arg2);
    }
}

void Trade_UpdateBoxMachineFlowConfirmDone(unk_D_82F20A40* arg0) {
    if (arg0->timer > 0) {
        arg0->timer--;
    }

    Trade_UpdateBoxMachineRect(&arg0->confirmRect, arg0->timer, 0xA);

    if ((arg0->timer <= 0) && (D_82F210D8[1] == -1)) {
        arg0->flowState = 0xA;
        arg0->timer = 0xA;
    }
}

void Trade_UpdateBoxMachineFlowClose(unk_D_82F20A40* arg0) {
    arg0->timer--;
    arg0->topBarRect.x = 0x40 - (arg0->timer * 0) / 10;
    arg0->topBarRect.y = 0x8C - ((arg0->timer * 0x44) / 10);
    arg0->topBarRect.width = 0x200 - (arg0->timer * 0) / 10;
    arg0->topBarRect.height = -((arg0->timer * -0x88) / 10);

    Trade_UpdateBoxMachineSlots(arg0, arg0->timer, 0xA);
    Trade_UpdateBoxMachineRect(&arg0->confirmRect, arg0->timer, 0xA);

    arg0->titleRect.x = 0x38 - (arg0->timer * 0) / 10;
    arg0->titleRect.y = 0x1A4 - ((arg0->timer * 0x14) / 10);
    arg0->titleRect.width = 0x210 - (arg0->timer * 0) / 10;
    arg0->titleRect.height = -((arg0->timer * -0x28) / 10);

    if (arg0->timer <= 0) {
        arg0->flowState = 0;
        arg0->timer = 0;
    }
}

void Trade_DrawBoxMachineTopBar(unk_D_82F20A40* arg0) {
    Color_RGB8* var_v0;
    TradeRect* temp_s4 = &arg0->topBarRect;
    s32 i;
    unk_D_82F14450* ptr;

    Trade_DrawPickScreenFrame(temp_s4->x, temp_s4->y, temp_s4->width, temp_s4->height);

    if ((temp_s4->width == 0x200) && (temp_s4->height == 0x88)) {
        gSPDisplayList(gDisplayListHead++, D_8006F498);

        Gfx_FillRectRgba(temp_s4->x, temp_s4->y, temp_s4->width, temp_s4->height, 0x32, 0x32, 0x96, 0xFF);

        gSPDisplayList(gDisplayListHead++, D_8006F630);

        for (i = 0; i < 2; i++) {
            Color_RGB8 sp5C = { 0x1E, 0x1E, 0x64 };
            Color_RGB8 sp58 = { 0x64, 0x64, 0xC8 };

            if (D_82F210D8[i] == -1) {
                var_v0 = &sp58;
            } else {
                var_v0 = &sp5C;
            }

            Trade_DrawRoundedFrameSmall(temp_s4->x + (i * 0x110) + 0x10, temp_s4->y + 0x28, 0xD0, 0x50, var_v0->r,
                          var_v0->g, var_v0->b, 0xFF);
        }

        Trade_DrawBoxMachineTransferArrows(temp_s4->x + 0xE0, temp_s4->y + 0x28);
        Font_BeginTranslucentTextRendering();
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Font_SetActive(0x10, 0);
        Font_Printf(temp_s4->x + 0x5A, temp_s4->y + 8, Text_GetString(NULL, 0, gTradeStrings, 1));

        for (i = 0; i < 2; i++) {
            if (D_82F210D8[i] != -1) {
                if (i == 0) {
                    if (D_82F20A88[0].slotType == 0) {
                        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
                    } else {
                        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0x80);
                    }
                } else {
                    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
                }

                ptr = &D_82F14450[D_82F210D8[i]];
                if (ptr) {}

                Font_SetActive(0x10, 0);
                Font_Printf(temp_s4->x + (i * 0x110) + 0x24, temp_s4->y + 0x3C, ptr->unk_08);
                Font_SetActive(8, 0);
                Font_Printf(temp_s4->x + (i * 0x110) + 0x24, temp_s4->y + 0x54, "ID%05d", ptr->unk_06);
            }
        }

        Font_EndTexturedTextRendering();
    }
}

void Trade_DrawBoxMachineSlots(unk_D_82F20A40* arg0) {
    s32 i;
    Color_RGB8* var_v0;
    s32 var_v1;
    char* var_s0_2;
    unk_D_82F14450* var_s2;
    Color_RGB8 sp88 = { 0x1E, 0x1E, 0x82 };
    Color_RGB8 sp84 = { 0x64, 0x1E, 0x1E };
    char sp68[28];
    TradeRect* ptr;

    for (i = 0; i < 3; i++) {
        ptr = &arg0->panelRects[i];

        if ((arg0->panelRects[i].width == 0x78) && (ptr->height == 0)) {
            continue;
        }

        if (D_82F20A88[i + 1].unk_002 == -1) {
            var_v1 = 0x80;
        } else {
            var_v1 = 0xFF;
        }

        if (D_82F20A88[i + 1].slotType == 0) {
            var_v0 = &sp88;
        } else {
            var_v0 = &sp84;
        }

        Trade_DrawRoundedFrameLarge(ptr->x, ptr->y, ptr->width, ptr->height, var_v0->r, var_v0->g, var_v0->b, var_v1);
    }

    Font_BeginTranslucentTextRendering();
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);

    for (i = 0; i < 3; i++) {
        var_s2 = NULL;
        if (D_82F20A88[i + 1].unk_002 == -1) {
            continue;
        }

        ptr = &arg0->panelRects[i];

        switch (D_82F20A88[i + 1].slotType) {
            case 0:
            case 3:
            case 4:
                var_s2 = &D_82F14450[D_82F20A88[i + 1].unk_002];
                break;
        }

        if ((arg0->panelRects[i].width == 0x78) && (ptr->height == 0x38)) {
            Font_SetActive(8, 0);
            if (var_s2 != NULL) {
                var_s0_2 = var_s2->unk_08;
            } else {
                var_s0_2 = "?????";
            }

            Font_Printf(((ptr->width - Font_MeasureTextExtent(0, 0, var_s0_2)) / 2) + ptr->x, ptr->y + 8, var_s0_2);
            Font_SetActive(4, 0);

            if (var_s2 != NULL) {
                sprintf(sp68, "ID%05d", var_s2->unk_06);
                var_s0_2 = sp68;
            } else {
                var_s0_2 = "ID*****";
            }

            Font_Printf(((ptr->width - Font_MeasureTextExtent(0, 0, var_s0_2)) / 2) + ptr->x, ptr->y + 0x20,
                          var_s0_2);
        }
    }

    Font_EndTexturedTextRendering();
}

void Trade_DrawBoxMachineTitle(unk_D_82F20A40* arg0) {
    TradeRect* temp_s1 = &arg0->titleRect;
    char* temp_v0;

    Trade_DrawRoundedFrameLarge(arg0->titleRect.x, arg0->titleRect.y, arg0->titleRect.width, arg0->titleRect.height, 0x1E, 0x1E, 0x82,
                  0xFF);
    if ((temp_s1->width == 0x210) && (temp_s1->height == 0x28)) {
        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        temp_v0 = Text_GetString(NULL, 0, gTradeStrings, 2);
        Font_Printf((temp_s1->x - (Font_MeasureTextExtent(0, 0, temp_v0) / 2)) + 0x108, temp_s1->y + 8, temp_v0);
        Font_EndTexturedTextRendering();
    }
}

void Trade_DrawBoxMachineConfirm(unk_D_82F20A40* arg0) {
    char* temp_v0;
    TradeRect* temp_s3 = &arg0->confirmRect;
    s32 i;
    s32 tmp;
    Color_RGB8 sp54 = { 0x1E, 0x1E, 0x82 };
    char* sp4C[2];

    if ((temp_s3->width == 0xE8) && (temp_s3->height == 0)) {
        return;
    }

    if ((temp_s3->width >= 0xE) && (temp_s3->height >= 0xE)) {
        Ui_DrawGradientPanel(temp_s3->x, temp_s3->y, temp_s3->width, temp_s3->height, &sp54, &sp54);
    }

    if ((temp_s3->width != 0xE8) || (temp_s3->height != 0x70)) {
        return;
    }

    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    temp_v0 = Text_GetString(NULL, 0, gTradeStrings, 3);
    tmp = (temp_s3->x - (Font_MeasureTextExtent(0, 0, temp_v0) / 2)) + 0x74;
    Font_Printf(tmp, temp_s3->y + 0x10, temp_v0);

    for (i = 0; i < 2; i++) {
        sp4C[0] = Text_GetString(NULL, 0, gTradeStrings, 4);
        sp4C[1] = Text_GetString(NULL, 0, gTradeStrings, 5);
        if (i != D_82F210D4) {
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        } else {
            Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF);
        }
        // clang-format off
        tmp = (temp_s3->x - (Font_MeasureTextExtent(0, 0, sp4C[i]) / 2)) + 0x74; Font_Printf(tmp, temp_s3->y + (i * 0x1C) + 0x2C, sp4C[i]);
        // clang-format on
    }

    Font_EndTexturedTextRendering();
}

void Trade_DrawBoxMachineBanner(unk_D_82F20A40* arg0) {
    UNUSED s32 pad;
    char* sp48;
    TradeRect* temp_s0 = &arg0->bannerRect;
    Color_RGB8 sp40 = { 0x64, 0x1E, 0x1E };

    if (arg0->selectedPanelType == 0) {
        return;
    }

    if ((arg0->targetRectA == NULL) || (arg0->targetRectB == NULL)) {
        return;
    }

    if ((arg0->bannerRect.width != arg0->targetRectA->width) || (arg0->bannerRect.height != arg0->targetRectA->height)) {

        if ((temp_s0->width >= 0xE) && (temp_s0->height >= 0xE)) {
            Ui_DrawGradientPanel(temp_s0->x, temp_s0->y, temp_s0->width, temp_s0->height, &sp40, &sp40);
        }

        if ((temp_s0->width >= arg0->targetRectB->width) && (temp_s0->height >= arg0->targetRectB->height)) {
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            Font_SetLineHeight(0x1A);

            switch (arg0->selectedPanelType) {
                case 1:
                    sp48 = Text_GetString(NULL, 0, gTradeStrings, 6);
                    Font_Printf((temp_s0->x + (temp_s0->width / 2)) - (Font_MeasureTextExtent(0, 0, sp48) / 2),
                                  temp_s0->y + 0x10, sp48);
                    break;

                case 2:
                    sp48 = Text_GetString(NULL, 0, gTradeStrings, 7);
                    Font_Printf((temp_s0->x + (temp_s0->width / 2)) - (Font_MeasureTextExtent(0, 0, sp48) / 2),
                                  temp_s0->y + 0x10, sp48);
                    break;

                case 3:
                    sp48 = Text_GetString(NULL, 0, gTradeStrings, 8);
                    Font_Printf((temp_s0->x + (temp_s0->width / 2)) - (Font_MeasureTextExtent(0, 0, sp48) / 2),
                                  temp_s0->y + 0x10, sp48);
                    break;

                case 4:
                    sp48 = Text_GetString(NULL, 0, gTradeStrings, 9);
                    Font_Printf((temp_s0->x + (temp_s0->width / 2)) - (Font_MeasureTextExtent(0, 0, sp48) / 2),
                                  temp_s0->y + 0x10, sp48);
                    break;
            }

            Font_EndTexturedTextRendering();
        }
    }
}

void Trade_DrawBoxMachineTransferArrows(s16 arg0, s16 arg1) {
    static s16 D_82F13F5C = 0;

    s16 sp86 = ROUND_MAX(2.0f * SINS(D_82F13F5C));

    gSPDisplayList(gDisplayListHead++, D_8006F518);
    gDPLoadTextureBlock(gDisplayListHead++, D_302AC68, G_IM_FMT_RGBA, G_IM_SIZ_16b, 48, 24, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0 + sp86, arg1, 0x30, 0x18, 0, 0, 0x400, 0x400, 0);
    Gfx_DrawTexturedRectClipped((arg0 - sp86) + 0x10, arg1 + 0x28, 0x30, 0x18, 0x600, 0, -0x400, 0x400, 0);

    gDPLoadTextureBlock(gDisplayListHead++, D_302B568, G_IM_FMT_RGBA, G_IM_SIZ_16b, 48, 24, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0 + sp86, arg1 + 0x18, 0x30, 0x18, 0, 0, 0x400, 0x400, 0);
    Gfx_DrawTexturedRectClipped((arg0 - sp86) + 0x10, arg1 + 0x40, 0x30, 0x18, 0x600, 0, -0x400, 0x400, 0);

    gSPDisplayList(gDisplayListHead++, D_8006F630);

    D_82F13F5C += 0x1000;
}

void Trade_DrawBoxMachineCursor(s16 arg0) {
    static s16 D_82F13F60 = 0;

    s16 sp1E;
    UNUSED s16 sp1C;
    s16 sp1A;
    s32 var_v1 = 0;

    switch (arg0) {
        case 2:
            if (D_82F210D2 != -1) {
                var_v1 = 1;
                sp1E = (D_82F210D2 * 0x88) + 0x78;
                sp1A = 0x154;
            }
            break;

        case 8:
            var_v1 = 1;
            sp1E = 0xE4;
            sp1A = (D_82F210D4 * 0x1C) + 0x12A;
            break;
    }

    if (var_v1 != 0) {
        s16 sins = SINS(D_82F13F60) * 3.0f;

        Ui_DrawTextureMarker(sp1E + sins, sp1A);
    }

    D_82F13F60 += 0x1000;
}

void Trade_InitBoxMachine(void) {
    UNUSED s32 pad;
    s32 i;
    MemoryBlock* sp1C = MainPool_AllocState(main_pool_get_available(), 0);

    D_82F210C8 = process_geo_layout(sp1C, D_82F13E8C);
    D_82F210CC = process_geo_layout(sp1C, D_82F13E58);
    MainPool_FinalizeAllocation(sp1C);

    D_82F20A40.flowState = 0;
    D_82F20A40.timer = 0;
    D_82F20A40.selectedPanelType = 0;

    for (i = 0; i < 2; i++) {
        D_82F210D8[i] = -1;
    }

    D_82F210D8[0] = GbSave_GetActivePort();
    D_82F210D2 = 0;
    Trade_SetupBoxSlots(D_82F210D8[0]);
}

void Trade_UpdateBoxMachine(void) {
    unk_D_82F20A40* ptr = &D_82F20A40;

    if (ptr->flowState == 0) {
        return;
    }

    switch (ptr->flowState) {
        case 1:
            Trade_UpdateBoxMachineFlowOpen(ptr, 1);
            break;

        case 10:
            Trade_UpdateBoxMachineFlowOpen(ptr, 0);
            break;

        case 2:
            Trade_UpdateBoxMachineFlowSelect(ptr);
            break;

        case 3:
            Trade_UpdateBoxMachineFlowFrameMove(ptr);
            break;

        case 4:
            Trade_UpdateBoxMachineFlowConfirmWait(ptr);
            break;

        case 5:
            Trade_UpdateBoxMachineFlowFrameReturn(ptr);
            break;

        case 6:
            Trade_UpdateBoxMachineFlowPanelsFull(ptr);
            break;

        case 7:
            Trade_UpdateBoxMachineFlowConfirmOpen(ptr);
            break;

        case 8:
            Trade_UpdateBoxMachineFlowConfirmInput(ptr);
            break;

        case 9:
            Trade_UpdateBoxMachineFlowConfirmDone(ptr);
            break;

        case 11:
            Trade_UpdateBoxMachineFlowClose(ptr);
            break;
    }

    Trade_UpdateBoxSlotTransforms();
}

void Trade_DrawBoxMachine(void) {
    unk_D_82F20A40* ptr = &D_82F20A40;

    if (ptr->flowState != 0) {
        GeoRender_AdvanceFrameCounter();
        Trade_DrawBoxMachineTopBar(&D_82F20A40);
        Trade_DrawBoxMachineSlots(&D_82F20A40);
        Trade_DrawBoxMachineTitle(&D_82F20A40);
        Trade_DrawBoxMachineConfirm(&D_82F20A40);
        Geo_RenderRootNode(D_82F210C8);
        Trade_DrawBoxMachineCursor(ptr->flowState);
        Trade_DrawBoxMachineBanner(&D_82F20A40);
    }
}
