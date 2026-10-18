#include "global.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

// seems to allocate a pool area, and init it's MainPool struct?
// the other file is memory_main.c but this one is memory.c?

/**
 * Allocate a memory pool from the main pool. This pool supports arbitrary
 * order for allocation/freeing.
 * Return NULL if there is not enough space in the main pool.
 */
MemoryPool* mem_pool_try_init(u32 size, s32 side) {
    MainPoolBlock* block;
    MemoryPool* ret;

    size = ALIGN4(size);
    block = main_pool_alloc(size, side);
    if (block != NULL) {
        ret = mem_pool_init(block, size);
    }
    return ret; //! UB: Uninitialized return if the earlier call is not done.
}

/**
 * Perform the actual mem pool init. This is called by the earlier function.
 * In earlier EAD titles, this was part of mem_pool_try_init.
 */
// TODO: This function is strange, it cant be using MemoryPool, as it allocates
// more variables than the MemoryPool struct, and it doesnt line up. Whats going
// on with these structs?
MainPool* mem_pool_init(MainPool* mainPool, s32 size) {
    // PORT: the original wrote through the MainPool view of the same memory, relying on 32-bit pointer offsets
    // (the first free block started at offset 0x28). Build the MemoryPool layout explicitly instead.
    MemoryPool* pool = (MemoryPool*)mainPool;
    struct MemoryBlock* first = (struct MemoryBlock*)((u8*)pool + ALIGN16(sizeof(MemoryPool)));
    s32 usable = ALIGN4(size - 3) - (s32)ALIGN16(sizeof(MemoryPool));

    first->next = NULL;
    first->size = usable;
    pool->available = usable;
    pool->firstBlock = first;
    pool->freeList.next = first;
    pool->freeList.size = 0;
    osCreateMesgQueue(&pool->queue, &pool->msgs[0], 1);
    osSendMesg(&pool->queue, NULL, 0);
    return mainPool;
}

/**
 * Allocate from a memory pool. Return NULL if there is not enough space.
 */
void* mem_pool_alloc(MemoryPool* node, s32 size) {
    struct MemoryBlock* freeBlock;
    void* addr;

    osRecvMesg(&node->queue, NULL, 1);

    addr = NULL;
    size = ALIGN4(size) + sizeof(struct MemoryBlock);
    freeBlock = &node->freeList;

    while (freeBlock->next != NULL) {
        if (freeBlock->next->size >= size) {
            addr = (u8*)freeBlock->next + sizeof(struct MemoryBlock); // get data after header
            if (freeBlock->next->size - size <= sizeof(struct MemoryBlock)) {
                freeBlock->next = freeBlock->next->next;
            } else {
                struct MemoryBlock* newBlock = (struct MemoryBlock*)((u8*)freeBlock->next + size);
                newBlock->size = freeBlock->next->size - size; // set size
                newBlock->next = freeBlock->next->next;
                freeBlock->next->size = size; // set size
                freeBlock->next = newBlock;
            }
            break;
        }
        freeBlock = freeBlock->next;
    }
    osSendMesg(&node->queue, 0, 0);
    return addr;
}

/**
 * Free a block that was allocated using mem_pool_alloc.
 */
void mem_pool_free(MemoryPool* pool, void* addr) {
    struct MemoryBlock* block;
    struct MemoryBlock* freeList;

    if (addr != NULL) {
        osRecvMesg(&pool->queue, NULL, 1);
        block = (struct MemoryBlock*)((u8*)addr - sizeof(struct MemoryBlock));
        freeList = pool->freeList.next;
        if (pool->freeList.next == NULL) {
            pool->freeList.next = block;
            block->next = NULL;
        } else if (block < pool->freeList.next) {
            if ((uintptr_t)pool->freeList.next == ((uintptr_t)block + (u32)block->size)) {
                block->size += ((u32)freeList->size);
                block->next = freeList->next;
                pool->freeList.next = block;
            } else {
                block->next = pool->freeList.next;
                pool->freeList.next = block;
            }
        } else {
            while (freeList->next != NULL) {
                if (freeList < block && block < freeList->next) {
                    break;
                }
                freeList = freeList->next;
            }
            if (((uintptr_t)freeList + (u32)freeList->size) == (uintptr_t)block) {
                freeList->size += block->size;
                block = freeList;
            } else {
                block->next = freeList->next;
                freeList->next = block;
            }
            if (block->next != NULL && (uintptr_t)block->next == ((uintptr_t)block + (u32)block->size)) {
                block->size = (u32)block->size + (u32)block->next->size;
                block->next = block->next->next;
            }
        }
        osSendMesg(&pool->queue, NULL, 0);
    }
}

void* MainPool_AllocState(u32 size, s32 side) {
    MainPoolBlock* block;
    void* ptr = NULL;

    size = ALIGN4(size);
    ptr = 0;
    block = main_pool_alloc(size, side);
    if (block != NULL) {
        ptr = MainPool_InitState(block, size);
    }
    return ptr;
}

// PORT: the arena helpers below view the same memory as MainPoolState and MemoryBlock, which only lines up with
// 32-bit pointers (freeSpace == next, listHeadL == size). They are rewritten against MainPoolState with
// pointer-width arithmetic. In this state listHeadL is the number of bytes handed out (not a pointer),
// listHeadR is the base, prev is the bump pointer.
void MainPool_FinalizeAllocation(struct MainPoolState* state) {
    s32 size = ALIGN16((uintptr_t)state->listHeadL + sizeof(struct MainPoolState));

    main_pool_realloc(state, size);
    state->freeSpace = size - sizeof(struct MainPoolState);
}

void* MainPool_InitState(struct MainPoolState* block, s32 size) {
    void* temp_v1 = (void*)((u8*)block + sizeof(struct MainPoolState)); // PORT: header is 0x20 bytes with 64-bit pointers (was 0x10)

    block->listHeadL = 0;
    block->freeSpace = ((size & ~3) - sizeof(struct MainPoolState)); // this doesnt match an ALIGN4 macro or whatnot
    block->listHeadR = temp_v1;
    block->prev = temp_v1;
    return block;
}

void* MainPool_AllocAligned(struct MainPoolState* state, s32 arg1, s32 arg2) {
    uintptr_t prev = (uintptr_t)state->prev;
    uintptr_t aligned;
    uintptr_t ret = 0;
    s32 used;

    if (arg2 > 0) {
        aligned = (prev + (uintptr_t)arg2 - 1) & ~((uintptr_t)arg2 - 1);
    } else {
        aligned = prev;
    }
    used = (s32)(aligned - prev) + arg1;
    if (used > 0) {
        uintptr_t newUsed = (uintptr_t)state->listHeadL + (uintptr_t)used;
        if ((uintptr_t)(u32)state->freeSpace >= newUsed) {
            ret = aligned;
            state->prev = (void*)(prev + (uintptr_t)used);
            state->listHeadL = (void*)newUsed;
        }
    }
    return (void*)ret;
}

void MainPool_ResetStateToOffset(struct MainPoolState* state, s32 size) {
    if ((s32)state->freeSpace >= size) {
        state->listHeadL = (void*)(uintptr_t)size;
        state->prev = (void*)((uintptr_t)state->listHeadR + (uintptr_t)size);
    }
}

void MainPool_ResetState(struct MainPoolState* state) {
    void* mem = (u8*)((uintptr_t)state + sizeof(struct MainPoolState));

    state->listHeadL = 0;
    state->listHeadR = mem;
    state->prev = mem;
}
