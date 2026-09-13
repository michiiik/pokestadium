#include "trade_machine.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/pokemon_stats.h"
#include "src/text_system.h"
#include "src/audio_sfx.h"

static unk_D_82F210E0 D_82F210E0;
static unk_func_82F00838_sp30_00C* D_82F21120[2];
static s16 D_82F21128;
static s32 D_82F2112C;
static s32 D_82F21130;

s32 Trade_WriteDeckAndCommitCart(unk_func_82F00838_sp30* arg0) {
    DeckHandle* sp24;
    s32 var_v1;

    if (arg0->unk_04 == 0) {
        sp24 = Deck_Open(0x20, arg0->unk_02, 0, 1);
    } else {
        sp24 = Deck_Open(0x21, arg0->unk_02, arg0->unk_04 - 1, 1);
    }

    Deck_WriteEntries(&arg0->unk_0C->unk_00, arg0->unk_08, sp24);
    Deck_CloseAndFlush(sp24);

    var_v1 = GbSave_WriteBoxBanks(arg0->unk_02);
    if (var_v1 != 0) {
        var_v1 = GbSave_CommitMainData(arg0->unk_02);
    }
    return var_v1;
}

s32 Trade_StepCartWriteSequence(unk_D_82F210E0* arg0) {
    s32 i;
    unk_func_82F00838_sp30* ptr;
    unk_func_82F00838_sp30 sp30[2];
    s32 sp2C = 0;

    for (i = 0; i < 2; i++) {
        Trade_GetPickScreenSnapshot(i, &sp30[i]);
    }

    if (arg0->writeStep < 0) {
        arg0->writeStep = 0;
        for (i = 0, ptr = &sp30[0]; i < 2; i++, ptr++) {
            GbSave_ValidateBeforeWrite(ptr->unk_02);
        }
    }

    if (arg0->validatedPortCount < 2) {
        for (i = 0, ptr = &sp30[0]; i < 2; i++, ptr++) {
            if (i < arg0->validatedPortCount) {
                continue;
            }

            if (GbSave_InvalidateMainDataChecksum(ptr->unk_02) != 0) {
                arg0->validatedPortCount++;
            } else {
                break;
            }
        }
    }

    if (arg0->validatedPortCount < 2) {
        return 0;
    }

    for (i = 0, ptr = &sp30[0]; i < 2; i++, ptr++) {
        if (i < arg0->writtenPortCount) {
            continue;
        }

        if (Trade_WriteDeckAndCommitCart(ptr) != 0) {
            arg0->writtenPortCount += 1;
            GbSave_RequirePakPresent(ptr->unk_02);
        } else {
            break;
        }
    }

    if (arg0->writtenPortCount == 2) {
        sp2C = 1;
    }
    return sp2C;
}

void Trade_InitSaveSequence(s32 arg0) {
    UNUSED s32 pad;
    unk_D_82F210E0* ptr = &D_82F210E0;
    s32 i;
    unk_func_82F00838_sp30 sp2C;

    ptr->state = 1;
    ptr->timer = 0xA;
    ptr->flags = 0;
    ptr->writeStep = -1;
    ptr->validatedPortCount = 0;
    ptr->writtenPortCount = 0;
    ptr->unk_0C = 0;
    ptr->unk_0E = 0;
    ptr->menuSelection = -1;
    ptr->blinkTimer = -1;
    if (arg0 != 0) {
        ptr->flags |= 1;
    }

    for (i = 0; i < 2; i++) {
        Trade_GetPickScreenSnapshot(i, &sp2C);
        D_82F21120[i] = &sp2C.unk_0C[sp2C.unk_06];
    }

    D_82F21128 = 0;
    D_82F2112C = 0;
    D_82F21130 = 0;

    for (i = 0; i < 2; i++) {
        ptr->boxPanelRects[i].x = ptr->boxPanelRects[i].y = 0;
        ptr->boxPanelRects[i].width = 0xC8;
        ptr->boxPanelRects[i].height = 0;
    }

    ptr->menuPanelRect.x = 0;
    ptr->menuPanelRect.width = 0xE8;
    ptr->menuPanelRect.height = 0;
    ptr->bannerPanelRect.x = 0;
    ptr->bannerPanelRect.width = 0x210;
    ptr->bannerPanelRect.height = 0;
    ptr->subBannerRect.x = 0;
    ptr->subBannerRect.width = 0xE8;
    ptr->subBannerRect.height = 0;
    ptr->menuPanelRect.y = 0;
    ptr->bannerPanelRect.y = 0;
    ptr->subBannerRect.y = 0;

    if (arg0 != 0) {
        Audio_PlaySoundEffectById(0x1E);
    } else {
        Audio_PlaySoundEffectById(4);
    }
}

void Trade_UpdateSaveSeqPanelSlideIn(unk_D_82F210E0* arg0) {
    s32 i;
    TradeRect* ptr;

    arg0->timer--;

    for (i = 0; i < 2; i++) {
        arg0->boxPanelRects[i].x = ((i * 0x118) - ((arg0->timer * 0) / 10)) + 0x50;
        arg0->boxPanelRects[i].y = 0x60 - ((arg0->timer * -0x54) / 10);
        arg0->boxPanelRects[i].width = 0xC8 - ((arg0->timer * 0) / 10);
        arg0->boxPanelRects[i].height = 0xA8 - ((arg0->timer * 0xA8) / 10);
    }

    ptr = &arg0->menuPanelRect;
    if (!(arg0->flags & 1)) {
        ptr->x = 0xCC - ((arg0->timer * 0) / 10);
        ptr->y = 0x11C - ((arg0->timer * -0x38) / 10);
        ptr->width = 0xE8 - ((arg0->timer * 0) / 10);
        ptr->height = 0x70 - ((arg0->timer * 0x70) / 10);
    }

    ptr = &arg0->bannerPanelRect;
    if (arg0->flags & 1) {
        ptr->x = 0x38 - ((arg0->timer * 0) / 10);
        ptr->y = 0x190 - ((arg0->timer * -0x14) / 10);
        ptr->width = 0x210 - ((arg0->timer * 0) / 10);
        ptr->height = 0x28 - ((arg0->timer * 0x28) / 10);
    }

    if (arg0->timer <= 0) {
        if (arg0->flags & 1) {
            arg0->state = 3;
            arg0->timer = 0x14;
            arg0->flags |= 4;
        } else {
            arg0->state = 2;
            arg0->timer = 0;
            arg0->flags |= 6;
        }
    }
}

void Trade_UpdateSaveSeqMenuInput(unk_D_82F210E0* arg0) {
    s32 var_t0 = 0;
    s32 var_a2;
    s16 tmp;

    if (arg0->flags & 1) {
        var_a2 = 3;
    } else {
        var_a2 = 2;
    }

    tmp = D_82F21128;
    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DUP) && (D_82F21128 > 0)) {
        D_82F21128--;
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_DDOWN) && (D_82F21128 < (var_a2 - 1))) {
        D_82F21128++;
    }

    if (D_82F21128 != tmp) {
        var_t0 = 0;
        Audio_PlaySoundEffectById(1);
    }

    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        var_t0 = 1;
    } else if (BTN_IS_PRESSED(gPlayer1Controller, BTN_B) && !(arg0->flags & 1)) {
        var_t0 = 1;
        D_82F21128 = 1;
    }

    if (var_t0 != 0) {
        if (arg0->flags & 1) {
            switch (D_82F21128) {
                case 0:
                    arg0->state = 5;
                    arg0->timer = 0xA;
                    D_82F21130 = 0;
                    Audio_PlaySoundEffectById(0x1D);
                    break;

                case 1:
                    arg0->state = 5;
                    arg0->timer = 0xA;
                    D_82F21130 = 1;
                    Audio_PlaySoundEffectById(0x22);
                    break;

                default:
                    arg0->state = 8;
                    arg0->timer = 0xA;
                    arg0->flags &= ~4;
                    D_82F21130 = 0;
                    Audio_PlaySoundEffectById(3);
                    break;
            }
        } else {
            arg0->state = 8;
            arg0->timer = 0xA;
            arg0->flags &= ~6;
            if (D_82F21128 == 0) {
                D_82F2112C = 1;
                Audio_PlaySoundEffectById(2);
            } else {
                D_82F2112C = 0;
                Audio_PlaySoundEffectById(3);
            }
        }
        arg0->blinkTimer = -1;
    }
}

void Trade_UpdateSaveSeqConfirmWait(unk_D_82F210E0* arg0) {
    arg0->timer--;
    if (BTN_IS_PRESSED(gPlayer1Controller, BTN_A)) {
        arg0->timer = 0;
    }

    if (arg0->timer <= 0) {
        arg0->state = 4;
        arg0->timer = 0xA;
    }
}

void Trade_UpdateSaveSeqPanelClose(unk_D_82F210E0* arg0) {
    arg0->timer--;
    arg0->menuPanelRect.x = 0xA2 - (arg0->timer * 0) / 10;
    arg0->menuPanelRect.y = 0x116 - ((arg0->timer * -0x2E) / 10);
    arg0->menuPanelRect.width = 0x13C - (arg0->timer * 0) / 10;
    arg0->menuPanelRect.height = 0x70 - ((arg0->timer * 0x70) / 10);
    if (arg0->timer <= 0) {
        arg0->state = 2;
        arg0->timer = 0;
    }
}

void Trade_UpdateSaveSeqPanelReopen(unk_D_82F210E0* arg0) {
    arg0->timer--;
    arg0->menuPanelRect.x = 0xA2 - (arg0->timer * 0) / 10;
    arg0->menuPanelRect.y = 0x144 - ((arg0->timer * 0x2E) / 10);
    arg0->menuPanelRect.width = 0x13C - (arg0->timer * 0) / 10;
    arg0->menuPanelRect.height = -((arg0->timer * -0x70) / 10);
    if (arg0->timer <= 0) {
        arg0->state = 6;
        arg0->timer = 0xA;
    }
}

void Trade_UpdateSaveSeqBeginWrite(unk_D_82F210E0* arg0) {
    arg0->timer--;
    arg0->subBannerRect.x = 0xCC - (arg0->timer * 0) / 10;
    arg0->subBannerRect.y = 0x134 - ((arg0->timer * -0x1C) / 10);
    arg0->subBannerRect.width = 0xE8 - (arg0->timer * 0) / 10;
    arg0->subBannerRect.height = 0x38 - ((arg0->timer * 0x38) / 10);
    if (arg0->timer <= 0) {
        arg0->state = 7;
        arg0->timer = 5;
        arg0->writeStep = -1;
        arg0->validatedPortCount = 0;
        arg0->writtenPortCount = 0;
        arg0->unk_0C = 0;
        arg0->unk_0E = 0;
        arg0->menuSelection = -1;
    }
}

void Trade_UpdateSaveSeqWriteCarts(unk_D_82F210E0* arg0) {
    s32 temp_v0;
    unk_func_82F00838_sp30 sp24;

    arg0->timer--;
    if (arg0->timer <= 0) {
        arg0->timer = 0;
        if (arg0->unk_0C == 1) {
            Trade_GetPickScreenSnapshot(arg0->writtenPortCount, &sp24);
            temp_v0 = GbSave_PollCartRemoval(sp24.unk_02);
            if (temp_v0 == 1) {
                arg0->menuSelection = 1;
            }

            if (temp_v0 == 3) {
                arg0->menuSelection = 3;
            }

            if (temp_v0 == 2) {
                arg0->unk_0C = 2;
                arg0->unk_0E = 3;
                arg0->menuSelection = 0;
            }
        }

        if (arg0->unk_0C == 2) {
            arg0->unk_0E -= 1;
            if (arg0->unk_0E <= 0) {
                arg0->unk_0C = 0;
                arg0->unk_0E = 0;
            }
        }

        if (arg0->unk_0C == 0) {
            if (Trade_StepCartWriteSequence(arg0) != 0) {
                arg0->state = 8;
                arg0->timer = 0xA;
                arg0->flags &= ~4;
                arg0->menuSelection = -1;
                Audio_PlaySoundEffectById(9);
            } else {
                arg0->writeStep += 1;
                arg0->unk_0C = 1;
                arg0->menuSelection = 2;
                Trade_GetPickScreenSnapshot(arg0->writtenPortCount, &sp24);
                temp_v0 = GbSave_PollCartRemoval(sp24.unk_02);
                if (temp_v0 == 1) {
                    arg0->menuSelection = 1;
                }

                if (temp_v0 == 3) {
                    arg0->menuSelection = 3;
                }
            }
        }
    }
}

void Trade_UpdateSaveSeqPanelSlideOut(unk_D_82F210E0* arg0) {
    TradeRect* temp_v1;
    s32 i;

    arg0->timer--;

    for (i = 0; i < 2; i++) {
        arg0->boxPanelRects[i].x = ((i * 0x118) - ((arg0->timer * 0) / 10)) + 0x50;
        arg0->boxPanelRects[i].y = 0xB4 - ((arg0->timer * 0x54) / 10);
        arg0->boxPanelRects[i].width = 0xC8 - ((arg0->timer * 0) / 10);
        arg0->boxPanelRects[i].height = -((arg0->timer * -0xA8) / 10);
    }

    temp_v1 = &arg0->menuPanelRect;
    if (arg0->flags & 1) {
        if ((temp_v1->width != 0x13C) || (temp_v1->height != 0)) {
            temp_v1->x = 0xA2 - ((arg0->timer * 0) / 10);
            temp_v1->y = 0x144 - ((arg0->timer * 0x2E) / 10);
            temp_v1->width = 0x13C - ((arg0->timer * 0) / 10);
            temp_v1->height = -((arg0->timer * -0x70) / 10);
        }
    } else if ((temp_v1->width != 0xE8) || (temp_v1->height != 0)) {
        temp_v1->x = 0xCC - ((arg0->timer * 0) / 10);
        temp_v1->y = 0x154 - ((arg0->timer * 0x38) / 10);
        temp_v1->width = 0xE8 - ((arg0->timer * 0) / 10);
        temp_v1->height = -((arg0->timer * -0x70) / 10);
    }

    temp_v1 = &arg0->bannerPanelRect;
    if ((arg0->bannerPanelRect.width != 0x210) || (arg0->bannerPanelRect.height != 0)) {
        temp_v1->x = 0x38 - ((arg0->timer * 0) / 10);
        temp_v1->y = 0x1A4 - ((arg0->timer * 0x14) / 10);
        temp_v1->width = 0x210 - ((arg0->timer * 0) / 10);
        temp_v1->height = -((arg0->timer * -0x28) / 10);
    }

    temp_v1 = &arg0->subBannerRect;
    if ((arg0->subBannerRect.width != 0xE8) || (arg0->subBannerRect.height != 0)) {
        temp_v1->x = 0xCC - ((arg0->timer * 0) / 10);
        temp_v1->y = 0x150 - ((arg0->timer * 0x1C) / 10);
        temp_v1->width = 0xE8 - ((arg0->timer * 0) / 10);
        temp_v1->height = -((arg0->timer * -0x38) / 10);
    }

    if (arg0->timer <= 0) {
        arg0->state = 0;
        arg0->timer = 0;
        arg0->flags = 0;
    }
}

void Trade_DrawSaveSeqBoxThumbnail(s16 arg0, s16 arg1, s16 arg2) {
    UNUSED s32 pad;
    u16* img = Trade_GetPickBuffer(arg0);

    gDPLoadTextureBlock(gDisplayListHead++, img, G_IM_FMT_RGBA, G_IM_SIZ_16b, 40, 40, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg1, arg2, 0x3C, 0x3C, 0, 0, 0x2AB, 0x2AB, 0);
}

void Trade_DrawSaveSeqBoxPanels(unk_D_82F210E0* arg0) {
    TradeRect* var_s0;
    s32 sp74[2];
    unk_func_82F00838_sp30_00C* temp_s3;
    s32 i;

    if (arg0->blinkTimer > 0) {
        arg0->blinkTimer--;
    }

    for (i = 0; i < 2; i++) {
        var_s0 = &arg0->boxPanelRects[i];
        if ((var_s0->width != 0xC8) || (var_s0->height != 0)) {
            Trade_DrawPickScreenFrame(var_s0->x, var_s0->y, var_s0->width, var_s0->height);
        }
    }

    for (i = 0; i < 2; i++) {
        if ((arg0->boxPanelRects[i].width == 0xC8) && (arg0->boxPanelRects[i].height == 0xA8)) {
            sp74[i] = 1;
        } else {
            sp74[i] = 0;
        }
    }

    gSPDisplayList(gDisplayListHead++, D_8006F498);

    for (i = 0; i < 2; i++) {
        var_s0 = &arg0->boxPanelRects[i];
        if (sp74[i] != 0) {
            Gfx_FillRectRgba(var_s0->x, var_s0->y, var_s0->width, var_s0->height, 0x32, 0x32, 0x96, 0xFF);
        }
    }

    gSPDisplayList(gDisplayListHead++, D_8006F630);
    gSPDisplayList(gDisplayListHead++, D_8006F518);

    for (i = 0; i < 2; i++) {
        if (sp74[i] != 0) {
            Trade_DrawSaveSeqBoxThumbnail(i, arg0->boxPanelRects[i].x + 0x46, arg0->boxPanelRects[i].y + 0xA);
        }
    }

    gSPDisplayList(gDisplayListHead++, D_8006F630);

    Font_BeginTranslucentTextRendering();
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);

    for (i = 0; i < 2; i++) {
        if (sp74[i] != 0) {
            temp_s3 = D_82F21120[i];
            var_s0 = &arg0->boxPanelRects[i];
            Font_SetActive(4, 0);
            Font_Printf(var_s0->x + 0xA, var_s0->y + 0x4A, "No.");
            Font_SetActive(4, 0);
            Font_Printf(var_s0->x + 0x26, var_s0->y + 0x4A, "%03d", temp_s3->unk_00);
            Font_SetActive(8, 0);
            Font_Printf(var_s0->x + 0x62, var_s0->y + 0x46, Trade_GetSpeciesName(temp_s3->unk_00));
            Font_SetActive(8, 0);
            Font_Printf(var_s0->x + 0x10, var_s0->y + 0x5E, Text_GetString(NULL, 0, gTradeStrings, 0x14));
            Font_Printf(var_s0->x + 0x46, var_s0->y + 0x5E, "/%s", temp_s3->unk_30);
            Font_Printf(var_s0->x + 0x10, var_s0->y + 0x74, Text_GetString(NULL, 0, gTradeStrings, 0x15));
            Font_Printf(var_s0->x + 0x46, var_s0->y + 0x74, "/%s", temp_s3->unk_3B);
            Font_Printf(var_s0->x + 0x10, var_s0->y + 0x8A, "ID%05d", temp_s3->unk_0E);
        }
    }

    Font_EndTexturedTextRendering();
}

void Trade_DrawSaveSeqMenuPanel(unk_D_82F210E0* arg0) {
    s32 i;
    TradeRect* var_s4 = &arg0->menuPanelRect;
    Color_RGB8* var_v0;
    s16 temp_ft3;
    s16 arg2;
    Color_RGB8 sp64 = { 0x1E, 0x1E, 0x82 };
    Color_RGB8 sp60 = { 0x64, 0x1E, 0x1E };

    if (arg0->flags & 1) {
        if ((var_s4->width == 0x13C) && (var_s4->height == 0)) {
            return;
        }
    } else if ((var_s4->width == 0xE8) && (var_s4->height == 0)) {
        return;
    }

    if (arg0->flags & 1) {
        var_v0 = &sp60;
    } else {
        var_v0 = &sp64;
    }

    if ((var_s4->width >= 0xE) && (var_s4->height >= 0xE)) {
        Ui_DrawGradientPanel(var_s4->x, var_s4->y, var_s4->width, var_s4->height, var_v0, var_v0);
    }

    if (arg0->flags & 1) {
        if ((var_s4->width != 0x13C) || (var_s4->height != 0x70)) {
            return;
        }
    } else if ((var_s4->width != 0xE8) || (var_s4->height != 0x70)) {
        return;
    }

    {
        static s16 D_82F13F78 = 0;

        temp_ft3 = SINS(D_82F13F78) * 3.0f;

        if (arg0->flags & 1) {
            Ui_DrawTextureMarker(var_s4->x + temp_ft3 + 0x10, var_s4->y + (D_82F21128 * 0x1C) + 0x14);
        } else {
            Ui_DrawTextureMarker(var_s4->x + temp_ft3 + 0x18, var_s4->y + (D_82F21128 * 0x1C) + 0x32);
        }

        Font_BeginTranslucentTextRendering();
        Font_SetActive(0x10, 0);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);

        if (arg0->flags & 1) {
            char* sp54[3];
            for (i = 0; i < 3; i++) {
                sp54[0] = Text_GetString(NULL, 0, gTradeStrings, 0x1A);
                sp54[1] = Text_GetString(NULL, 0, gTradeStrings, 0x1B);
                sp54[2] = Text_GetString(NULL, 0, gTradeStrings, 0x1C);
                Font_Printf(var_s4->x + 0x38, var_s4->y + i * 0x1C + 0x10, sp54[i]);
            }
        } else {
            char* temp_v0 = Text_GetString(NULL, 0, gTradeStrings, 0x16);
            char* sp48[2];

            arg2 = Font_MeasureTextExtent(0, 0, temp_v0);
            Font_Printf((var_s4->x + (var_s4->width / 2)) - (arg2 / 2), var_s4->y + 0x10, temp_v0);

            for (i = 0; i < 2; i++) {
                sp48[0] = Text_GetString(NULL, 0, gTradeStrings, 0x17);
                sp48[1] = Text_GetString(NULL, 0, gTradeStrings, 0x18);
                if (i != D_82F21128) {
                    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
                } else {
                    Gfx_SetEnvColor(0xFF, 0xFF, 0, 0xFF);
                }
                arg2 = Font_MeasureTextExtent(0, 0, sp48[i]);
                Font_Printf((var_s4->x + (var_s4->width / 2)) - (arg2 / 2), var_s4->y + i * 0x1C + 0x2C,
                              sp48[i]);
            }
        }

        Font_EndTexturedTextRendering();
        D_82F13F78 += 0x1000;
    }
}

void Trade_DrawSaveSeqBannerPanel(unk_D_82F210E0* arg0) {
    s32 sp34;
    TradeRect* temp_s0 = &arg0->bannerPanelRect;

    if ((arg0->bannerPanelRect.width != 0x210) || (arg0->bannerPanelRect.height != 0)) {
        Trade_DrawRoundedFrameLarge(temp_s0->x, temp_s0->y, temp_s0->width, temp_s0->height, 0x1E, 0x1E, 0x82, 0xFF);
        if ((temp_s0->width == 0x210) && (temp_s0->height == 0x28)) {
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            sp34 = Font_MeasureTextExtent(0, 0, Text_GetString(NULL, 0, gTradeStrings, 0x1D)) / 2;
            Font_Printf((temp_s0->x + (temp_s0->width / 2)) - sp34, temp_s0->y + 8,
                          Text_GetString(NULL, 0, gTradeStrings, 0x1D));
            Font_EndTexturedTextRendering();
        }
    }
}

void Trade_DrawSaveSeqSubBanner(unk_D_82F210E0* arg0) {
    TradeRect* temp_s0 = &arg0->subBannerRect;
    char* sp40;
    UNUSED s32 pad;
    Color_RGB8 sp38 = { 0x64, 0x1E, 0x1E };

    if ((arg0->subBannerRect.width != 0xE8) || (arg0->subBannerRect.height != 0)) {
        if ((temp_s0->width >= 0xE) && (temp_s0->height >= 0xE)) {
            Ui_DrawGradientPanel(temp_s0->x, temp_s0->y, temp_s0->width, temp_s0->height, &sp38, &sp38);
        }

        if ((temp_s0->width == 0xE8) && (temp_s0->height == 0x38)) {
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            sp40 = Text_GetString(NULL, 0, gTradeStrings, 0x1E);
            Font_Printf((temp_s0->x + (temp_s0->width / 2)) - (Font_MeasureTextExtent(0, 0, sp40) / 2),
                          temp_s0->y + 0x10, sp40);
            Font_EndTexturedTextRendering();
        }
    }
}

void Trade_DrawSaveSeqMessageBackdrop(s16 arg0, s16 arg1, s16 arg2, s16 arg3) {
    Color_RGB8 sp24 = { 0x58, 0x08, 0x00 };
    Color_RGB8 sp20 = { 0x58, 0x08, 0x00 };

    Ui_DrawGradientPanel(arg0, arg1, arg2, arg3, &sp24, &sp20);
}

#ifdef NON_MATCHING
void Trade_DrawSaveSeqMessageText(unk_D_82F210E0* arg0) {
    static s16 D_82F13F88 = 0;

    s32 pad[2];
    char sp38[64];
    s32 pad2[2];
    unk_func_82F00838_sp30 sp20;

    if (arg0->validatedPortCount < 2) {
        Trade_GetPickScreenSnapshot(arg0->validatedPortCount, &sp20);
    } else {
        Trade_GetPickScreenSnapshot(arg0->writtenPortCount, &sp20);
    }

    if (arg0->menuSelection == -1) {
        return;
    }

    Trade_DrawSaveSeqMessageBackdrop(0x6E, 0x72, 0x1A4, 0x114);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);

    switch (arg0->menuSelection) {
        case 0:
            Font_EnableTwoCycleTexturing();
            Gfx_SetPrimColor(0xFF, 0xF1, 0x78, 0xFF);
            Font_Printf(0xEC, 0xA8, Text_GetString(NULL, 0, gTradeStrings, 0x1F));
            Font_DisableTwoCycleTexturing();
            break;

        case 2:
            Font_EnableTwoCycleTexturing();
            Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
            Gfx_SetPrimColor(0xFF, 0xF1, 0x78, 0xFF);
            // Text_SetNumberToken(1, (((((((((tmp) & 0xFFFFFFFFFFFFFFFF) & 0xFFFFFFFFFFFFFFFF) & 0xFFFFFFFFFFFFFFFF) &
            // 0xFFFFFFFFFFFFFFFF) & 0xFFFFFFFFFFFFFFFF) & 0xFFFFFFFFFFFFFFFF) & 0xFFFFFFFFFFFFFFFF) &
            // 0xFFFFFFFFFFFFFFFF) & 0xFFFFFFFFFFFFFFFF);
            Text_SetNumberToken(1, sp20.unk_02 + 1);
            Font_Printf(0xB0, 0x90, Text_GetString(sp38, sizeof(sp38), gTradeStrings, 0x20));
            Font_DisableTwoCycleTexturing();
            break;

        case 3:
            Font_EnableTwoCycleTexturing();
            Gfx_SetPrimColor(0xFF, 0xF1, 0x78, 0xFF);
            Font_Printf(0x80, 0x90, Text_GetString(NULL, 0, gTradeStrings, 0x21));
            Font_DisableTwoCycleTexturing();
            break;
    }

    if (arg0->menuSelection != 0) {
        Font_EnableTwoCycleTexturing();
        Gfx_SetPrimColor(0xFF, 0xF1, 0x78, 0xFF);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Font_SetLineHeight(0x24);
        Font_Printf(0x80, 0xF0, Text_GetString(NULL, 0, gTradeStrings, 0x22));
    }

    Font_EndTexturedTextRendering();
    D_82F13F88 += 0x800;
}
#else
static s16 D_82F13F88 = 0;
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/trade_machine/trade_machine_291D60/Trade_DrawSaveSeqMessageText.s")
#endif

s32 Trade_GetSaveSeqState(void) {
    return D_82F210E0.state;
}

s32 Trade_GetSaveDirection(void) {
    return D_82F2112C;
}

s32 Trade_GetSaveSlotIndex(void) {
    return D_82F21130;
}

void Trade_ResetSaveSeq(void) {
    unk_D_82F210E0* ptr = &D_82F210E0;
    s32 i;

    ptr->state = 0;
    ptr->timer = 0;
    ptr->flags = 0;

    for (i = 0; i < 2; i++) {
        D_82F210E0.boxPanelRects[i].x = D_82F210E0.boxPanelRects[i].y = 0;
        D_82F210E0.boxPanelRects[i].width = 0xC8;
        D_82F210E0.boxPanelRects[i].height = 0;
    }

    ptr->menuPanelRect.x = 0;
    ptr->menuPanelRect.width = 0xE8;
    ptr->menuPanelRect.height = 0;
    ptr->bannerPanelRect.x = 0;
    ptr->bannerPanelRect.width = 0x210;
    ptr->bannerPanelRect.height = 0;
    ptr->subBannerRect.x = 0;
    ptr->subBannerRect.width = 0xE8;
    ptr->subBannerRect.height = 0;

    ptr->menuPanelRect.y = 0;
    ptr->bannerPanelRect.y = ptr->subBannerRect.y = 0;

    for (i = 0; i < 2; i++) {
        D_82F21120[i] = NULL;
    }
}

void Trade_UpdateSaveSeq(void) {
    unk_D_82F210E0* ptr = &D_82F210E0;

    if (ptr->state == 0) {
        return;
    }

    switch (ptr->state) {
        case 1:
            Trade_UpdateSaveSeqPanelSlideIn(&D_82F210E0);
            break;

        case 2:
            Trade_UpdateSaveSeqMenuInput(&D_82F210E0);
            break;

        case 3:
            Trade_UpdateSaveSeqConfirmWait(&D_82F210E0);
            break;

        case 4:
            Trade_UpdateSaveSeqPanelClose(&D_82F210E0);
            break;

        case 5:
            Trade_UpdateSaveSeqPanelReopen(&D_82F210E0);
            break;

        case 6:
            Trade_UpdateSaveSeqBeginWrite(&D_82F210E0);
            break;

        case 7:
            Trade_UpdateSaveSeqWriteCarts(&D_82F210E0);
            break;

        case 8:
            Trade_UpdateSaveSeqPanelSlideOut(&D_82F210E0);
            break;
    }
}

void Trade_DrawSaveSeq(void) {
    static s16 D_82F13F8C[] = { 0x32, 0x14A };

    unk_D_82F210E0* ptr = &D_82F210E0;
    s32 i;

    if (ptr->state != 0) {
        if (ptr->blinkTimer != 0) {
            Trade_DrawSaveSeqBoxPanels(&D_82F210E0);
        }

        Trade_DrawSaveSeqMenuPanel(&D_82F210E0);
        Trade_DrawSaveSeqBannerPanel(&D_82F210E0);
        Trade_DrawSaveSeqSubBanner(&D_82F210E0);

        if (ptr->flags & 2) {
            Trade_DrawBoxMachineTransferArrows(0x120, 0x8C);
        }

        if (ptr->flags & 4) {
            for (i = 0; i < 2; i++) {
                Trade_DrawPortInfoPanel(i, D_82F13F8C[i], 0x2A);
            }
        }
        Trade_DrawSaveSeqMessageText(&D_82F210E0);
    }
}
