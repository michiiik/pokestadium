#ifndef _FRAGMENT43_H_
#define _FRAGMENT43_H_

#include "global.h"
#include "gallery.h"

typedef struct MenuPanelInfo {
    /* 0x00 */ s16 unk_00;
    /* 0x00 */ s16 unk_02;
    /* 0x00 */ s16 unk_04;
    /* 0x00 */ s16 unk_06;
    /* 0x00 */ s8 unk_08;
    /* 0x00 */ s8 unk_09;
    /* 0x00 */ s16 unk_0A;
    /* 0x00 */ s16* unk_0C;
} MenuPanelInfo; // size = 0x10

typedef struct GallerySelection {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ char unk_0A[0x2];
} GallerySelection; // size >= 0xC

typedef struct unk_D_83407B18 {
    /* 0x00 */ GalleryCursor unk_00;
    /* 0x08 */ GallerySelection unk_08;
    /* 0x14 */ char unk14[0x4];
} unk_D_83407B18; // size = 0x18

extern unk_D_83402EE0 gDisplayableMonIndices[24];
extern s32 gMonSelectionFlags[24];
extern PhotoSceneData gGalleryEntries[36];
extern PhotoSceneData gGalleryCamEntries[21];
extern PhotoSceneData* gCurrentCamEntry;
extern PhotoSceneData* gCurrentCamEntryPage;
extern PhotoMonSpecies gGallerySpecialSpecies;
extern s32 gGallerySpecialUnlocked;
extern s32 gGallerySpecialViewed;
extern s32 gGallerySpecialUnlockEnabled;
extern GalleryCursor gGallerySelection;
extern GalleryCursor gGalleryMainSelectionCam;
extern GalleryCursor gGallerySubSelection;
extern GalleryCursor gGalleryTertiarySelection;
extern unk_D_83407B18 gGalleryFinalSelection;
extern GalleryCursor gGalleryMainSelection;
extern GallerySceneGrid gGalleryCamList;

extern s32 gGalleryPhotoPoolCount;
extern s32 gGalleryPhotoPoolMin;
extern s32 gGalleryPhotoPoolMax;

void Gallery_LoadUnlockedBackgrounds(void);
s32 Gallery_ControlRecordIsPopulated(PhotoSceneControl* arg0);
s32 Gallery_ControlRecordIsDirty(PhotoSceneControl* arg0);
s32 Gallery_ArraysDiffer(PhotoSceneData* arg0, PhotoSceneData* arg1, s32 arg2);
s32 Gallery_BackgroundsChanged(void);
s32 Gallery_SaveUnlockedBackgrounds(s32 arg0, s32 arg1);
void func_8340051C(void);
void Gallery_ResetPhotoPool(void);
void Gallery_ClearPhotoRecord(PhotoSceneData* arg0);
void Gallery_SwapPhotoRecords(PhotoSceneData* arg0, PhotoSceneData* arg1);
s32 Gallery_PhotoRecordsEqual(PhotoSceneData* arg0, PhotoSceneData* arg1);
void Gallery_SetEnlargeTarget(PhotoSceneData* arg0);
PhotoSceneData* Gallery_GetEnlargeTarget(s32 arg0);
void Gallery_InitPageCursor(GalleryCursor* arg0);
s32 Gallery_CountTextLines(s8* arg0);
s32 Gallery_CountValidPhotos(PhotoSceneData* arg0, s32 arg1, s32 arg2);
s32 Gallery_FindNthEmptySlot(PhotoSceneData* arg0, s32 arg1, s32 arg2, s32 arg3);
s32 Gallery_FindFirstEmptySlot(PhotoSceneData* arg0, s32 arg1);
void Gallery_DrawBluePanel(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Gallery_DrawRoundedFrame(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7);
void Gallery_DrawHeaderBar(s32 arg0, s32 arg1, s32 arg2, Color_RGBA8* arg3, Color_RGBA8* arg4);
void Gallery_DrawImageStrips(u16* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5);
void Gallery_FillRect(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Gallery_DrawWhiteBorder(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
void Gallery_DrawDropShadow(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Gallery_DrawPhotoThumbnail(u16* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6);
void Gallery_DrawSelectionCorners(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void Gallery_DrawStatusIcon(s32 arg0, s32 arg1, s32 arg2);
void Gallery_InitSceneGrid(GallerySceneGrid* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, ModelLoadContext* arg5,
    BinArchive* arg6, PhotoSceneData* arg7, s32 arg8, s32 arg9);
void Gallery_ProcessSceneGrid(GallerySceneGrid* arg0);
void Gallery_FinishActiveScene(void);
s32 Gallery_CountReadyScenesOnPage(GallerySceneGrid* arg0, s32 arg1);
void Gallery_ClearSavedPosition(GalleryCursor* arg0);
void Gallery_SavePosition(GallerySceneGrid* arg0, GalleryCursor* arg1);
void Gallery_RestorePosition(GallerySceneGrid* arg0, GalleryCursor* arg1);
void Gallery_IndexToGridPos(GallerySceneGrid* arg0, s32* arg1, s32* arg2, s32 arg3);
s32 Gallery_GridPosToIndex(GallerySceneGrid* arg0, s32 arg1, s32 arg2);
void Gallery_RebindSceneGridResources(GallerySceneGrid* arg0, ModelLoadContext* arg1, BinArchive* arg2);

void Gallery_InitStringTables(void);
char* Gallery_GetUiString(s32 arg0);
char* Gallery_CopyUiString(char* arg0, s32 arg1, s32 arg2);
char* Gallery_GetCommonUiString(s32 arg0);
char* Gallery_CopyCommonUiString(char* arg0, s32 arg1, u32 arg2);

void Gallery_DrawYesNoOptions(MenuPanelInfo* arg0, s16 arg1);
void Gallery_DrawSaveOptions(MenuPanelInfo* arg0);
void Gallery_DrawDeleteOptions(MenuPanelInfo* arg0);
void Gallery_DrawMessagePrompt(MenuPanelInfo* arg0);
void Gallery_ResetConfirmPrompt(void);
void Gallery_DrawConfirmPrompt(UNUSED s32 arg0);
void Gallery_UpdateConfirmPromptInput(void);
void Gallery_AdvanceConfirmPromptOpen(void);
void Gallery_AdvanceConfirmPromptClose(void);
s32 Gallery_UpdateConfirmPrompt(void);
s32 Gallery_OpenConfirmPrompt(s16 arg0, Controller* arg1);
s32 Gallery_RequestCloseConfirmPrompt(s16 arg0);
s16 Gallery_GetConfirmPromptResult(s16 arg0);

#endif // _FRAGMENT43_H_
