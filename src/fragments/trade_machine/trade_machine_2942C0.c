#include "trade_machine.h"
#include "src/model_animation.h"
#include "src/display_object_textures.h"
#include "src/poke_icon.h"
#include "src/model_renderer.h"
#include "src/graphics_textures.h"
#include "src/gb_data.h"
#include "src/pokemon_stats.h"
#include "src/text_system.h"
#include "src/audio_sfx.h"
#include "src/gfx_rect.h"
#include "src/math_util.h"
#include "src/memory.h"
#include "src/stage_loader.h"

static unk_D_82F21140 D_82F21140;
static unk_D_80068BB0* D_82F2115C;
static unk_D_82F21160 D_82F21160[2];
static unk_D_82F211A0 D_82F211A0[2];
static unk_D_82F21238 D_82F21238;

void TradeCable_DrawScissoredTexRect(s16 arg0, s16 arg1, s16 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6, s16 arg7, s32 arg8) {
    gSPScisTextureRectangle(gDisplayListHead++, arg0 << 2, arg1 << 2, (arg0 + arg2) << 2, (arg1 + arg3) << 2, 0, arg4,
                            arg5, arg6, arg7);
}

void TradeCable_ResetBalls(void) {
    unk_D_82F211A0* var_v1;
    s32 i;

    for (i = 0, var_v1 = &D_82F211A0[0]; i < 2; i++, var_v1++) {
        var_v1->state = 0;
    }
}

void TradeCable_SetBallState(s32 arg0, s16 arg1) {
    s32 i;
    unk_D_82F211A0* temp_v0 = &D_82F211A0[arg0];

    temp_v0->state = arg1;

    switch (arg1) {
        case 1:
            temp_v0->screenX = (arg0 * 0x5A) + 0xFA;
            temp_v0->screenY = 0x5A;
            temp_v0->spawnTimer = 0x28;
            temp_v0->alpha = 0;
            temp_v0->ghostCount = 0;

            for (i = 0; i < 10; i++) {
                temp_v0->trailGhosts[i].x = temp_v0->trailGhosts[i].y = 0;
                temp_v0->trailGhosts[i].timer = 0;
            }
            break;

        case 2:
            temp_v0->spawnTimer = 0x1E;
            temp_v0->spinAngle = 0;
            temp_v0->alpha = 0xFF;
            break;

        case 3:
            temp_v0->spawnTimer = 0;
            temp_v0->spinAngle = 0;
            temp_v0->velocityX = 0;
            temp_v0->alpha = 0xFF;
            break;
    }
}

void TradeCable_UpdateBallFadeIn(unk_D_82F211A0* arg0) {
    arg0->spawnTimer--;
    arg0->alpha = 0xFF - ((arg0->spawnTimer * 0xFF) / 40);
    if (arg0->spawnTimer <= 0) {
        TradeCable_SetBallState(arg0 - D_82F211A0, 2);
    }
}

void TradeCable_UpdateBallSpinPulse(unk_D_82F211A0* arg0) {
    s32 sp1C;

    if (arg0->spawnTimer > 0) {
        arg0->spawnTimer--;
    }

    arg0->spinAngle += 0x200;

    arg0->alpha = ROUND_MAX(((COSS(arg0->spinAngle) * 0.5f) + 0.5f) * 255.0f);

    if ((arg0->spawnTimer <= 0) && (arg0->alpha >= 0xFF)) {
        sp1C = arg0 - D_82F211A0;
        TradeCable_SetBallState(sp1C, 3);
        if (sp1C == 0) {
            Audio_PlaySoundEffectById(0x01200006);
        }
    }
}

void TradeCable_SpawnBallTrailGhost(unk_D_82F211A0* arg0) {
    s32 i;
    unk_D_82F211A0_010* var_v0 = NULL;

    for (i = 0; i < 10; i++) {
        if (arg0->trailGhosts[i].timer == 0) {
            break;
        }
    }

    if (i < 10) {
        var_v0 = &arg0->trailGhosts[i];
    }

    if (var_v0 != NULL) {
        var_v0->x = arg0->screenX;
        var_v0->y = arg0->screenY;
        var_v0->timer = 0x14;
        arg0->ghostCount++;
    }
}

void TradeCable_UpdateBallTrailGhosts(unk_D_82F211A0* arg0) {
    s32 i;
    unk_D_82F211A0_010* ptr;

    for (i = 0, ptr = &arg0->trailGhosts[0]; i < 10; i++, ptr++) {
        if (ptr->timer != 0) {
            ptr->timer--;
            if (ptr->timer <= 0) {
                arg0->ghostCount--;
            }
        }
    }
}

void TradeCable_UpdateBallTravel(unk_D_82F211A0* arg0) {
    s32 idx = arg0 - D_82F211A0;

    if (arg0->ghostCount > 0) {
        TradeCable_UpdateBallTrailGhosts(arg0);
    }

    if ((arg0->screenX < 0xA0) || (arg0->screenX >= 0x1C3)) {
        if (arg0->ghostCount == 0) {
            arg0->state = 0;
            TradeCable_SetLightState(idx ^ 1, 6);
        }
    } else {
        if (arg0->spawnTimer > 0) {
            arg0->spawnTimer--;
        }

        if ((arg0->spawnTimer == 0) && (arg0->ghostCount < 0xA)) {
            TradeCable_SpawnBallTrailGhost(arg0);
            arg0->spawnTimer = 2;
        }

        if (idx != 0) {
            arg0->velocityX--;
        } else {
            arg0->velocityX++;
        }

        arg0->screenX += arg0->velocityX;
    }
}

void TradeCable_UpdateBalls(void) {
    s32 i;
    unk_D_82F211A0* var_s0 = &D_82F211A0[0];

    for (i = 0; i < 2; i++, var_s0++) {
        switch (var_s0->state) {
            case 1:
                TradeCable_UpdateBallFadeIn(var_s0);
                break;

            case 2:
                TradeCable_UpdateBallSpinPulse(var_s0);
                break;

            case 3:
                TradeCable_UpdateBallTravel(var_s0);
                break;
        }
    }
}

void TradeCable_DrawBalls(void) {
    s32 i;
    s32 j;
    s32 sp84 = 0;
    s16 var_s0;
    u16* temp_s0;
    unk_D_82F211A0* var_s4;
    s16 v;
    s16 tmp;

    for (i = 0, var_s4 = &D_82F211A0[0]; i < 2; i++, var_s4++) {
        if (var_s4->state != 0) {
            temp_s0 = Trade_GetPickBuffer(i);

            if (sp84 == 0) {
                sp84 = 1;
                Gfx_SetScissorRect(&gDisplayListHead, 0xEC, 0x50, 0xAA, 0x50);
            }

            gDPPipeSync(gDisplayListHead++);

            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, var_s4->alpha);
            gDPLoadTextureBlock(gDisplayListHead++, temp_s0, G_IM_FMT_RGBA, G_IM_SIZ_16b, 40, 40, 0,
                                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);

            v = 60;

            if (var_s4->state == 1) {
                for (j = 0; j < v; j++) {
                    var_s0 = ((MathUtil_Random16() % 33) * var_s4->spawnTimer) / 40;
                    if (MathUtil_Random16() & 0x8000) {
                        var_s0 = -var_s0;
                    }
                    Gfx_DrawTexturedRectClipped(var_s4->screenX + var_s0, var_s4->screenY + j, 0x3C, 1, 0,
                                  ROUND_MAX((j * 32.0f) / 1.5f), 0x2AB, 0x2AB, 0);
                }
            } else {
                Gfx_DrawTexturedRectClipped(var_s4->screenX, var_s4->screenY, 0x3C, 0x3C, 0, 0, 0x2AB, 0x2AB, 0);
            }

            if (var_s4->ghostCount == 0) {
            } else {
                for (j = 0; j < 10; j++) {
                    if (var_s4->trailGhosts[j].timer == 0) {
                        continue;
                    }

                    tmp = (var_s4->trailGhosts[j].timer * 0xFF) / 20;

                    gDPPipeSync(gDisplayListHead++);
                    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, tmp);

                    Gfx_DrawTexturedRectClipped(var_s4->trailGhosts[j].x, var_s4->trailGhosts[j].y, 0x3C, 0x3C, 0, 0, 0x2AB, 0x2AB,
                                  0);
                }
            }
        }
    }

    if (sp84 != 0) {
        Gfx_SetScissorRect(&gDisplayListHead, 0, 0, 0x280, 0x1E0);
    }
}

void TradeEvo_ResetState(void) {
    D_82F21238.phase = 0;
}

void TradeEvo_CreatePreviewModel(unk_D_82F21238* arg0, s32 arg1) {
    arg0->framebuffers = PokeIcon_AllocFramebuffers(1);
    arg0->modelPreview = PokeIcon_CreateModelPreview(arg0->framebuffers, 0xC0, 0xC0, 0, 0, 0xC0, 0xC0, NULL);
    arg0->modelPreview->flags &= ~2;
    PokeIcon_SetPreviewMon(arg0->modelPreview, arg0->mon, 1);

    if (arg1 != 0) {
        arg0->animState = 2;
        arg0->modelPreview->flags &= ~4;
    } else {
        arg0->animState = 1;
        arg0->modelPreview->flags |= 4;
    }
    ModelAnim_SetAnimation(arg0->modelPreview->displayObject, arg0->animState);
}

void TradeEvo_BeginSequence(s16 arg0) {
    unk_func_82F00838_sp30 sp20;
    unk_D_82F21238* ptr = &D_82F21238;

    main_pool_push_state('intr');
    Trade_GetPickScreenSnapshot(arg0, &sp20);
    D_82F21238.partySlot = arg0;
    D_82F21238.phase = 1;
    D_82F21238.gbPort = sp20.unk_02;
    D_82F21238.animState = 1;
    D_82F21238.flashTriggered = 0;
    ptr->coinFlip = MathUtil_Random16() % 2;
    ptr->timer = 0;
    ptr->rectX1 = 0x140;
    ptr->rectY1 = 0xC4;
    ptr->rectX2 = 0;
    ptr->rectY2 = 0;
    ptr->alpha = 0xFF;
    ptr->renderResult = 0;
    ptr->mon = &sp20.unk_0C[sp20.unk_06];
    TradeEvo_CreatePreviewModel(&D_82F21238, 0);
    ptr->captionBox.x = ptr->captionBox.y = 0;
    ptr->captionBox.width = ptr->captionBox.height = -1;

    Audio_PlaySoundEffectById(4);
    GbSave_SetSeenOwnedBits(ptr->gbPort, ptr->mon->species.dexId, 3);
}

void TradeEvo_UpdatePreviewModel(unk_D_82F21238* arg0) {
    unk_D_86002F58_004_000* sp24 = arg0->modelPreview->displayObject;

    if ((ModelAnim_IsFinished(sp24) != 0) && (arg0->animState == 2)) {
        arg0->animState = 1;
        arg0->modelPreview->flags |= 4;
        ModelAnim_SetAnimation(sp24, arg0->animState);
    }
    arg0->renderResult = PokeIcon_RenderPreview(arg0->modelPreview);
}

void TradeEvo_UpdateOpenPreview(unk_D_82F21238* arg0) {
    if (arg0->rectX2 < 0xC0) {
        arg0->rectX2 += 0x20;
        if (arg0->rectX2 >= 0xC1) {
            arg0->rectX2 = 0xC0;
        }
    } else {
        arg0->rectY2 += 0x20;
        if (arg0->rectY2 >= 0xC1) {
            arg0->rectY2 = 0xC0;
        }
    }

    arg0->rectX1 = ((arg0->rectX2 * -0x60) / 192) + 0x140;
    arg0->rectY1 = ((arg0->rectY2 * -0x60) / 192) + 0xC4;

    TradeEvo_UpdatePreviewModel(arg0);

    if ((arg0->rectX2 == 0xC0) && (arg0->rectY2 == 0xC0)) {
        arg0->phase = 2;
        arg0->timer = 0;
        arg0->rectX1 = 0xE0;
        arg0->rectY1 = 0x64;
    }
}

s32 Trade_SpeciesEvolvesOnTransfer(s32 arg0) {
    s32 var_v1 = 0;

    if ((arg0 == 0x40) || (arg0 == 0x43) || (arg0 == 0x4B) || (arg0 == 0x5D)) {
        var_v1 = 1;
    }
    return var_v1;
}

void TradeEvo_UpdateWaitForConfirm(unk_D_82F21238* arg0) {
    TradeEvo_UpdatePreviewModel(arg0);

    if (arg0->flashTriggered != 0) {
        arg0->timer -= 1;
        if (arg0->timer <= 0) {
            arg0->timer = 0;
        } else {
            return;
        }
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        if (Trade_SpeciesEvolvesOnTransfer(arg0->mon->species.dexId) != 0) {
            arg0->phase = 3;
            arg0->flashTriggered = 1;
            arg0->timer = 0x3C;
            arg0->flashAngle = 0;
            arg0->flashRadius = 0x800;
            arg0->flashHeight = 0;
            Audio_PlaySoundEffectById(2);
            Audio_PlaySoundEffectById(0x01200007);
        } else {
            arg0->phase = 6;
            arg0->timer = 0xA;
            if (arg0->flashTriggered != 0) {
                Audio_PlaySoundEffectById(0x01200009);
            } else {
                Audio_PlaySoundEffectById(2);
            }
        }
    }
}

void TradeEvo_UpdateFlashIn(unk_D_82F21238* arg0) {
    arg0->timer--;
    TradeEvo_UpdatePreviewModel(arg0);
    arg0->alpha = (arg0->timer * 0xFF) / 60;

    switch (arg0->coinFlip) {
        case 1:
            break;

        case 0:
            arg0->flashHeight = ((0x3C - arg0->timer) * 0xF0) / 60;
            arg0->flashRadius = 0x1000 - ((arg0->timer << 0xB) / 60);
            arg0->flashAngle += 0x1000;
            break;
    }

    if (arg0->timer <= 0) {
        arg0->phase = 4;
        arg0->timer = 1;
        arg0->alpha = 0;
        arg0->renderResult = 0;
    }
}

s32 Trade_NicknameIsSpeciesDefault(s16 arg0, s8* arg1) {
    s32 sp64;
    s8* var_v1;
    char sp20[64];

    sp64 = 1;
    Text_CopyString(sp20, Trade_GetSpeciesName(arg0));

    var_v1 = sp20;
    while (*var_v1 != '\x00') {
        if (*var_v1 != *arg1) {
            sp64 = 0;
            break;
        }
        var_v1++;
        arg1++;
    }

    return sp64;
}

void Trade_ApplyEvolution(s16 arg0, BattleMon* arg1) {
    s16 sp2E = 0;

    if (Trade_SpeciesEvolvesOnTransfer(arg1->species.dexId) != 0) {
        sp2E = arg1->species.dexId;
        arg1->species.dexId++;
    }

    if (sp2E != 0) {
        if (Trade_NicknameIsSpeciesDefault(sp2E, arg1->nickname) != 0) {
            Text_CopyString(arg1->nickname, Trade_GetSpeciesName(arg1->species.dexId));
        }
        Pokemon_RecalcStats(arg1);
        GbSave_SetSeenOwnedBits(arg0, arg1->species.dexId, 3);
    }
}

void TradeEvo_UpdateApplyEvolution(unk_D_82F21238* arg0) {
    arg0->timer--;
    if (arg0->timer <= 0) {
        main_pool_pop_state('intr');
        main_pool_push_state('intr');

        arg0->phase = 5;
        arg0->timer = 0x3C;
        arg0->alpha = 0;
        if (arg0->coinFlip == 0) {
            arg0->flashHeight = 0xF0;
        }
        arg0->renderResult = 0;
        Trade_ApplyEvolution(arg0->gbPort, arg0->mon);
        Trade_LoadPickBufferIcon(arg0->partySlot, arg0->mon);
        TradeEvo_CreatePreviewModel(arg0, 1);
        arg0->captionBox.x = 0x38;
        arg0->captionBox.y = 0x168;
        arg0->captionBox.width = 0x210;
        arg0->captionBox.height = 0;
        Audio_PlaySoundEffectById(0x01200008);
    }
}

void TradeEvo_UpdateShowCaption(unk_D_82F21238* arg0) {
    s16 var_v1;
    TradeRect* ptr = &arg0->captionBox;

    TradeEvo_UpdatePreviewModel(arg0);
    arg0->timer--;
    arg0->alpha = ((0x3C - arg0->timer) * 0xFF) / 60;

    switch (arg0->coinFlip) {
        case 1:
            break;

        case 0:
            arg0->flashHeight = (arg0->timer * 0xF0) / 60;
            arg0->flashRadius = 0x800 - ((arg0->timer * -0x800) / 60);
            arg0->flashAngle += 0x1000;
            break;
    }

    var_v1 = arg0->timer;
    if (var_v1 >= 0xB) {
        var_v1 = 0xA;
    }

    ptr->x = 0x38 - (var_v1 * 0) / 10;
    ptr->y = 0x154 - ((var_v1 * -0x14) / 10);
    ptr->width = 0x210 - (var_v1 * 0) / 10;
    ptr->height = 0x28 - ((var_v1 * 0x28) / 10);

    if (arg0->timer <= 0) {
        arg0->phase = 2;
        arg0->timer = 0x5A;
        arg0->alpha = 0xFF;
    }
}

void TradeEvo_UpdateClosePreview(unk_D_82F21238* arg0) {
    TradeRect* temp_v0_2 = &arg0->captionBox;

    if (arg0->timer > 0) {
        arg0->timer--;
    }

    if (arg0->captionBox.width != -1) {
        temp_v0_2->x = 0x38 - ((arg0->timer * 0) / 10);
        temp_v0_2->y = 0x168 - ((arg0->timer * 0x14) / 10);
        temp_v0_2->width = 0x210 - ((arg0->timer * 0) / 10);
        temp_v0_2->height = -((arg0->timer * -0x28) / 10);
    }

    if (arg0->rectY2 > 0) {
        arg0->rectY2 -= 0x20;
        if (arg0->rectY2 < 0) {
            arg0->rectY2 = 0;
        }
    } else {
        arg0->rectX2 -= 0x20;
        if (arg0->rectX2 < 0) {
            arg0->rectX2 = 0;
        }
    }

    arg0->rectX1 = ((arg0->rectX2 * -0x60) / 192) + 0x140;
    arg0->rectY1 = ((arg0->rectY2 * -0x60) / 192) + 0xC4;
    TradeEvo_UpdatePreviewModel(arg0);

    if ((arg0->rectX2 == 0) && (arg0->rectY2 == 0)) {
        if (arg0->timer <= 0) {
            arg0->phase = 0;
            arg0->timer = 0;
            arg0->renderResult = 0;

            temp_v0_2->x = temp_v0_2->y = 0;
            temp_v0_2->width = temp_v0_2->height = -1;

            main_pool_pop_state('intr');
        }
    }
}

void TradeEvo_Update(void) {
    switch (D_82F21238.phase) {
        case 0:
            break;

        case 1:
            TradeEvo_UpdateOpenPreview(&D_82F21238);
            break;

        case 2:
            TradeEvo_UpdateWaitForConfirm(&D_82F21238);
            break;

        case 3:
            TradeEvo_UpdateFlashIn(&D_82F21238);
            break;

        case 4:
            TradeEvo_UpdateApplyEvolution(&D_82F21238);
            break;

        case 5:
            TradeEvo_UpdateShowCaption(&D_82F21238);
            break;

        case 6:
            TradeEvo_UpdateClosePreview(&D_82F21238);
            break;
    }
}

void TradeEvo_DrawGlowBackdrop(s16 arg0, s16 arg1, s16 arg2) {
    static Gfx D_82F13F90[] = {
        gsDPPipeSync(),
        gsDPSetCycleType(G_CYC_2CYCLE),
        gsDPSetRenderMode(G_RM_PASS, G_RM_XLU_SURF2),
        gsDPSetCombineLERP(TEXEL0, TEXEL1, PRIM_LOD_FRAC, TEXEL1, TEXEL0, TEXEL1, PRIM_LOD_FRAC, TEXEL1, PRIMITIVE,
                           ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, ENVIRONMENT),
        gsDPSetPrimColor(0, 0x80, 0, 0, 255, 0),
        gsSPEndDisplayList(),
    };
    static f32 D_82F13FC0 = 0.0f;
    static f32 D_82F13FC4 = 0.0f;
    static f32 D_82F13FC8 = 0.0f;
    static f32 D_82F13FCC = 0.0f;

    UNUSED s32 pad[2];
    f32 temp_fa0;
    f32 temp_fa4;
    f32 temp_fa8;
    f32 temp_faC;

    gSPDisplayList(gDisplayListHead++, D_82F13F90);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 50, (arg2 << 7) / 255);

    gDPLoadMultiBlock_4b(gDisplayListHead++, D_3026368, 0x0100, 1, G_IM_FMT_I, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                         G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 1, G_TX_NOLOD);
    temp_fa0 = D_82F13FC0 + 31.0f;
    temp_fa4 = D_82F13FC4 + 31.0f;
    gDPSetTileSize(gDisplayListHead++, 1, D_82F13FC0 * 4.0f, D_82F13FC4 * 4.0f, temp_fa0 * 4.0f, temp_fa4 * 4.0f);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_3026168, G_IM_FMT_I, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, 1);
    temp_fa8 = D_82F13FC8 + 31.0f;
    temp_faC = D_82F13FCC + 31.0f;
    gDPSetTileSize(gDisplayListHead++, G_TX_RENDERTILE, D_82F13FC8 * 4.0f, D_82F13FCC * 4.0f, temp_fa8 * 4.0f,
                   temp_faC * 4.0f);

    TradeCable_DrawScissoredTexRect(arg0, arg1, 0xC0, 0xC0, 0, 0, 0xAB, 0xAB, 0x100000);

    D_82F13FC0 -= 0.25f;
    if (D_82F13FC0 < 0.0f) {
        D_82F13FC0 += 32.0f;
    }

    D_82F13FC4 -= 0.25f;
    if (D_82F13FC4 < 0.0f) {
        D_82F13FC4 += 32.0f;
    }

    D_82F13FC8 += 0.25f;
    if (D_82F13FC8 >= 32.0f) {
        D_82F13FC8 -= 32.0f;
    }

    D_82F13FCC -= 0.5f;
    if (D_82F13FCC < 0.0f) {
        D_82F13FCC += 32.0f;
    }
}

void TradeEvo_DrawPreviewFrame(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    s16 tmp = (arg1 + (arg3 / 2)) - 8;

    gSPDisplayList(gDisplayListHead++, D_8006F518);

    Gfx_DrawTextureRgba16(arg0 - 0xD, arg1 - 0xD, 0x10, 0x10, D_3026568, 0x10, 0);
    Gfx_DrawTexturedRectClipped(arg0 - 0xD, arg1 + 3, 0x10, (arg3 / 2) - 3, 0, 0x1E0, 0x400, 0, 0);
    Gfx_DrawTextureRgba16((arg0 + arg2) - 3, arg1 - 0xD, 0x10, 0x10, D_3026768, 0x10, 0);
    Gfx_DrawTexturedRectClipped(arg0 + 3, arg1 - 0xD, arg2 - 6, 0x10, 0, 0, 0, 0x400, 0);
    Gfx_DrawTexturedRectClipped((arg0 + arg2) - 3, arg1 + 3, 0x10, (arg3 / 2) - 3, 0, 0x1E0, 0x400, 0, 0);
    Gfx_DrawTextureRgba16(arg0 - 0xD, (arg1 + arg3) - 3, 0x10, 0x10, D_3026D68, 0x10, 0);

    Gfx_DrawTexturedRectClipped(arg0 - 0xD, tmp + 8, 0x10, (arg3 / 2) - 3, 0, 0, 0x400, 0, 0);
    Gfx_DrawTextureRgba16((arg0 + arg2) - 3, (arg1 + arg3) - 3, 0x10, 0x10, D_3026F68, 0x10, 0);
    Gfx_DrawTexturedRectClipped(arg0 + 3, (arg1 + arg3) - 3, arg2 - 6, 0x10, 0, 0, 0, 0x400, 0);
    Gfx_DrawTexturedRectClipped((arg0 + arg2) - 3, tmp + 8, 0x10, (arg3 / 2) - 3, 0, 0, 0x400, 0, 0);

    if (arg3 >= 0x10) {
        Gfx_DrawTextureRgba16(arg0 - 0xD, tmp, 0x10, 0x10, D_3026968, 0x10, 0);
        Gfx_DrawTextureRgba16((arg0 + arg2) - 3, tmp, 0x10, 0x10, D_3026B68, 0x10, 0);
    }

    gSPDisplayList(gDisplayListHead++, D_8006F630);
}

void TradeEvo_DrawPreviewStrips(unk_D_82F21238* arg0) {
    UNUSED s32 pad;
    s16 i;
    s32 sp5C;
    s16 var_a0;
    s16 var_s2;
    s16 var_s3;

    sp5C = 0;
    if (arg0->coinFlip == 1) {
        sp5C = 1;
    }

    gSPDisplayList(gDisplayListHead++, D_8006F518);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, arg0->alpha);

    if (sp5C != 0) {
        gDPSetAlphaCompare(gDisplayListHead++, G_AC_DITHER);
    }

    var_s3 = 8;
    if ((arg0->phase == 3) || (arg0->phase == 5)) {
        var_s3 = 1;
    }

    for (i = 0; i < 0xC0; i += var_s3) {
        var_s2 = CLAMP_MIN(0xC0 - i, var_s3);

        gDPLoadTextureTile(gDisplayListHead++, arg0->renderResult + (i * 0x180), G_IM_FMT_RGBA, G_IM_SIZ_16b, 192, 0, 0, 0,
                           191, var_s2 - 1, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                           G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        if (var_s3 != 1) {
            var_a0 = 0xE0;
        } else {
            var_a0 = (s16)(SINS(arg0->flashAngle + (arg0->flashRadius * i)) * arg0->flashHeight) + 0xE0;
        }
        Gfx_DrawTexturedRectClipped(var_a0, i + 0x64, 0xC0, var_s2, 0, 0, 0x400, 0x400, 0);
    }

    if (sp5C != 0) {
        gDPPipeSync(gDisplayListHead++);
        gDPSetAlphaCompare(gDisplayListHead++, G_AC_NONE);
    }
}

void func_82F0ECA4(void) {
}

void TradeEvo_DrawEvolutionCaption(TradeRect* arg0, BattleMon* arg1) {
    s16 spC6;
    s8 sp44[0x80];
    char* sp40;
    char* sp3C;
    char* sp38;
    s16 tmp1;

    sp44[0] = '\x00';
    sp3C = NULL;
    sp40 = NULL;

    if (Trade_SpeciesEvolvesOnTransfer(arg1->species.dexId - 1) != 0) {
        sp40 = Trade_GetSpeciesName(arg1->species.dexId - 1);
        sp3C = Trade_GetSpeciesName(arg1->species.dexId);
    }

    if (sp40 != NULL) {
        Text_SetStringToken(0x21, sp40);
        Text_SetStringToken(0x22, sp3C);
        sp38 = Text_GetString(sp44, 0x80, gTradeStrings, 0x19);
    }

    if ((sp44[0] != '\x00') && ((arg0->width != 0x210) || (arg0->height != 0))) {
        Trade_DrawRoundedFrameLarge(arg0->x, arg0->y, arg0->width, arg0->height, 0x1E, 0x1E, 0x82, 0xFF);
        if ((arg0->width == 0x210) && (arg0->height == 0x28)) {
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            spC6 = arg0->x + (arg0->width / 2);
            tmp1 = Font_MeasureTextExtent(0, 0, sp38) / 2;
            Font_Printf(spC6 - tmp1, arg0->y + 8, sp38);
            Font_EndTexturedTextRendering();
        }
    }
}

void TradeEvo_Draw(void) {
    unk_D_82F21238* ptr = &D_82F21238;
    UNUSED s32 pad;
    s32 sp24;

    if (ptr->phase != 0) {
        sp24 = 0;
        if (ptr->rectX2 > 0) {
            if (ptr->rectY2 > 0) {
                sp24 = 1;
                Gfx_SetScissorRect(&gDisplayListHead, ptr->rectX1, ptr->rectY1, ptr->rectX2, ptr->rectY2);
                if (ptr->renderResult != 0) {
                    TradeEvo_DrawPreviewStrips(&D_82F21238);
                }
                TradeEvo_DrawGlowBackdrop(0xE0, 0x64, ptr->alpha);
            }
        }

        if (sp24 != 0) {
            Gfx_SetScissorRect(&gDisplayListHead, 0, 0, 0x280, 0x1E0);
        }

        TradeEvo_DrawPreviewFrame(ptr->rectX1, ptr->rectY1, ptr->rectX2, ptr->rectY2);

        if (ptr->captionBox.width != -1) {
            TradeEvo_DrawEvolutionCaption(&ptr->captionBox, ptr->mon);
        }
    }
}

void TradeCable_ResetLights(void) {
    s32 i;
    unk_D_82F21160* var_v1;

    for (i = 0, var_v1 = &D_82F21160[0]; i < 2; i++, var_v1++) {
        var_v1->state = 0;
        var_v1->baseOffset = 0;
        var_v1->screenX = 0;
        var_v1->unk_06 = 0;
        var_v1->unk_08 = 0;
        var_v1->unk_0A = 0;
        var_v1->unk_0C = 0;
        var_v1->unk_0E = 0;
        var_v1->unk_10 = 0;
        var_v1->animPhase = 0;
        var_v1->opacity = 0.0f;
        var_v1->unk_18 = 0.0f;
        var_v1->unk_1C = 0.0f;
    }
}

void TradeCable_SetLightState(s32 arg0, s16 arg1) {
    static s16 D_82F13FD0[] = { 0x64, 0x2C8 };
    static s16 D_82F13FD4[] = { 0x50, 0x236, 0, 0, 0x3F80, 0 };

    unk_D_82F21160* temp_v0 = &D_82F21160[arg0];

    temp_v0->state = arg1;

    switch (arg1) {
        case 1:
            temp_v0->baseOffset = D_82F13FD0[arg0];
            temp_v0->screenX = D_82F21140.scrollOffset + temp_v0->baseOffset;
            temp_v0->unk_06 = 0xFA;
            temp_v0->unk_08 = temp_v0->screenX - 0x40;
            temp_v0->unk_0A = 0xAE;
            temp_v0->unk_0C = 0x80;
            temp_v0->unk_0E = 0x48;
            temp_v0->unk_10 = 0;
            temp_v0->animPhase = 0;
            temp_v0->opacity = 1.0f;
            temp_v0->unk_18 = 0.0f;
            temp_v0->unk_1C = 9.0f;
            break;

        case 2:
            temp_v0->unk_08 = 0;
            temp_v0->unk_0A = 0;
            temp_v0->unk_0C = 0;
            temp_v0->unk_0E = 0;
            temp_v0->unk_06 = 0xBE;
            break;

        case 3:
            temp_v0->unk_08 = 0;
            temp_v0->unk_0A = 0;
            temp_v0->unk_0C = 0;
            temp_v0->unk_0E = 0;
            temp_v0->unk_10 = D_82F21140.transitionTimer;
            break;

        case 4:
            if (arg0 == 0) {
                temp_v0->baseOffset = 0x50;
            } else {
                temp_v0->baseOffset = 0x236;
            }
            temp_v0->screenX = D_82F21140.scrollOffset + temp_v0->baseOffset;
            temp_v0->opacity = 0.75f;
            break;

        case 5:
            temp_v0->animPhase = 0;
            temp_v0->unk_08 = temp_v0->screenX - 0x40;
            if (temp_v0->unk_08 < 0) {
                temp_v0->unk_08 = 0;
            }
            temp_v0->unk_0A = 0xC2;
            temp_v0->unk_0C = 0x80;
            temp_v0->unk_0E = 0x3C;
            break;

        case 6:
            temp_v0->baseOffset = D_82F13FD4[arg0];
            temp_v0->screenX = D_82F21140.scrollOffset + temp_v0->baseOffset;
            temp_v0->unk_06 = 0xA0;
            temp_v0->unk_08 = temp_v0->screenX - 0x40;
            if (temp_v0->unk_08 < 0) {
                temp_v0->unk_08 = 0;
            }
            temp_v0->unk_0A = 0xC2;
            temp_v0->unk_0C = 0x80;
            temp_v0->unk_0E = 0x3C;
            temp_v0->unk_10 = 0;
            temp_v0->animPhase = 0;
            temp_v0->opacity = 0.75f;
            temp_v0->unk_18 = 0.0f;
            temp_v0->unk_1C = 1.0f;
            break;

        case 8:
            temp_v0->baseOffset = D_82F13FD4[arg0];
            temp_v0->screenX = D_82F21140.scrollOffset + temp_v0->baseOffset;
            temp_v0->unk_06 = 0xA0;
            temp_v0->unk_08 = temp_v0->screenX - 0x40;
            if (temp_v0->unk_08 < 0) {
                temp_v0->unk_08 = 0;
            }
            temp_v0->unk_0A = 0xC2;
            temp_v0->unk_0C = 0x80;
            temp_v0->unk_0E = 0x3C;
            temp_v0->unk_10 = 0;
            temp_v0->animPhase = 0;
            temp_v0->opacity = 0.75f;
            temp_v0->unk_18 = 0.0f;
            temp_v0->unk_1C = 1.0f;
            break;
    }
}

void TradeCable_UpdateLightShrinkToIdle(unk_D_82F21160* arg0) {
    arg0->unk_06 -= 2;
    if (arg0->unk_06 < 0xBF) {
        TradeCable_SetLightState(arg0 - D_82F21160, 2);
    }
}

void TradeCable_UpdateLightIdlePulse(unk_D_82F21160* arg0) {
    arg0->screenX = D_82F21140.scrollOffset + arg0->baseOffset;
    arg0->unk_06 = (s16)ROUND_MAX(SINS(arg0->animPhase) * 3.0f) + 0xBE;

    arg0->animPhase += 0x800;

    arg0->unk_1C -= 0.2f;
    if (arg0->unk_1C < 1.0f) {
        arg0->unk_1C = 1.0f;
    }
}

void TradeCable_UpdateLightSlideIn(unk_D_82F21160* arg0) {
    f32 one = 1.0f;
    s16 var_v0;
    s16 tmp1;
    s32 temp_a1;
    s32 temp_a3;
    f32 tmpf1;

    var_v0 = D_82F21140.transitionTimer;
    if (arg0->unk_10 < var_v0) {
        var_v0 = 0;
    }
    temp_a1 = (var_v0 * -0x14);

    temp_a3 = arg0 - D_82F21160;
    if (temp_a3 == 0) {
        arg0->baseOffset = ((var_v0 * 0x14) / arg0->unk_10) + 0x50;
    } else {
        arg0->baseOffset = ((var_v0 * 0x92) / arg0->unk_10) + 0x236;
    }

    arg0->screenX = D_82F21140.scrollOffset + arg0->baseOffset;

    tmp1 = (temp_a1 / arg0->unk_10) + 0xD2;
    tmpf1 = (((var_v0 * one) / arg0->unk_10) + 2.0f);

    arg0->unk_06 = (s16)ROUND_MAX(SINS(arg0->animPhase) * tmpf1) + tmp1;

    arg0->animPhase += 0x800;
    arg0->opacity = ((var_v0 * 0.25f) / arg0->unk_10) + 0.75f;

    if (temp_a1 >= -0x13) {
        TradeCable_SetLightState(temp_a3, 4);
    }
}

void TradeCable_UpdateLightWaitPulse(unk_D_82F21160* arg0) {
    arg0->screenX = D_82F21140.scrollOffset + arg0->baseOffset;
    arg0->unk_06 = (s16)ROUND_MAX(2.0f * SINS(arg0->animPhase)) + 0xD2;
    arg0->animPhase += 0x800;
}

void TradeCable_UpdateLightShrinkOut(unk_D_82F21160* arg0) {
    arg0->screenX = D_82F21140.scrollOffset + arg0->baseOffset;
    arg0->animPhase--;
    arg0->unk_06 += arg0->animPhase;
    if (arg0->unk_06 < 0x79) {
        arg0->state = 0;
        arg0->unk_06 = 0x78;
    }
}

void TradeCable_UpdateLightGrowFlash(unk_D_82F21160* arg0) {
    s16 idx;

    arg0->animPhase++;
    arg0->unk_06 += arg0->animPhase;
    if (arg0->unk_06 >= 0xD2) {
        arg0->state = 7;
        arg0->unk_06 = 0xD2;
        arg0->unk_18 = MathUtil_Random16() % 16;
        arg0->animPhase = MathUtil_Random16();
        idx = arg0 - D_82F21160;
        if (idx == 0) {
            D_82F21140.flags &= ~0x40;
        }
    }
}

void TradeCable_UpdateLightFlashPulse(unk_D_82F21160* arg0) {
    arg0->screenX = D_82F21140.scrollOffset + arg0->baseOffset;
    arg0->unk_06 = (s16)ROUND_MAX(2.0f * SINS(arg0->animPhase)) + 0xD2;
    arg0->animPhase += 0x800;
}

void TradeCable_UpdateLightFadeInThenPulse(unk_D_82F21160* arg0) {
    if (arg0->unk_1C < 9.0f) {
        arg0->screenX = D_82F21140.scrollOffset + arg0->baseOffset;
        arg0->unk_06 = (s16)ROUND_MAX(2.0f * SINS(arg0->animPhase)) + 0xD2;
        arg0->animPhase += 0x800;

        arg0->unk_1C += 0.2f;
        if (arg0->unk_1C >= 9.0f) {
            arg0->unk_1C = 9.0f;
        }
    } else {
        arg0->unk_1C = 9.0f;
        arg0->unk_06 += 2;
        if (arg0->unk_06 >= 0x104) {
            arg0->state = 0;
            arg0->unk_06 = 0;
        }
    }
}

void TradeCable_UpdateLights(void) {
    s32 var_s1;
    unk_D_82F21160* var_s0;
    s32 i;

    var_s0 = D_82F21160;

    for (i = 0; i < 2; i++, var_s0++) {
        var_s1 = 1;

        switch (var_s0->state) {
            case 0:
                var_s1 = 0;
                break;

            case 1:
                TradeCable_UpdateLightShrinkToIdle(var_s0);
                break;

            case 2:
                TradeCable_UpdateLightIdlePulse(var_s0);
                break;

            case 3:
                TradeCable_UpdateLightSlideIn(var_s0);
                break;

            case 4:
                TradeCable_UpdateLightWaitPulse(var_s0);
                break;

            case 5:
                TradeCable_UpdateLightShrinkOut(var_s0);
                break;

            case 6:
                TradeCable_UpdateLightGrowFlash(var_s0);
                break;

            case 7:
                TradeCable_UpdateLightFlashPulse(var_s0);
                break;

            case 8:
                TradeCable_UpdateLightFadeInThenPulse(var_s0);
                break;
        }

        if (var_s1 != 0) {
            s32 tmp;

            var_s0->unk_18 += var_s0->unk_1C;

            tmp = var_s0->unk_18;
            tmp /= 16;

            var_s0->unk_18 -= (tmp * 0x10);
        }
    }
}

void TradeCable_DrawLightSpark(s16 arg0, s16 arg1, f32 arg2, s16 arg3) {
    static u8* D_82F13FE0[] = {
        D_300D868, D_300EA68, D_300FC68, D_3010E68, D_3012068, D_3013268, D_3013268, D_3013268,
        D_3013268, D_3013268, D_3013268, D_3013268, D_3012068, D_3010E68, D_300FC68, D_300EA68,
    };

    s32 i;
    s16 var_fv0_4;
    s16 sp60;
    s16 sp5E;
    s16 sp5C;
    f32 var_fv0;
    u8* var_s2;

    if ((arg0 < 0x281) && (arg0 >= -0x30) && (arg1 < 0x1E1)) {
        sp60 = 0;
        if (arg1 >= -0x30) {
            var_fv0 = arg2;
            if ((arg3 >= 0xC) && (arg3 < 0x10)) {
                sp60 = 0x600;
                var_fv0 = 0.0f - arg2;
            }

            sp5E = ROUND_MAX(1024.0f / var_fv0);
            sp5C = ROUND_MAX(1024.0f / arg2);
            var_fv0_4 = ROUND_MAX(24.0f * arg2);

            var_s2 = D_82F13FE0[arg3];
            for (i = 0; i < 2; i++) {
                gDPLoadTextureBlock(gDisplayListHead++, var_s2, G_IM_FMT_RGBA, G_IM_SIZ_16b, 48, 24, 0,
                                    G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                                    G_TX_NOLOD, G_TX_NOLOD);

                TradeCable_DrawScissoredTexRect(arg0, arg1 + (var_fv0_4 * i), 0x30, 0x18, sp60, 0, sp5E, sp5C, 0);
                var_s2 += 0x900;
            }
        }
    }
}

void TradeCable_DrawLights(void) {
    s32 i;
    s32 var_s3 = 0;
    unk_D_82F21160* var_s0;

    for (i = 0, var_s0 = D_82F21160; i < 2; i++, var_s0++) {
        if (var_s0->state == 0) {
            continue;
        }

        if ((var_s0->unk_0C != 0) && (var_s0->unk_0E != 0)) {
            var_s3 = 1;
            var_s0->unk_08 = var_s0->screenX - 0x40;
            if (var_s0->unk_08 < 0) {
                var_s0->unk_08 = 0;
            }
            Gfx_SetScissorRect(&gDisplayListHead, var_s0->unk_08, var_s0->unk_0A, var_s0->unk_0C, var_s0->unk_0E);
        }

        TradeCable_DrawLightSpark(var_s0->screenX, var_s0->unk_06, var_s0->opacity, var_s0->unk_18);
    }

    if (var_s3 != 0) {
        Gfx_SetScissorRect(&gDisplayListHead, 0, 0, 0x280, 0x1E0);
    }
}

s32 TradeCable_IsSequenceActive(void) {
    s32 var_v1 = 0;

    if (D_82F21140.sequenceTimer != 0) {
        var_v1 = 1;
    }
    return var_v1;
}

void TradeCable_SetSequenceState(s16 arg0) {
    UNUSED s32 pad;
    unk_D_82F21140* ptr = &D_82F21140;
    unk_D_82F21160* ptr2;
    s32 i;
    u16 tmp = 0;

    if (arg0 < 0x12) {
        ptr->state = arg0;
    }

    switch (arg0) {
        case 0:
            ptr->scrollOffset = -0x14;
            ptr->tubeY = 0x18;
            ptr->tubeScale = 1.5f;
            ptr->unk_06 = 0xB4;
            ptr->unk_08 = 0xE4;
            ptr->unk_0A = 0xAA;
            ptr->sequenceTimer = -1;
            ptr->brightness = 0x80;
            ptr->railAlpha = 0x40;
            ptr->flags = tmp;
            ptr->flags |= 0x1F;
            break;

        case 1:
            ptr->transitionTimer = 0x1E;
            ptr->flags |= 0x20;
            Audio_PlaySoundEffectById(0x01200002);
            break;

        case 2:
        case 4:
            ptr->transitionTimer = 0xA;
            break;

        case 3:
            ptr->transitionTimer = 2;
            break;

        case 6:
            ptr->transitionTimer = 0xA;
            break;

        case 7:
            ptr->transitionTimer = 2;
            break;

        case 8:
        case 9:
            ptr->transitionTimer = 0xA;

            ptr2 = &D_82F21160[0];
            for (i = 0; i < 2; i++, ptr2++) {
                if (ptr2->state != 0) {
                    TradeCable_SetLightState(i, 3);
                }
            }
            break;

        case 11:
            TradeCable_SetLightState(0, 8);
            TradeCable_SetLightState(1, 8);
            break;

        case 12:
            ptr->transitionTimer = 0xA;
            break;

        case 14:
            ptr->transitionTimer = 0xA;
            break;

        case 16:
            ptr->transitionTimer = 0xA;
            break;

        case 5:
        case 17:
            ptr->transitionTimer = 0x1E;
            break;

        case 19:
            TradeCable_SetLightState(0, 1);
            Audio_PlaySoundEffectById(0x01200004);
            break;

        case 20:
            TradeCable_SetLightState(1, 1);
            Audio_PlaySoundEffectById(0x01200004);
            break;

        case 21:
            if (1) {}
            if (1) {}
            TradeCable_SetLightState(0, 5);
            TradeCable_SetLightState(1, 5);
            break;
    }
}

void TradeCable_UpdateSequenceIdle(unk_D_82F21140* arg0) {
    if (Trade_GetBoxMachineFlowState() != 0) {
        switch (Trade_GetBoxMachineFlowState()) {
            case 1:
            case 11:
                arg0->sequenceTimer = -1;
                arg0->flags |= 0x1E;
                break;

            default:
                arg0->flags &= ~0x1E;
                break;
        }
    } else if (Trade_GetSaveSeqState() != 0) {
        switch (Trade_GetSaveSeqState()) {
            case 1:
            case 8:
                arg0->sequenceTimer = -1;
                arg0->flags |= 0x1E;
                break;

            default:
                arg0->flags &= ~0x1E;
                break;
        }
    } else {
        arg0->sequenceTimer = -1;
        arg0->flags |= 0x1E;
    }
}

void TradeCable_UpdateSequenceOpen(unk_D_82F21140* arg0) {
    arg0->transitionTimer--;
    arg0->brightness = ((arg0->transitionTimer * 0x60) / 30) + 0x20;
    arg0->railAlpha = ((arg0->transitionTimer * -0x40) / 30) + 0x80;
    if (arg0->transitionTimer <= 0) {
        TradeCable_SetSequenceState(2);
    }
}

void TradeCable_UpdateSequenceSlideVertical(unk_D_82F21140* arg0) {
    s16 var_v0;

    arg0->transitionTimer--;
    var_v0 = arg0->transitionTimer;
    if (arg0->state == 4) {
        var_v0 = 0xA - var_v0;
    }

    arg0->scrollOffset = ((var_v0 * -0xAE) / 10) + 0x9A;
    arg0->tubeY = ((var_v0 * 0x43) / 10) - 0x2B;
    arg0->tubeScale = ((var_v0 * -0.39999998f) / 10) + 1.9f;
    arg0->unk_0A = ((var_v0 * 3) / 10) + 0xA7;

    if (arg0->transitionTimer <= 0) {
        if (arg0->state == 4) {
            TradeCable_SetSequenceState(5);
        } else {
            TradeCable_SetSequenceState(3);
        }
    }
}

void TradeCable_UpdateSequenceWaitLight0(unk_D_82F21140* arg0) {
    if (arg0->transitionTimer > 0) {
        arg0->transitionTimer--;
    }

    switch (Trade_IsPickScreenActive()) {
        case 3:
            break;

        case 2:
        case 4:
            if (arg0->sequenceTimer == -1) {
                arg0->sequenceTimer = 2;
            }
            arg0->flags &= ~1;
            break;

        case 1:
            arg0->flags &= ~0x1E;
            break;

        default:
            arg0->sequenceTimer = -1;
            arg0->flags |= 0x1F;
            break;
    }

    if (TradeCable_CheckSequenceCondition(4) != 0) {
        TradeCable_SetSequenceState(6);
    }
}

void TradeCable_UpdateSequenceSlideHorizontal(unk_D_82F21140* arg0) {
    arg0->transitionTimer--;
    arg0->scrollOffset = ((arg0->transitionTimer * 0x1F8) / 10) - 0x15E;
    if (arg0->transitionTimer <= 0) {
        TradeCable_SetSequenceState(7);
    }
}

void TradeCable_UpdateSequenceWaitLight1(unk_D_82F21140* arg0) {
    if (arg0->transitionTimer > 0) {
        arg0->transitionTimer--;
    }

    switch (Trade_IsPickScreenActive()) {
        case 3:
            break;

        case 2:
        case 4:
            if (arg0->sequenceTimer == -1) {
                arg0->sequenceTimer = 2;
            }
            arg0->flags &= ~1;
            break;

        case 1:
            arg0->flags &= ~0x1E;
            break;

        default:
            arg0->sequenceTimer = -1;
            arg0->flags |= 0x1F;
            break;
    }

    if (TradeCable_CheckSequenceCondition(5) != 0) {
        TradeCable_SetSequenceState(9);
    }
}

void TradeCable_UpdateSequenceSlideBack(unk_D_82F21140* arg0) {
    arg0->transitionTimer--;
    arg0->scrollOffset = ((arg0->transitionTimer * -0x14A) / 10) - 0x14;
    arg0->tubeY = ((arg0->transitionTimer * -0x43) / 10) + 0x18;
    arg0->tubeScale = ((arg0->transitionTimer * 0.39999998f) / 10.0f) + 1.5f;
    arg0->unk_0A = ((arg0->transitionTimer * -3) / 10) + 0xAA;

    if (arg0->transitionTimer <= 0) {
        if (arg0->state == 8) {
            TradeCable_SetSequenceState(5);
        } else {
            TradeCable_SetSequenceState(0xA);
        }
    }
}

void TradeCable_UpdateSequenceLaunchBalls(unk_D_82F21140* arg0) {
    if (Trade_GetSaveSeqState() == 2) {
        arg0->flags &= ~0x10;
        arg0->flags &= ~2;
    } else {
        arg0->flags |= 0x12;
    }

    if (TradeCable_CheckSequenceCondition(7) != 0) {
        if (!(arg0->flags & 0x40)) {
            arg0->flags |= 0x40;
            TradeCable_SetBallState(0, 1);
            TradeCable_SetBallState(1, 1);
            Audio_PlaySoundEffectById(0x01200005);
        }
    }
}

void TradeCable_UpdateSequenceWaitBallsArrived(unk_D_82F21140* arg0) {
    if (TradeCable_CheckSequenceCondition(7) != 0) {
        TradeCable_SetSequenceState(5);
    }
}

void TradeCable_UpdateSequenceSlideToEvoSlot0(unk_D_82F21140* arg0) {
    s16 var_v1 = arg0->transitionTimer;

    arg0->transitionTimer--;
    if (var_v1 < 0) {
        var_v1 = 0;
    }

    arg0->scrollOffset = ((var_v1 * -0x1E4) / 10) + 0x1D0;
    arg0->tubeY = ((var_v1 * 0) / 10) + 0x18;
    if (arg0->transitionTimer < -5) {
        TradeCable_SetSequenceState(0xD);
        TradeEvo_BeginSequence(0);
    }
}

void TradeCable_UpdateSequenceWaitEvoSlot0(unk_D_82F21140* arg0) {
    if (TradeCable_CheckSequenceCondition(0xB) != 0) {
        TradeCable_SetSequenceState(0xE);
    }
}

void TradeCable_UpdateSequenceSlideToEvoSlot1(unk_D_82F21140* arg0) {
    s16 var_v1 = arg0->transitionTimer;

    arg0->transitionTimer--;
    if (var_v1 < 0) {
        var_v1 = 0;
    }

    arg0->scrollOffset = ((var_v1 * 0x3A0) / 10) - 0x1D0;
    arg0->tubeY = ((var_v1 * 0) / 10) + 0x18;
    if (arg0->transitionTimer < -5) {
        TradeCable_SetSequenceState(0xF);
        TradeEvo_BeginSequence(1);
    }
}

void TradeCable_UpdateSequenceWaitEvoSlot1(unk_D_82F21140* arg0) {
    if (TradeCable_CheckSequenceCondition(0xB) != 0) {
        TradeCable_SetSequenceState(0x10);
    }
}

void TradeCable_UpdateSequenceFinishEvo(unk_D_82F21140* arg0) {
    arg0->transitionTimer--;
    arg0->scrollOffset = ((arg0->transitionTimer * -0x1BC) / 10) - 0x14;
    arg0->tubeY = ((arg0->transitionTimer * 0) / 10) + 0x18;
    if (arg0->transitionTimer <= 0) {
        TradeCable_SetSequenceState(0x11);
        TradeCable_SetLightState(0, 8);
        TradeCable_SetLightState(1, 8);
    }
}

void TradeCable_UpdateSequenceClose(unk_D_82F21140* arg0) {
    s32 i;

    if (arg0->state == 5) {
        for (i = 0; i < 2; i++) {
            if (D_82F21160[i].state == 4) {
                TradeCable_SetLightState(i, 8);
            }
        }

        if ((D_82F21160->state != 0) || (D_82F21160[1].state != 0)) {
            return;
        }
    } else if ((arg0->state == 0x11) && (TradeCable_CheckSequenceCondition(9) == 0)) {
        return;
    }

    if (arg0->flags & 0x20) {
        arg0->flags &= ~0x20;
        Audio_PlaySoundEffectById(0x01200003);
    }

    arg0->transitionTimer--;
    arg0->brightness = ((arg0->transitionTimer * -0x60) / 30) + 0x80;
    arg0->railAlpha = ((arg0->transitionTimer << 6) / 30) + 0x40;
    if (arg0->transitionTimer <= 0) {
        TradeCable_SetSequenceState(0);
    }
}

#ifdef NON_MATCHING
void TradeCable_DrawTubeWalls(s16 arg0, s16 arg1, f32 arg2) {
    s16 sp58;
    s16 temp_ft1;
    s16 temp_ft2;
    s32 temp_ft0;
    s32 i;
    s32 var_s2;
    u8* var_s6 = D_3000008;

    temp_ft2 = ROUND_MAX(228.0f * arg2);
    temp_ft1 = ROUND_MAX(19.0f * arg2);
    temp_ft0 = ROUND_MAX(1024.0f / arg2);
    sp58 = ROUND_MAX(-1024.0f / arg2);

    for (i = 0, var_s2 = arg1; i < 11; i++, var_s2 += temp_ft1, var_s6 += 0x876) {
        gDPLoadTextureTile_4b(gDisplayListHead++, var_s6, G_IM_FMT_I, 228, 0, 0, 0, 227, 18, 0,
                              G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                              G_TX_NOLOD, G_TX_NOLOD);

        TradeCable_DrawScissoredTexRect(arg0, var_s2, temp_ft2, temp_ft1, 0, 0, temp_ft0, temp_ft0, 0);
        TradeCable_DrawScissoredTexRect(arg0 + temp_ft2, var_s2, temp_ft2, temp_ft1, 0x1C60, 0, sp58, temp_ft0, 0);
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/trade_machine/trade_machine_2942C0/TradeCable_DrawTubeWalls.s")
#endif

void TradeCable_DrawTubeRailBar(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5) {
    s16 sp6E;
    s16 sp6C;
    s16 sp6A;
    s16 sp68;
    s16 sp66;
    s32 temp_fv1_4;
    s32 sp5C;

    sp6E = ROUND_MAX(48.0f * arg2);
    sp6C = ROUND_MAX(30.0f * arg2);
    sp6A = ROUND_MAX(arg3 * arg2);
    sp68 = ROUND_MAX(arg4 * arg2);
    sp66 = ROUND_MAX(arg5 * arg2);
    sp5C = ROUND_MAX(1024.0f / arg2);
    temp_fv1_4 = ROUND_MAX(-1024.0f / arg2);

    gDPLoadTextureTile_4b(gDisplayListHead++, D_3005D28, G_IM_FMT_I, 48, 0, 0, 0, 47, 29, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                          G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    TradeCable_DrawScissoredTexRect(arg0 + sp6A, arg1 + sp66, sp6E, sp6C, 0, 0, sp5C, sp5C, 0);
    TradeCable_DrawScissoredTexRect(arg0 + sp68, arg1 + sp66, sp6E, sp6C, 0x5E0, 0, temp_fv1_4, sp5C, 0);
}

void TradeCable_DrawTubeCenterBar(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5) {
    s16 sp6E;
    s16 sp6C;
    s16 sp6A;
    s16 sp68;
    s16 sp66;
    s32 temp_fv1_4;
    s32 sp5C;

    sp6E = ROUND_MAX(24.0f * arg2);
    sp6C = ROUND_MAX(30.0f * arg2);
    sp6A = ROUND_MAX(arg3 * arg2);
    sp68 = ROUND_MAX(arg4 * arg2);
    sp66 = ROUND_MAX(arg5 * arg2);
    sp5C = ROUND_MAX(1024.0f / arg2);
    temp_fv1_4 = ROUND_MAX(-1024.0f / arg2);

    gDPLoadTextureTile_4b(gDisplayListHead++, D_300D108, G_IM_FMT_I, 24, 0, 0, 0, 23, 29, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                          G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    TradeCable_DrawScissoredTexRect(arg0 + sp6A, arg1 + sp66, sp6E, sp6C, 0, 0, sp5C, sp5C, 0);
    TradeCable_DrawScissoredTexRect(arg0 + sp68, arg1 + sp66, sp6E, sp6C, 0x2E0, 0, temp_fv1_4, sp5C, 0);
}

void TradeCable_DrawTubeJoint(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5) {
    s16 sp6E;
    s16 sp6C;
    s16 sp6A;
    s16 sp68;
    s16 sp66;
    s32 temp_fv1_4;
    s32 sp5C;

    sp6E = ROUND_MAX(16.0f * arg2);
    sp6C = ROUND_MAX(14.0f * arg2);
    sp6A = ROUND_MAX(arg3 * arg2);
    sp68 = ROUND_MAX(arg4 * arg2);
    sp66 = ROUND_MAX(arg5 * arg2);
    sp5C = ROUND_MAX(1024.0f / arg2);
    temp_fv1_4 = ROUND_MAX(-1024.0f / arg2);

    gDPLoadTextureTile_4b(gDisplayListHead++, D_300D278, G_IM_FMT_I, 16, 0, 0, 0, 15, 13, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                          G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    TradeCable_DrawScissoredTexRect(arg0 + sp6A, arg1 + sp66, sp6E, sp6C, 0, 0, sp5C, sp5C, 0);
    TradeCable_DrawScissoredTexRect(arg0 + sp68, arg1 + sp66, sp6E, sp6C, 0x1E0, 0, temp_fv1_4, sp5C, 0);
}

void TradeCable_DrawTubeSparkBandDim(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5) {
    s16 spB6;
    s16 i;
    s32 sp60;
    u8* var_s5;
    s16 temp_fa0;
    s16 temp_fa0_2;
    s16 temp_fv1_2;
    s16 var_s4;
    s16 var_t3;
    s16 temp_ft0;
    s32 temp_ft1;

    spB6 = ROUND_MAX(100.0f * arg2);
    temp_fa0 = ROUND_MAX(arg3 * arg2);
    temp_fv1_2 = ROUND_MAX(arg4 * arg2);
    temp_fa0_2 = ROUND_MAX(arg5 * arg2);
    temp_ft1 = ROUND_MAX(1024.0f / arg2);
    sp60 = ROUND_MAX(-1024.0f / arg2);

    var_s4 = 0;
    var_s5 = D_3006000;
    var_t3 = 0;
    for (i = 0x83; i > 0;) {
        if ((i - 0x48) >= 0) {
            var_t3 = 0x48;
        } else {
            var_t3 = i;
        }

        gDPLoadTextureTile_4b(gDisplayListHead++, var_s5, G_IM_FMT_I, 100, 0, 0, 0, 99, var_t3, 0,
                              G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                              G_TX_NOLOD, G_TX_NOLOD);

        i -= var_t3;
        temp_ft0 = var_t3 * arg2;
        TradeCable_DrawScissoredTexRect(arg0 + temp_fa0, arg1 + temp_fa0_2 + var_s4, spB6, temp_ft0, 0, 0, temp_ft1, temp_ft1, 0);
        TradeCable_DrawScissoredTexRect(arg0 + temp_fv1_2, arg1 + temp_fa0_2 + var_s4, spB6, temp_ft0, 0xC60, 0, sp60, temp_ft1, 0);
        var_s5 += 0xE10;
        var_s4 += temp_ft0;
    }
}

void TradeCable_DrawTubeSparkBandBright(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5, s16 arg6) {
    static Gfx D_82F14020[] = {
        gsDPPipeSync(),
        gsDPSetCycleType(G_CYC_2CYCLE),
        gsDPSetRenderMode(G_RM_PASS, G_RM_XLU_SURF2),
        gsDPSetCombineLERP(NOISE, 0, TEXEL1, 0, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT,
                           0, 0, 0, COMBINED),
        gsDPSetEnvColor(200, 255, 255, 255),
        gsSPEndDisplayList(),
    };

    s16 spE6;
    s16 i;
    s32 sp6C;
    s32 sp60;
    u8* var_fp;
    u8* var_s7;
    s16 temp_fa0;
    s16 temp_fv1_2;
    s16 temp_fv1_3;
    s16 var_ra;
    s16 var_s6;
    s16 temp_ft0;

    spE6 = ROUND_MAX(100.0f * arg2);
    if (1) {}
    temp_fv1_2 = ROUND_MAX(arg3 * arg2);
    temp_fa0 = ROUND_MAX(arg4 * arg2);
    temp_fv1_3 = ROUND_MAX(arg5 * arg2);
    sp6C = ROUND_MAX(1024.0f / arg2);
    sp60 = ROUND_MAX(-1024.0f / arg2);

    var_s6 = 0;
    var_s7 = D_3009DC8;
    var_fp = D_300B768;
    i = 0x83;

    gSPDisplayList(gDisplayListHead++, D_82F14020);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, 100, 100, 255, arg6);

    for (; i > 0;) {
        if ((i - 0x20) >= 0) {
            var_ra = 0x20;
        } else {
            var_ra = i;
        }

        gDPLoadMultiTile_4b(gDisplayListHead++, var_fp, 0x0100, 1, G_IM_FMT_I, 100, 0, 0, 0, 99, var_ra, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);

        i -= var_ra;
        temp_ft0 = var_ra * arg2;

        gDPLoadMultiTile_4b(gDisplayListHead++, var_s7, 0x0000, G_TX_RENDERTILE, G_IM_FMT_I, 100, 0, 0, 0, 99, var_ra,
                            0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);

        TradeCable_DrawScissoredTexRect(arg0 + temp_fv1_2, arg1 + temp_fv1_3 + var_s6, spE6, temp_ft0, 0, 0, sp6C, sp6C, 0x100000);
        TradeCable_DrawScissoredTexRect(arg0 + temp_fa0, arg1 + temp_fv1_3 + var_s6, spE6, temp_ft0, 0xC60, 0, sp60, sp6C, 0x100000);
        var_s7 += 0x640;
        var_fp += 0x640;
        var_s6 += temp_ft0;
    }
}

void TradeCable_DrawTubeEndCap(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5) {
    s16 sp76;
    s16 sp74;
    s16 sp72;
    s16 sp70;
    s16 sp6E;
    UNUSED s32 pad;
    s16 sp5C;
    s32 sp60;

    sp76 = ROUND_MAX(64.0f * arg2);
    sp74 = ROUND_MAX(64.0f * arg2);
    sp72 = ROUND_MAX(arg3 * arg2);
    sp70 = ROUND_MAX(arg4 * arg2);
    sp6E = ROUND_MAX(arg5 * arg2);
    sp60 = ROUND_MAX(1024.0f / arg2);
    sp5C = ROUND_MAX(-1024.0f / arg2);

    gDPLoadTextureTile_4b(gDisplayListHead++, D_30079A0, G_IM_FMT_I, 64, 0, 0, 0, 63, 63, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                          G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    TradeCable_DrawScissoredTexRect(arg0 + sp72, arg1 + sp6E, sp76, sp74, 0, 0, sp60, sp60, 0);
    TradeCable_DrawScissoredTexRect(arg0 + sp70, arg1 + sp6E, sp76, sp74, 0x7E0, 0, sp5C, sp60, 0);
}

void TradeCable_RenderReflectionTexture(void) {
    static f32 D_82F14050 = 0.0f;
    static f32 D_82F14054 = 0.0f;
    static f32 D_82F14058 = 0.0f;
    static f32 D_82F1405C = 0.0f;
    static Gfx D_82F14060[] = {
        gsDPPipeSync(),
        gsDPSetCycleType(G_CYC_2CYCLE),
        gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
        gsDPSetCombineLERP(TEXEL0, TEXEL1, PRIM_LOD_FRAC, TEXEL1, 0, 0, 0, PRIMITIVE, PRIMITIVE, ENVIRONMENT, COMBINED,
                           ENVIRONMENT, 0, 0, 0, COMBINED),
        gsDPSetPrimColor(0, 0x80, 55, 255, 255, 255),
        gsDPSetEnvColor(0, 0, 255, 255),
        gsSPEndDisplayList(),
    };

    f32 temp_fa0;
    f32 temp_fa4;
    f32 temp_fa8;
    f32 temp_faC;

    GfxImage_SetRenderTarget(&gDisplayListHead, D_82F2115C);

    gSPDisplayList(gDisplayListHead++, D_82F14060);

    gDPLoadMultiBlock_4b(gDisplayListHead++, D_3009BC0, 0x0100, 1, G_IM_FMT_I, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                         G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, 1);

    temp_fa8 = D_82F14058 + 31.0f;
    temp_faC = D_82F1405C + 31.0f;
    if (1) {}
    gDPSetTileSize(gDisplayListHead++, 1, D_82F14058 * 4.0f, D_82F1405C * 4.0f, temp_fa8 * 4.0f, temp_faC * 4.0f);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_30099B8, G_IM_FMT_I, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, 2);

    temp_fa0 = D_82F14050 + 31.0f;
    temp_fa4 = D_82F14054 + 31.0f;
    gDPSetTileSize(gDisplayListHead++, G_TX_RENDERTILE, D_82F14050 * 4.0f, D_82F14054 * 4.0f, temp_fa0 * 4.0f,
                   temp_fa4 * 4.0f);

    Gfx_DrawTexturedRectClipped(0, 0, 0x20, 0x20, 0, 0, 0x400, 0x400, 0x100000);

    D_82F14050 += 0.75f;
    if (D_82F14050 >= 32.0f) {
        D_82F14050 -= 32.0f;
    }

    D_82F14054 += 5.75f;
    if (D_82F14054 >= 32.0f) {
        D_82F14054 -= 32.0f;
    }

    D_82F1405C -= 2.5f;
    if (D_82F1405C < 0.0f) {
        D_82F1405C += 32.0f;
    }

    gDPPipeSync(gDisplayListHead++);

    BgStage_DrawFrame();
}

void TradeCable_DrawTubeEndCapReflective(s16 arg0, s16 arg1, f32 arg2, s16 arg3, s16 arg4, s16 arg5) {
    static Gfx D_82F14098[] = {
        gsDPPipeSync(),
        gsDPSetCycleType(G_CYC_2CYCLE),
        gsDPSetRenderMode(G_RM_PASS, G_RM_XLU_SURF2),
        gsDPSetCombineLERP(0, 0, 0, TEXEL1, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
        gsDPSetEnvColor(0, 0, 0, 60),
        gsSPEndDisplayList(),
    };

    s16 sp96;
    s16 sp94;
    s16 sp92;
    s16 sp90;
    s16 sp8E;
    UNUSED s32 pad;
    s32 temp_fa0_3;
    s32 sp80;

    TradeCable_RenderReflectionTexture();

    sp96 = ROUND_MAX(64.0f * arg2);
    sp94 = ROUND_MAX(64.0f * arg2);
    sp92 = ROUND_MAX(arg3 * arg2);
    sp90 = ROUND_MAX(arg4 * arg2);
    sp8E = ROUND_MAX(arg5 * arg2);
    sp80 = ROUND_MAX(1024.0f / arg2);
    temp_fa0_3 = ROUND_MAX(-1024.0f / arg2);

    gSPDisplayList(gDisplayListHead++, D_82F14098);
    gDPLoadMultiBlock(gDisplayListHead++, D_82F2115C->img_p, 0x0100, 1, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0,
                      G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 1, 1);
    gDPLoadTextureBlock_4b(gDisplayListHead++, D_30081A8, G_IM_FMT_I, 64, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    TradeCable_DrawScissoredTexRect(arg0 + sp92, arg1 + sp8E, sp96, sp94, 0, 0, sp80, sp80, 0x100000);
    TradeCable_DrawScissoredTexRect(arg0 + sp90, arg1 + sp8E, sp96, sp94, 0x7E0, 0, temp_fa0_3, sp80, 0x100000);
}

void TradeCable_InitSequence(void) {
    TradeCable_SetSequenceState(0);
    TradeCable_ResetLights();
    TradeCable_ResetBalls();
    TradeEvo_ResetState();
    D_82F2115C = GfxImage_Allocate(0, 2, 0x20, 0x20, 1);
}

void TradeCable_UpdateSequence(void) {
    switch (D_82F21140.state) {
        case 0:
            TradeCable_UpdateSequenceIdle(&D_82F21140);
            break;

        case 1:
            TradeCable_UpdateSequenceOpen(&D_82F21140);
            break;

        case 2:
        case 4:
            TradeCable_UpdateSequenceSlideVertical(&D_82F21140);
            break;

        case 3:
            TradeCable_UpdateSequenceWaitLight0(&D_82F21140);
            break;

        case 6:
            TradeCable_UpdateSequenceSlideHorizontal(&D_82F21140);
            break;

        case 7:
            TradeCable_UpdateSequenceWaitLight1(&D_82F21140);
            break;

        case 8:
        case 9:
            TradeCable_UpdateSequenceSlideBack(&D_82F21140);
            break;

        case 10:
            TradeCable_UpdateSequenceLaunchBalls(&D_82F21140);
            break;

        case 11:
            TradeCable_UpdateSequenceWaitBallsArrived(&D_82F21140);
            break;

        case 12:
            TradeCable_UpdateSequenceSlideToEvoSlot0(&D_82F21140);
            break;

        case 13:
            TradeCable_UpdateSequenceWaitEvoSlot0(&D_82F21140);
            break;

        case 14:
            TradeCable_UpdateSequenceSlideToEvoSlot1(&D_82F21140);
            break;

        case 15:
            TradeCable_UpdateSequenceWaitEvoSlot1(&D_82F21140);
            break;

        case 16:
            TradeCable_UpdateSequenceFinishEvo(&D_82F21140);
            break;

        case 5:
        case 17:
            TradeCable_UpdateSequenceClose(&D_82F21140);
            break;
    }
    TradeCable_UpdateLights();
    TradeCable_UpdateBalls();
    TradeEvo_Update();
}

void TradeCable_DrawSequence(void) {
    unk_D_82F21140* ptr = &D_82F21140;
    u8 sp83;
    s32 var_v0_2;
    s32 tmp1;

    if (ptr->sequenceTimer > 0) {
        ptr->sequenceTimer--;
    }

    gSPDisplayList(gDisplayListHead++, D_8006F518);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetEnvColor(gDisplayListHead++, ptr->brightness, ptr->brightness, ptr->brightness, 255);

    TradeCable_DrawTubeWalls(ptr->scrollOffset, ptr->tubeY, ptr->tubeScale);
    tmp1 = D_82F14424 & 0x3F;
    sp83 = (ptr->railAlpha * tmp1) / 63u;
    if (D_82F14424 & 0x40) {
        sp83 = ptr->railAlpha - sp83;
    }

    if (ptr->flags & 1) {
        if (Trade_GetTextPrinterState() == 1) {
            tmp1 = D_82F14424 & 3;
            sp83 = (tmp1 * 0xFF) / 3u;
            if (D_82F14424 & 4) {
                sp83 = 0xFF - sp83;
            }
        }
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

    if (ptr->flags & 0x10) {
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sp83);
        TradeCable_DrawTubeRailBar(ptr->scrollOffset, ptr->tubeY, ptr->tubeScale, ptr->unk_06, ptr->unk_08, ptr->unk_0A);
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);

    TradeCable_DrawLights();
    TradeCable_DrawBalls();

    tmp1 = D_82F14424 & 0x3F;
    sp83 = (tmp1 * 0xFF) / 63u;
    if (D_82F14424 & 0x40) {
        sp83 = 0xFF - sp83;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sp83);

    tmp1 = D_82F14424 & 0x3F;
    sp83 = ((tmp1) << 7) / 63u;
    if (D_82F14424 & 0x40) {
        sp83 = 0x80 - sp83;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sp83);

    if (ptr->flags & 2) {
        TradeCable_DrawTubeCenterBar(ptr->scrollOffset, ptr->tubeY, ptr->tubeScale, 0xB4, 0xFA, 0x73);
    }

    if (ptr->flags & 4) {
        TradeCable_DrawTubeJoint(ptr->scrollOffset, ptr->tubeY, ptr->tubeScale, 0xD4, 0xE4, 2);
    }

    if (ptr->flags & 4) {
        if (ptr->flags & 0x40) {
            var_v0_2 = 0x64;
        } else {
            var_v0_2 = 0x32;
        }
        TradeCable_DrawTubeSparkBandBright(ptr->scrollOffset, ptr->tubeY, ptr->tubeScale, 0x81, 0xE5, 0, var_v0_2);

        gSPDisplayList(gDisplayListHead++, D_8006F518);
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sp83);

        if (!(ptr->flags & 0x40)) {
            TradeCable_DrawTubeSparkBandDim(ptr->scrollOffset, ptr->tubeY, ptr->tubeScale, 0x81, 0xE5, 0);
        }
    }

    if (ptr->flags & 8) {
        if (ptr->flags & 0x20) {
            TradeCable_DrawTubeEndCapReflective(ptr->scrollOffset, ptr->tubeY, ptr->tubeScale, 0x23, 0x164, 0x68);
        } else {
            TradeCable_DrawTubeEndCap(ptr->scrollOffset, ptr->tubeY, ptr->tubeScale, 0x23, 0x164, 0x68);
        }
    }

    TradeEvo_Draw();
    gSPDisplayList(gDisplayListHead++, D_8006F630);
}

s32 TradeCable_CheckSequenceCondition(s16 arg0) {
    unk_D_82F21160* ptr = &D_82F21160[0];
    s32 var_v1 = 0;

    switch (arg0) {
        case 1:
            if ((D_82F21140.state == 3) && (D_82F21140.transitionTimer == 0)) {
                var_v1 = 1;
            }
            break;

        case 2:
            if ((D_82F21140.state == 7) && (D_82F21140.transitionTimer == 0)) {
                var_v1 = 1;
            }
            break;

        case 3:
            if (D_82F21140.state == 0) {
                var_v1 = 1;
            }
            break;

        case 4:
            if ((D_82F21160[0].state == 2) && (ptr[0].unk_1C <= 1.0f)) {
                var_v1 = 1;
            }
            break;

        case 5:
            if ((D_82F21160[1].state == 2) && (ptr[1].unk_1C <= 1.0f)) {
                var_v1 = 1;
            }
            break;

        case 6:
            if (D_82F21140.state == 0xA) {
                var_v1 = 1;
            }
            break;

        case 8:
            if ((D_82F21160[0].state == 7) && (D_82F21160[1].state == 7)) {
                var_v1 = 1;
            }
            break;

        case 7:
        case 9:
            if ((D_82F21160[0].state == 0) && (D_82F21160[1].state == 0)) {
                var_v1 = 1;
            }
            break;

        case 10:
            if (D_82F21140.state == 0) {
                var_v1 = 1;
            }
            break;

        case 11:
            if (D_82F21238.phase == 0) {
                var_v1 = 1;
            }
            break;

        case 12:
            if (D_82F21238.phase == 2) {
                var_v1 = 1;
            }
            break;
    }

    return var_v1;
}

void TradeCable_SetSequenceTimer(s16 arg0) {
    D_82F21140.sequenceTimer = arg0;
}

s16 TradeCable_GetSequenceState(void) {
    return D_82F21140.state;
}
