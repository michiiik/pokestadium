#ifndef _6A40_H_
#define _6A40_H_

#include "global.h"

void Gfx_InitDisplayListBuffers(u32 arg0, s32 arg1);
void Gfx_FreeDisplayListBuffers(void);
void Gfx_SwapDisplayListBuffer(void);
void Gfx_GetDisplayListRange(uintptr_t* arg0, uintptr_t* arg1);
void* Gfx_AllocDisplayList(s32 arg0);
// PORT: a display list the original allocated by its N64 size (8 bytes per command); host commands are sizeof(Gfx)
#define GFX_N64_BYTES(n) ((n) / 8 * (s32)sizeof(Gfx))
s32 Gfx_GetDisplayListRemaining(void);

#endif // _6A40_H_
