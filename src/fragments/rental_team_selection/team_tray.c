#include "rental_team_selection.h"
#include "src/poke_icon.h"
#include "src/graphics_textures.h"
#include "src/pokemon_stats.h"
#include "src/save_data.h"
#include "src/audio_sfx.h"
#include "src/hal_libc.h"

s16 D_842115B0[] = { 0x78, 0xFC, 0x180, 0x78, 0xFC, 0x180 };
s16 D_842115BC[] = {
    4, 4, 4, 0x30, 0x30, 0x30,
};

void Rental_DrawTeamSlotFrame(s16 arg0, s16 arg1, s32 arg2) {
    if (arg2 & 0x80000000) {
        TeamSelection_DrawColoredFrame(arg0, arg1, 0x84, 0x2C, 0x64, 0x64, 0xC8, 0xFF);
    } else {
        TeamSelection_DrawColoredFrame(arg0, arg1, 0x84, 0x2C, 0x3C, 0x3C, 0xA0, 0xFF);
    }
}

void Rental_DrawTeamTrayFrame(RentalTeamTray* arg0, s16 arg1, s16 arg2) {
    s16 i;

    TeamSelection_DrawCornerFrame(arg1, arg2, 0x208, 0x60);
    if (((D_800AE540.modeCategory == 0) || (D_800AE540.modeCategory == 8)) && (arg0->needsRedraw != 0) && (arg0->filledCount > 0) &&
        (arg0->filledCount < 6)) {
        TeamSelection_DrawColoredFrame(arg1 + 4, arg2 + 4, 0x74, 0x58, 0, 0, 0, 0xFF);

        gSPDisplayList(gDisplayListHead++, D_8006F518);

        Gfx_DrawTextureRgba16(arg1 + 0x32, arg2 + 0x32, 0x14, 0x14, D_20068E0, 0x14, 0);

        gSPDisplayList(gDisplayListHead++, D_8006F630);
    } else {
        TeamSelection_DrawColoredFrame(arg1 + 4, arg2 + 4, 0x74, 0x58, 0x64, 0x64, 0xC8, 0xFF);
    }

    for (i = 0; i < 6; i++) {
        s16 tmp1 = D_842115B0[i] + arg1;
        s16 tmp2 = D_842115BC[i] + arg2;

        Rental_DrawTeamSlotFrame(tmp1, tmp2, arg0->monSlots[i].raw);
    }
}

void Rental_DrawTeamCursor(RentalTeamTray* arg0) {
    s16 temp_s0 = arg0->unk_0018 + (arg0->column * 0x84) + 0x78;
    s16 temp_a1 = arg0->unk_001A + (arg0->row * 0x2C) + 4;

    if (arg0->state == 0xD) {
        if (arg0->unk_000C > 0) {
            temp_a1 = ((temp_a1 + ((arg0->unk_000C * 0x2C) / 4)) - 0x2C);
        } else {
            temp_a1 = (temp_a1 + ((arg0->unk_000C * 0x2C) / 4) + 0x2C);
        }
        TeamSelection_DrawAnimatedGoldCorners(temp_s0, temp_a1, 0x84, 0x2C);
    } else if (arg0->state == 0xC) {
        if (arg0->unk_000C > 0) {
            temp_s0 = ((temp_s0 + ((arg0->unk_000C * 0x84) / 4)) - 0x84);
        } else {
            temp_s0 = (temp_s0 + ((arg0->unk_000C * 0x84) / 4) + 0x84);
        }
        TeamSelection_DrawAnimatedGoldCorners(temp_s0, temp_a1, 0x84, 0x2C);
    } else if (arg0->state == 8) {
        TeamSelection_DrawAnimatedGoldCorners(temp_s0, temp_a1, 0x84, 0x2C);
    }
}

void Rental_DrawTeamTray(RentalTeamTray* arg0, s32 arg1) {
    char* temp_v0_10;
    char* temp_v0_4;
    char* temp_v0_5;
    char* temp_v0_6;
    char* temp_v0_7;
    char* temp_v0_8;
    s32 i;
    s16 sp42;
    s16 sp40;
    char* temp_v0_9;

    if ((arg0->state != 1) && (arg0->state != 0) && ((arg1 != 0) || (arg0->subState != 0))) {
        Rental_DrawTeamTrayFrame(arg0, arg0->unk_0018, arg0->unk_001A);

        gSPDisplayList(gDisplayListHead++, D_8006F518);

        for (i = 0; i < 6; i++) {
            if (arg0->monSlots[i].mon.species.dexId != 0) {
                s16 tmp1 = D_842115B0[i] + arg0->unk_0018 + 2;
                s16 tmp2 = D_842115BC[i] + arg0->unk_001A + 2;

                Gfx_DrawTextureRgba16(tmp1, tmp2, 0x28, 0x28, arg0->monSlots[i].iconTexture, 0x28, 0);
            }
        }

        gSPDisplayList(gDisplayListHead++, D_8006F630);

        Font_BeginTranslucentTextRendering();

        if (((D_800AE540.modeCategory == 0) || (D_800AE540.modeCategory == 8)) && (arg0->needsRedraw != 0) &&
            (arg0->filledCount > 0) && (arg0->filledCount < 6)) {
            Font_SetActive(8, 0);
            temp_v0_4 = TeamSelection_GetLabelText(arg0->filledCount + 0x2A);
            Font_Printf((arg0->unk_0018 - (Font_MeasureTextExtent(8, 0, temp_v0_4) / 2)) + 0x3C, arg0->unk_001A + 8,
                          temp_v0_4);
            if (arg0->mode == 0) {
                temp_v0_5 = TeamSelection_GetLabelText(0x31);
                Font_Printf((arg0->unk_0018 - (Font_MeasureTextExtent(8, 0, temp_v0_5) / 2)) + 0x3C, arg0->unk_001A + 0x1C,
                              temp_v0_5);
            } else {
                temp_v0_6 = TeamSelection_GetLabelText(0x32);
                Font_Printf((arg0->unk_0018 - (Font_MeasureTextExtent(8, 0, temp_v0_6) / 2)) + 0x3C, arg0->unk_001A + 0x1C,
                              temp_v0_6);
            }
            Font_SetActive(4, 0);
            temp_v0_7 = TeamSelection_GetLabelText(0x33);
            Font_Printf((arg0->unk_0018 - (Font_MeasureTextExtent(4, 0, temp_v0_7) / 2)) + 0x3C, arg0->unk_001A + 0x46,
                          temp_v0_7);
        } else {
            Font_SetActive(4, 0);
            if (arg0->mode == 0) {
                temp_v0_8 = TeamSelection_GetLabelText(0x34);
                Font_Printf((arg0->unk_0018 - (Font_MeasureTextExtent(4, 0, temp_v0_8) / 2)) + 0x3C, arg0->unk_001A + 9,
                              temp_v0_8);
            } else {
                temp_v0_9 = TeamSelection_GetLabelText(0x35);
                Font_Printf((arg0->unk_0018 - (Font_MeasureTextExtent(4, 0, temp_v0_9) / 2)) + 0x3C, arg0->unk_001A + 9,
                              temp_v0_9);
            }
            temp_v0_10 = TeamSelection_GetLabelText(0x36);
            Font_Printf((arg0->unk_0018 - (Font_MeasureTextExtent(4, 0, temp_v0_10) / 2)) + 0x3C, arg0->unk_001A + 0x1A,
                          temp_v0_10);
            Font_SetActive(8, 0);
            Font_Printf((arg0->unk_0018 - (Font_MeasureTextExtent(8, 0, arg0->unk_001E) / 2)) + 0x3C, arg0->unk_001A + 0x2F,
                          arg0->unk_001E);
            Font_SetActive(4, 0);
            Font_Printf((arg0->unk_0018 - (Font_MeasureTextExtent(4, 0, "ID  00000") / 2)) + 0x3C, arg0->unk_001A + 0x46,
                          "ID  %05d", arg0->unk_001C);
        }
        Font_SetActive(4, 0);

        for (i = 0; i < 6; i++) {
            if (arg0->monSlots[i].mon.species.dexId != 0) {
                s16 tmp1 = D_842115B0[i] + arg0->unk_0018 + 0x2C;
                s16 tmp2 = D_842115BC[i] + arg0->unk_001A + 5;

                Font_Printf(tmp1, tmp2, arg0->monSlots[i].mon.nickname);
            }
        }

        Font_SetActive(4, 0);

        for (i = 0; i < 6; i++) {
            if (arg0->monSlots[i].mon.species.dexId != 0) {
                sp42 = D_842115B0[i] + arg0->unk_0018 + 0x2C;
                sp40 = D_842115BC[i] + arg0->unk_001A + 0x17;
                Font_Printf(sp42, sp40, "%s %d", TeamSelection_GetInstructionText(0x15), arg0->monSlots[i].mon.level);
            }
        }

        Font_EndTexturedTextRendering();
        Rental_DrawTeamCursor(arg0);

        if (arg0->subState > 0) {
            arg0->subState--;
        }
    }
}

void Rental_ClearTeamSlot(RentalTeamTray* arg0) {
    s16 temp_v0 = arg0->column + (arg0->row * 3);

    if (arg0->monSlots[temp_v0].mon.species.dexId != 0) {
        arg0->monSlots[temp_v0].mon.species.dexId = 0;
        arg0->monSlots[temp_v0].raw = 0;
        arg0->filledCount -= 1;
    }
}

void Rental_BeginSlotReplace(RentalTeamTray* arg0) {
    s16 sp1E = arg0->column + (arg0->row * 3);

    if (Rental_CarouselResumeInput(arg0->unk_4D40) != 0) {
        if (arg0->monSlots[sp1E].mon.species.dexId != 0) {
            arg0->monSlots[sp1E].mon.species.dexId = 0;
            arg0->monSlots[sp1E].raw = 0;
            arg0->filledCount--;
        }
        arg0->monSlots[sp1E].raw |= 0x80000000;
        arg0->needsRedraw = 1;
        arg0->state = 3;
    }
}

s32 Rental_SaveTeamToDeck(RentalTeamTray* arg0) {
    s16 i;
    s16 temp_v0;
    s32 var_s0 = 0;
    DeckHandle* temp_s3;

    temp_v0 = Deck_FindFirstFreeTeamSlot();
    if (temp_v0 < 0xA) {
        temp_s3 = Deck_OpenAndSetName(0x10, 0, temp_v0, arg0->unk_001E, arg0->unk_001E, arg0->unk_001C);

        for (i = 0; i < arg0->filledCount; i++) {
            Deck_WriteEntries(&arg0->monSlots[i].mon.species.dexId, 1, temp_s3);
        }

        Deck_CloseAndFlush(temp_s3);
        Save_FlushDirtyBanks();
        var_s0 = 1;
    }
    return var_s0;
}

s32 Rental_CheckTeamLevelCap(RentalTeamTray* arg0) {
    UNUSED s32 pad;
    s32 temp_v1;
    s16 i;
    s16 sp8[3];

    for (i = 0; i < 3; i++) {
        sp8[i] = 0x3E7;
    }

    for (i = 0; i < arg0->filledCount; i++) {
        temp_v1 = arg0->monSlots[i].mon.level;
        if (sp8[2] >= temp_v1) {
            sp8[2] = temp_v1;
        }

        if (sp8[1] >= temp_v1) {
            sp8[2] = sp8[1];
            sp8[1] = temp_v1;
        }

        if (sp8[0] >= temp_v1) {
            sp8[1] = sp8[0];
            sp8[0] = temp_v1;
        }
    }

    switch (D_800AE540.modeCategory) {
        default:
            temp_v1 = 0;
            break;

        case 3:
            temp_v1 = (s16)(sp8[0] + sp8[1] + sp8[2]);
            temp_v1 = temp_v1 >= 0x9C;
            break;

        case 4:
            temp_v1 = (s16)(sp8[0] + sp8[1] + sp8[2]);
            temp_v1 = temp_v1 >= 0x51;
            break;

        case 5:
            temp_v1 = (s16)(sp8[0] + sp8[1] + sp8[2]);
            temp_v1 = temp_v1 >= 0x33;
            break;
    }
    return temp_v1;
}

void Rental_TeamTrayHide(RentalTeamTray* arg0) {
    arg0->unk_0018 = arg0->unk_0014 + 0x280;
    arg0->unk_001A = arg0->unk_0016;
}

void Rental_TeamTrayShow(RentalTeamTray* arg0) {
    arg0->unk_0018 = arg0->unk_0014;
    arg0->unk_001A = arg0->unk_0016;
}

void Rental_OpenSlotAssignmentDialog(RentalTeamTray* a0) {
    RentalTeamTray* arg0 = a0;
    s16 temp_v0;
    s16 temp_v1;
    s16 tmp;

    TeamSelection_RulePrompt_BeginClose(1);
    if (TeamSelection_RulePrompt_TryFinish(1) >= 0) {
        tmp = a0->column + a0->row * 3;

        temp_v0 = a0->unk_0014 + (a0->column * 0x84) + 0x74;
        temp_v1 = a0->unk_0016 + (a0->row * 0x2C);

        arg0->monSlots[tmp].raw |= 0x80000000;
        TeamSelection_Preview_StartFull(a0->unk_4D44, a0->unk_0002, 0x30, 0xEC, temp_v0, temp_v1, &a0->monSlots[tmp], 2);
        a0->state = 6;
    }
    a0->unk_0018 = a0->unk_0014;
    a0->unk_001A = a0->unk_0016;
    a0->subState = 2;
}

void Rental_ResolveSlotPick(RentalTeamTray* arg0) {
    s16 sp26 = arg0->column + (arg0->row * 3);
    s32 temp_v0 = TeamSelection_Preview_GetSelectionState(arg0->unk_4D44);

    if (temp_v0 == 1) {
        Rental_CarouselResumeInput(arg0->unk_4D40);
        arg0->needsRedraw = 0;
        arg0->state = 3;
    } else if (temp_v0 == 2) {
        arg0->monSlots[sp26].raw &= ~0x80000000;
        TeamSelection_RulePrompt_Open(1, NULL);
        arg0->unk_000A = 0;
        arg0->state = 8;
    }
    arg0->unk_0018 = arg0->unk_0014;
    arg0->unk_001A = arg0->unk_0016;
}

void Rental_TeamTraySlideIn(RentalTeamTray* arg0) {
    arg0->unk_000A++;
    if (arg0->unk_000A <= 0) {
        arg0->unk_0018 = arg0->unk_0014 + 0x280;
        arg0->subState = 2;
    } else if (arg0->unk_000A == 0xA) {
        arg0->unk_000A = 0;
        arg0->unk_0018 = arg0->unk_0014;
        if (arg0->filledCount == arg0->lastFilledCount) {
            arg0->subState = 2;
            arg0->state = 7;
        } else {
            // clang-format off
            *(s8*)&arg0->state = 2; arg0->subState = 2;
            // clang-format on
        }
    } else {
        arg0->unk_0018 = (arg0->unk_0014 - (arg0->unk_000A << 6)) + 0x280;
        arg0->subState = 2;
    }
    arg0->unk_001A = arg0->unk_0016;
}

void Rental_TeamTraySlideOut(RentalTeamTray* arg0) {
    arg0->unk_000A++;
    if (arg0->unk_000A == 0xA) {
        arg0->state = 0;
        arg0->unk_000A = 0;
        arg0->unk_0018 = arg0->unk_0014 + 0x280;
    } else {
        arg0->unk_0018 = arg0->unk_0014 - (arg0->unk_000A << 6);
    }
    arg0->unk_001A = arg0->unk_0016;
    arg0->subState = 2;
}

void Rental_BeginSlotReplacement(RentalTeamTray* arg0) {
    Rental_BeginSlotReplace(arg0);
    arg0->unk_0018 = arg0->unk_0014;
    arg0->unk_001A = arg0->unk_0016;
    arg0->subState = 2;
}

void Rental_TeamCursorLeft(RentalTeamTray* arg0) {
    if (arg0->column > 0) {
        arg0->column--;
        arg0->unk_000C = -1;
        arg0->unk_000E = -4;
        arg0->state = 0xC;
        Audio_PlaySoundEffectById(1);
    }
}

void Rental_TeamCursorRight(RentalTeamTray* arg0) {
    if (arg0->column < 2) {
        arg0->column++;
        arg0->unk_000C = 1;
        arg0->unk_000E = 4;
        arg0->state = 0xC;
        Audio_PlaySoundEffectById(1);
    }
}

void Rental_TeamCursorUp(RentalTeamTray* arg0) {
    if (arg0->row > 0) {
        arg0->row--;
        arg0->unk_000C = -1;
        arg0->unk_000E = -4;
        arg0->state = 0xD;
        Audio_PlaySoundEffectById(1);
    }
}

void Rental_TeamCursorDown(RentalTeamTray* arg0) {
    if (arg0->row <= 0) {
        arg0->row++;
        arg0->unk_000C = 1;
        arg0->unk_000E = 4;
        arg0->state = 0xD;
        Audio_PlaySoundEffectById(1);
    }
}

void Rental_TeamGridHandleInput(RentalTeamTray* arg0) {
    Controller* cont = &gControllers[arg0->unk_0002];

    if (arg0->unk_000A < 8) {
        arg0->unk_000A++;
    } else if (BTN_IS_PRESSED(cont, BTN_A)) {
        s32 temp_v0 = arg0->column + (arg0->row * 3);

        if (arg0->monSlots[temp_v0].mon.species.dexId != 0) {
            Audio_PlaySoundEffectById(2);
            arg0->state = 0xB;
        }
    } else if (BTN_IS_PRESSED(cont, BTN_B)) {
        Audio_PlaySoundEffectById(3);
        arg0->state = 0x10;
    } else if (BTN_IS_PRESSED(cont, BTN_DUP)) {
        Rental_TeamCursorUp(arg0);
    } else if (BTN_IS_PRESSED(cont, BTN_DDOWN)) {
        Rental_TeamCursorDown(arg0);
    } else if (BTN_IS_PRESSED(cont, BTN_DRIGHT)) {
        Rental_TeamCursorRight(arg0);
    } else if (BTN_IS_PRESSED(cont, BTN_DLEFT)) {
        Rental_TeamCursorLeft(arg0);
    }

    arg0->unk_0018 = arg0->unk_0014;
    arg0->unk_001A = arg0->unk_0016;
    arg0->subState = 2;
}

void Rental_ConfirmTeam(RentalTeamTray* arg0) {
    TeamSelection_RulePrompt_BeginClose(1);
    if (TeamSelection_RulePrompt_TryFinish(1) >= 0) {
        if (Rental_CheckTeamLevelCap(arg0) != 0) {
            arg0->state = 0xF;
        } else if (arg0->mode == 0) {
            arg0->state = 7;
        } else {
            arg0->state = 0xE;
        }
    }
    arg0->subState = 2;
}

void Rental_HandleTeamConfirmation(RentalTeamTray* arg0) {
    Controller* cont = &gControllers[arg0->unk_0002];
    s16 i;

    TeamSelection_RulePrompt_Open(0, cont);

    switch (TeamSelection_RulePrompt_TryFinish(0)) {
        case 0:
            i = arg0->filledCount;
            if (i == 6) {
                i--;
            }
            arg0->column = i % 3;
            arg0->row = i / 3;
            arg0->lastFilledCount = 6;
            arg0->state = 2;
            break;

        case 1:
            Rental_CarouselAcknowledgeAssign(arg0->unk_4D40);
            arg0->unk_000A = 0;
            arg0->state = 5;
            break;

        case 2:
            if (Rental_SaveTeamToDeck(arg0) != 0) {
                arg0->state = 9;
            } else {
                arg0->state = 0xA;
            }
            break;

        case 3:
            TeamSelection_RulePrompt_Open(1, NULL);
            arg0->column = 0;
            arg0->row = 0;
            arg0->unk_000A = 0;
            arg0->state = 8;
            break;

        case 4:
            for (i = 0; i < 6; i++) {
                arg0->monSlots[i].raw = 0;
                arg0->monSlots[i].mon.species.dexId = 0;
            }

            Rental_CarouselRequestRedraw(arg0->unk_4D40);

            arg0->filledCount = 0;
            arg0->column = 0;
            arg0->row = 0;
            arg0->lastFilledCount = 6;
            arg0->state = 2;
            break;
    }
    arg0->unk_0018 = arg0->unk_0014;
    arg0->unk_001A = arg0->unk_0016;
}

void Rental_HandleRegisteredTeamConfirmation(RentalTeamTray* arg0) {
    Controller* cont = &gControllers[arg0->unk_0002];
    s16 i;

    TeamSelection_RulePrompt_Open(0xA, cont);
    switch (TeamSelection_RulePrompt_TryFinish(0xA)) {
        case 0:
            i = arg0->filledCount;
            if (i == 6) {
                i--;
            }
            arg0->column = i % 3;
            arg0->row = i / 3;
            arg0->lastFilledCount = 6;
            arg0->state = 2;
            break;

        case 1:
            Rental_CarouselAcknowledgeAssign(arg0->unk_4D40);
            RegistrationManager_Initialize(arg0->unk_4D48, arg0->unk_001C, arg0->unk_001E, arg0->monSlots);
            arg0->state = 3;
            break;

        case 2:
            TeamSelection_RulePrompt_Open(1, NULL);
            arg0->column = 0;
            arg0->row = 0;
            arg0->unk_000A = 0;
            arg0->state = 8;
            break;

        case 3:
            for (i = 0; i < 6; i++) {
                arg0->monSlots[i].raw = 0;
                arg0->monSlots[i].mon.species.dexId = 0;
            }

            Rental_CarouselRequestRedraw(arg0->unk_4D40);

            arg0->filledCount = 0;
            arg0->column = 0;
            arg0->row = 0;
            arg0->lastFilledCount = 6;
            arg0->state = 2;
            break;
    }

    arg0->unk_0018 = arg0->unk_0014;
    arg0->unk_001A = arg0->unk_0016;
}

void Rental_HandleLevelCapWarning(RentalTeamTray* arg0) {
    UNUSED s16 pad;
    s16 sp1C = 0xD;
    s32 temp_v0_2;
    Controller* cont = &gControllers[arg0->unk_0002];

    switch (D_800AE540.modeCategory) {
        case 3:
            break;

        case 4:
            sp1C = 0xE;
            break;

        case 5:
            sp1C = 0xF;
            break;
    }

    TeamSelection_RulePrompt_Open(sp1C, cont);

    temp_v0_2 = TeamSelection_RulePrompt_TryFinish(sp1C);
    if (temp_v0_2 != -1) {
        if (temp_v0_2 == 0) {
            arg0->column = 2;
            arg0->row = 1;
            arg0->lastFilledCount = 6;
            arg0->state = 2;
        } else {
            TeamSelection_RulePrompt_Open(1, NULL);
            arg0->column = 0;
            arg0->row = 0;
            arg0->unk_000A = 0;
            arg0->state = 8;
        }
    }

    arg0->unk_0018 = arg0->unk_0014;
    arg0->unk_001A = arg0->unk_0016;
}

void Rental_HandleTeamSaveResult(RentalTeamTray* arg0) {
    Controller* cont = &gControllers[arg0->unk_0002];

    TeamSelection_RulePrompt_Open(2, cont);
    if (TeamSelection_RulePrompt_TryFinish(2) >= 0) {
        Rental_CarouselAcknowledgeAssign(arg0->unk_4D40);
        arg0->unk_000A = 0;
        arg0->state = 5;
    }
}

void Rental_HandleTeamCancel(RentalTeamTray* arg0) {
    Controller* cont = &gControllers[arg0->unk_0002];

    TeamSelection_RulePrompt_Open(3, cont);
    if (TeamSelection_RulePrompt_TryFinish(3) >= 0) {
        arg0->state = 7;
    }
}

void Rental_TeamTraySlideTick(RentalTeamTray* arg0) {
    if (arg0->unk_000E > 0) {
        arg0->unk_000C++;
        if (arg0->unk_000C >= arg0->unk_000E) {
            arg0->state = 8;
        }
    } else {
        arg0->unk_000C--;
        if (arg0->unk_000E >= arg0->unk_000C) {
            arg0->state = 8;
        }
    }
    arg0->unk_0018 = arg0->unk_0014;
    arg0->unk_001A = arg0->unk_0016;
    arg0->subState = 2;
}

s32 Rental_TeamSlotsUpdate(RentalTeamTray* arg0) {
    s32 sp1C = 0;

    switch (arg0->state) {
        case 0:
            Rental_TeamTrayHide(arg0);
            break;

        case 1:
            Rental_TeamTrayHide(arg0);
            break;

        case 3:
            Rental_TeamTrayShow(arg0);
            break;

        case 2:
            Rental_BeginSlotReplacement(arg0);
            break;

        case 4:
            Rental_TeamTraySlideIn(arg0);
            sp1C = 1;
            break;

        case 5:
            Rental_TeamTraySlideOut(arg0);
            sp1C = 1;
            break;

        case 6:
            Rental_ResolveSlotPick(arg0);
            break;

        case 7:
            Rental_HandleTeamConfirmation(arg0);
            break;

        case 8:
            Rental_TeamGridHandleInput(arg0);
            break;

        case 9:
            Rental_HandleTeamSaveResult(arg0);
            break;

        case 10:
            Rental_HandleTeamCancel(arg0);
            break;

        case 11:
            Rental_OpenSlotAssignmentDialog(arg0);
            break;

        case 12:
            Rental_TeamTraySlideTick(arg0);
            break;

        case 13:
            Rental_TeamTraySlideTick(arg0);
            break;

        case 14:
            Rental_HandleRegisteredTeamConfirmation(arg0);
            break;

        case 15:
            Rental_HandleLevelCapWarning(arg0);
            break;

        case 16:
            Rental_ConfirmTeam(arg0);
            break;
    }
    return sp1C;
}

s32 Rental_InitTeamTray(RentalTeamTray* arg0, RentalCarouselState* arg1, unk_D_8423D3A8* arg2, unk_D_84229EB0* arg3, s16 arg4,
                  u16 arg5, char* arg6, s16 arg7, s16 arg8, s16 arg9) {
    s32 i;

    arg0->mode = arg4;
    arg0->state = 1;
    arg0->unk_0002 = arg9;
    arg0->needsRedraw = 0;
    arg0->filledCount = 0;
    arg0->lastFilledCount = 6;
    arg0->unk_000A = 0;
    arg0->column = 0;
    arg0->row = 0;
    arg0->unk_0014 = arg7;
    arg0->unk_0016 = arg8;
    arg0->unk_0018 = arg7 + 0x280;
    arg0->unk_001A = arg8;
    arg0->unk_001C = arg5;

    arg0->unk_4D40 = arg1;
    arg0->unk_4D44 = arg2;
    arg0->unk_4D48 = arg3;

    for (i = 0; i < 6; i++) {
        arg0->monSlots[i].raw = 0;
        arg0->monSlots[i].mon.species.dexId = 0;
    }

    HAL_Strcpy(arg0->unk_001E, arg6);
    return 1;
}

s32 Rental_TeamTrayOpen(RentalTeamTray* arg0, s16 arg1) {
    if (arg0->state != 1) {
        return 0;
    }
    arg0->state = 4;
    arg0->unk_000A = -arg1;
    return 1;
}

s32 Rental_TeamTrayResume(RentalTeamTray* arg0) {
    if (arg0->state != 3) {
        return 0;
    }
    arg0->state = 5;
    arg0->unk_000A = 0;
    return 1;
}

s32 Rental_ResetTeamSlots(RentalTeamTray* arg0) {
    s16 i;

    if (arg0->state != 3) {
        return 0;
    }

    for (i = 0; i < 6; i++) {
        arg0->monSlots[i].raw = 0;
        arg0->monSlots[i].mon.species.dexId = 0;
    }

    Rental_CarouselRequestRedraw(arg0->unk_4D40);
    arg0->filledCount = 0;
    arg0->lastFilledCount = 6;
    arg0->column = 0;
    arg0->row = 0;
    arg0->state = 2;
    return 1;
}

s32 Rental_RemoveHighlightedSlot(RentalTeamTray* arg0) {
    s32 temp_v0;
    s32 temp_v1;

    temp_v1 = arg0->column + (arg0->row * 3);

    if (arg0->state == 3) {
        if (arg0->filledCount == 0) {
            arg0->state = 5;
            arg0->unk_000A = 0;
            return -1;
        }

        arg0->monSlots[temp_v1].raw &= 0x7FFFFFFF;

        if (arg0->monSlots[temp_v1].raw == 0) {
            temp_v0 = temp_v1 - 1;
            arg0->column = temp_v0 % 3;
            arg0->row = temp_v0 / 3;
            Rental_ClearTeamSlot(arg0);
            arg0->state = 2;
        } else {
            TeamSelection_RulePrompt_Open(1, NULL);
            arg0->unk_000A = 0;
            arg0->state = 8;
        }
        return 1;
    }

    return 0;
}

s32 Rental_AssignMonToSlot(RentalTeamTray* arg0, unk_D_838067F0_0168_0000* arg1) {
    s32 var_v0 = arg0->column + (arg0->row * 3);

    if (arg0->state != 3) {
        return 0;
    }

    if (arg0->monSlots[var_v0].mon.species.dexId == 0) {
        arg0->filledCount += 1;
    }

    arg0->monSlots[var_v0] = *arg1;

    if (arg0->filledCount == arg0->lastFilledCount) {
        arg0->subState = 2;
        arg0->needsRedraw = 0;
        if (Rental_CheckTeamLevelCap(arg0) != 0) {
            arg0->state = 15;
        } else if (arg0->mode == 0) {
            arg0->state = 7;
        } else {
            arg0->state = 0xE;
        }
    } else {
        do {
            var_v0 = (var_v0 + 1) % 6;
        } while (arg0->monSlots[var_v0].mon.species.dexId != 0);
        arg0->column = var_v0 % 3;
        arg0->row = var_v0 / 3;
        arg0->state = 2;
    }
    return 1;
}

s32 Rental_ClearCurrentSlotSelection(RentalTeamTray* arg0) {
    s32 idx = (arg0->column + (arg0->row * 3));

    if (arg0->state != 3) {
        return 0;
    }

    if (arg0->filledCount == 0) {
        return 0;
    }

    if (arg0->needsRedraw == 0) {
        return 0;
    }

    if ((D_800AE540.modeCategory != 0) && (D_800AE540.sessionMode != 8)) {
        return 0;
    }

    arg0->subState = 2;
    arg0->needsRedraw = 0;
    arg0->lastFilledCount = arg0->filledCount;
    arg0->monSlots[idx].raw = 0;

    if (arg0->mode == 0) {
        arg0->state = 7;
    } else {
        arg0->state = 0xE;
    }
    return 1;
}

s32 Rental_CanAddMonToTeam(RentalTeamTray* arg0, unk_D_838067F0_0168_0000* arg1) {
    s32 i;
    u8 temp_a2;
    unk_D_838067F0_0168_0000* ptr;

    ptr = &arg0->monSlots[0];
    temp_a2 = arg1->mon.species.dexId;

    if (arg1->mon.species.dexId == 0) {
        return 0;
    }

    for (i = 0; i < 6; i++, ptr++) {
        if ((ptr->mon.species.dexId != 0) && !(ptr->raw & 0x80000000)) {
            if (D_800AE540.modeCategory == 0) {
                if (ptr->raw == arg1->raw) {
                    return 0;
                }
            } else if (ptr->mon.species.dexId == temp_a2) {
                return 0;
            }
        }
    }

    return 1;
}

s32 Rental_TeamContainsMon(RentalTeamTray* arg0, unk_D_838067F0_0168_0000* arg1) {
    s32 i;
    unk_D_838067F0_0168_0000* ptr;

    if (arg1->mon.species.dexId == 0) {
        return 0;
    }

    ptr = &arg0->monSlots[0];
    for (i = 0; i < 6; i++, ptr++) {
        if ((ptr->mon.species.dexId != 0) && !(ptr->raw & 0x80000000) && (ptr->raw == arg1->raw)) {
            return 1;
        }
    }

    return 0;
}

s32 Rental_TeamTrayIsIdle(RentalTeamTray* arg0) {
    return arg0->state == 0;
}

void Rental_CopyRosterToTeam(RentalTeamTray* arg0, BattleMon* arg1, s16 arg2) {
    s32 i;

    arg0->filledCount = arg2;
    arg0->lastFilledCount = arg2;

    for (i = 0; i < arg2; i++) {
        PokeIcon_LoadModelTextureForMon(arg0->monSlots[i].iconTexture, 0, &arg1[i]);

        arg0->monSlots[i].mon = arg1[i];

        arg0->monSlots[i].state = (arg1[i].sourceAndFlags & 0x70) >> 4;
        arg0->monSlots[i].deckSlot = arg1[i].sourceAndFlags & 0xF;
        arg0->monSlots[i].partyIndex = arg1[i].sourceSlot;
        arg0->monSlots[i].speciesId = arg1[i].species.dexId;
    }
}
