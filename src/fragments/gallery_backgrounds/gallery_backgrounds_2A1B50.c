#include "gallery_backgrounds.h"
#include "src/graphics_textures.h"
#include "src/ui_graphics.h"
#include "src/gb_data.h"
#include "src/save_data.h"
#include "src/gfx_rect.h"
#include "src/controller_ram.h"
#include "src/memory.h"

unk_D_83402EE0 gDisplayableMonIndices[24];
s32 gMonSelectionFlags[24];
PhotoSceneData gGalleryEntries[36];
static PhotoSceneData gGalleryEntriesBackup[36];
PhotoSceneData gGalleryCamEntries[21];
static PhotoSceneData gGalleryCamEntriesBackup[21];
PhotoSceneData* gCurrentCamEntry;
PhotoSceneData* gCurrentCamEntryPage;
static PhotoSceneControl* gCurrentCamEntrySpecial;
PhotoMonSpecies gGallerySpecialSpecies;
s32 gGallerySpecialUnlocked;
s32 gGallerySpecialViewed;
s32 gGallerySpecialUnlockEnabled;
static ModeSettings gGallerySessionSettings;
GalleryCursor gGallerySelection;
GalleryCursor gGalleryMainSelectionCam;
GalleryCursor gGallerySubSelection;
GalleryCursor gGalleryTertiarySelection;
unk_D_83407B18 gGalleryFinalSelection;
GalleryCursor gGalleryMainSelection;
GallerySceneGrid gGalleryCamList;

s32 gGalleryPhotoPoolCount = 0;
s32 gGalleryPhotoPoolMin = 1;
s32 gGalleryPhotoPoolMax = 0;
PhotoSceneData* gCurrentGalleryEntryPtr = NULL;

void Gallery_LoadUnlockedBackgrounds(void) {
    s32 i;

    main_pool_push_state('bkgl');

    Save_EnsureBankLoaded(2);
    bzero(gGalleryEntries, sizeof(PhotoSceneData) * 36);
    bzero(gGalleryEntriesBackup, sizeof(PhotoSceneData) * 36);
    bzero(&gGalleryCamEntries, sizeof(PhotoSceneData) * 21);
    bzero(&gGalleryCamEntriesBackup, sizeof(PhotoSceneData) * 21);

    for (i = 0; i < 36; i++) {
        Deck_ReadSaveEntry(0x16, 0, i, &gGalleryEntries[i]);
    }

    _bcopy(&gGalleryEntries, &gGalleryEntriesBackup, sizeof(PhotoSceneData) * 36);

    for (i = 0; i < 21; i++) {
        Deck_ReadSaveEntry(0x17, 0, i, &gGalleryCamEntries[i]);
    }

    Save_GetModeSettings(&gGallerySessionSettings, 1);
    if (gGallerySessionSettings.clearedModes == 0x1F8) {
        gGallerySpecialUnlockEnabled = 1;
    } else {
        gGallerySpecialUnlockEnabled = 0;
    }

    gCurrentCamEntry = &gGalleryCamEntries[0];
    gCurrentCamEntryPage = &gGalleryCamEntries[4];
    gCurrentCamEntrySpecial = &gGalleryCamEntries[20];

    if (Gallery_ControlRecordIsPopulated(gCurrentCamEntrySpecial) != 0) {
        gGallerySpecialUnlocked = 1;
    } else {
        gGallerySpecialUnlocked = 0;
    }

    if (Gallery_ControlRecordIsDirty(gCurrentCamEntrySpecial) != 0) {
        gGallerySpecialViewed = 1;
        gCurrentCamEntrySpecial->unk_6C &= ~2;

        for (i = 0; i < 21; i++) {
            Save_WriteTypedRecord(0x17, 0, i, &gGalleryCamEntries[i]);
        }

        Save_CommitBankRecords(2);
        Save_FlushBank(2);
    } else {
        gGallerySpecialViewed = 0;
    }

    _bcopy(&gGalleryCamEntries, &gGalleryCamEntriesBackup, sizeof(PhotoSceneData) * 21);

    main_pool_pop_state('bkgl');

    Gallery_InitPageCursor(&gGalleryMainSelection);
    Gallery_ClearSavedPosition(&gGalleryMainSelectionCam);
    Gallery_ClearSavedPosition(&gGallerySubSelection);
    Gallery_ClearSavedPosition(&gGalleryTertiarySelection);
    Gallery_ClearSavedPosition(&gGalleryFinalSelection.unk_00);
    Gallery_ClearPhotoRenderTarget();
    Gallery_InitSceneGrid(&gGalleryCamList, 0x3C, 0x2D, 0x10, 4, 0, 0, gCurrentCamEntryPage, 0x10, 0);
}

s32 Gallery_ControlRecordIsPopulated(PhotoSceneControl* arg0) {
    s32 var_v1 = 0;

    if (arg0->unk_6C & 1) {
        var_v1 = 1;
    }
    return var_v1;
}

s32 Gallery_ControlRecordIsDirty(PhotoSceneControl* arg0) {
    s32 var_v1 = 0;

    if (arg0->unk_6C & 2) {
        var_v1 = 1;
    }
    return var_v1;
}

s32 Gallery_ArraysDiffer(PhotoSceneData* arg0, PhotoSceneData* arg1, s32 arg2) {
    s32 sp1C = 0;

    if (bcmp(arg0, arg1, arg2) != 0) {
        sp1C = 1;
    }
    return sp1C;
}

s32 Gallery_BackgroundsChanged(void) {
    s32 sp1C = 0;

    if (gGallerySpecialUnlocked != 0) {
        gCurrentCamEntrySpecial->unk_6C |= 1;
    } else {
        gCurrentCamEntrySpecial->unk_6C &= ~1;
    }

    if (gGallerySpecialViewed != 0) {
        gCurrentCamEntrySpecial->unk_6C |= 2;
    } else {
        gCurrentCamEntrySpecial->unk_6C &= ~2;
    }

    if ((Gallery_ArraysDiffer(gGalleryEntries, gGalleryEntriesBackup, 0x13B0) != 0) || (Gallery_ArraysDiffer(gGalleryCamEntries, gGalleryCamEntriesBackup, 0xB7C) != 0)) {
        sp1C = 1;
    }
    return sp1C;
}

s32 Gallery_SaveUnlockedBackgrounds(s32 arg0, s32 arg1) {
    s32 i;
    s32 sp28;

    if (arg1 != 0) {
        Controller_WriteAccessoryStatus(0xCC);
    }

    main_pool_push_state('bkgl');

    Save_EnsureBankLoaded(2);
    if (arg0 != 0) {
        sp28 = Gallery_BackgroundsChanged();
    } else {
        sp28 = 1;
    }

    if (sp28 != 0) {
        for (i = 0; i < 36; i++) {
            Save_WriteTypedRecord(0x16, 0, i, &gGalleryEntries[i]);
        }

        for (i = 0; i < 21; i++) {
            Save_WriteTypedRecord(0x17, 0, i, &gGalleryCamEntries[i]);
        }

        Save_CommitBankRecords(2);
        Save_FlushBank(2);
    }

    main_pool_pop_state('bkgl');

    if (arg1 != 0) {
        Controller_WriteAccessoryStatus(0x33);
    }

    return sp28;
}

void func_8340051C(void) {
}

void Gallery_ResetPhotoPool(void) {
    gGalleryPhotoPoolCount = 0;
    bzero(gDisplayableMonIndices, 0xD20);
    bzero(gMonSelectionFlags, 0x60);
    Gallery_ClearSavedPosition(&gGallerySelection);
}

void Gallery_ClearPhotoRecord(PhotoSceneData* arg0) {
    bzero(arg0, sizeof(PhotoSceneData));
}

void Gallery_SwapPhotoRecords(PhotoSceneData* arg0, PhotoSceneData* arg1) {
    PhotoSceneData sp4;

    sp4 = *arg0;
    *arg0 = *arg1;
    *arg1 = sp4;
}

s32 Gallery_PhotoRecordsEqual(PhotoSceneData* arg0, PhotoSceneData* arg1) {
    s32 sp1C = 0;

    if (bcmp(arg0, arg1, sizeof(PhotoSceneData)) == 0) {
        sp1C = 1;
    }
    return sp1C;
}

void Gallery_SetEnlargeTarget(PhotoSceneData* arg0) {
    gCurrentGalleryEntryPtr = arg0;
}

PhotoSceneData* Gallery_GetEnlargeTarget(s32 arg0) {
    PhotoSceneData* ret = gCurrentGalleryEntryPtr;

    if (arg0 != 0) {
        gCurrentGalleryEntryPtr = NULL;
    }

    return ret;
}

void Gallery_InitPageCursor(GalleryCursor* arg0) {
    arg0->unk_00 = 0;
    arg0->unk_04 = -1;
}

s32 Gallery_CountTextLines(s8* arg0) {
    s32 var_v1 = 1;

    while (*arg0 != '\x00') {
        if (*arg0 == '\n') {
            var_v1++;
        }
        arg0++;
    }

    return var_v1;
}

s32 Gallery_CountValidPhotos(PhotoSceneData* arg0, s32 arg1, s32 arg2) {
    s32 var_s0 = 0;
    s32 i;

    for (i = 0; i < arg1; i++) {
        if (arg2 != 0) {
            if (Gallery_IsPhotoSpeciesValid(&arg0[i]) != 0) {
                var_s0++;
            }
        } else if (Gallery_IsPhotoSpeciesValid(&arg0[i]) == 0) {
            var_s0++;
        }
    }

    return var_s0;
}

s32 Gallery_FindNthEmptySlot(PhotoSceneData* arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 var_s0 = arg2;
    s32 var_s1 = -1;

    while (true) {
        if (Gallery_IsPhotoSpeciesValid(&arg0[var_s0]) == 0) {
            var_s1++;
        }

        if (var_s1 == arg3) {
            break;
        }

        var_s0 = (var_s0 + 1) % arg1;
    }
    return var_s0;
}

s32 Gallery_FindFirstEmptySlot(PhotoSceneData* arg0, s32 arg1) {
    s32 i;
    s32 sp28 = -1;

    for (i = 0; i < arg1; i++) {
        if (Gallery_IsPhotoSpeciesValid(&arg0[i]) == 0) {
            sp28 = i;
            break;
        }
    }

    return sp28;
}

void Gallery_DrawBluePanel(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    Color_RGB8 sp24 = { 0x00, 0x64, 0xFF };
    Color_RGB8 sp20 = { 0x64, 0xFF, 0xFF };

    Ui_DrawGradientPanel(arg0, arg1, arg2, arg3, &sp24, &sp20);
}

void Gallery_DrawRoundedFrame(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7) {
    s16 var_s2;
    s16 var_s3;
    s16 tmp1;
    s16 tmp2;

    if (arg2 >= 0x20) {
        var_s3 = 0x10;
    } else {
        var_s3 = arg2 / 2;
    }

    if (arg3 >= 0x20) {
        var_s2 = 0x10;
    } else {
        var_s2 = arg3 / 2;
    }

    tmp1 = arg2 - (var_s3 * 2);
    tmp2 = arg3 - (var_s2 * 2);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetEnvColor(gDisplayListHead++, arg4, arg5, arg6, arg7);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_300C440, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, arg1, var_s3, var_s2, 0, 0, 0x4000 / var_s3, 0x4000 / var_s2, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_300C4C0, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((arg0 + arg2) - var_s3, arg1, var_s3, var_s2, 0, 0, 0x4000 / var_s3, 0x4000 / var_s2, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_300C340, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, (arg1 + arg3) - var_s2, var_s3, var_s2, 0, 0, 0x4000 / var_s3, 0x4000 / var_s2, 0);

    gDPLoadTextureBlock_4b(gDisplayListHead++, D_300C3C0, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((arg0 + arg2) - var_s3, (arg1 + arg3) - var_s2, var_s3, var_s2, 0, 0, 0x4000 / var_s3,
                  0x4000 / var_s2, 0);

    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    if ((tmp1 > 0) && (tmp2 > 0)) {
        Gfx_FillRectRgba(var_s3 + arg0, arg1, tmp1, var_s2, arg4, arg5, arg6, arg7);
        Gfx_FillRectRgba(arg0, var_s3 + arg1, arg2, tmp2, arg4, arg5, arg6, arg7);
        Gfx_FillRectRgba(var_s3 + arg0, (var_s3 + arg1) + tmp2, tmp1, var_s2, arg4, arg5, arg6, arg7);
    } else if (tmp1 > 0) {
        Gfx_FillRectRgba(var_s3 + arg0, arg1, tmp1, arg3, arg4, arg5, arg6, arg7);
    } else if (tmp2 > 0) {
        Gfx_FillRectRgba(arg0, var_s2 + arg1, arg2, tmp2, arg4, arg5, arg6, arg7);
    }

    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void Gallery_DrawHeaderBar(s32 arg0, s32 arg1, s32 arg2, Color_RGBA8* arg3, Color_RGBA8* arg4) {
    gSPDisplayList(gDisplayListHead++, gDL_XluBorder);
    gDPSetEnvColor(gDisplayListHead++, arg4->r, arg4->g, arg4->b, arg4->a);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, arg3->r, arg3->g, arg3->b, arg3->a);
    gDPLoadTextureBlock(gDisplayListHead++, D_300BF40, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, arg1, 0x10, 0x20, 0, 0, 0x400, 0x400, 0);

    gDPLoadTextureBlock(gDisplayListHead++, D_300C140, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 32, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped((arg0 + arg2) - 0x10, arg1, 0x10, 0x20, 0, 0, 0x400, 0x400, 0);
    Gfx_DrawTexturedRectClipped(arg0 + 0x10, arg1, arg2 - 0x20, 0x20, 0, 0, 0, 0x400, 0);
}

void Gallery_DrawImageStrips(u16* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    s32 temp_v0;
    s32 tmp = 8;
    s32 i;

    for (i = 0; i < arg4; i += tmp) {
        temp_v0 = CLAMP_MIN(arg4 - i, tmp);

        gDPLoadTextureTile(gDisplayListHead++, &arg0[i * arg3], G_IM_FMT_RGBA, G_IM_SIZ_16b, arg3, 0, 0, 0, arg3,
                           temp_v0 - 1, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                           G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        if (arg5 != 0) {
            Gfx_DrawTexturedRectClipped(arg1, arg2 + i, arg3, temp_v0, (arg3 - 1) << 5, 0, -0x400, 0x400, 0);
        } else {
            Gfx_DrawTexturedRectClipped(arg1, arg2 + i, arg3, temp_v0, 0, 0, 0x400, 0x400, 0);
        }
    }
}

void Gallery_FillRect(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    gDPFillRectangle(gDisplayListHead++, arg0, arg1, (arg0 + arg2) - 1, (arg1 + arg3) - 1);
}

void Gallery_DrawWhiteBorder(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    UNUSED s32 pad;

    gSPDisplayList(gDisplayListHead++, gDL_FillCycle);
    gDPSetFillColor(gDisplayListHead++, 0xFFFFFFFF);

    Gallery_FillRect(arg0 - arg4, arg1 - arg4, (arg4 * 2) + arg2, arg4);
    Gallery_FillRect(arg0 - arg4, arg1 + arg3, (arg4 * 2) + arg2, arg4);
    Gallery_FillRect(arg0 - arg4, arg1, arg4, arg3);
    Gallery_FillRect(arg0 + arg2, arg1, arg4, arg3);
}

void Gallery_DrawDropShadow(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

    Gfx_FillRectRgba(arg0 - 4, arg1 - 4, arg2 + 8, arg3 + 8, 0, 0, 0, 0x80);
}

void Gallery_DrawPhotoThumbnail(u16* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6) {
    UNUSED s32 pad;
    s32 temp_v0;
    s32 i;

    if (arg6 != 0) {
        Gallery_DrawWhiteBorder(arg1, arg2, arg3, arg4, 4);
    }

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);

    for (i = 0; i < arg4; i += arg5) {
        temp_v0 = CLAMP_MIN(arg4 - i, arg5);

        gDPLoadTextureTile(gDisplayListHead++, &arg0[i * arg3], G_IM_FMT_RGBA, G_IM_SIZ_16b, arg3, 0, 0, 0, arg3 - 1,
                           temp_v0 - 1, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                           G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        Gfx_DrawTexturedRectClipped(arg1, arg2 + i, arg3, temp_v0, 0, 0, 0x400, 0x400, 0);
    }
}

void Gallery_DrawSelectionCorners(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    static s16 gGalleryCornerAnimTimer = 0;

    s16 sp4E = SINS(gGalleryCornerAnimTimer) * 2;
    UNUSED s32 pad[4];

    gDPPipeSync(gDisplayListHead++);
    gDPSetEnvColor(gDisplayListHead++, 240, 212, 104, 255);

    Gfx_DrawTextureIa8(sp4E + arg0, sp4E + arg1, 0x10, 0x10, D_300C540, 0x10, 0);
    Gfx_DrawTextureIa8(sp4E + arg0, ((arg1 + arg3) - sp4E) - 0x10, 0x10, 0x10, D_300C840, 0x10, 0);
    Gfx_DrawTextureIa8(((arg0 + arg2) - sp4E) - 0x10, sp4E + arg1, 0x10, 0x10, D_300C640, 0x10, 0);
    Gfx_DrawTextureIa8(((arg0 + arg2) - sp4E) - 0x10, ((arg1 + arg3) - sp4E) - 0x10, 0x10, 0x10, D_300C740, 0x10, 0);
    gGalleryCornerAnimTimer += 0x2000;
}

void Gallery_DrawStatusIcon(s32 arg0, s32 arg1, s32 arg2) {
    u8* var_v0;

    if (arg2 != 0) {
        var_v0 = D_300AE38;
    } else {
        var_v0 = D_3009E38;
    }

    Gfx_DrawTextureRgba16(arg0, arg1, 0x40, 0x20, var_v0, 0x40, 0);
}

void Gallery_InitSceneGrid(GallerySceneGrid* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, ModelLoadContext* arg5,
                   BinArchive* arg6, PhotoSceneData* arg7, s32 arg8, s32 arg9) {
    UNUSED s32 pad;
    DepthBuffer* sp168;
    DepthBuffer* sp68[64];
    UNUSED s32 pad2;
    s32 i;

    arg0->unk_00 = 0;
    arg0->unk_04 = -1;
    arg0->unk_08 = 0;

    arg0->unk_0C = arg8 / arg3;
    if ((arg8 % arg3) > 0) {
        arg0->unk_0C++;
    }

    arg0->unk_10 = 0;
    arg0->unk_14 = arg3;
    arg0->unk_18 = arg4;
    arg0->unk_1C = arg3 / arg4;

    sp168 = GfxImage_Allocate(G_IM_FMT_RGBA, IMAGE_SIZE_BITS_16b, arg1, arg2, 1);

    for (i = 0; i < arg3; i++) {
        sp68[i] = GfxImage_Allocate(G_IM_FMT_RGBA, IMAGE_SIZE_BITS_16b, arg1, arg2, 0);
    }

    if (arg9 != 0) {
        Gallery_ResetActiveScene();
    }

    for (i = 0; i < 36; i++) {
        if (i < arg8) {
            arg0->unk_20[i] = Geo_CreateSceneInstance(arg1, arg2, sp68[i % arg3], sp168, arg5, arg6, &arg7[i]);
        } else {
            arg0->unk_20[i] = 0;
        }
    }

    arg0->unk_B0 = Geo_CreateSceneInstance(arg1, arg2, 0, sp168, arg5, arg6, NULL);
}

void Gallery_ProcessSceneGrid(GallerySceneGrid* arg0) {
    s32 temp_v0 = Gallery_FindReadyScene(arg0->unk_20, 0x25);
    Camera* var_s0;
    s32 i;

    if (temp_v0 == -1) {
        for (i = 0; i < arg0->unk_14 + 1; i++) {
            if (i < arg0->unk_14) {
                var_s0 = arg0->unk_20[arg0->unk_08 * arg0->unk_14 + i];
            } else if (arg0->unk_04 != -1) {
                var_s0 = arg0->unk_B0;
            } else {
                var_s0 = NULL;
            }

            if ((Gallery_IsSceneReady(var_s0) != 0) && (var_s0->unk_00 == 0)) {
                main_pool_push_state('albu');
                Gallery_ProcessSceneInstance(var_s0);
                break;
            }
        }
    } else {
        Gallery_ProcessSceneInstance(arg0->unk_20[temp_v0]);
    }
}

void Gallery_FinishActiveScene(void) {
    Camera* temp_v0 = Gallery_GetActiveScene();

    if ((temp_v0 != NULL) && (temp_v0->unk_00 == 2)) {
        main_pool_pop_state('albu');
        Gallery_ClearActiveScene();
    }
}

s32 Gallery_CountReadyScenesOnPage(GallerySceneGrid* arg0, s32 arg1) {
    s32 i;
    s32 idx;
    s32 var_s2 = 0;

    if ((arg1 >= 0) && (arg1 < arg0->unk_0C)) {
        for (i = 0; i < arg0->unk_14; i++) {
            idx = (arg0->unk_14 * arg1) + i;
            if (Gallery_IsSceneReady(arg0->unk_20[idx]) != 0) {
                var_s2++;
            }
        }
    }
    return var_s2;
}

void Gallery_ClearSavedPosition(GalleryCursor* arg0) {
    arg0->unk_00 = -1;
    arg0->unk_04 = -1;
}

void Gallery_SavePosition(GallerySceneGrid* arg0, GalleryCursor* arg1) {
    arg1->unk_00 = arg0->unk_00;
    arg1->unk_04 = arg0->unk_08;
}

void Gallery_RestorePosition(GallerySceneGrid* arg0, GalleryCursor* arg1) {
    if (arg1->unk_00 != -1) {
        arg0->unk_00 = arg1->unk_00;
        arg0->unk_08 = arg1->unk_04;
    }
}

void Gallery_IndexToGridPos(GallerySceneGrid* arg0, s32* arg1, s32* arg2, s32 arg3) {
    *arg1 = arg3 % arg0->unk_18;
    *arg2 = arg3 / arg0->unk_18;
}

s32 Gallery_GridPosToIndex(GallerySceneGrid* arg0, s32 arg1, s32 arg2) {
    return (arg0->unk_18 * arg2) + arg1;
}

void Gallery_RebindSceneGridResources(GallerySceneGrid* arg0, ModelLoadContext* arg1, BinArchive* arg2) {
    s32 i;

    for (i = 0; i < 37; i++) {
        if (arg0->unk_20[i] != NULL) {
            arg0->unk_20[i]->unk_10 = arg1;
            arg0->unk_20[i]->unk_14 = arg2;
        }
    }
}
