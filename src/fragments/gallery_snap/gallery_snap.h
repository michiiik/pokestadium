#ifndef _FRAGMENT42_H_
#define _FRAGMENT42_H_

#include "global.h"
#include "src/gallery.h"

void Gallery_SnapDrawRoundedFrame(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7);
GallerySceneGrid* Gallery_SnapGetActiveGrid(void);
void Gallery_SnapUpdateMenuIconStates(void);
s32 Gallery_SnapConfirmSaveBackgrounds(void);
s32 Gallery_SnapUpdateMenuInput(void);
void Gallery_SnapResetToMenu(void);
s32 Gallery_SnapUpdateGridInput(GallerySceneGrid* arg0);
void Gallery_SnapDrawScaledImage(u16* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, f32 arg5, f32 arg6);
void DrawMenuScreen(void);
void Gallery_SnapDrawTitleBar(s32 arg0, s32 arg1, s32 arg2);
void Gallery_SnapDrawHintBox(GallerySceneGrid* arg0, s32 arg1, s32 arg2);
void Gallery_SnapInit(s32 arg0, s32 arg1);
void Gallery_SnapDrawGradientQuad(s32 arg0, s32 arg1, s32 arg2, s32 arg3, Color_RGBA8* arg4, Color_RGBA8* arg5);
void Gallery_SnapDrawBackgroundWipe(void);
void DrawPokemonGrid(GallerySceneGrid* arg0);
void Gallery_SnapDrawCursor(GallerySceneGrid* arg0);
void Gallery_SnapDrawBackPrompt(s32 arg0);
void Gallery_SnapDrawFrame(void);
void Gallery_SnapUpdatePasteConfirm(GallerySceneGrid* arg0);
void Gallery_SnapUpdateSaveConfirm(void);
void Gallery_SnapUpdateExitConfirm(void);
void Gallery_SnapUpdateExitDelay(void);
s32 Gallery_SnapUpdate(void);
void Gallery_SnapReadInput(void);
void Gallery_SnapLoop(void);
s32 Gallery_SnapGetResult(void);
void Gallery_SnapMain(s32 arg0, s32 arg1);

#endif // _FRAGMENT42_H_
