#include <stdio.h>
#include <stdlib.h>
#include "../../src/port/port_platform.h"
#include "global.h"
#include "gfx_buffer.h"
#include "src/memory.h"

// PORT: the original kept two 32-bit buffer addresses and indexed them through an s32*. Rewritten with real
// pointers, and the buffers are twice the requested size because a host Gfx command is 16 bytes (N64: 8).
typedef struct UnkStruct800A7428 {
    u8* buf[2];     // the two display-list buffers (double-buffered)
    u8* limit;      // dynamic allocations grow down from the end of the active buffer to here
    s32 unkC;       // requested size in N64 bytes
    s16 unk10;      // active buffer index
} UnkStruct800A7428;

Gfx* gDisplayListHead;
UnkStruct800A7428 D_800A7428;
unk_D_800A7440 D_800A7440;

void Gfx_SwapDisplayListBuffer(void);

void Gfx_InitDisplayListBuffers(u32 arg0, s32 arg1) {
    D_800A7428.unk10 = 0;
    D_800A7428.unkC = arg0;
    D_800A7428.buf[0] = main_pool_alloc_with_func(arg0 * 2, arg1, 'DYN0', NULL);
    D_800A7428.buf[1] = main_pool_alloc_with_func(arg0 * 2, arg1, 'DYN1', NULL);
    Gfx_SwapDisplayListBuffer();
}

void Gfx_FreeDisplayListBuffers(void) {
    main_pool_try_free(D_800A7428.buf[1]);
    main_pool_try_free(D_800A7428.buf[0]);
}

void Gfx_SwapDisplayListBuffer(void) {
    D_800A7428.unk10 ^= 1;
    D_800A7428.limit = D_800A7428.buf[D_800A7428.unk10] + D_800A7428.unkC * 2;
    gDisplayListHead = (Gfx*)D_800A7428.buf[D_800A7428.unk10];
}

void Gfx_GetDisplayListRange(uintptr_t* arg0, uintptr_t* arg1) {
    // arg1 receives the number of bytes written (the callers treat it as the task's data size)
    *arg1 = (uintptr_t)gDisplayListHead - (uintptr_t)D_800A7428.buf[D_800A7428.unk10];
    *arg0 = (uintptr_t)D_800A7428.buf[D_800A7428.unk10];
}

extern void Port_LogAlloc(void* p, int n, void* caller);
void* Gfx_AllocDisplayList(s32 arg0) {
    u8* newLimit;
    void* ret;

    arg0 = ALIGN8(arg0);
    newLimit = D_800A7428.limit - arg0;
    ret = NULL;
    if (newLimit >= (u8*)gDisplayListHead) {
        D_800A7428.limit = newLimit;
        ret = newLimit;
    }
    Port_LogAlloc(ret, arg0, __builtin_return_address(0));
    { // PORT debug: PORT_DLALLOC_AT=<hex offset in the buffer>: log allocations covering it, with the caller
        static long sAt = -2;
        if (sAt == -2) sAt = getenv("PORT_DLALLOC_AT") ? strtol(getenv("PORT_DLALLOC_AT"), NULL, 16) : -1;
        if (sAt >= 0 && ret != NULL) {
            long off = (long)((u8*)ret - D_800A7428.buf[0]);
            if (off < 0 || off >= D_800A7428.unkC * 2) off = (long)((u8*)ret - D_800A7428.buf[1]);
            if (off <= sAt && sAt < off + arg0 + 0x40) {
                fprintf(stderr, "[dlalloc] off=0x%lx size=0x%x caller=%p slide=0x%lx\n", off, arg0, __builtin_return_address(0),
                        (unsigned long)Port_ImageSlide());
            }
        }
    }
    return ret;
}

s32 Gfx_GetDisplayListRemaining(void) {
    return (s32)(D_800A7428.limit - (u8*)gDisplayListHead);
}

// PORT debug: start of the display-list buffer being rendered (for PORT_DLALLOC_AT offsets)
void Port_DlBufferWhere(uintptr_t p, int* which, long* off) {
    for (int k = 0; k < 2; k++) {
        if (p >= (uintptr_t)D_800A7428.buf[k] && p < (uintptr_t)D_800A7428.buf[k] + D_800A7428.unkC * 2) {
            *which = k;
            *off = (long)(p - (uintptr_t)D_800A7428.buf[k]);
            return;
        }
    }
    *which = -1;
    *off = 0;
}
int Port_DlBufferActive(void) {
    return D_800A7428.unk10;
}
uintptr_t Port_DlBufferBase(void) {
    extern Gfx* gDisplayListHead;
    (void)gDisplayListHead;
    return (uintptr_t)D_800A7428.buf[0];
}
