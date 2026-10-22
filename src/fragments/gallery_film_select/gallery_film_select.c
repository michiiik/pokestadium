#include "gallery_film_select.h"
#include "src/display_object_textures.h"
#include "src/graphics_textures.h"
#include "src/input.h"
#include "src/text_system.h"
#include "src/gallery.h"
#include "src/jpeg_stream.h"
#include "src/ui_graphics.h"
#include "src/audio_sfx.h"
#include "src/gfx_buffer.h"
#include "src/controller.h"
#include "src/fragments/minigame_select/minigame_select.h"
#include "src/fragments/gallery_backgrounds/gallery_backgrounds.h"
#include "src/memory.h"
#include "src/stage_loader.h"

s32 gSelectionState;
s32 gSelectionMessageState;
BinArchive* D_836032F8;
GallerySceneGrid gSelectionCamList;
s32 gSelectedMonCount;
s32 gMaxSelectableMonCount;
s32 gFilmSelectSlowFadeInFlag;
u16 gBackgroundGradientMask;
s32 gCursorCanMoveUp;
s32 gCursorCanMoveDown;

void Gallery_FilmSelectCommitPhotos(GallerySceneGrid* arg0) {
    s32 i;
    s32 sp40;
    PhotoSceneData* temp_s0;
    s32 temp_v0;
    s32 idx;

    sp40 = 0;

    for (i = 35; i >= 0; i--) {
        if (Gallery_IsPhotoSpeciesValid(&gGalleryEntries[i]) == 0) {
            idx = i;
            while ((idx > 0) && Gallery_IsPhotoSpeciesValid(&gGalleryEntries[idx - 1]) == 0) {
                idx--;
            }
            sp40 = idx;
            break;
        }
    }

    for (i = 0; i < 24; i++) {
        if (gMonSelectionFlags[i] != 0) {
            idx = Gallery_FindNthEmptySlot(gGalleryEntries, 0x24, sp40, 0);

            temp_s0 = arg0->unk_20[i]->unk_18;

            gGalleryEntries[idx] = *temp_s0;

            temp_v0 = Gallery_FindFirstEmptySlot(gCurrentCamEntry, 4);
            if (temp_v0 >= 0) {
                gCurrentCamEntry[temp_v0] = *temp_s0;
            }

            temp_v0 = Gallery_FindFirstEmptySlot(gCurrentCamEntryPage, 0x10);
            if (temp_v0 >= 0) {
                gCurrentCamEntryPage[temp_v0] = *temp_s0;
            }
        }
    }
}

s32 Gallery_FilmSelectHasUnsavedPhoto(void) {
    s32 i;
    s32 sp28 = 0;

    for (i = 0; i < 0x18; i++) {
        if ((Gallery_IsPhotoSpeciesValid(&gDisplayableMonIndices[i]) != 0) && (gMonSelectionFlags[i] == 0)) {
            sp28 = 1;
            break;
        }
    }

    return sp28;
}

void Gallery_FilmSelectUpdatePageAvailability(GallerySceneGrid* arg0) {
    UNUSED s32 pad;
    s32 sp18 = arg0->unk_08 + 1;

    if (Gallery_CountReadyScenesOnPage(arg0, arg0->unk_08 - 1) > 0) {
        gCursorCanMoveUp = 1;
    } else {
        gCursorCanMoveUp = 0;
    }

    if (Gallery_CountReadyScenesOnPage(arg0, sp18) > 0) {
        gCursorCanMoveDown = 1;
    } else {
        gCursorCanMoveDown = 0;
    }
}

s32 Gallery_FilmSelectHandleInput(GallerySceneGrid* arg0) {
    s32 sp54;
    s32 sp50;
    s32 sp4C;
    s32 sp48;
    s32 sp44;
    s32 sp40;
    s32 sp3C;
    s32 sp1C;
    s32 idx;
    s32 i;
    Camera* ptr;

    sp54 = 0;
    sp50 = arg0->unk_08;
    sp4C = arg0->unk_00;
    sp48 = arg0->unk_00 % 3;
    sp44 = arg0->unk_00 / 3;
    sp40 = 0;
    sp3C = 0;

    if (Gallery_FindReadyScene(arg0->unk_20, 0x25) != -1) {
        return 0;
    }

    if (gPlayer1Controller->buttonPressed & 0x800) {
        sp44--;
        if (sp44 < 0) {
            sp44 = 2;
        }
    }

    if (gPlayer1Controller->buttonPressed & 0x400) {
        sp44++;
        if (sp44 >= 3) {
            sp44 = 0;
        }
    }

    if (gPlayer1Controller->buttonPressed & 0x200) {
        sp48--;
        if ((sp44 == 2) && (sp48 == 1)) {
            sp48--;
        }

        if (sp44 < 2) {
            if (sp48 < 0) {
                if (gCursorCanMoveUp != 0) {
                    sp48 = 2;
                } else {
                    sp48 = 0;
                }
                sp40 = 1;
            }
        } else if (sp48 < 0) {
            sp48 = 2;
        }
    }

    if (gPlayer1Controller->buttonPressed & 0x100) {
        sp48++;
        if ((sp44 == 2) && (sp48 == 2)) {
            sp48++;
        }

        if (sp44 < 2) {
            if (sp48 >= 3) {
                if (gCursorCanMoveDown != 0) {
                    sp48 = 0;
                } else {
                    sp48 = 2;
                }
                sp3C = 1;
            }
        } else if (sp48 >= 3) {
            sp48 = 0;
        }
    }

    sp1C = sp44 * 3;

    arg0->unk_00 = sp1C + sp48;
    if (sp4C != arg0->unk_00) {
        gSelectionMessageState = 0;
        Audio_PlaySoundEffectById(SFX_MENU_SCROLL);
    }

    if ((gPlayer1Controller->buttonPressed & 0x20) || (sp40 != 0)) {
        s32 tmp = arg0->unk_08;
        if (gCursorCanMoveUp != 0) {
            tmp--;
            arg0->unk_08 = tmp;
        }
    } else if ((gPlayer1Controller->buttonPressed & 0x10) || (sp3C != 0)) {
        s32 tmp = arg0->unk_08;
        if (gCursorCanMoveDown != 0) {
            tmp++;
            arg0->unk_08 = tmp;
        }
    } else if (gPlayer1Controller->buttonPressed & 0x8000) {
        idx = (arg0->unk_08 * arg0->unk_14) + arg0->unk_00;
        if (sp1C < 6) {
            if (Gallery_IsSceneReady(arg0->unk_20[idx]) != 0) {
                if (gMonSelectionFlags[idx] == 0) {
                    if (gSelectedMonCount < gMaxSelectableMonCount) {
                        gMonSelectionFlags[idx] = 1;
                        gSelectedMonCount++;
                        Audio_PlaySoundEffectById(0xD);
                    } else {
                        gSelectionMessageState = 1;
                        Audio_PlaySoundEffectById(SFX_MENU_ERROR);
                    }
                } else {
                    gMonSelectionFlags[idx] = 0;
                    gSelectedMonCount--;
                    Audio_PlaySoundEffectById(SFX_MENU_BACK);
                }
            } else {
                Audio_PlaySoundEffectById(SFX_MENU_ERROR);
            }
        } else if (sp48 == 0) {
            sp54 = 1;
            Audio_PlaySoundEffectById(0x1C);
        } else if (Gallery_FilmSelectHasUnsavedPhoto() != 0) {
            gSelectionState = 2;
            gSelectionMessageState = 2;
            Audio_PlaySoundEffectById(0x31);
        } else {
            sp54 = 1;
            Audio_PlaySoundEffectById(SFX_MENU_BACK);
        }
    } else if (gPlayer1Controller->buttonPressed & 4) {
        idx = (arg0->unk_08 * arg0->unk_14) + arg0->unk_00;
        if ((sp1C < 6) && (Gallery_IsSceneReady(arg0->unk_20[idx]) != 0)) {
            Audio_PlaySoundEffectById(SFX_ZOOM_IN);
            Gallery_SetEnlargeTarget(arg0->unk_20[idx]->unk_18);
            sp54 = 2;
        } else {
            Audio_PlaySoundEffectById(SFX_MENU_ERROR);
        }
    }

    if (sp50 != arg0->unk_08) {
        gSelectionMessageState = 0;
        Audio_PlaySoundEffectById(SFX_PAGE_CHANGE);
        Gallery_FilmSelectUpdatePageAvailability(arg0);

        for (i = 0; i < arg0->unk_14; i++) {
            ptr = arg0->unk_20[(arg0->unk_08 * arg0->unk_14) + i];

            if (ptr != NULL) {
                ptr->unk_00 = 0;
            }
        }
    }
    return sp54;
}

void Gallery_FilmSelectInit(s32 arg0) {
    s32 i;

    gSelectionState = 0;
    gSelectionMessageState = 0;
    gSelectedMonCount = 0;

    for (i = 0; i < 24; i++) {
        if (gMonSelectionFlags[i] != 0) {
            gSelectedMonCount++;
        }
    }

    gMaxSelectableMonCount = Gallery_CountValidPhotos(gGalleryEntries, 0x24, 0);
    gBackgroundGradientMask = 0xFFE3;

    if (arg0 != 0) {
        gFilmSelectSlowFadeInFlag = 1;
    } else {
        gFilmSelectSlowFadeInFlag = 0;
    }

    Gallery_InitSceneGrid(&gSelectionCamList, 0x88, 0x66, 6, 3, PokeIcon_AllocFramebuffers(5), D_836032F8, gDisplayableMonIndices, 0x18, 1);

    if (gGallerySelection.unk_00 != -1) {
        Gallery_RestorePosition(&gSelectionCamList, &gGallerySelection);
    }

    Gallery_FilmSelectUpdatePageAvailability(&gSelectionCamList);
}

void Gallery_FilmSelectDrawBandedStrip(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 sp50;
    u8* var_s6;
    s32 i;
    u8* tmp_s6;

    sp50 = arg2 / 40;

    if (arg3 != 0) {
        var_s6 = D_4004848;
    } else {
        var_s6 = D_4004AC8;
    }
    tmp_s6 = var_s6;

    if (arg3 == 0) {
        gSPDisplayList(gDisplayListHead++, gUiTranslucentFillDL);

        Gfx_FillRectRgba(arg0, arg1, arg2, 4, 0, 0, 0, 0x80);
    }

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPLoadTextureBlock(gDisplayListHead++, var_s6, G_IM_FMT_IA, G_IM_SIZ_8b, 40, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 128);

    for (i = 0; i < sp50; i++) {
        Gfx_DrawTexturedRectClipped(arg0 + 4 + i * 0x28, arg1 + 4, 0x28, 0x10, 0, 0, 0x400, 0x400, 0);
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);

    if (var_s6 != tmp_s6) {
        gDPLoadTextureBlock(gDisplayListHead++, var_s6, G_IM_FMT_IA, G_IM_SIZ_8b, 40, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    }

    for (i = 0; i < sp50; i++) {
        Gfx_DrawTexturedRectClipped(arg0 + i * 0x28, arg1, 0x28, 0x10, 0, 0, 0x400, 0x400, 0);
    }
}

void Gallery_FilmSelectFillRectSolid(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    gSPDisplayList(gDisplayListHead++, gDL_FillCycle);
    gDPSetFillColor(gDisplayListHead++, 0x00000001);
    gDPFillRectangle(gDisplayListHead++, arg0, arg1, (arg0 + arg2) - 1, (arg1 + arg3) - 1);
    gSPDisplayList(gDisplayListHead++, gUiRestoreTextureDL);
}

void Gallery_FilmSelectDrawPhotoBadge(s32 arg0, s32 arg1, s16 arg2) {
    s16 end = 16;
    s32 i;
    UNUSED s32 pad;
    s32 spC0;
    s32 spBC;
    s32 temp_v0_2;

    spC0 = 0x30;
    spBC = (s32)ROUND_MAX((SINS(arg2) + 1.0f) * 255.0f * 0.5f) & 0xFF;

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    for (i = 0; i < spC0; i += end) {
        temp_v0_2 = CLAMP_MIN(spC0 - i, end);

        gDPLoadTextureBlock(gDisplayListHead++, &D_4004D48[i * spC0], G_IM_FMT_RGBA, G_IM_SIZ_16b, spC0, temp_v0_2, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);

        Gfx_DrawTexturedRectClipped(arg0, arg1 + i, spC0, temp_v0_2, 0, 0, 0x400, 0x400, 0);
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetCombineLERP(gDisplayListHead++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                      PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, 255, 255, 255, spBC);
    gDPSetEnvColor(gDisplayListHead++, 255, 55, 0, 255);

    spC0 = 0x30;

    gDPLoadTextureBlock(gDisplayListHead++, D_4006748, G_IM_FMT_IA, G_IM_SIZ_8b, spC0, spC0, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, arg1, spC0, spC0, 0, 0, 0x400, 0x400, 0);
}

void Gallery_FilmSelectDrawCounterBadge(s32 arg0, s32 arg1, s16 arg2) {
    s32 temp_v0_2;
    s32 end = 0x20;
    s32 i;
    s32 spC0;
    s32 spBC;
    UNUSED s32 pad;

    spC0 = 0x20;
    spBC = (s32)ROUND_MAX((SINS(arg2) + 1.0f) * 255.0f * 0.5f) & 0xFF;

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    for (i = 0; i < spC0; i += end) {
        temp_v0_2 = CLAMP_MIN(spC0 - i, end);

        gDPLoadTextureBlock(gDisplayListHead++, &D_4005F48[i * spC0], G_IM_FMT_RGBA, G_IM_SIZ_16b, spC0, temp_v0_2, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);

        Gfx_DrawTexturedRectClipped(arg0, arg1 + i, spC0, temp_v0_2, 0, 0, 0x400, 0x400, 0);
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetCombineLERP(gDisplayListHead++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                      PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(gDisplayListHead++, 0, 0, 255, 255, 255, spBC);
    gDPSetEnvColor(gDisplayListHead++, 255, 55, 0, 255);

    spC0 = 0x20;

    gDPLoadTextureBlock(gDisplayListHead++, D_4007048, G_IM_FMT_IA, G_IM_SIZ_8b, spC0, spC0, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0, arg1, spC0, spC0, 0, 0, 0x400, 0x400, 0);
}

void Gallery_FilmSelectDrawPhotoGrid(GallerySceneGrid* arg0) {
    static s16 D_83603280 = 0;

    s32 temp_s0_2;
    s32 temp_s1;
    char sp70[16];
    s32 temp_s4;
    s32 temp_s5;
    s32 i;
    Camera* temp_s0;

    for (i = 0; i < arg0->unk_14; i++) {
        temp_s5 = (arg0->unk_08 * arg0->unk_14) + i;
        temp_s0 = arg0->unk_20[temp_s5];
        if ((Gallery_IsSceneReady(temp_s0) != 0) && (temp_s0->unk_00 == 2)) {
            temp_s4 = (i / 3) * 0x6E;
            temp_s1 = (i % 3) * 0xA4;
            Gallery_DrawPhotoThumbnail(temp_s0->unk_08->img_p, temp_s1 + 0x58, temp_s4 + 0x6E, 0x88, 0x66, 8, 0);
            if (gMonSelectionFlags[temp_s5] != 0) {
                Gallery_FilmSelectDrawPhotoBadge(temp_s1 + 0xB0, temp_s4 + 0xA6, D_83603280);
            }
        }
        Font_BeginTranslucentTextRendering();
        Font_SetActive(8, 0);
        sprintf(sp70, "%d", temp_s5 + 1);
        temp_s0_2 = Font_MeasureTextExtent(0, 0, sp70);
        Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
        Font_Printf((((i % 3) * 0xA4) - temp_s0_2) + 0x54, ((i / 3) * 0x6E) + 0x6E, sp70);
        Font_EndTexturedTextRendering();
    }

    D_83603280 += 0x400;
}

void Gallery_FilmSelectDrawSelectionCursor(GallerySceneGrid* arg0) {
    s32 temp_hi = arg0->unk_00 % 3;
    s32 temp_lo = arg0->unk_00 / 3;
    s32 var_a0;

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    if (temp_lo < 2) {
        Gallery_DrawSelectionCorners((temp_hi * 0xA4) + 0x58, (temp_lo * 0x6E) + 0x6E, 0x88, 0x66);
        return;
    }

    var_a0 = -1;
    switch (temp_hi) {
        case 0:
            var_a0 = 0x30;
            break;

        case 1:
        case 2:
            var_a0 = 0x1E1;
            break;
    }

    if (var_a0 >= 0) {
        Gallery_DrawSelectionCorners(var_a0, 0x172, 0x6C, 0x45);
    }
}

void Gallery_FilmSelectDrawBanner(s32 arg0, s32 arg1) {
    static Color_RGBA8 D_83603284 = { 0x9A, 0x37, 0x54, 0xFF };
    static Color_RGBA8 D_83603288 = { 0xC3, 0x54, 0x84, 0xFF };
    static s16 D_8360328C = 0;

    char* sp84;
    s32 sp78;
    s32 sp7C;
    s32 sp80;
    Color_RGBA8 sp74;
    Color_RGBA8 sp70;

    sp84 = Gallery_GetUiString(2);
    sp74 = D_83603284;
    sp70 = D_83603288;
    sp7C = SINS(D_8360328C) * 4.0f;
    Gallery_DrawHeaderBar(arg0, arg1, 0x1B6, &sp74, &sp70);

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    gDPLoadTextureTile(gDisplayListHead++, D_4003A38, G_IM_FMT_IA, G_IM_SIZ_8b, 100, 36, 0, 0, 99, 35, 0,
                       G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                       G_TX_NOLOD);

    Gfx_DrawTexturedRectClipped(arg0 + 0x36, arg1 - 3, 0x64, 0x24, 0, 0, 0x400, 0x400, 0);
    Gfx_DrawTexturedRectClipped(arg0 + 0x11C, arg1 - 3, 0x64, 0x24, 0xC60, 0, -0x400, 0x400, 0);

    if (gCursorCanMoveUp != 0) {
        sp78 = sp7C;
        sp80 = 0xFF;
    } else {
        sp78 = 0;
        sp80 = 0x80;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sp80);

    Gallery_DrawStatusIcon((arg0 - sp78) - 0x27, arg1 + 0x1A, 0);

    if (gCursorCanMoveDown != 0) {
        sp78 = sp7C;
        sp80 = 0xFF;
    } else {
        sp78 = 0;
        sp80 = 0x80;
    }

    gDPPipeSync(gDisplayListHead++);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sp80);

    Gallery_DrawStatusIcon(arg0 + sp78 + 0x1A2, arg1 + 0x1A, 1);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(0x10, 0);

    sp78 = (0x1B6 - Font_MeasureTextExtent(0, 0, sp84)) / 2;
    Gfx_SetEnvColor(0, 0, 0, 0xFF);
    Font_Printf(arg0 + sp78 + 2, arg1 + 5, sp84);
    Gfx_SetEnvColor(0xFF, 0xFF, 0x77, 0xFF);
    Font_Printf(arg0 + sp78, arg1 + 3, sp84);
    Font_EndTexturedTextRendering();

    D_8360328C += 0x1000;
}

void Gallery_FilmSelectDrawActionPanel(int arg0, int arg1, s32 arg2, f32 arg3) {
    s32 temp_s2;
    s32 temp_s3;
    f32 tmp = 1024.0f;
    s32 iter = 16;
    s32 spFC = 0x6C;
    s32 spF8 = 0x45;
    s32 temp_ft2 = ROUND_MAX((spFC * arg3) + 0.4f);
    s32 temp_fv1 = ROUND_MAX((spF8 * arg3) + 0.4f);
    s32 i;
    u16* spE4[] = { gRentalViewerIconTop, D_03005F80 };
    s16 spE0[] = { 0x39, 0x37 };
    u16* spDC = spE4[arg2];
    char* spD8 = Gallery_GetUiString(spE0[arg2]);
    Color_RGB8 spD0[] = {
        { 0xE6, 0xDC, 0x0F },
        { 0xE6, 0x6E, 0x82 },
    };

    if (arg3 <= 0.0f) {
        return;
    }

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    for (i = 0; i < spF8; i += iter) {
        s32 temp_v0_2 = CLAMP_MIN(spF8 - i, iter);
        s32 temp_fv1_2 = ROUND_MAX((temp_v0_2 * arg3) + 0.4f);
        s32 temp_fa0 = ROUND_MAX((i * arg3) + 0.4f);

        gDPLoadTextureTile(gDisplayListHead++, &spDC[i * spFC], G_IM_FMT_RGBA, G_IM_SIZ_16b, spFC, 0, 0, 0, spFC,
                           temp_v0_2 - 1, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
                           G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        Gfx_DrawTexturedRectClipped(arg0 + ((spFC - temp_ft2) / 2), arg1 + ((spF8 - temp_fv1) / 2) + temp_fa0, temp_ft2, temp_fv1_2,
                      0, 0, tmp / arg3, tmp / arg3, 0);
    }

    temp_s3 = (spFC - Font_MeasureTextExtent(0, 0, spD8)) / 2;
    temp_s2 = (spF8 - (Gallery_CountTextLines(spD8) * 0x14)) / 2;

    Font_BeginTranslucentTextRendering();
    Font_SetActive(8, 0);
    Font_SetLineHeight(0x14);
    Gfx_SetEnvColor(0, 0, 0, 0xFF);
    Font_Printf(arg0 + temp_s3 + 2, arg1 + temp_s2 + 2, spD8);
    Font_EnableTwoCycleTexturing();
    Gfx_SetEnvColor(spD0[arg2].r, spD0[arg2].g, spD0[arg2].b, 0xFF);
    Gfx_SetPrimColor(0xFF, 0xFF, 0xFF, 0xFF);
    Font_Printf(arg0 + temp_s3, arg1 + temp_s2, spD8);
    Font_DisableTwoCycleTexturing();
    Font_EndTexturedTextRendering();
}

void Gallery_FilmSelectDrawBackgroundGradient(void) {
    static Color_RGB8 gGradColorTopLeft = { 0xCD, 0xD7, 0x9B };
    static Color_RGB8 gGradColorBottomLeft = { 0x28, 0x64, 0xAF };
    static Color_RGB8 gGradColorTopRight = { 0xCD, 0xEB, 0x9B };
    static Color_RGB8 gGradColorBottomRight = { 0, 0, 0xAF };

    s32 i;
    s32 r;
    s32 g;
    s32 b;
    s32 end = 0x1E0;
    s32 idx;

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);
    gDPSetCombineMode(gDisplayListHead++, G_CC_BLENDPEDECALA, G_CC_BLENDPEDECALA);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPLoadTextureBlock_4b(gDisplayListHead++, D_4007448, G_IM_FMT_I, 128, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, 7, 6, G_TX_NOLOD, G_TX_NOLOD);

    idx = 0;
    for (i = 0; i < end; i += 0x40, idx++) {
        s32 temp_v0_2 = CLAMP_MIN(end - i, 0x40);

        if (gBackgroundGradientMask & (1 << idx)) {
            r = (((gGradColorTopRight.r - gGradColorTopLeft.r) * idx) / 8) + gGradColorTopLeft.r;
            g = (((gGradColorTopRight.g - gGradColorTopLeft.g) * idx) / 8) + gGradColorTopLeft.g;
            b = (((gGradColorTopRight.b - gGradColorTopLeft.b) * idx) / 8) + gGradColorTopLeft.b;

            gDPPipeSync(gDisplayListHead++);

            gDPSetPrimColor(gDisplayListHead++, 0, 0, r, g, b, 255);

            r = (((gGradColorBottomRight.r - gGradColorBottomLeft.r) * idx) / 8) + gGradColorBottomLeft.r;
            g = (((gGradColorBottomRight.g - gGradColorBottomLeft.g) * idx) / 8) + gGradColorBottomLeft.g;
            b = (((gGradColorBottomRight.b - gGradColorBottomLeft.b) * idx) / 8) + gGradColorBottomLeft.b;

            gDPSetEnvColor(gDisplayListHead++, r, g, b, 255);

            Gfx_DrawTexturedRectClipped(0, i, 0x280, temp_v0_2, 0, 0, 0x400, 0x400, 0);
        }
    }
}

char* Gallery_FilmSelectGetHelpText(GallerySceneGrid* arg0) {
    static s32 gHelpTextMsgIds[] = { 0x21, 0x24 };

    char* var_v0;
    s32 temp_lo = arg0->unk_00 % 3;
    s32 temp_hi = arg0->unk_00 / 3;

    if (gSelectionMessageState == 0) {
        if (temp_hi < 2) {
            var_v0 = Gallery_GetUiString(0x1E);
        } else if (temp_lo == 0) {
            var_v0 = Gallery_GetUiString(0x22);
        } else {
            var_v0 = Gallery_GetUiString(0x23);
        }
    } else {
        var_v0 = Gallery_GetUiString(gHelpTextMsgIds[gSelectionMessageState - 1]);
    }
    return var_v0;
}

void Gallery_FilmSelectDrawHelpText(GallerySceneGrid* arg0, s32 arg1, s32 arg2) {
    char* sp1C = Gallery_FilmSelectGetHelpText(arg0);

    Gallery_DrawBluePanel(arg1, arg2, 0x136, 0x4C);
    if (sp1C != NULL) {
        Font_BeginTranslucentTextRendering();
        Font_SetActive(8, 0);
        Font_SetLineHeight(0x14);
        Font_Printf(arg1 + 0xA, arg2 + 8, sp1C);
        Font_EndTexturedTextRendering();
    }
}

void Gallery_FilmSelectDrawPhotoCounter(s32 arg0, s32 arg1) {
    static s16 D_836032BC = 0;

    char sp28[255];
    char* sp24;
    s32 sp20;
    s32 sp1C;

    Text_SetNumberToken(2, 0x63);
    sp24 = Gallery_CopyUiString(sp28, 0x100, 0x20);
    sp20 = Font_MeasureTextExtent(8, 0, sp24) / 2;
    Gallery_FilmSelectDrawCounterBadge((arg0 - sp20) - 0x26, arg1 - 4, D_836032BC);
    Text_SetNumberToken(2, gMaxSelectableMonCount - gSelectedMonCount);

    sp24 = Gallery_CopyUiString(sp28, 0x100, 0x20);
    sp1C = Font_MeasureTextExtent(8, 0, sp24);

    Font_BeginTranslucentTextRendering();
    Font_SetActive(8, 0);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    Font_Printf((arg0 + sp20) - sp1C, arg1, sp24);
    Font_EndTexturedTextRendering();

    D_836032BC += 0x400;
}

void Gallery_FilmSelectDrawIconLabel(s32 arg0, s32 arg1) {
    char* sp2C;
    s32 sp28;

    sp2C = Gallery_GetUiString(0x3C);
    sp28 = -Font_MeasureTextExtent(8, 0, sp2C) / 2;

    gSPDisplayList(gDisplayListHead++, gUiEnvTintTextureDL);

    Gfx_DrawTextureRgba16((arg0 + sp28) - 0x20, arg1 - 2, 0x18, 0x18, D_30099B8, 0x18, 0);
    Font_BeginTranslucentTextRendering();
    Font_SetActive(8, 0);
    Gfx_SetEnvColor(0xFF, 0xFF, 0xFF, 0xFF);
    Font_Printf(arg0 + sp28, arg1, sp2C);
    Font_EndTexturedTextRendering();
}

f32 Gallery_FilmSelectComputeScalePulse(s32* arg0, u32* arg1, s32 arg2) {
    f32 coss;

    if (*arg0 < arg2) {
        *arg0 += 1;
        coss = 1.0f;
    } else {
        coss = (COSS(*arg1) * 0.1f) + 0.9f;

        *arg1 += 0x2000;
        if (*arg1 >= 0x10000) {
            *arg0 = 0;
            *arg1 = 0;
        }
    }

    return coss;
}

void Gallery_FilmSelectDraw(void) {
    static s32 D_836032C0 = 0;
    static u32 D_836032C4 = 0;
    static s32 D_836032C8 = 0;
    static u32 D_836032CC = 0;

    f32 tmp;
    s32 sp28;
    s32 sp24;
    UNUSED s32 pad;
    f32 sp1C;
    f32 sp18;

    Gallery_IndexToGridPos(&gSelectionCamList, &sp28, &sp24, gSelectionCamList.unk_00);
    BgStage_DrawFrame();
    Gallery_FilmSelectDrawBackgroundGradient();
    Gallery_FilmSelectDrawBandedStrip(0, 0x4F, 0x280, 1);
    Gallery_FilmSelectDrawBandedStrip(0, 0x151, 0x280, 0);
    Gallery_FilmSelectFillRectSolid(0, 0x5F, 0x280, 0xF2);
    Gallery_FilmSelectDrawPhotoGrid(&gSelectionCamList);

    sp1C = Gallery_FilmSelectComputeScalePulse(&D_836032C0, &D_836032C4, 0x1E);
    sp18 = Gallery_FilmSelectComputeScalePulse(&D_836032C8, &D_836032CC, 0xF);

    tmp = ((sp24 >= 2) && (sp28 == 0)) ? sp18 : sp1C;
    Gallery_FilmSelectDrawActionPanel(0x30, 0x172, 0, tmp);

    tmp = ((sp24 >= 2) && (sp28 != 0)) ? sp18 : sp1C;
    Gallery_FilmSelectDrawActionPanel(0x1E1, 0x172, 1, tmp);

    Gallery_FilmSelectDrawSelectionCursor(&gSelectionCamList);
    Gallery_FilmSelectDrawBanner(0x65, 0x2B);
    Gallery_FilmSelectDrawHelpText(&gSelectionCamList, 0xA5, 0x16B);
    Gallery_FilmSelectDrawPhotoCounter(0xF0, 0x14E);
    Gallery_FilmSelectDrawIconLabel(0x1E0, 0x14E);
    BgStage_AdvanceFrame();
}

void Gallery_FilmSelectHandleCancelInput(void) {
    if (gPlayer1Controller->buttonPressed & 0x8000) {
        gSelectionState = 3;
        StageContext_SetClearColor(0xFFFF);
        StageFade_StartFromTransparent(0xF);
        Audio_PlaySoundEffectById(0x1D);
    } else if (gPlayer1Controller->buttonPressed & 0x4000) {
        gSelectionMessageState = 0;
        gSelectionState = 1;
        Audio_PlaySoundEffectById(SFX_MENU_BACK);
    }
}

s32 Gallery_FilmSelectUpdate(void) {
    static s32 D_836032D0 = 2;

    s32 sp1C = 1;
    s32 temp_v0_2;

    switch (gSelectionState) {
        case 0:
            if (StageContext_GetFadeMode() == 0) {
                D_836032D0--;
                if (D_836032D0 <= 0) {
                    gSelectionState = 1;
                }
            }
            break;

        case 1:
            temp_v0_2 = Gallery_FilmSelectHandleInput(&gSelectionCamList);
            if (temp_v0_2 != 0) {
                gSelectionState = 3;
                if (temp_v0_2 == 2) {
                    StageContext_SetClearColor(1);
                    StageFade_StartFromTransparent(1);
                } else {
                    StageContext_SetClearColor(0xFFFF);
                    StageFade_StartFromTransparent(0xF);
                }
            }
            break;

        case 2:
            Gallery_FilmSelectHandleCancelInput();
            break;

        case 3:
            if (StageContext_GetFadeMode() == 1) {
                sp1C = 0;
            }
            break;
    }
    return sp1C;
}

void Gallery_FilmSelectReadInputs(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

void Gallery_FilmSelectRunLoop(void) {
    if (gFilmSelectSlowFadeInFlag == 0) {
        StageFade_StartFromOpaque(1);
    } else {
        StageFade_StartFromOpaque(7);
    }

    do {
        Gallery_FilmSelectReadInputs();

        switch (gSelectionState) {
            case 0:
            case 3:
                break;

            default:
                Gallery_ProcessSceneGrid(&gSelectionCamList);
                break;
        }

        Gallery_FilmSelectDraw();
        Gallery_FinishActiveScene();
    } while (Gallery_FilmSelectUpdate() != 0);
}

s32 Gallery_FilmSelectFinalize(GallerySceneGrid* arg0) {
    s32 temp_hi = arg0->unk_00 % 3;
    s32 temp_lo = arg0->unk_00 / 3;
    s32 sp1C;

    if (temp_lo < 2) {
        sp1C = 4;
    } else if (temp_hi == 0) {
        sp1C = 3;
    } else {
        sp1C = 0;
    }

    if ((sp1C == 0) && (gSelectedMonCount > 0)) {
        Gallery_FilmSelectCommitPhotos(arg0);
    }

    Gallery_SavePosition(&gSelectionCamList, &gGallerySelection);
    return sp1C;
}

void Gallery_FilmSelectMain(s32 arg0, UNUSED s32 arg1) {
    StageContext* sp24;

    main_pool_push_state('FILM');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    sp24 = StageContext_Allocate(1, 0, 2, 0, 2, 1);
    gTextObjectHandle = Font_Init(0x18, 0);

    ASSET_LOAD(D_1000000, common_menu1_ui, 0);
    ASSET_LOAD(D_5000000, snap_mode_ui, 0);
    ASSET_LOAD(D_3000000, gallery_ui, 0);
    ASSET_LOAD(gRentalViewerIconTop, _6A9750, 0);
    FRAGMENT_LOAD(FRAGMENT_PARTICLE_DATA_LIBRARY);

    PokeIcon_OpenModelArchives();
    D_836032F8 = BinArchive_Open(stadium_models_ROM_START, stadium_models_ROM_END, 1, 1);
    Text_InitStringTables();
    Gallery_InitStringTables();
    Gallery_FilmSelectInit(arg0);
    StageContext_Activate(sp24);
    Gallery_FilmSelectRunLoop();
    StageContext_Deactivate();
    Font_Free();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('FILM');

    Gallery_FilmSelectFinalize(&gSelectionCamList);
}
