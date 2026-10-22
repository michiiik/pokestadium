#include "global.h"
#include "gfx_buffer.h"

typedef struct UnkStruct800A7428 {
    u32* unk0;
    u32* unk4;
    u32 unk8;
    s32 unkC;
    s16 unk10;
} UnkStruct800A7428; // size = 0x12

Gfx* gDisplayListHead;
UnkStruct800A7428 gDLBuffers;
ClipRect gClipRect;

void Gfx_SwapDisplayListBuffer(void);

void Gfx_InitDisplayListBuffers(u32 buffer_size, s32 pool_side) {
    gDLBuffers.unk10 = 0;
    gDLBuffers.unkC = buffer_size;
    gDLBuffers.unk0 = main_pool_alloc_with_func(buffer_size, pool_side, 'DYN0', NULL);
    gDLBuffers.unk4 = main_pool_alloc_with_func(buffer_size, pool_side, 'DYN1', NULL);
    Gfx_SwapDisplayListBuffer();
}

void Gfx_FreeDisplayListBuffers(void) {
    main_pool_try_free(gDLBuffers.unk4);
    main_pool_try_free(gDLBuffers.unk0);
}

void Gfx_SwapDisplayListBuffer(void) {
    s32* temp_v0 = (s32*)&gDLBuffers;

    gDLBuffers.unk10 ^= 1;
    temp_v0 += gDLBuffers.unk10;

    gDLBuffers.unk8 = *temp_v0 + gDLBuffers.unkC;
    gDisplayListHead = *temp_v0;
}

void Gfx_GetDisplayListRange(Gfx** arg0, Gfx** arg1) {
    s32* ptr = (s32*)&gDLBuffers;
    *arg1 = (Gfx*)((uintptr_t)gDisplayListHead - ptr[gDLBuffers.unk10]);
    *arg0 = (Gfx*)(ptr[gDLBuffers.unk10]);
}

void* Gfx_AllocDisplayList(s32 size) {
    u32 temp_v0;
    u32 ret;

    size = ALIGN8(size);
    temp_v0 = (gDLBuffers.unk8 - size);
    ret = NULL;
    if (temp_v0 >= (uintptr_t)gDisplayListHead) {
        gDLBuffers.unk8 = temp_v0;
        ret = temp_v0;
    }
    return ret;
}

s32 Gfx_GetDisplayListRemaining(void) {
    return gDLBuffers.unk8 - (uintptr_t)gDisplayListHead;
}
