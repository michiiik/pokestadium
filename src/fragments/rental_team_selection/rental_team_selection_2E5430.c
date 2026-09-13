#include "rental_team_selection.h"
#include "src/ui_graphics.h"
#include "src/gb_data.h"
#include "src/pokemon_stats.h"
#include "src/save_data.h"
#include "src/audio_sfx.h"
#include "src/display_object_textures.h"
#include "src/poke_icon.h"
#include "src/graphics_textures.h"
#include "src/status_icons.h"
#include "src/text_system.h"
#include "src/gfx_rect.h"
#include "src/matrix.h"
#include "src/dp_intro.h"
#include "src/hal_libc.h"

u8* D_84211850[] = {
    D_202C080, D_202C500, D_202C980, D_202CE00, D_202D280, D_202D700, D_202DB80, D_202E000,
};
s16 D_84211870[] = { 0x74, 0xF8, 0x17C, 0x74, 0xF8, 0x17C };
s16 D_8421187C[] = { 0, 0, 0, 0x2C, 0x2C, 0x2C };

s32 RegisteredTeam_Save(unk_D_84229EB0_00024* arg0) {
    s16 temp_v0;
    s16 i;
    s32 ret = 0;
    DeckHandle* temp_s3;

    temp_v0 = Deck_FindFirstFreeTeamSlot();
    if (temp_v0 < 0xA) {
        temp_s3 = Deck_OpenAndSetName(0x10, 0, temp_v0, &arg0->playerName, &arg0->playerName, arg0->trainerId);

        for (i = 0; i < arg0->monCount; i++) {
            Deck_WriteEntries(&arg0->monSlots[i].mon.species.dexId, 1, temp_s3);
        }

        Deck_CloseAndFlush(temp_s3);
        Save_FlushDirtyBanks();
        ret = 1;
    }
    return ret;
}

void RegisteredTeam_DeleteAndCompact(unk_D_84229EB0_00024* arg0) {
    RegisteredTeamSlot sp30;
    s16 tmp = arg0->slotIndex;
    s16 i;

    if (tmp >= 0xA) {
        return;
    }

    for (i = tmp; i < 9; i++) {
        RegisteredTeam_ReadSlot(&sp30, i + 1);
        RegisteredTeam_WriteSlot(&sp30, i);
        Save_CommitTypedRecord(0x10, i);
    }

    Save_ResetAndCommitTypedRecord(0x10, 9);
    Save_FlushDirtyBanks();
}

void TeamSelection_RegisteredTeam_DrawCard(unk_D_84229EB0* arg0, s16 arg1, s16 arg2, unk_D_84229EB0_00024* arg3) {
    UNUSED s32 pad;
    s16 i;
    s16 j;
    Color_RGBA8 sp8C;
    s16 temp_a0;
    s16 temp_a1;
    s16 temp_s2;
    s16 temp_s3;
    s16 var_s1;

    if ((arg0->state == 8) || (arg0->state == 9) || (arg0->state == 0xA)) {
        TeamSelection_DrawColoredFrame(arg1, arg2, 0x74, 0x58, 0x64, 0x64, 0xC8, 0xFF);

        for (var_s1 = 0, i = 0; i < 2; i++) {
            for (j = 0; j < 3; j++, var_s1++) {
                temp_a0 = D_84211870[var_s1] + arg1;
                temp_a1 = D_8421187C[var_s1] + arg2;
                if ((j == arg0->unk_00008) && (i == arg0->unk_0000A)) {
                    TeamSelection_DrawColoredFrame(temp_a0, temp_a1, 0x84, 0x2C, 0x64, 0x64, 0xC8, 0xFF);
                } else {
                    TeamSelection_DrawColoredFrame(temp_a0, temp_a1, 0x84, 0x2C, 0x3C, 0x3C, 0xA0, 0xFF);
                }
            }
        }
    } else {
        if (arg3->slotIndex == arg0->cursorIndex) {
            Color_SetRGB(&sp8C, 0x64, 0x64, 0xC8);
        } else {
            Color_SetRGB(&sp8C, 0x3C, 0x3C, 0xC8);
        }
        TeamSelection_DrawColoredFrame(arg1, arg2, 0x74, 0x58, sp8C.r, sp8C.g, sp8C.b, 0xFF);

        for (var_s1 = 0; var_s1 < 6; var_s1++) {
            temp_a0 = D_84211870[var_s1] + arg1;
            temp_a1 = D_8421187C[var_s1] + arg2;

            TeamSelection_DrawColoredFrame(temp_a0, temp_a1, 0x84, 0x2C, sp8C.r, sp8C.g, sp8C.b, 0xFF);
        }
    }

    gSPDisplayList(gDisplayListHead++, D_8006F470);

    Gfx_FillRectRgb(arg1, arg2 + 0x58, 0x200, 4, 0x32, 0x32, 0x32);

    gSPDisplayList(gDisplayListHead++, D_8006F518);

    if (arg3->monCount != 0) {
        if (arg3->monCount > 0) {
            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 128);
        } else {
            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
        }
        Gfx_DrawTextureRgba16(arg1 + 0x14, arg2 + 0xE, 0x4C, 0x14, D_2029CE0, 0x4C, 0);
        Gfx_DrawTextureRgba16(arg1 + 0x14, arg2 + 0x22, 0x4C, 0x14, D_202A8C0, 0x4C, 0);
        Gfx_DrawTextureRgba16(arg1 + 0x14, arg2 + 0x36, 0x4C, 0x14, D_202B4A0, 0x4C, 0);
    }

    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);

    for (var_s1 = 0; var_s1 < 6; var_s1++) {
        if (arg3->monSlots[var_s1].raw != 0) {
            temp_s2 = D_84211870[var_s1] + arg1 + 2;
            temp_s3 = D_8421187C[var_s1] + arg2 + 2;
            Gfx_DrawTextureRgba16(temp_s2, temp_s3, 0x28, 0x28, arg3->monSlots[var_s1].iconTexture, 0x28, 0);
            if (arg3->monSlots[var_s1].raw & 0x80000000) {
                u8* ptr = D_84211850[arg3->monSlots[var_s1].raw & 7];

                Gfx_DrawTextureI4(temp_s2 - 4, temp_s3 - 4, 0x30, 0x30, ptr, 0x30, 0);
            }
        }
    }

    gSPDisplayList(gDisplayListHead++, D_8006F630);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(8, 0);
    Font_Printf(arg1 + 2, arg2 + 2, "%d", arg3->slotIndex + 1);

    if (arg3->monCount > 0) {
        Font_SetActive(8, 0);
        Font_Printf(arg1 + 8, arg2 + 0x2B, &arg3->playerName);
        Font_SetActive(4, 0);
        Font_Printf(arg1 + 8, arg2 + 0x42, "ID %05d", arg3->trainerId);
    }

    Font_SetActive(4, 0);

    for (var_s1 = 0; var_s1 < 6; var_s1++) {
        if (arg3->monSlots[var_s1].mon.species.dexId != 0) {
            s16 tmp1 = D_84211870[var_s1] + arg1 + 0x2C;
            s16 tmp2 = D_8421187C[var_s1] + arg2 + 5;

            Font_Printf(tmp1, tmp2, arg3->monSlots[var_s1].mon.nickname);
        }
    }

    Font_SetActive(4, 0);

    for (var_s1 = 0; var_s1 < 6; var_s1++) {
        if (arg3->monSlots[var_s1].mon.species.dexId != 0) {
            temp_s2 = D_84211870[var_s1] + arg1 + 0x2C;
            temp_s3 = D_8421187C[var_s1] + arg2 + 0x17;
            Font_Printf(temp_s2, temp_s3, "%s%d", TeamSelection_GetInstructionText(0x15), arg3->monSlots[var_s1].mon.level);
        }
    }

    Font_EndTexturedTextRendering();
}

void TeamSelection_RegisteredTeam_DrawHeader(s16 arg0, s16 arg1) {
    char* sp54 = TeamSelection_GetLabelText(D_800AE540.modeCategory + 0x3F);
    s32 sp50 = Font_MeasureTextExtent(8, 0, sp54);

    gSPDisplayList(gDisplayListHead++, D_8006F558);

    gDPSetEnvColor(gDisplayListHead++, 170, 60, 150, 255);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, 80, 40, 60, 255);

    Gfx_DrawTextureIa8(arg0, arg1, 8, 0x1C, D_2000980, 8, 0);
    Gfx_DrawTextureIa8(arg0 + 0x218, arg1, 8, 0x1C, D_2000A60, 8, 0);
    Gfx_DrawTexturedRectClipped(arg0 + 4, arg1, 0x218, 0x1C, 0, 0, 0, 0x400, 0);

    gSPDisplayList(gDisplayListHead++, D_8006F630);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(8, 0);
    Font_Printf(arg0 + 8, arg1 + 3, TeamSelection_GetLabelText(0xE));
    Font_Printf((arg0 - sp50) + 0x20E, arg1 + 3, sp54);
    Font_EndTexturedTextRendering();
}

void TeamSelection_RegisteredTeam_DrawScrollbar(unk_D_84229EB0* arg0) {
    static s16 D_84211888 = 0;

    s16 sp66;
    s16 sp64;
    s16 sp62;
    s16 temp_s2;

    sp62 = arg0->screenX + 4;
    temp_s2 = arg0->screenY + 0x20;
    sp64 = arg0->unk_0001C;

    gSPDisplayList(gDisplayListHead++, D_8006F470);

    Gfx_FillRectRgb(sp62 + 0, temp_s2, 0x18, arg0->overrideHeight, 0x3C, 0x3C, 0xA0);
    Gfx_FillRectRgb(sp62 + 0x18, temp_s2, 8, arg0->overrideHeight, 0x32, 0x32, 0x32);

    if (sp64 < 3) {
        sp64 = 3;
    }

    if (arg0->overrideHeight == 0x110) {
        if ((arg0->state == 0xC) || (arg0->state == 0xB)) {
            if (arg0->velocityX > 0) {
                sp66 = ((((arg0->cursorIndex * 4) + arg0->velocityX) * 0xF8) - 0x3E0) / ((sp64 * 4) - 4);
            } else {
                sp66 = ((((arg0->cursorIndex * 4) + arg0->velocityX) * 0xF8) + 0x3E0) / ((sp64 * 4) - 4);
            }
        } else {
            sp66 = (arg0->cursorIndex * 0xF8) / (sp64 - 1);
        }

        gSPDisplayList(gDisplayListHead++, D_8006F558);
        gDPSetEnvColor(gDisplayListHead++, 130, 130, 220, 255);
        gDPSetPrimColor(gDisplayListHead++, 0, 0, 60, 60, 150, 255);

        Gfx_DrawTextureIa8(sp62 + 4, temp_s2 + 4, 0x10, 8, D_200A200, 0x10, 0);
        Gfx_DrawTextureIa8(sp62 + 4, temp_s2 + 0x104, 0x10, 8, D_200A280, 0x10, 0);
        Gfx_DrawTexturedRectClipped(sp62 + 4, temp_s2 + 0xC, 0x10, 0xF8, 0, 0, 0x400, 0, 0);

        gSPDisplayList(gDisplayListHead++, D_8006F518);

        if (D_84211888 < 4) {
            Gfx_DrawTextureRgba16(sp62, temp_s2 + sp66, 0x18, 0x18, D_200A300, 0x18, 0);
        } else {
            Gfx_DrawTextureRgba16(sp62, temp_s2 + sp66, 0x18, 0x18, D_200A780, 0x18, 0);
        }
    }

    gSPDisplayList(gDisplayListHead++, D_8006F630);

    if (arg0->state == 2) {
        D_84211888 = (D_84211888 + 1) % 8;
    }
}

void TeamSelection_RegisteredTeam_DrawAnimatedListFrame(unk_D_84229EB0* arg0) {
    s16 sp1E = arg0->screenX + 0x24;
    s16 temp_a1 = arg0->screenY + ((arg0->cursorIndex - arg0->scrollTop) * 0x5C) + 0x20;

    if (arg0->state == 0xB) {
        if (arg0->velocityX > 0) {
            temp_a1 = ((temp_a1 + ((arg0->velocityX * 0x5C) / 4)) - 0x5C);
        } else {
            temp_a1 = (temp_a1 + ((arg0->velocityX * 0x5C) / 4) + 0x5C);
        }
        TeamSelection_DrawAnimatedGoldCorners(sp1E, temp_a1, 0x200, 0x58);
    } else if ((arg0->state == 2) || (arg0->state == 0xC)) {
        TeamSelection_DrawAnimatedGoldCorners(sp1E, temp_a1, 0x200, 0x58);
    }
}

void TeamSelection_RegisteredTeam_DrawAnimatedCursor(unk_D_84229EB0* arg0) {
    s16 temp_s0 = arg0->screenX + (arg0->unk_00008 * 0x84) + 0x98;
    s16 temp_a1 = arg0->screenY + (arg0->unk_0000A * 0x2C) + 0x20;

    if (arg0->state == 0xA) {
        if (arg0->velocityX > 0) {
            temp_a1 = ((temp_a1 + ((arg0->velocityX * 0x2C) / 4)) - 0x2C);
        } else {
            temp_a1 = (temp_a1 + ((arg0->velocityX * 0x2C) / 4) + 0x2C);
        }
        TeamSelection_DrawAnimatedGoldCorners(temp_s0, temp_a1, 0x84, 0x2C);
    } else if (arg0->state == 9) {
        if (arg0->velocityX > 0) {
            temp_s0 = ((temp_s0 + ((arg0->velocityX * 0x84) / 4)) - 0x84);
        } else {
            temp_s0 = (temp_s0 + ((arg0->velocityX * 0x84) / 4) + 0x84);
        }
        TeamSelection_DrawAnimatedGoldCorners(temp_s0, temp_a1, 0x84, 0x2C);
    } else if (arg0->state == 8) {
        TeamSelection_DrawAnimatedGoldCorners(temp_s0, temp_a1, 0x84, 0x2C);
    }
}

void TeamSelection_RegisteredTeam_DrawVisibleCards(unk_D_84229EB0* arg0) {
    s16 i;
    s16 temp_s4;
    s16 temp_s5;
    s16 var_s0;
    unk_D_84229EB0_00024* var_s1;

    var_s1 = arg0->currentCard;
    temp_s5 = arg0->screenX + 0x24;
    temp_s4 = arg0->screenY + 0x20;

    var_s0 = (arg0->overrideHeight - ((arg0->cursorIndex - arg0->scrollTop) * 0x5C)) - 0x58;
    if (var_s0 > 0) {
        var_s0 = 0;
    }
    Gfx_SetScissorRect(&gDisplayListHead, 0, temp_s4, 0x280, arg0->overrideHeight);

    for (i = 0; i < 3; i++) {
        if ((var_s0 >= -0x5B) && (var_s0 < arg0->overrideHeight)) {
            TeamSelection_RegisteredTeam_DrawCard(arg0, temp_s5, temp_s4 + var_s0, var_s1);
        }
        var_s0 += 0x5C;
        var_s1 = var_s1->prev;
    }

    Gfx_SetScissorRect(&gDisplayListHead, 0, 0, 0x280, 0x1E0);
}

void TeamSelection_RegisteredTeam_Render(unk_D_84229EB0* arg0, s32 arg1) {
    UNUSED s32 pad;
    s32 i;
    char* temp_v0_3;
    unk_D_84229EB0_00024* var_s0;
    s16 tmp1;
    s16 tmp2;
    s16 sp42;

    tmp1 = arg0->screenX;
    tmp2 = arg0->screenY;

    if ((arg0->state != 1) && (arg0->state != 0) && ((arg1 != 0) || (arg0->needsRedraw != 0))) {
        TeamSelection_DrawCornerFrame(tmp1, tmp2, 0x228, arg0->overrideHeight + 0x24);
        TeamSelection_RegisteredTeam_DrawHeader(tmp1 + 4, tmp2 + 4);
        TeamSelection_RegisteredTeam_DrawScrollbar(arg0);

        if (arg0->state == 0xC) {
            sp42 = tmp2 - ((arg0->velocityX * 0x5C) / 4);
            Gfx_SetScissorRect(&gDisplayListHead, tmp1 + 0x24, tmp2 + 0x20, 0x200, 0x110);
            if (arg0->velocityY > 0) {
                var_s0 = arg0->currentCard->next;
                for (i = 0; i < 4; i++) {
                    TeamSelection_RegisteredTeam_DrawCard(arg0, tmp1 + 0x24, sp42 + 0x20 + i * 0x5C, var_s0);
                    var_s0 = var_s0->prev;
                }
            } else {
                var_s0 = arg0->currentCard;
                for (i = 0; i < 4; i++) {
                    TeamSelection_RegisteredTeam_DrawCard(arg0, tmp1 + 0x24, sp42 - 0x3C + i * 0x5C, var_s0);
                    var_s0 = var_s0->prev;
                }
            }
            Gfx_SetScissorRect(&gDisplayListHead, 0, 0, 0x280, 0x1E0);
        } else {
            TeamSelection_RegisteredTeam_DrawVisibleCards(arg0);
        }

        TeamSelection_RegisteredTeam_DrawAnimatedListFrame(arg0);
        TeamSelection_RegisteredTeam_DrawAnimatedCursor(arg0);

        if ((arg1 != 0) && (arg0->mode == 1)) {
            TeamSelection_DrawTexturedPanel(0x38, 0x190, 0x210, 0x28, 0x1E, 0x1E, 0x82, 0x96);
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            temp_v0_3 = TeamSelection_GetLabelText(0x10);
            Font_Printf(0x140 - (Font_MeasureTextExtent(0x10, 0, temp_v0_3) / 2), 0x198, temp_v0_3);
            Font_EndTexturedTextRendering();
        }

        if ((arg1 != 0) && (arg0->mode == 2)) {
            TeamSelection_DrawTexturedPanel(0x38, 0x190, 0x210, 0x28, 0x64, 0x1E, 0x1E, 0x96);
            Font_BeginTranslucentTextRendering();
            Font_SetActive(0x10, 0);
            temp_v0_3 = TeamSelection_GetLabelText(0x11);
            Font_Printf(0x140 - (Font_MeasureTextExtent(0x10, 0, temp_v0_3) / 2), 0x198, temp_v0_3);
            Font_EndTexturedTextRendering();
        }

        if (arg0->needsRedraw > 0) {
            arg0->needsRedraw--;
        }
    }
}

unk_D_84229EB0_00024* TeamSelection_RegisteredTeam_GetNext(unk_D_84229EB0_00024* arg0, s32 arg1) {
    while (arg1-- > 0) {
        arg0 = arg0->next;
    }

    return arg0;
}

unk_D_84229EB0_00024* TeamSelection_RegisteredTeam_GetPrevious(unk_D_84229EB0_00024* arg0, s32 arg1) {
    while (arg1-- > 0) {
        arg0 = arg0->prev;
    }

    return arg0;
}

s32 TeamSelection_RegisteredTeam_Load(unk_D_84229EB0_00024* arg0, s16 arg1) {
    s16 i;
    s16 var_s4;
    DeckHandle* temp_v0;

    var_s4 = 0;
    arg0->trainerId = 0;
    arg0->playerName[0] = 0;

    for (i = 0; i < 6; i++) {
        arg0->monSlots[i].mon.species.dexId = 0;
        arg0->monSlots[i].raw = 0;
    }

    temp_v0 = Deck_Open(0x10, 0, arg1, 0);
    if (temp_v0 != NULL) {
        for (i = 0; i < 6; i++) {
            var_s4 += Deck_ReadEntries(&arg0->monSlots[i].mon, 1, temp_v0);
        }
        Deck_CloseAndFlush(temp_v0);
        arg0->trainerId = Deck_GetSaveEntryTrainerId(0x10, arg1);
        Deck_GetSaveEntryName(0x10, arg1, arg0->playerName);
    }

    for (i = 0; i < var_s4; i++) {
        PokeIcon_LoadModelTextureForMon(arg0->monSlots[i].iconTexture, 0, &arg0->monSlots[i].mon);
        arg0->monSlots[i].state = 4;
        arg0->monSlots[i].deckSlot = arg1;
        arg0->monSlots[i].partyIndex = i;
        arg0->monSlots[i].speciesId = arg0->monSlots[i].mon.species.dexId;
    }

    arg0->slotIndex = arg1;
    arg0->monCount = var_s4;

    return var_s4;
}

void TeamSelection_RegisteredTeam_UpdatePreview(unk_D_84229EB0* arg0) {
    s16 temp_s1;
    s16 sp34;
    s16 sp32;
    unk_D_84229EB0_00024* temp_v0_2;

    if (arg0->animTimer == 1) {
        temp_s1 = arg0->unk_00008 + (arg0->unk_0000A * 3);
        sp34 = arg0->baseX + (arg0->unk_00008 * 0x84) + 0x94;
        sp32 = arg0->baseY + (arg0->unk_0000A * 0x2C) + 0x1C;
        temp_v0_2 = TeamSelection_RegisteredTeam_GetPrevious(arg0->currentCard, arg0->cursorIndex - arg0->scrollTop);
        if (arg0->selectedIndex == -1) {
            TeamSelection_Preview_StartFull(arg0->unk_134F0, arg0->controllerIndex, 0x30, 0xEC, sp34, sp32, &temp_v0_2->monSlots[temp_s1], 0);
        } else if (temp_s1 != arg0->selectedIndex) {
            TeamSelection_Preview_StartCompact(arg0->unk_134F0, sp34, sp32, &temp_v0_2->monSlots[temp_s1]);
            arg0->animTimer = 0xA;
        }
        arg0->selectedIndex = temp_s1;
    }

    if (arg0->animTimer < 0xA) {
        arg0->animTimer++;
    }
}

void TeamSelection_RegisteredTeam_InitializeSelection(unk_D_84229EB0* arg0) {
    arg0->unk_00008 = 0;
    arg0->unk_0000A = 0;
    arg0->animTimer = 0;
    arg0->selectedIndex = -1;
    arg0->state = 0xD;
    arg0->subState = 8;
}

void TeamSelection_RegisteredTeam_SetOpeningPosition(unk_D_84229EB0* arg0) {
    arg0->screenX = arg0->baseX + 0x280;
    arg0->screenY = arg0->baseY;
}

void TeamSelection_RegisteredTeam_AdvanceOpen(unk_D_84229EB0* arg0) {
    arg0->animTimer++;
    if (arg0->animTimer == 0xA) {
        arg0->state = 2;
        arg0->animTimer = 0;
        arg0->screenX = arg0->baseX;
    } else {
        arg0->screenX = (arg0->baseX - (arg0->animTimer << 6)) + 0x280;
    }
    arg0->screenY = arg0->baseY;
    arg0->needsRedraw = 2;
}

void TeamSelection_RegisteredTeam_AdvanceClose(unk_D_84229EB0* arg0) {
    arg0->animTimer++;
    if (arg0->animTimer == 0xA) {
        arg0->state = 0;
        arg0->animTimer = 0;
        arg0->screenX = arg0->baseX + 0x280;
    } else {
        arg0->screenX = arg0->baseX - (arg0->animTimer << 6);
    }
    arg0->screenY = arg0->baseY;
    arg0->needsRedraw = 2;
}

void TeamSelection_RegisteredTeam_AdvanceReturnFromEdit(unk_D_84229EB0* arg0) {
    arg0->animTimer++;
    if (arg0->animTimer == 0x14) {
        arg0->state = 0x11;
        arg0->animTimer = 0;
        arg0->screenX = arg0->baseX;
    } else {
        arg0->screenX = (arg0->baseX - (arg0->animTimer << 5)) + 0x280;
    }
    arg0->overrideHeight = 0x58;
    arg0->screenY = arg0->baseY;
    arg0->needsRedraw = 2;
}

void TeamSelection_RegisteredTeam_MoveListUp(unk_D_84229EB0* arg0) {
    unk_D_84229EB0_00024* sp24;

    if (arg0->inputLock == 0) {
        if ((arg0->cursorIndex - arg0->scrollTop) > 0) {
            arg0->cursorIndex--;
            arg0->velocityX = -1;
            arg0->velocityY = -4;
            arg0->subState = 2;
            arg0->state = 0xB;
            Audio_PlaySoundEffectById(1);
        } else if (arg0->scrollTop > 0) {
            sp24 = TeamSelection_RegisteredTeam_GetNext(arg0->currentCard, 1);
            Audio_PlaySoundEffectById(1);
            TeamSelection_RegisteredTeam_Load(sp24, arg0->cursorIndex - 1);
            arg0->scrollTop -= 1;
            arg0->cursorIndex -= 1;
            arg0->currentCard = arg0->currentCard->next;
            arg0->velocityX = -1;
            arg0->velocityY = -4;
            arg0->subState = 2;
            arg0->state = 0xC;
        }
        arg0->inputLock = arg0->inputLockNext;
        arg0->inputLockNext = 2;
    }
}

void TeamSelection_RegisteredTeam_MoveListDown(unk_D_84229EB0* arg0) {
    if (arg0->inputLock == 0) {
        if ((arg0->cursorIndex - arg0->scrollTop) < 2) {
            arg0->cursorIndex++;
            arg0->velocityX = 1;
            arg0->velocityY = 4;
            arg0->subState = 2;
            arg0->state = 0xB;
            Audio_PlaySoundEffectById(1);
        } else if ((arg0->scrollTop < 7) &&
                   ((TeamSelection_RegisteredTeam_Load(TeamSelection_RegisteredTeam_GetPrevious(arg0->currentCard, 3), arg0->cursorIndex + 1) != 0) ||
                    (arg0->mode != 0))) {
            Audio_PlaySoundEffectById(1);
            arg0->scrollTop += 1;
            arg0->cursorIndex += 1;
            arg0->currentCard = arg0->currentCard->prev;
            arg0->velocityX = 1;
            arg0->velocityY = 4;
            arg0->subState = 2;
            arg0->state = 0xC;
        }
        arg0->inputLock = arg0->inputLockNext;
        arg0->inputLockNext = 2;
    }
}

void TeamSelection_RegisteredTeam_HandleListInput(unk_D_84229EB0* arg0) {
    Controller* cont = &gControllers[arg0->controllerIndex];
    unk_D_84229EB0_00024* temp_v0 = TeamSelection_RegisteredTeam_GetPrevious(arg0->currentCard, arg0->cursorIndex - arg0->scrollTop);

    if (BTN_IS_PRESSED(cont, BTN_A)) {
        if (temp_v0->monSlots[0].mon.species.dexId != 0) {
            Audio_PlaySoundEffectById(2);

            switch (arg0->mode) {
                case 0:
                    arg0->state = 5;
                    break;

                case 1:
                    TeamSelection_RegisteredTeam_InitializeSelection(arg0);
                    break;

                case 2:
                    arg0->state = 6;
                    break;
            }
        } else {
            Audio_PlaySoundEffectById(8);
        }
    } else if (BTN_IS_PRESSED(cont, BTN_B)) {
        Audio_PlaySoundEffectById(3);
        arg0->exitRequested = 1;
        arg0->animTimer = 0;
        arg0->state = 4;
    } else if (BTN_IS_DOWN(cont, BTN_DUP)) {
        TeamSelection_RegisteredTeam_MoveListUp(arg0);
    } else if (BTN_IS_DOWN(cont, BTN_DDOWN)) {
        TeamSelection_RegisteredTeam_MoveListDown(arg0);
    } else {
        arg0->inputLockNext = 8;
        arg0->inputLock = 0;
    }
    arg0->screenX = arg0->baseX;
    arg0->screenY = arg0->baseY;
    arg0->needsRedraw = 2;
}

void TeamSelection_RegisteredTeam_MovePokemonLeft(unk_D_84229EB0* arg0) {
    if (arg0->unk_00008 > 0) {
        unk_D_84229EB0_00024* ptr = TeamSelection_RegisteredTeam_GetPrevious(arg0->currentCard, arg0->cursorIndex - arg0->scrollTop);
        s32 idx = ((arg0->unk_0000A * 3) + arg0->unk_00008) - 1;

        if (ptr->monSlots[idx].mon.species.dexId != 0) {
            arg0->unk_00008 = arg0->unk_00008 - 1;
            arg0->velocityX = -1;
            arg0->velocityY = -4;
            arg0->animTimer = 0;
            arg0->subState = 8;
            arg0->state = 9;
            Audio_PlaySoundEffectById(1);
        }
    }
}

void TeamSelection_RegisteredTeam_MovePokemonRight(unk_D_84229EB0* arg0) {
    if (arg0->unk_00008 < 2) {
        unk_D_84229EB0_00024* ptr = TeamSelection_RegisteredTeam_GetPrevious(arg0->currentCard, arg0->cursorIndex - arg0->scrollTop);
        s32 idx = ((arg0->unk_0000A * 3) + arg0->unk_00008) + 1;

        if (ptr->monSlots[idx].mon.species.dexId != 0) {
            arg0->unk_00008++;
            arg0->velocityX = 1;
            arg0->velocityY = 4;
            arg0->animTimer = 0;
            arg0->subState = 8;
            arg0->state = 9;
            Audio_PlaySoundEffectById(1);
        }
    }
}

void TeamSelection_RegisteredTeam_MovePokemonUp(unk_D_84229EB0* arg0) {
    if (arg0->unk_0000A > 0) {
        unk_D_84229EB0_00024* ptr = TeamSelection_RegisteredTeam_GetPrevious(arg0->currentCard, arg0->cursorIndex - arg0->scrollTop);
        s32 idx = ((arg0->unk_0000A * 3) + arg0->unk_00008) - 3;

        if (ptr->monSlots[idx].mon.species.dexId != 0) {
            arg0->unk_0000A--;
            arg0->velocityX = -1;
            arg0->velocityY = -4;
            arg0->animTimer = 0;
            arg0->subState = 8;
            arg0->state = 0xA;
            Audio_PlaySoundEffectById(1);
        }
    }
}

void TeamSelection_RegisteredTeam_MovePokemonDown(unk_D_84229EB0* arg0) {
    if (arg0->unk_0000A <= 0) {
        unk_D_84229EB0_00024* ptr = TeamSelection_RegisteredTeam_GetPrevious(arg0->currentCard, arg0->cursorIndex - arg0->scrollTop);
        s32 idx = ((arg0->unk_0000A * 3) + arg0->unk_00008) + 3;

        if (ptr->monSlots[idx].mon.species.dexId != 0) {
            arg0->unk_0000A = arg0->unk_0000A + 1;
            arg0->velocityX = 1;
            arg0->velocityY = 4;
            arg0->animTimer = 0;
            arg0->subState = 8;
            arg0->state = 0xA;
            Audio_PlaySoundEffectById(1);
        }
    }
}

void TeamSelection_RegisteredTeam_HandlePokemonInput(unk_D_84229EB0* arg0) {
    Controller* cont = &gControllers[arg0->controllerIndex];

    TeamSelection_RegisteredTeam_UpdatePreview(arg0);

    if (arg0->animTimer == 0xA) {
        if (TeamSelection_Preview_GetSelectionState(arg0->unk_134F0) != 0) {
            if (arg0->mode == 2) {
                arg0->subState = 6;
            } else {
                arg0->subState = 2;
            }
            arg0->state = 0xE;
        } else if (BTN_IS_PRESSED(cont, BTN_DUP)) {
            TeamSelection_RegisteredTeam_MovePokemonUp(arg0);
        } else if (BTN_IS_PRESSED(cont, BTN_DDOWN)) {
            TeamSelection_RegisteredTeam_MovePokemonDown(arg0);
        } else if (BTN_IS_PRESSED(cont, BTN_DRIGHT)) {
            TeamSelection_RegisteredTeam_MovePokemonRight(arg0);
        } else if (BTN_IS_PRESSED(cont, BTN_DLEFT)) {
            TeamSelection_RegisteredTeam_MovePokemonLeft(arg0);
        }
    }
    arg0->screenX = arg0->baseX;
    arg0->screenY = arg0->baseY;
    arg0->needsRedraw = 2;
}

void TeamSelection_RegisteredTeam_AdvanceCursorTransition(unk_D_84229EB0* arg0) {
    if (arg0->velocityY > 0) {
        arg0->velocityX++;
        if (arg0->velocityX >= arg0->velocityY) {
            arg0->state = arg0->subState;
        }
    } else {
        arg0->velocityX--;
        if (arg0->velocityY >= arg0->velocityX) {
            arg0->state = arg0->subState;
        }
    }
    arg0->screenX = arg0->baseX;
    arg0->screenY = arg0->baseY;
    arg0->needsRedraw = 2;
}

void TeamSelection_RegisteredTeam_AdvancePanelShrink(unk_D_84229EB0* arg0) {
    if (arg0->overrideHeight == 0x58) {
        if (arg0->animTimer == 0) {
            arg0->state = arg0->subState;
        } else {
            arg0->animTimer--;
        }
    } else {
        arg0->overrideHeight -= 0x2E;
    }
    arg0->needsRedraw = 2;
}

void TeamSelection_RegisteredTeam_AdvancePanelExpand(unk_D_84229EB0* arg0) {
    arg0->overrideHeight += 0x2E;
    if (arg0->overrideHeight == 0x110) {
        arg0->state = arg0->subState;
    }
    arg0->needsRedraw = 2;
}

void TeamSelection_RegisteredTeam_HandleRulePromptA(unk_D_84229EB0* arg0) {
    s16 sp26 = 4;

    if ((arg0->cursorIndex - arg0->scrollTop) == 2) {
        sp26 = 5;
    }

    TeamSelection_RulePrompt_Open(sp26, &gControllers[arg0->controllerIndex]);

    switch (TeamSelection_RulePrompt_TryFinish(sp26)) {
        case 0:
            arg0->state = 2;
            break;

        case 1:
            arg0->animTimer = 8;
            arg0->state = 0xD;
            arg0->subState = 4;
            break;

        case 2:
            arg0->state = 2;
            break;

        case 3:
            TeamSelection_RegisteredTeam_InitializeSelection(arg0);
            break;
    }

    arg0->screenX = arg0->baseX;
    arg0->screenY = arg0->baseY;
}

void TeamSelection_RegisteredTeam_HandleRulePromptB(unk_D_84229EB0* arg0) {
    s16 sp26 = 6;

    if ((arg0->cursorIndex - arg0->scrollTop) == 2) {
        sp26 = 7;
    }

    TeamSelection_RulePrompt_Open(sp26, &gControllers[arg0->controllerIndex]);

    switch (TeamSelection_RulePrompt_TryFinish(sp26)) {
        case 0:
            arg0->state = 2;
            break;

        case 1:
            arg0->animTimer = 0;
            arg0->state = 0xD;
            arg0->subState = 0xF;
            break;

        case 2:
            TeamSelection_RegisteredTeam_InitializeSelection(arg0);
            break;

        case 3:
            arg0->state = 2;
            break;
    }

    arg0->screenX = arg0->baseX;
    arg0->screenY = arg0->baseY;
}

void TeamSelection_RegisteredTeam_HandleSavePrompt(unk_D_84229EB0* arg0) {
    s32 sp24;

    if (Deck_FindFirstFreeTeamSlot() == 0) {
        sp24 = 0x13;
    } else {
        sp24 = 9;
    }

    TeamSelection_RulePrompt_Open(sp24, &gControllers[arg0->controllerIndex]);

    switch (TeamSelection_RulePrompt_TryFinish(sp24)) {
        case 1:
            arg0->animTimer = 0;
            arg0->state = 0xE;
            arg0->subState = 0x13;
            break;

        case 0:
        case 2:
            arg0->animTimer = 0;
            arg0->state = 4;
            break;
    }

    arg0->screenX = arg0->baseX;
    arg0->screenY = arg0->baseY;
}

void TeamSelection_RegisteredTeam_EditTeam(unk_D_84229EB0* arg0) {
    unk_D_84229EB0_00024* temp_v0;
    s16 temp_hi;
    s16 temp_lo;

    temp_v0 = TeamSelection_RegisteredTeam_GetPrevious(arg0->currentCard, arg0->cursorIndex - arg0->scrollTop);
    temp_lo = arg0->animTimer / 12;
    temp_hi = arg0->animTimer % 12;

    if (temp_lo < temp_v0->monCount) {
        if (temp_hi < 8) {
            temp_v0->monSlots[temp_lo].raw = temp_hi + 0x80000000;
        } else {
            temp_v0->monSlots[temp_lo].raw = 1;
        }

        if (temp_hi == 4) {
            temp_v0->monSlots[temp_lo].mon.species.dexId = 0;
            PokeIcon_LoadModelTextureForMon(temp_v0->monSlots[temp_lo].iconTexture, 0x84, NULL);
            Audio_PlaySoundEffectById(0x01100010);
        }
    }

    if (((temp_v0->monCount * 0xC) - 1) >= arg0->animTimer) {
        if (arg0->animTimer == ((temp_v0->monCount * 0xC) - 1)) {
            temp_v0->monCount = -1;
            RegisteredTeam_DeleteAndCompact(temp_v0);
            TeamSelection_RulePrompt_Open(8, &gControllers[arg0->controllerIndex]);
        }
        arg0->animTimer += 1;
    } else if (TeamSelection_RulePrompt_TryFinish(8) >= 0) {
        arg0->state = 7;
    }

    arg0->screenX = arg0->baseX;
    arg0->screenY = arg0->baseY;
    arg0->needsRedraw = 2;
}

void TeamSelection_RegisteredTeam_SaveFromTray(unk_D_84229EB0* arg0) {
    s32 i;
    unk_D_84229EB0_00024* temp_a3;
    s16 temp_hi;
    s16 temp_lo;
    unk_D_84229EB0_00024* temp_v0;

    temp_a3 = arg0->currentCard;
    temp_v0 = temp_a3->prev;

    temp_lo = arg0->animTimer / 12;
    temp_hi = arg0->animTimer % 12;

    if (arg0->animTimer == 0) {
        temp_a3->monCount = 0;
        for (i = 0; i < 6; i++) {
            if (temp_v0->monSlots[i].mon.species.dexId != 0) {
                temp_a3->monCount++;
            }
        }
    }

    if (temp_lo < temp_a3->monCount) {
        if (temp_hi < 8) {
            temp_a3->monSlots[temp_lo].raw = temp_hi + 0x80000000;
        } else {
            temp_a3->monSlots[temp_lo].raw = temp_v0->monSlots[temp_lo].raw;
        }

        if (temp_hi == 4) {
            temp_a3->monSlots[temp_lo].mon.species.dexId = temp_v0->monSlots[temp_lo].mon.species.dexId;
            _bcopy(temp_v0->monSlots[temp_lo].iconTexture, temp_a3->monSlots[temp_lo].iconTexture,
                   sizeof(temp_v0->monSlots[temp_lo].iconTexture));
            Audio_PlaySoundEffectById(0x01100010);
        }
    }

    if ((temp_a3->monCount * 0xC) - 1 >= arg0->animTimer) {
        if (arg0->animTimer == (temp_a3->monCount * 0xC) - 1) {
            RegisteredTeam_Save(temp_a3);
            TeamSelection_RulePrompt_Open(0xB, &gControllers[arg0->controllerIndex]);
        }
        arg0->animTimer++;
    } else if (TeamSelection_RulePrompt_TryFinish(0xB) >= 0) {
        if (Deck_FindFirstFreeTeamSlot() >= 0xA) {
            Rental_TeamTrayResume(arg0->unk_134E8);
            arg0->animTimer = 0;
            arg0->state = 4;
        } else {
            arg0->state = 0x12;
        }
    }

    arg0->screenX = arg0->baseX;
    arg0->screenY = arg0->baseY;
    arg0->needsRedraw = 2;
}

void TeamSelection_RegisteredTeam_HandleDiscardPrompt(unk_D_84229EB0* arg0) {
    TeamSelection_RulePrompt_Open(0xC, &gControllers[arg0->controllerIndex]);

    switch (TeamSelection_RulePrompt_TryFinish(0xC)) {
        case 1:
            Rental_ResetTeamSlots(arg0->unk_134E8);
            Rental_CarouselResumeAfterAssign(arg0->unk_134EC);
            arg0->animTimer = 0;
            arg0->state = 4;
            break;

        case 0:
        case 2:
            Rental_TeamTrayResume(arg0->unk_134E8);
            arg0->animTimer = 0;
            arg0->state = 4;
            break;
    }

    arg0->screenX = arg0->baseX;
    arg0->screenY = arg0->baseY;
}

void TeamSelection_RegisteredTeam_LoadVisible(unk_D_84229EB0* arg0) {
    s32 i;
    unk_D_84229EB0_00024* var_s1;

    arg0->animTimer++;

    if (arg0->animTimer == 4) {
        var_s1 = arg0->currentCard;
        for (i = 0; i < 3; i++) {
            TeamSelection_RegisteredTeam_Load(var_s1, arg0->scrollTop + i);
            var_s1 = var_s1->prev;
        }

        arg0->needsRedraw = 2;
        arg0->animTimer = 0;
        arg0->state = 2;
    }
}

s32 TeamSelection_RegisteredTeam_Update(unk_D_84229EB0* arg0) {
    s32 sp1C = 0;

    if (arg0->inputLock > 0) {
        arg0->inputLock--;
    }

    switch (arg0->state) {
        case 0:
            TeamSelection_RegisteredTeam_SetOpeningPosition(arg0);
            break;

        case 1:
            TeamSelection_RegisteredTeam_SetOpeningPosition(arg0);
            break;

        case 2:
            TeamSelection_RegisteredTeam_HandleListInput(arg0);
            break;

        case 3:
            TeamSelection_RegisteredTeam_AdvanceOpen(arg0);
            sp1C = 1;
            break;

        case 4:
            TeamSelection_RegisteredTeam_AdvanceClose(arg0);
            sp1C = 1;
            break;

        case 8:
            TeamSelection_RegisteredTeam_HandlePokemonInput(arg0);
            break;

        case 9:
            TeamSelection_RegisteredTeam_AdvanceCursorTransition(arg0);
            break;

        case 10:
            TeamSelection_RegisteredTeam_AdvanceCursorTransition(arg0);
            break;

        case 12:
            TeamSelection_RegisteredTeam_AdvanceCursorTransition(arg0);
            break;

        case 11:
            TeamSelection_RegisteredTeam_AdvanceCursorTransition(arg0);
            break;

        case 5:
            TeamSelection_RegisteredTeam_HandleRulePromptA(arg0);
            break;

        case 6:
            TeamSelection_RegisteredTeam_HandleRulePromptB(arg0);
            break;

        case 7:
            TeamSelection_RegisteredTeam_HandleSavePrompt(arg0);
            break;

        case 13:
            TeamSelection_RegisteredTeam_AdvancePanelShrink(arg0);
            sp1C = 1;
            break;

        case 14:
            TeamSelection_RegisteredTeam_AdvancePanelExpand(arg0);
            sp1C = 1;
            break;

        case 15:
            TeamSelection_RegisteredTeam_EditTeam(arg0);
            break;

        case 16:
            TeamSelection_RegisteredTeam_AdvanceReturnFromEdit(arg0);
            sp1C = 1;
            break;

        case 17:
            TeamSelection_RegisteredTeam_SaveFromTray(arg0);
            break;

        case 18:
            TeamSelection_RegisteredTeam_HandleDiscardPrompt(arg0);
            break;

        case 19:
            TeamSelection_RegisteredTeam_LoadVisible(arg0);
            break;
    }
    return sp1C;
}

s32 TeamSelection_RegisteredTeam_Initialize(unk_D_84229EB0* arg0, RentalTeamTray* arg1, RentalCarouselState* arg2, unk_D_8423D3A8* arg3, s16 arg4,
                  s16 arg5, s16 arg6, s16 arg7) {
    s32 i;

    arg0->mode = arg4;
    arg0->state = 1;
    arg0->controllerIndex = arg7;
    arg0->animTimer = 0;
    arg0->scrollTop = 0;
    arg0->cursorIndex = 0;
    arg0->unk_00008 = 0;
    arg0->unk_0000A = 0;
    arg0->baseX = arg5;
    arg0->baseY = arg6;
    arg0->screenX = arg5 + 0x280;
    arg0->screenY = arg6;
    arg0->overrideHeight = 0x110;
    arg0->unk_0001C = 0xA;
    arg0->inputLock = 0;
    arg0->inputLockNext = 8;
    arg0->unk_134E8 = arg1;
    arg0->unk_134EC = arg2;
    arg0->unk_134F0 = arg3;
    arg0->currentCard = arg0->cardPool;

    for (i = 0; i < 4; i++) {
        arg0->cardPool[i].prev = &arg0->cardPool[(i + 1) & 3];
        arg0->cardPool[i].next = &arg0->cardPool[(i - 1) & 3];
    }

    if (arg4 != 3) {
        if (arg4 != 4) {
            for (i = 0; i < 3; i++) {
                TeamSelection_RegisteredTeam_Load(&arg0->cardPool[i], arg0->scrollTop + i);
            }
        }
    }

    if (arg0->mode == 0) {
        arg0->unk_0001C = Deck_FindFirstFreeTeamSlot();
    }
    return 4;
}

s32 RegistrationManager_Begin(unk_D_84229EB0* arg0) {
    if (arg0->state != 1) {
        return 0;
    }
    arg0->exitRequested = 0;
    arg0->animTimer = 0;
    arg0->state = 3;
    return 1;
}

s32 RegistrationManager_Initialize(unk_D_84229EB0* arg0, u16 arg1, char* arg2, unk_D_838067F0_0168_0000* arg3) {
    unk_D_84229EB0_00024* ptr = arg0->cardPool;
    s16 i;

    _bcopy(arg3, &arg0->cardPool[0], sizeof(unk_D_838067F0_0168_0000) * 6);
    _bcopy(arg3, &arg0->cardPool[1], sizeof(unk_D_838067F0_0168_0000) * 6);
    ptr->slotIndex = Deck_FindFirstFreeTeamSlot();
    ptr->monCount = -1;
    ptr->trainerId = arg1;
    HAL_Strcpy(ptr->playerName, arg2);

    for (i = 0; i < 6; i++) {
        if (ptr->monSlots[i].mon.species.dexId != 0) {
            ptr->monSlots[i].mon.species.dexId = 0;
            ptr->monSlots[i].raw = 1;
            PokeIcon_LoadModelTextureForMon(ptr->monSlots[i].iconTexture, 0x84, NULL);
        } else {
            ptr->monSlots[i].raw = 0;
        }
    }

    arg0->animTimer = 0;
    arg0->state = 0x10;
    return 1;
}

s32 TeamSelection_RegisteredTeam_GetSelection(unk_D_84229EB0* arg0) {
    return arg0->state == 0;
}

s32 RegisteredTeam_LoadSelected(unk_D_84229EB0* arg0, unk_D_84229EB0_00024** arg1) {
    s32 i;
    s16 var_t0 = 0;

    *arg1 = NULL;

    if (arg0->exitRequested == 0) {
        *arg1 = TeamSelection_RegisteredTeam_GetPrevious(arg0->currentCard, arg0->cursorIndex - arg0->scrollTop);
        var_t0 = (*arg1)->monCount;

        for (i = 0; i < 6; i++) {
            (*arg1)->monSlots[i].mon.sourceAndFlags = 0x40;
            (*arg1)->monSlots[i].mon.sourceSlot = arg0->cursorIndex;
        }
    }
    return var_t0;
}

void TeamSelection_RegisteredTeam_ApplySelection(unk_D_84229EB0* arg0, s16 arg1) {
    s32 tmp;

    arg0->cursorIndex = arg1;
    if (arg0->unk_0001C < 4) {
        arg0->scrollTop = 0;
    } else {
        tmp = arg0->unk_0001C - 3;
        if (arg1 < tmp) {
            arg0->scrollTop = arg1;
        } else {
            arg0->scrollTop = tmp;
        }
    }

    for (arg1 = 0; arg1 < 3; arg1++) {
        TeamSelection_RegisteredTeam_Load(&arg0->cardPool[arg1], arg0->scrollTop + arg1);
    }
}
