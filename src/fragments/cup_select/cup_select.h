#ifndef _FRAGMENT60_H_
#define _FRAGMENT60_H_

#include "global.h"
#include "src/geo_node.h"

void CupSelect_PollController(void);
void CupSelect_DrawSelectionCorners(s16 arg0, s16 arg1, s16 arg2, s16 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7);
s32 CupSelect_IconGeoPostCallback(s32 arg0, unk_func_80011B94* arg1);
s32 CupSelect_DividerGeoPostCallback(s32 arg0, unk_func_80011B94* arg1);
void CupSelect_RenderFrame(s32 arg0, s32 arg1);
void CupSelect_BuildDivisionList(void);
s32 CupSelect_HandleInput(void);
s32 CupSelect_Loop(void);
void CupSelect_InitGeoLayouts(void);
s32 CupSelect_Main(UNUSED s32 arg0, UNUSED s32 arg1);

#endif // _FRAGMENT60_H_
