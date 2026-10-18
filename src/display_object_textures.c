#include "display_object_textures.h"
#include <stdio.h>
#include <stdlib.h>
#include "src/geo_layout.h"
#include "src/dma.h"
#include "src/memmap.h"
#include "src/memory.h"
#include "src/util.h"
#include <PR/os_internal_reg.h>
#include <PR/leo.h>
#include "src/memory.h"

typedef struct ret_func_unk_D_800ABE10 {
    /* 0x00 */ char unk00[0x3];
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ char unk04[0x4];
    /* 0x08 */ unk_D_86002F34** unk_08;
    /* 0x0C */ char unk0C[0x8];
    /* 0x14 */ func_unk_D_800ABE10 unk_14;
} ret_func_unk_D_800ABE10;

// .bss
unk_D_800ABE10 D_800ABE10;

void func_80018C40(unk_D_86002F34* arg0, arg1_func_80010CA8 arg1) {
    s32 i;
    s32 j;
    unk_D_86002F34_018* var_s2 = arg0->unk_18;
    unk_D_86002F34_01C* var_s4 = arg0->unk_1C;

    if ((uintptr_t)var_s2 < 0x100000000ULL || (uintptr_t)var_s4 < 0x100000000ULL) {
        // PORT: the tables of a model loaded from ROM are big-endian records (0xC bytes, 32-bit link addresses), and so
        // are the texels they point to.
        const u8* t = (const u8*)Util_ConvertAddrToVirtAddr((uintptr_t)var_s2);
        const u8* c = (const u8*)Util_ConvertAddrToVirtAddr((uintptr_t)var_s4);

        for (i = 0; t != NULL && i < arg0->unk_24; i++, t += 0xC) {
            u8* tex = (u8*)Util_ConvertAddrToVirtAddr(PORT_BE32(*(const u32*)(t + 8)));
            s32 n = (s16)((t[6] << 8) | t[7]);
            if (t[0] != 0 || tex == NULL || (uintptr_t)tex < 0x100000000ULL) {
                continue;
            }
            if (t[1] == unk_D_86002F34_018_GFX_TYPE_2) {
                for (j = 0; j < n; j++, tex += 2) {
                    u16 v = Color_AdjustRGB5551((u16)((tex[0] << 8) | tex[1]), arg1);
                    tex[0] = v >> 8;
                    tex[1] = v;
                }
            } else if (t[1] == unk_D_86002F34_018_GFX_TYPE_3) {
                for (j = 0; j < n; j++, tex += 4) {
                    u32 v = Color_AdjustRGBA8(PORT_BE32(*(u32*)tex), arg1);
                    tex[0] = v >> 24;
                    tex[1] = v >> 16;
                    tex[2] = v >> 8;
                    tex[3] = v;
                }
            }
        }
        for (i = 0; c != NULL && i < arg0->unk_25; i++, c += 0xC) {
            u8* tex = (u8*)Util_ConvertAddrToVirtAddr(PORT_BE32(*(const u32*)(c + 4)));
            s32 n = (s32)PORT_BE32(*(const u32*)c);
            if (tex == NULL || (uintptr_t)tex < 0x100000000ULL) {
                continue;
            }
            for (j = 0; j < n; j++, tex += 2) {
                u16 v = Color_AdjustRGB5551((u16)((tex[0] << 8) | tex[1]), arg1);
                tex[0] = v >> 8;
                tex[1] = v;
            }
        }
        return;
    }

    for (i = 0; i < arg0->unk_24; i++, var_s2++) {
        if (var_s2->unk_00 == 0) {
            if (var_s2->type == unk_D_86002F34_018_GFX_TYPE_2) {
                u16* var_s1 = (u16*)var_s2->texture;
                // PORT: texels are in N64 (big-endian) byte order in memory; the adjust functions take numeric values
                for (j = 0; j < var_s2->num_texels; j++, var_s1++) *var_s1 = Port_Swap16(Color_AdjustRGB5551(Port_Swap16(*var_s1), arg1));
            }
            if (var_s2->type == unk_D_86002F34_018_GFX_TYPE_3) {
                u32* var_s1 = var_s2->texture;
                for (j = 0; j < var_s2->num_texels; j++, var_s1++) *var_s1 = PORT_BE32(Color_AdjustRGBA8(PORT_BE32(*var_s1), arg1));
            }
        }
    }
    for (i = 0; i < arg0->unk_25; i++, var_s4++) {
        u16* var_s1 = var_s4->unk_04;
        for (j = 0; j < var_s4->unk_00; j++, var_s1++) *var_s1 = Port_Swap16(Color_AdjustRGB5551(Port_Swap16(*var_s1), arg1));
    }
}

Fragment* Jpeg_AllocAndDecodeFragment(MemoryBlock* arg0, PRESJPEG* arg1, PRESJPEG* arg2) {
    Fragment* sp1C;
    s32 sp18;
    s32 temp_v0_2;

    sp18 = arg0->size;
    sp1C = MainPool_AllocAligned(arg0, arg2->unk_0C + 0x100, 0x10);

    if (sp1C != NULL) {
        temp_v0_2 = Jpeg_DecodeImage(sp1C, arg2->unk_0C + 0x100, (u8*)arg2 + arg2->unk_08);
        if (temp_v0_2 == 0) {
            MainPool_ResetStateToOffset(arg0, sp18);
            sp1C = NULL;
        } else {
            MainPool_ResetStateToOffset(arg0, sp18 + temp_v0_2);
        }
    }
    return sp1C;
}

Fragment* Yay0_AllocAndDecompressFragment(MainPoolState* arg0, PERSSZP* arg1, PERSSZP* arg2) {
    u8* sp1C = MainPool_AllocAligned(arg0, arg2->decompressed_size2, 0x10);

    if (sp1C != NULL) {
        Yay0_DecompressAndRelocate(arg2, sp1C);
    }
    return sp1C;
}

Fragment* Asset_AllocAndCopyFragment(MainPoolState* arg0, PERSSZP* arg1, s32 arg2, u32 arg3) {
    u32* sp1C;

    sp1C = MainPool_AllocAligned(arg0, arg3, 0x10);
    if (sp1C != NULL) {
        Util_Memcpy(sp1C, (u8*)arg1 + arg2, arg3 >> 2);
    }
    return sp1C;
}

Fragment* Fragment_LoadFromRomRange(MainPoolState* arg0, s32 start, s32 end, PERSSZP* arg3, s32 arg4) {
    UNUSED s32 pad;
    u32 size = ALIGN16(end - start);
    Fragment* var_v1 = NULL;

    if (arg4 >= size) {
        Dma_WriteChunks(arg3, start, end, 0);

        if (PORT_BE32(((u32*)arg3->magic)[0]) == 'PERS' && (PORT_BE32(((u32*)arg3->magic)[1]) == '-SZP')) {
            Port_FixPersHeader(arg3);
            var_v1 = Yay0_AllocAndDecompressFragment(arg0, arg3, arg3);
        } else if (PORT_BE32(((u32*)arg3->magic)[0]) == 'PRES' && (PORT_BE32(((u32*)arg3->magic)[1]) == 'JPEG')) {
            Port_FixPresJpegHeader((PRESJPEG*)arg3);
            var_v1 = Jpeg_AllocAndDecodeFragment(arg0, arg3, arg3);
        } else {
            var_v1 = Asset_AllocAndCopyFragment(arg0, arg3, 0, size);
        }
    }

    return var_v1;
}

Fragment* Fragment_LoadFromLbaRange(MainPoolState* arg0, unk_func_800041C0* arg1, PERSSZP* arg2, u32 arg3) {
    s32 sp3C;
    Fragment* sp38;
    PERSSZP* temp_s1;

    LeoLBAToByte(arg1->unk_00, arg1->unk_02, &sp3C);
    sp3C = ALIGN16(sp3C);

    if (arg3 >= sp3C) {
        temp_s1 = arg1->unk_04 + (u8*)arg2;

        Storage_QueueDiskTransfer(0, arg2, arg1->unk_00, arg1->unk_02, &D_800ABE10.unk_9EC, 0);
        osRecvMesg(&D_800ABE10.unk_9EC, NULL, 1);

        if (PORT_BE32(((u32*)temp_s1->magic)[0]) == 'PERS' && (PORT_BE32(((u32*)temp_s1->magic)[1]) == '-SZP')) {
            Port_FixPersHeader(temp_s1);
            sp38 = Yay0_AllocAndDecompressFragment(arg0, arg2, temp_s1);
        } else if (PORT_BE32(((u32*)temp_s1->magic)[0]) == 'PRES' && (PORT_BE32(((u32*)temp_s1->magic)[1]) == 'JPEG')) {
            Port_FixPresJpegHeader((PRESJPEG*)temp_s1);
            sp38 = Jpeg_AllocAndDecodeFragment(arg0, arg2, temp_s1);
        } else {
            sp38 = Asset_AllocAndCopyFragment(arg0, arg2, arg1->unk_04, arg1->unk_08);
        }
    }

    return sp38;
}

void Fragment_RelocateAndCommit(MainPoolState* arg0, u32 arg1, Fragment* arg2) {
    Memmap_RelocateFragment(arg1, arg2);
    Port_RelocateDataBlob(arg2); // PORT: before the relocation table is released by the reset below
    MainPool_ResetStateToOffset(arg0, ((uintptr_t)arg2 + arg2->sizeInRam) - (uintptr_t)arg0->listHeadR);
}

Fragment* BinArchive_LoadFileFromRomRange(MainPoolState* arg0, BinArchive* arg1, BinArchiveFile* arg2, uintptr_t arg3, s32 arg4) {
    s32 temp_a1 = arg1->unk_04 + arg2->offset;

    return Fragment_LoadFromRomRange(arg0, temp_a1, arg2->size + temp_a1, arg3, arg4);
}

Fragment* BinArchive_LoadFileFromLbaRange(MainPoolState* arg0, BinArchive* arg1, BinArchiveFile* arg2, uintptr_t arg3, s32 arg4) {
    unk_func_800041C0 sp1C;

    Storage_SetByteRange(&sp1C, arg1->unk_04, arg2->offset, arg2->size);
    return Fragment_LoadFromLbaRange(arg0, &sp1C, arg3, arg4);
}

u32 Port_PtrToHandle(void* p);
void* Port_HandleToPtr(u32 h);

Fragment* BinArchive_LoadFileToPool(MainPoolState* arg0, BinArchive* arg1, s32 arg2, uintptr_t arg3, s32 arg4) {
    s32 pad[4];
    Fragment* var_s0 = NULL;
    BinArchiveFile* sp2C = (u8*)arg1 + sizeof(BinArchive) + (arg2 * sizeof(BinArchiveFile));

    if (arg2 < arg1->num_files) {
        if (arg1->unk_00 & 1) {
            var_s0 = (u8*)arg1 + sp2C->offset;
        } else if (!(arg1->unk_00 & 4) && (sp2C->unk_08 != 0)) {
            var_s0 = Port_HandleToPtr(sp2C->unk_08); // PORT64: the 32-bit slot holds a handle (jpeg_stream.c)
        } else {
            if (arg1->unk_00 & 0x80) {
                var_s0 = BinArchive_LoadFileFromLbaRange(arg0, arg1, sp2C, arg3, arg4);
            } else {
                var_s0 = BinArchive_LoadFileFromRomRange(arg0, arg1, sp2C, arg3, arg4);
            }

            if (var_s0 != NULL) {
                if ((PORT_BE32(((u32*)var_s0->magic)[0]) == 'FRAG') && (PORT_BE32(((u32*)var_s0->magic)[1]) == 'MENT')) {
                    Fragment_RelocateAndCommit(arg0, arg1->unk_02, var_s0);
                }
                sp2C->unk_08 = Port_PtrToHandle(var_s0);
            }
        }
    }

    return var_s0;
}

intptr_t Fragment_ProcessGeoLayout(MemoryBlock* arg0, Fragment* arg1, arg1_func_80010CA8 arg2) {
    s32 i;
    unk_D_86002F34* temp_v0_2;
    ret_func_unk_D_800ABE10* temp_v0;
    ret_func_unk_D_800ABE10* (*func)(s32, s32) = arg1;

#ifdef PORT_STATIC_FRAGMENTS
    // PORT: model/animation blobs are data fragments whose entry is a tiny MIPS function returning a model table
    // (`lui v1,hi; addiu v1,v1,lo; ... jr ra`). The function cannot run on the host, so its constant is decoded from
    // the instructions. The table and the layouts it lists are big-endian with 32-bit link addresses.
    {
        const u8* blob = (const u8*)arg1;
        u32 tableLink = 0;
        const u8* table;
        const u8* layouts;
        u32 count;
        ret_func_unk_D_800ABE10* host;
        unk_D_86002F34** nodes;

        for (i = 0; i < 12 && tableLink == 0; i++) {
            u32 w = PORT_BE32(*(u32*)(blob + 0x20 + i * 4));
            if ((w >> 26) == 0x0F) { // lui
                u32 next = PORT_BE32(*(u32*)(blob + 0x24 + i * 4));
                if ((next >> 26) == 0x09 && ((next >> 21) & 31) == ((w >> 16) & 31)) { // addiu on the same register
                    tableLink = (w << 16) + (s32)(s16)(next & 0xFFFF);
                }
            }
        }
        if (getenv("PORT_TRACE")) { int q; fprintf(stderr, "[model] blob=%p tableLink=0x%x:", (void*)blob, tableLink); for (q = 0; q < 0x40; q++) fprintf(stderr, "%s%02x", (q & 3) ? "" : " ", blob[q]); fputc(10, stderr); }
        if (tableLink == 0) {
            return NULL;
        }
        {
            u32 pseudo = Port_VAddrOfLink((void*)blob, tableLink);
            table = (const u8*)Util_ConvertAddrToVirtAddr(pseudo != 0 ? pseudo : tableLink);
        }
        count = table[3];
        layouts = (const u8*)Util_ConvertAddrToVirtAddr(PORT_BE32(*(u32*)(table + 8)));

        host = MainPool_AllocAligned(arg0, sizeof(ret_func_unk_D_800ABE10), 8);
        nodes = MainPool_AllocAligned(arg0, (count + 1) * sizeof(unk_D_86002F34*), 8);
        if (getenv("PORT_TRACE")) fprintf(stderr, "[model] table=%p count=%u layouts=%p host=%p nodes=%p\n", (void*)table, count, (void*)layouts, (void*)host, (void*)nodes);
        if (host == NULL || nodes == NULL) {
            return NULL;
        }
        bzero(host, sizeof(*host));
        host->unk_03 = count;
        host->unk_08 = nodes;
        host->unk_14 = arg1;
        for (i = 0; i < count; i++) {
            temp_v0_2 = process_geo_layout(arg0, (void*)(uintptr_t)PORT_BE32(*(u32*)(layouts + i * 4)));
            if (temp_v0_2 != NULL && temp_v0_2->node.type == 0xE) {
                temp_v0_2->unk_28 = arg1;
                if (arg2.raw != 0) {
                    func_80018C40(temp_v0_2, arg2);
                }
            }
            nodes[i] = temp_v0_2;
        }
        return host;
    }
#endif
    temp_v0 = func(0, 0);
    temp_v0->unk_14 = arg1;

    for (i = 0; i < temp_v0->unk_03; i++) {
        temp_v0_2 = process_geo_layout(arg0, temp_v0->unk_08[i]);

        if (temp_v0_2->node.type == 0xE) {
            temp_v0_2->unk_28 = arg1;
            if (arg2.raw != 0) {
                func_80018C40(temp_v0_2, arg2);
            }
        }
        temp_v0->unk_08[i] = temp_v0_2;
    }

    return temp_v0;
}

void Fragment_ProcessGeoLayoutList(MemoryBlock* arg0, arg1_func_80019420* arg1) {
    while (arg1->unk_00) {
        if (arg1->unk_00 == 3) {
            arg1->unk_04 = process_geo_layout(arg0, arg1->unk_04);
        }

        arg1++;
    }
}

void PokeIcon_LoadModelArchive(unk_func_80019600* arg0) {
    UNUSED s32 pad;
    MemoryBlock* sp28;
    Fragment* sp24;

    sp28 = MainPool_InitState(arg0->pool, arg0->size);
    sp24 = BinArchive_LoadFileToPool(sp28, D_800ABE10.unk_A04.unk_00, arg0->unk_02, D_800ABE10.unk_A04.unk_08, 0x28000);

    if (sp24 != NULL) {
        arg0->unk_08 = Fragment_ProcessGeoLayout(sp28, sp24, arg0->unk_18);
    } else {
        arg0->unk_08 = 0;
    }

    arg0->size = sp28->size + 0x10;
}

// PORT: a resource table in a ROM fragment is big-endian with 8-byte records {s8 type; s16; u32 address} and a zero
// type at the end; returns the host-layout copy (pointer-sized address field), allocated from 'pool'.
arg1_func_80019420* Port_ConvertResourceTable(MemoryBlock* pool, const void* data) {
    const u8* raw = (const u8*)data;
    arg1_func_80019420* out;
    s32 n = 0;
    s32 k;

    while (raw[n * 8] != 0) {
        n++;
    }
    out = MainPool_AllocAligned(pool, (n + 1) * sizeof(arg1_func_80019420), 8);
    if (out != NULL) {
        for (k = 0; k <= n; k++) {
            out[k].unk_00 = (s8)raw[k * 8];
            out[k].unk_02 = (s16)((raw[k * 8 + 2] << 8) | raw[k * 8 + 3]);
            out[k].unk_04 = (void*)(uintptr_t)PORT_BE32(*(const u32*)(raw + k * 8 + 4));
        }
    }
    return out;
}

void PokeIcon_LoadMultipleModels(unk_func_80019600* arg0) {
    Fragment* temp_v0;
    MainPoolState* temp_s3;
    s32 var_s2;
    intptr_t* temp_s6; // PORT: receives table pointers
    s16* var_s0;

    temp_s6 = arg0->unk_08;
    var_s2 = 0;
    temp_s3 = MainPool_InitState(arg0->pool, arg0->size);
    var_s0 = arg0->unk_0C;

    while (*var_s0 != -1) {
        temp_v0 = BinArchive_LoadFileToPool(temp_s3, D_800ABE10.unk_A04.unk_04, *var_s0, D_800ABE10.unk_A04.unk_08, 0x28000);
        if (temp_v0 == NULL) {
            break;
        }

        {
            arg1_func_80019420* tbl = Port_ConvertResourceTable(temp_s3, temp_v0 + 1); // PORT: host-layout copy
            temp_s6[var_s2] = (intptr_t)tbl;
            if (tbl != NULL) Fragment_ProcessGeoLayoutList(temp_s3, tbl);
        }
        var_s0++;
        var_s2++;
    }

    temp_s6[var_s2] = 0;
    arg0->size = temp_s3->listHeadL + 1;
}

void PokeIcon_ThreadMain(UNUSED void* arg0) {
    unk_func_80019600* sp34;

    __osSetFpcCsr(0x01000C01);

    while (true) {
        osRecvMesg(&D_800ABE10.unk_9D0, &sp34, 1);

        switch (sp34->unk_00) {
            case 1:
                PokeIcon_LoadModelArchive(sp34);
                break;

            case 2:
                PokeIcon_LoadMultipleModels(sp34);
                break;
        }

        if (sp34->queue != NULL) {
            osSendMesg(sp34->queue, sp34, 1);
        }
    }
}

void PokeIcon_StartThread(void) {
    osCreateMesgQueue(&D_800ABE10.unk_9D0, &D_800ABE10.unk_9B0, 8);
    osCreateMesgQueue(&D_800ABE10.unk_9EC, &D_800ABE10.unk_9E8, 1);
    osCreateThread(&D_800ABE10.unk_000, 7, PokeIcon_ThreadMain, NULL, &D_800ABE10.unk_9B0, 0xA);
    osStartThread(&D_800ABE10.unk_000);
}

ModelLoadContext* PokeIcon_AllocFramebuffers(u32 arg0) {
    s16 i = 1;
    ModelLoadContext* temp_v0 = main_pool_alloc(sizeof(ModelLoadContext), 1);

    temp_v0->flags = arg0;
    temp_v0->pendingLoads = 0;
    temp_v0->bufferIndex = 0;
    temp_v0->loadedModel = 0;
    temp_v0->modelBuffers[0] = NULL;
    temp_v0->modelBuffers[1] = NULL;
    temp_v0->backgroundBuffer = NULL;
    temp_v0->colorVariant.raw = 0;

    if (arg0 & 1) {
        temp_v0->modelBuffers[0] = main_pool_alloc(0x3C000, 1);
        if (arg0 & 4) {
            temp_v0->modelBuffers[1] = main_pool_alloc(0x3C000, 1);
        } else if (arg0 & 8) {
            for (i = 1; i < 5; i++) {
                temp_v0->modelBuffers[i] = main_pool_alloc(0x3C000, 1);
            }
        }
    }

    if (arg0 & 2) {
        temp_v0->backgroundBuffer = main_pool_alloc(0x18000, 1);
    }

    osCreateMesgQueue(&temp_v0->modelQueue, &temp_v0->modelMessage, 2);
    osCreateMesgQueue(&temp_v0->backgroundQueue, &temp_v0->backgroundMessage, 2);
    return temp_v0;
}

#if defined(VERSION_US_REV2)
#pragma GLOBAL_ASM("asm/us/nonmatchings/display_object_textures/PokeIcon_OpenModelArchives.s")
#else
void PokeIcon_OpenModelArchives(void) {
    D_800ABE10.unk_A04.unk_00 = BinArchive_Open(0x920000, NULL, 1, 1);
    D_800ABE10.unk_A04.unk_04 = BinArchive_Open(0x8CC000, NULL, 1, 1);
    D_800ABE10.unk_A04.unk_08 = main_pool_alloc(0x28000, 1);
}
#endif

s32 PokeIcon_RequestFrameLoad(ModelLoadContext* arg0, u16 arg1, arg1_func_80010CA8 arg2) {
    s32 sp1C;
    sp18_func_800198E4* sp18;
    s32 var_v1;

    sp1C = 0;
    if ((arg0->flags & 1) && !(arg0->pendingLoads & 1)) {
        sp18 = Util_Malloc(sizeof(sp18_func_800198E4));
        if (sp18 != NULL) {
            var_v1 = arg0->bufferIndex;
            if (arg0->flags & 4) {
                var_v1 ^= 1;
            }

            sp18->unk_00 = 1;
            sp18->unk_02 = arg1 - 1;
            sp18->unk_04 = &arg0->modelQueue;
            sp18->unk_08 = 0;
            sp18->unk_0C = 0;
            sp18->unk_10 = arg0->modelBuffers[var_v1];
            sp18->unk_14 = 0x3C000;
            sp18->unk_18 = arg2;

            if ((arg0->loadedModel != NULL) && (arg0->loadedModel->modelId == arg1) && (arg0->colorVariant.raw == sp18->unk_18.raw)) {
                osSendMesg(sp18->unk_04, sp18, 1);
            } else {
                osSendMesg(&D_800ABE10.unk_9D0, sp18, 1);
            }

            arg0->pendingLoads |= 1;
            sp1C = 1;
        }
    }

    return sp1C;
}

s32 PokeIcon_RequestFrameLoadWithVariant(ModelLoadContext* arg0, u16 arg1, arg1_func_80010CA8 arg2, s16 arg3) {
    if ((arg0->flags & 8) && !(arg0->pendingLoads & 1)) {
        arg0->bufferIndex = arg3;
        PokeIcon_RequestFrameLoad(arg0, arg1, arg2);
    }
    return 0;
}

s32 PokeIcon_RequestBackgroundLoad(ModelLoadContext* arg0, intptr_t arg1, intptr_t arg2) {
    s32 sp1C = 0;
    sp18_func_800198E4* temp_v0;

    if ((arg0->flags & 2) && !(arg0->pendingLoads & 2)) {
        temp_v0 = Util_Malloc(sizeof(sp18_func_800198E4));
        if (temp_v0 != NULL) {
            temp_v0->unk_00 = 2;
            temp_v0->unk_02 = 0;
            temp_v0->unk_04 = &arg0->backgroundQueue;
            temp_v0->unk_08 = arg2;
            temp_v0->unk_0C = arg1;
            temp_v0->unk_10 = arg0->backgroundBuffer;
            temp_v0->unk_14 = 0x18000;

            osSendMesg(&D_800ABE10.unk_9D0, temp_v0, 1);

            sp1C = 1;
            arg0->pendingLoads |= 2;
        }
    }

    return sp1C;
}

s32 PokeIcon_ApplyLoadResult(ModelLoadContext* arg0, sp18_func_800198E4* arg1) {
    s32 sp1C = 0;

    switch (arg1->unk_00) {
        case 1:
            arg0->pendingLoads &= ~1;
            if (arg1->unk_08 != 0) {
                if (arg0->flags & 4) {
                    arg0->bufferIndex ^= 1;
                }
                arg0->loadedModel = arg1->unk_08;
                arg0->modelSize = arg1->unk_14;
                arg0->colorVariant = arg1->unk_18;
            }
            sp1C = 1;
            break;

        case 2:
            arg0->pendingLoads &= ~2;
            arg0->loadedBackground = arg1->unk_08;
            arg0->backgroundSize = arg1->unk_14;
            sp1C = 1;
            break;
    }

    Util_Free(arg1);
    return sp1C;
}

s32 PokeIcon_PollFrameLoad(ModelLoadContext* arg0) {
    sp18_func_800198E4* sp1C;
    s32 sp18 = 0;

    if (osRecvMesg(&arg0->modelQueue, &sp1C, 0) != -1) {
        sp18 = PokeIcon_ApplyLoadResult(arg0, sp1C);
    }

    return sp18;
}

s32 PokeIcon_PollBackgroundLoad(ModelLoadContext* arg0) {
    OSMesg sp1C;
    s32 sp18 = 0;

    if (osRecvMesg(&arg0->backgroundQueue, &sp1C, 0) != -1) {
        sp18 = PokeIcon_ApplyLoadResult(arg0, sp1C);
    }

    return sp18;
}

s32 PokeIcon_WaitFrameLoad(ModelLoadContext* arg0) {
    OSMesg sp1C;

    osRecvMesg(&arg0->modelQueue, &sp1C, 1);
    return PokeIcon_ApplyLoadResult(arg0, sp1C);
}

s32 PokeIcon_WaitBackgroundLoad(ModelLoadContext* arg0) {
    OSMesg sp1C;

    osRecvMesg(&arg0->backgroundQueue, &sp1C, 1);
    return PokeIcon_ApplyLoadResult(arg0, sp1C);
}

unk_D_86002F30* Model_LoadByArchiveIndex(s32 arg0) {
    Fragment* sp2C;
    arg1_func_80010CA8 sp28;
    unk_D_86002F30* sp24 = NULL;
    MemoryBlock* sp20;

    sp2C = BinArchive_GetFile(D_800ABE10.unk_A04.unk_00, arg0 - 1);
    if (getenv("PORT_TRACE")) fprintf(stderr, "[model] index=%d archive=%p file=%p\n", arg0, (void*)D_800ABE10.unk_A04.unk_00, (void*)sp2C);
    if (sp2C != NULL) {
        sp20 = MainPool_AllocState(main_pool_get_available(), 0);
        sp28.raw = 0;
        sp24 = Fragment_ProcessGeoLayout(sp20, sp2C, sp28);
        MainPool_FinalizeAllocation(sp20);
    }

    return sp24;
}

unk_D_86002F30* Model_LoadForPokemon(BattleMon* arg0) {
    Fragment* sp2C;
    arg1_func_80010CA8 sp28;
    unk_D_86002F30* sp24 = NULL;
    MemoryBlock* sp20;

    sp2C = BinArchive_GetFile(D_800ABE10.unk_A04.unk_00, arg0->unk_00.unk_00 - 1);
    if (sp2C != NULL) {
        sp20 = MainPool_AllocState(main_pool_get_available(), 0);
        Model_ComputeSizeVariant(&sp28, arg0);
        sp24 = Fragment_ProcessGeoLayout(sp20, sp2C, sp28);
        MainPool_FinalizeAllocation(sp20);
    }
    return sp24;
}

intptr_t Model_LoadWithScale(s32 arg0, s32 arg1) { // PORT: returns the model table pointer (was s32)
    Fragment* sp2C;
    arg1_func_80010CA8 sp28;
    intptr_t sp24 = 0;
    MemoryBlock* sp20;

    sp2C = BinArchive_GetFile(D_800ABE10.unk_A04.unk_00, arg0 - 1);
    if (sp2C != NULL) {
        sp20 = MainPool_AllocState(main_pool_get_available(), 0);
        sp28.raw = 0;
        sp28.unk_00 = arg1 * 64;
        sp24 = Fragment_ProcessGeoLayout(sp20, sp2C, sp28);
        MainPool_FinalizeAllocation(sp20);
    }
    return sp24;
}

arg1_func_80019420* BattleAnim_LoadResourceTable(s32 arg0) {
    Fragment* sp24;
    arg1_func_80019420* sp20;
    MemoryBlock* sp1C;

    sp20 = NULL;
    sp24 = BinArchive_GetFile(D_800ABE10.unk_A04.unk_04, arg0);
    if (sp24 != NULL) {
        sp1C = MainPool_AllocState(main_pool_get_available(), 0);
        sp20 = Port_ConvertResourceTable(sp1C, sp24->data);
        if (sp20 != NULL) Fragment_ProcessGeoLayoutList(sp1C, sp20);
        MainPool_FinalizeAllocation(sp1C);
    }
    return sp20;
}

Fragment* Fragment_LoadAndRelocateFromRomRange(MainPoolState* arg0, u32 arg1, intptr_t arg2, intptr_t arg3, PERSSZP* arg4, s32 arg5) {
    Fragment* sp24 = Fragment_LoadFromRomRange(arg0, arg2, arg3, arg4, arg5);

    if (sp24 != NULL) {
        Fragment_RelocateAndCommit(arg0, arg1, sp24);
    }
    return sp24;
}
