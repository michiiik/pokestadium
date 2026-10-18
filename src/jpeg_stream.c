#include "jpeg_stream.h"
#include <stdio.h>
#include <stdlib.h>
#include "src/memmap.h"
#include "src/jpeg_stream.h"
#include "src/flash.h"
#include "src/dma.h"
#include "src/dp_intro.h"
#include "src/util.h"
#include <PR/leo.h>

typedef enum {
    JPEG_MARKER_START_OF_FRAME = 0xC0,
    JPEG_MARKER_HUFFMAN_TABLE = 0xC4,
    JPEG_MARKER_RESTART0 = 0xD0,
    JPEG_MARKER_RESTART7 = 0xD7,
    JPEG_MARKER_START_OF_IMAGE = 0xD8,
    JPEG_MARKER_END_OF_IMAGE = 0xD9,
    JPEG_MARKER_START_OF_SCAN = 0xDA,
    JPEG_MARKER_QUANTIZATION_TABLE = 0xDB,
} JpegStreamMarker;

static u8 D_800A60C0[0x200];
static OSMesgQueue D_800A62C0;
static OSMesg D_800A62D8;

s32 Jpeg_ReadBigEndianU16(u8* arg0) {
    return ((arg0[0] << 8) | arg0[1]) & 0xFFFF;
}

void Jpeg_ParseStreamMarkers(unk_func_80003680_sp300* arg0, u8* arg1) {
    s32 temp_v0_2;
    s32 var_s2;
    s32 var_v0;
    u8 temp_t6;
    s32 temp_v0;

    var_s2 = 1;
    arg0->unk_01 = 0;

    while (var_s2 != 0) {
        // JPEG data consists of streams, which are identified by a marker.
        // Each marker is in the form [0xFF, 0xID] where ID identifies the type of stream.
        if (*arg1++ == 0xFF) {
            temp_v0 = *arg1++;
            switch (temp_v0) {
                case 0:
                    break;

                case JPEG_MARKER_END_OF_IMAGE:
                    var_s2 = 0;
                    break;

                case JPEG_MARKER_QUANTIZATION_TABLE:
                    arg0->quantization_table = arg1 + 2;
                    arg1 += Jpeg_ReadBigEndianU16(arg1);
                    break;

                case JPEG_MARKER_HUFFMAN_TABLE:
                    arg0->huffman_table = arg1 + 2;
                    arg1 += Jpeg_ReadBigEndianU16(arg1);
                    break;

                case JPEG_MARKER_START_OF_FRAME:
                    arg0->start_of_frame = arg1 + 2;
                    arg1 += Jpeg_ReadBigEndianU16(arg1);
                    break;

                case JPEG_MARKER_START_OF_SCAN:
                    arg1 += Jpeg_ReadBigEndianU16(arg1);
                    arg0->start_of_scan = arg1;
                    break;

                case JPEG_MARKER_START_OF_IMAGE:
                    break;

                default:
                    if ((temp_v0 >= JPEG_MARKER_RESTART0) && (temp_v0 <= JPEG_MARKER_RESTART7)) {
                        arg0->unk_01 = 1;
                    } else {
                        arg1 += Jpeg_ReadBigEndianU16(arg1);
                    }
                    break;
            }
        }
    }

    // Checking the second component's ID,
    // this seems weird as it will break entirely if number of components is less than 3.
    // Looking at every JPEG used in the Stadium rom, no JPEGs use this ID, they're all 0x22.
    if (arg0->start_of_frame[7] == 0x21) {
        arg0->unk_00 = 0;
    } else {
        arg0->unk_00 = 2;
    }

    arg0->width = (arg0->start_of_frame[3] << 8) | arg0->start_of_frame[4];
    arg0->height = (arg0->start_of_frame[1] << 8) | arg0->start_of_frame[2];
}

extern u64 njpgdspMainTextStart[];
extern u64 njpgdspMainDataStart[];

void Jpeg_InitializeDspTask(UnkStruct80001380* arg0, unk_func_80003680_sp90* arg1) {
    arg0->task.t.type = 4;
    arg0->task.t.flags = 0;
    arg0->task.t.ucode_boot = rspbootTextStart;
    arg0->task.t.ucode_boot_size = (uintptr_t)_binary_assets_us_F3DEX2_bin_start - (uintptr_t)rspbootTextStart;
    arg0->task.t.ucode = njpgdspMainTextStart;
    arg0->task.t.ucode_size = 0x1000;
    arg0->task.t.ucode_data = njpgdspMainDataStart;
    arg0->task.t.ucode_data_size = 0x800;
    arg0->task.t.dram_stack = NULL;
    arg0->task.t.dram_stack_size = 0;
    arg0->task.t.output_buff = NULL;
    arg0->task.t.output_buff_size = NULL;
    arg0->task.t.data_ptr = arg1;
    arg0->task.t.data_size = 0x20;
    arg0->task.t.yield_data_ptr = D_800A60C0;
    arg0->task.t.yield_data_size = 0x200;

    osCreateMesgQueue(&arg0->queue, &arg0->mesg, 1);
}

void Jpeg_BuildDspInput(JpegDecoder* decoder, JpegHuffmanTable* hTable, unk_func_80003680_sp300* arg2) {
    decoder->imageData = arg2->start_of_scan;
    decoder->mode = arg2->unk_00;
    decoder->unk_05 = 2;
    decoder->hTablePtrs[0] = &hTable[0];
    decoder->hTablePtrs[1] = &hTable[2];
    decoder->hTablePtrs[2] = &hTable[1];
    decoder->hTablePtrs[3] = &hTable[3];
    decoder->unk_18 = arg2->unk_01;
}

void Jpeg_BuildDspOutput(unk_func_80003680_sp90* arg0, u8(arg1)[2][0x80], unk_func_80003680_sp300* arg2) {
    arg0->unk_00 = 0;
    arg0->unk_08 = arg2->unk_00;
    arg0->unk_04 = 1;
    arg0->unk_0C = (uintptr_t)&arg1[0];
    arg0->unk_10 = (uintptr_t)&arg1[1];
    arg0->unk_14 = (uintptr_t)&arg1[1];
}

extern void Port_TraceJpeg(const char* what, int a, int b, int c);
s32 Jpeg_DecodeImage(uintptr_t addr, s32 arg1, u8* arg2) {
    UnkStruct80001380 sp318;
    unk_func_80003680_sp300 sp300;
    u8 sp200[2][0x80];
    JpegHuffmanTable spB0[4];
    u16* temp_s0;
    u8* temp_s3;
    unk_func_80003680_sp90 sp90;
    JpegDecoder sp74;
    JpegDecoderState sp60;
    u32 var_s1;
    s32 i;
    s32 j;
    s32 var_s4 = 0;
    s32 var_s2 = 0;

    temp_s3 = Util_Malloc(0x110);
    temp_s0 = Util_Malloc(0x210);

    Jpeg_ParseStreamMarkers(&sp300, arg2);
    Port_TraceJpeg("decode", sp300.width, sp300.height, arg1);
    JpegUtils_ProcessQuantizationTable(sp300.quantization_table, sp200, 2);
    JpegUtils_ProcessHuffmanTable(sp300.huffman_table, spB0, temp_s3, temp_s0, 4);

    Util_Free(temp_s0);
    Util_Free(temp_s3);

    Jpeg_BuildDspInput(&sp74, &spB0, &sp300);
    Jpeg_BuildDspOutput(&sp90, &sp200, &sp300);
    Jpeg_InitializeDspTask(&sp318, &sp90);

    while (Display_IsFrameReady() == 0) {}

    for (i = 0; i < sp300.height; i += 16) {
        for (j = 0; j < sp300.width; j += 16) {
            if (arg1 < (var_s2 + 0x300)) {
                Port_TraceJpeg("overflow", var_s2, arg1, 0);
                return 0;
            }

            if (Jpeg_DecodeMcus(&sp74, addr, 1, var_s4, &sp60) != 0) {
                Port_TraceJpeg("mcu-error", i, j, 0);
                return 0;
            }

            sp90.unk_00 = addr;
            Sched_SubmitTask(&sp318, 1);
            var_s4 = 1;
            Sched_WaitForTaskMessage(&sp318);

            addr += 0x200;
            var_s2 += 0x200;
        }
    }

    return var_s2;
}

void Dma_InitializeCompletionQueue(void) {
    osCreateMesgQueue(&D_800A62C0, &D_800A62D8, 1);
}

void Yay0_DecompressAndRelocate(u8* in_header, u8* memory) {
    UNUSED s32 pad2;
    PERSSZP* header = in_header;
    u32* sp2C = (u32*)header + 6;
    UNUSED s32 pad;
    s32 sp24 = header->decompressed_size2 - header->decompressed_size1;
    u32 var_v0;

    if (getenv("PORT_TRACE")) fprintf(stderr, "[yay0] hdr=%p mem=%p hsize=0x%x d1=0x%x d2=0x%x patches=%u\n", (void*)in_header, (void*)memory, header->header_size, header->decompressed_size1, header->decompressed_size2, header->unk_14);
    osInvalICache(memory, header->decompressed_size2);
    osInvalDCache(memory, header->decompressed_size2);

    Yay0_Decompress(in_header + header->header_size, memory);

    for (var_v0 = 0; var_v0 < header->unk_14; var_v0++) {
        // PORT: patch pairs are (value, offset) big-endian words; the patched data stays big-endian
        u32 value = PORT_BE32(sp2C[0]);
        u8* dst = memory + PORT_BE32(sp2C[1]);
        dst[0] = value >> 24;
        dst[1] = value >> 16;
        dst[2] = value >> 8;
        dst[3] = value;
        sp2C += 2;
    }

    if (sp24 != 0) {
        bzero(memory + header->decompressed_size1, sp24);
    }

    osWritebackDCache(memory, header->decompressed_size2);
}

void Dma_ReadChunks(u8* arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 var_s0;
    OSMesgQueue* mesq = &D_800A62C0;

    for (; arg2 > 0; arg2 -= var_s0) {
        if (arg2 >= 0x1000) {
            var_s0 = 0x1000;
        } else {
            var_s0 = arg2;
        }

        Storage_QueueSramTransfer(arg3, arg0, arg1, var_s0, mesq, 0);
        osRecvMesg(mesq, NULL, 1);

        arg0 += var_s0;
        arg1 += var_s0;
    }
}

s32 Flash_ProgramPages(u8* arg0, s16 arg1, s16 arg2, s32 arg3) {
    OSMesg sp44;
    OSMesgQueue* mesq = &D_800A62C0;

    for (; arg2 > 0;) {
        Storage_QueueFlashTransfer(arg3, arg0, arg1, mesq, 0);
        osRecvMesg(mesq, &sp44, 1);

        if ((arg3 == 1) && (Flash_ProgramPage(arg1) != 0)) {
            return -1;
        }

        arg0 += 0x80;
        arg1++;
        arg2--;
    }

    return 0;
}

s32 Flash_EraseSectorByIndex(s16 arg0) {
    return Flash_EraseSector(arg0 << 7);
}

void Dma_WriteChunks(uintptr_t arg0, uintptr_t arg1, uintptr_t arg2, s32 arg3) {
    s32 var_s0;
    s32 var_s1;
    OSMesgQueue* mesq = &D_800A62C0;

    for (var_s1 = arg2 - arg1; var_s1 > 0; var_s1 -= var_s0) {
        if (var_s1 >= 0x1000) {
            var_s0 = 0x1000;
        } else {
            var_s0 = var_s1;
        }

        Storage_QueueRomRead(arg3, arg0, arg1, var_s0, mesq, 0);
        osRecvMesg(mesq, NULL, 1);

        arg0 += var_s0;
        arg1 += var_s0;
    }
}

void Dma_CopyChunks(u8* arg0, u8* arg1, u8* arg2) {
    s32 var_s0;
    s32 var_s1;
    OSMesgQueue* mesq = &D_800A62C0;

    for (var_s1 = arg2 - arg1; var_s1 > 0; var_s1 -= var_s0) {
        if (var_s1 >= 0x1000) {
            var_s0 = 0x1000;
        } else {
            var_s0 = var_s1;
        }

        Storage_QueueRomWrite(arg0, arg1, var_s0, mesq, 0);
        osRecvMesg(mesq, NULL, 1);

        arg0 += var_s0;
        arg1 += var_s0;
    }
}

void* Jpeg_LoadDecodedImage(u8* addr, PRESJPEG* arg1, s32 side) {
    intptr_t new_addr = main_pool_alloc(arg1->unk_0C + 0x100, side);

    if (new_addr != NULL) {
        s32 size = Jpeg_DecodeImage(new_addr, arg1->unk_0C + 0x100, (uintptr_t)arg1 + arg1->unk_08);

        if (size == 0) {
            main_pool_try_free(new_addr);
            new_addr = NULL;
        } else if (side == 0) {
            main_pool_realloc(new_addr, size);
        }
    }

    main_pool_try_free(addr);

    return new_addr;
}

void* Yay0_LoadDecompressedAsset(intptr_t arg0, PERSSZP* arg1, s32 side) {
    void* vaddr = main_pool_alloc(arg1->decompressed_size2, side);

    if (vaddr != NULL) {
        Yay0_DecompressAndRelocate(arg1, vaddr);
    }

    main_pool_try_free((void*)arg0);

    return vaddr;
}

u32* Asset_CopyUncompressed(u8* arg0, s32 arg1, u32 arg2, s32 arg3) {
    u32* sp1C = main_pool_alloc(arg2, arg3);

    if (sp1C != NULL) {
        Util_Memcpy(sp1C, arg0 + arg1, arg2 >> 2);
    }

    main_pool_try_free(arg0);

    return sp1C;
}

void* Asset_LoadCompressed(u8* romStart, u8* romEnd, s32 arg2, s32 arg3) {
    u32* addr;
    u32* newaddr;
    u32* ret;
    UNUSED s32 pad;
    int side;

    side = (arg2 == 0) ? MEMORY_POOL_RIGHT : MEMORY_POOL_LEFT;
    addr = main_pool_alloc(0x18, side);

    if (addr != NULL) {
        Dma_WriteChunks(addr, romStart, romStart + 0x18, arg3);
        if ((PORT_BE32(addr[0]) == 'PERS') && (PORT_BE32(addr[1]) == '-SZP')) {
            newaddr = main_pool_alloc(ALIGN16(romEnd - romStart), side);
            if (newaddr != NULL) {
                Dma_WriteChunks(newaddr, romStart, romEnd, arg3);
                Port_FixPersHeader((PERSSZP*)newaddr);
                ret = Yay0_LoadDecompressedAsset(newaddr, newaddr, arg2);
            }
        } else if ((PORT_BE32(addr[0]) == 'PRES') && (PORT_BE32(addr[1]) == 'JPEG')) {
            newaddr = main_pool_alloc(ALIGN16(romEnd - romStart), side);
            if (newaddr != NULL) {
                Dma_WriteChunks(newaddr, romStart, romEnd, arg3);
                Port_FixPresJpegHeader((PRESJPEG*)newaddr);
                ret = Jpeg_LoadDecodedImage(newaddr, newaddr, arg2);
            }
        } else {
            newaddr = main_pool_alloc(ALIGN16(romEnd - romStart), arg2);
            ret = newaddr;
            if (newaddr != NULL) {
                Dma_WriteChunks(ret, romStart, romEnd, arg3);
            }
        }
        main_pool_try_free(addr);
    }

    return ret;
}

void* Storage_LoadAssetByLbaRange(unk_func_800041C0* arg0, s32 arg1) {
    s32 sp44;
    u32* sp40;
    PERSSZP* temp_s0;
    s32 var_s0;
    u8* temp_v0;

    if (arg1 == 0) {
        var_s0 = 1;
    } else {
        var_s0 = 0;
    }

    LeoLBAToByte(arg0->unk_00, arg0->unk_02, &sp44);
    sp44 = ALIGN16(sp44);

    temp_v0 = main_pool_alloc(sp44, var_s0);

    if (temp_v0 != NULL) {
        temp_s0 = arg0->unk_04 + temp_v0;

        Storage_QueueDiskTransfer(0, temp_v0, arg0->unk_00, arg0->unk_02, &D_800A62C0, 0);
        osRecvMesg(&D_800A62C0, NULL, 1);

        if (PORT_BE32(((u32*)temp_s0->magic)[0]) == 'PERS' && (PORT_BE32(((u32*)temp_s0->magic)[1]) == '-SZP')) {
            sp40 = Yay0_LoadDecompressedAsset(temp_v0, temp_s0, arg1);
        } else if (PORT_BE32(((u32*)temp_s0->magic)[0]) == 'PRES' && (PORT_BE32(((u32*)temp_s0->magic)[1]) == 'JPEG')) {
            sp40 = Jpeg_LoadDecodedImage(temp_v0, temp_s0, arg1);
        } else {
            sp40 = Asset_CopyUncompressed(temp_v0, arg0->unk_04, arg0->unk_08, arg1);
        }
    }

    return sp40;
}

unk_func_800041C0* Storage_SetLbaRange(unk_func_800041C0* arg0, s32 arg1, s32 arg2) {
    arg0->unk_00 = arg1;
    arg0->unk_02 = arg2 - arg1;
    arg0->unk_04 = 0;

    LeoLBAToByte(arg0->unk_00, arg0->unk_02, &arg0->unk_08);

    return arg0;
}

unk_func_800041C0* Storage_SetByteRange(unk_func_800041C0* arg0, u32 arg1, u32 offset, u32 size) {
    s32 sp2C;
    s32 sp28;
    s32 sp24;

    LeoByteToLBA(arg1, offset + 1, &sp28);
    sp28 -= 1;
    LeoByteToLBA(arg1, offset + size, &sp24);
    sp24 -= 1;

    if (sp28 > 0) {
        LeoLBAToByte(arg1, sp28, &sp2C);
    } else {
        sp2C = 0;
    }

    arg0->unk_00 = sp28 + arg1;
    arg0->unk_02 = (sp24 - sp28) + 1;
    arg0->unk_04 = offset - sp2C;
    arg0->unk_08 = size;

    return arg0;
}

void Storage_PrefetchLbaRange(s32 arg0, s32 arg1, s32 arg2) {
    unk_func_800041C0 sp1C;

    Storage_SetLbaRange(&sp1C, arg0, arg1);
    Storage_LoadAssetByLbaRange(&sp1C, arg2);
}

void Memmap_ClearSegmentOnBlockFree(uintptr_t block_addr, u32 addr) {
    u32 base = addr & 0xFF000000;
    u32 sp1C = addr & 0xFFFFFF;

    if ((base == 0x82000000) && (Memmap_GetSegmentBaseVaddr(sp1C) == block_addr)) {
        Memmap_ClearSegmentMemmap(sp1C);
    }
}

u8* Asset_LoadToSegment(s32 id, u8* rom_start, u8* rom_end, s32 arg3) {
    u8* vaddr = Asset_LoadCompressed(rom_start, rom_end, arg3, 0);

    if ((vaddr != NULL) && (id > 0)) {
        Memmap_SetSegmentMap(id, vaddr, main_pool_get_block_dist(vaddr));
        main_pool_set_func(vaddr, id + 0x82000000, &Memmap_ClearSegmentOnBlockFree);
    }
    return vaddr;
}

MainPoolBlock* Asset_LoadToSegmentByLbaRange(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    UNUSED s32 pad;
    unk_func_800041C0 sp28;
    MainPoolBlock* temp_v0 = Storage_LoadAssetByLbaRange(Storage_SetLbaRange(&sp28, arg1, arg2), arg3);

    if ((temp_v0 != NULL) && (arg0 > 0)) {
        Memmap_SetSegmentMap(arg0, temp_v0, main_pool_get_block_dist(temp_v0));
        main_pool_set_func(temp_v0, arg0 + 0x82000000, Memmap_ClearSegmentOnBlockFree);
    }

    return temp_v0;
}

void Fragment_FreeCallback(uintptr_t base_addr, u32 addr) {
    Port_VFree((void*)base_addr);
    u32 sp1C;
    u32 temp_a0;

    if ((addr & 0xFF000000) == 0x81000000) {
        sp1C = addr & 0xFFFFFF;
        if (Memmap_GetFragmentBaseVaddr(sp1C) == base_addr) {
            Memmap_ClearFragmentMemmap(sp1C);
        }
    }
}

// PORT: set while registering a data blob from an archive, so its pointers are moved into the virtual address space
// before the relocation table (which lies past sizeInRam) is released by the shrink below.
int gPortRelocateDataBlob;

void Fragment_Register(s32 arg0, Fragment* addr) {
    UNUSED s32 pad;
    Memmap_NormalizeFragmentHeader(addr);
    u32 sp20 = main_pool_get_block_dist(addr);

    main_pool_set_func(addr, arg0 + 0x81000000, &Fragment_FreeCallback);

    if (sp20 < addr->sizeInRam) {
        main_pool_realloc(addr, addr->sizeInRam);
        Memmap_RelocateFragment(arg0, addr);
        if (gPortRelocateDataBlob) Port_RelocateDataBlob(addr);
    } else {
        Memmap_RelocateFragment(arg0, addr);
        if (gPortRelocateDataBlob) Port_RelocateDataBlob(addr);
        main_pool_realloc(addr, addr->sizeInRam);
    }
}

#ifdef PORT_STATIC_FRAGMENTS
// PORT: fragment code is statically linked; the table maps a fragment's ROM offset to the function its header
// jumps to (src/port_fragment_entries.c, generated). Loading still brings the fragment's data image into memory.
struct PortFragmentEntry {
    unsigned romStart;
    unsigned textStart;
    void* entry;
    const char* name;
};
extern const struct PortFragmentEntry gPortFragmentEntries[];
static void* sPortFragmentBlock[0x100];
static unsigned sPortFragmentRom[0x100];

static const struct PortFragmentEntry* Port_FindFragment(u8* romStart) {
    const struct PortFragmentEntry* e;
    for (e = gPortFragmentEntries; e->entry != NULL; e++) {
        if (e->romStart == (unsigned)(uintptr_t)romStart) {
            return e;
        }
    }
    return NULL;
}

void Port_FragmentRemember(u8* romStart, void* block) {
    int i;
    for (i = 0; i < 0x100; i++) {
        if (sPortFragmentBlock[i] == NULL || sPortFragmentRom[i] == (unsigned)(uintptr_t)romStart) {
            sPortFragmentBlock[i] = block;
            sPortFragmentRom[i] = (unsigned)(uintptr_t)romStart;
            return;
        }
    }
}

void Port_FragmentFree(u8* romStart) {
    int i;
    for (i = 0; i < 0x100; i++) {
        if (sPortFragmentBlock[i] != NULL && sPortFragmentRom[i] == (unsigned)(uintptr_t)romStart) {
            main_pool_try_free(sPortFragmentBlock[i]);
            sPortFragmentBlock[i] = NULL;
            return;
        }
    }
}

void* Port_FragmentEntry(u8* romStart) {
    const struct PortFragmentEntry* e = Port_FindFragment(romStart);
    return e ? e->entry : NULL;
}
#endif

ret_func_80004454 Fragment_Load(s32 arg0, u8* romStart, u8* romEnd) {
    void* addr = Asset_LoadCompressed(romStart, romEnd, 0, 0);

    if (addr != NULL) {
        Fragment_Register(arg0, (Fragment*)addr);
    }
#ifdef PORT_STATIC_FRAGMENTS
    Port_FragmentRemember(romStart, addr);
    Port_NoteLoadedFragment(arg0, romStart);
    {
        // the N64 reloads a fragment's .data/.bss from ROM on every load (see tools/gen_frag_sections.py)
        extern void Port_FragmentResetData(void* entry);
        Port_FragmentResetData(Port_FragmentEntry(romStart));
    }
    return (ret_func_80004454)Port_FragmentEntry(romStart);
#else
    return addr;
#endif
}

void* Fragment_LoadByLbaRange(s32 arg0, s32 arg1, s32 arg2) {
    Fragment* sp2C;
    unk_func_800041C0 sp20;

    sp2C = Storage_LoadAssetByLbaRange(Storage_SetLbaRange(&sp20, arg1, arg2), 0);
    if (sp2C != NULL) {
        Fragment_Register(arg0, sp2C);
    }
    return sp2C;
}

// PORT: header words of PERS-SZP / PRESJPEG assets are big-endian; swap them in place once, right after the file is
// read (never twice: callers only reach these after the tag check on the freshly loaded buffer).
void Port_FixPersHeader(PERSSZP* h) {
    u32* w = (u32*)h;
    w[2] = PORT_BE32(w[2]);
    w[3] = PORT_BE32(w[3]);
    w[4] = PORT_BE32(w[4]);
    w[5] = PORT_BE32(w[5]);
}

void Port_FixPresJpegHeader(PRESJPEG* h) {
    u32* w = (u32*)h;
    w[2] = PORT_BE32(w[2]);
    w[3] = PORT_BE32(w[3]);
}

// PORT: code pointers stored as 32-bit link addresses in ROM data (geo-layout callbacks, ...) are mapped back to the
// statically linked functions. Fragments overlap in link space, so the lookup is keyed by the ROM offset of the
// fragment currently loaded in that slot.
struct PortFuncEntry {
    unsigned rom;
    unsigned addr;
    void* fn;
};
extern const struct PortFuncEntry gPortFuncTable[];
extern const unsigned gPortFuncTableCount;
static unsigned sPortSlotRom[240];

void Port_NoteLoadedFragment(u32 slot, u8* romStart) {
    // PORT_TRACE_FRAGMENTS=1: which fragment each scene loads
    static int sTrace = -1;
    if (sTrace < 0) sTrace = getenv("PORT_TRACE_FRAGMENTS") != NULL;
    if (sTrace) {
        extern unsigned long long Port_ViTick(void);
        const struct PortFragmentEntry* e = Port_FindFragment(romStart);
        fprintf(stderr, "[fragment] slot %u: %s @frame %llu\n", (unsigned)slot, e && e->name ? e->name : "?", Port_ViTick());
    }
    if (slot < 240) {
        sPortSlotRom[slot] = (unsigned)(uintptr_t)romStart;
    }
}

// PORT: some archive files are tiny relocatable code fragments: a leaf function (switch on its first argument through a
// jump table) that returns addresses of data inside the file or small constants (e.g. the Gallery's background sets).
// The host cannot run MIPS code, so this interprets such a leaf function: the integer/branch/load subset these use,
// loads only through Memmap_GetFragmentVaddr, a scratch stack, at most a few hundred instructions. Returns v0; a
// fragment address is returned as the host address it maps to.
uintptr_t Port_CallFragmentLeaf(const void* code, s32 a0, s32 a1) {
    u32 r[32] = { 0 };
    u8 stack[0x200]; // vaddr 0x7FFFFE00..0x7FFFFFFF
    u32 pc;
    const u8* host = (const u8*)code;
    u32 insn;
    s32 steps;
    u32 branchTarget = 0;
    s32 inDelay = 0;

    r[4] = (u32)a0;
    r[5] = (u32)a1;
    r[29] = 0x7FFFFF00u; // scratch stack: stores at sp go to 'stack'
    r[31] = 0xFFFFFFF0u; // return marker
    insn = ((u32)host[0] << 24) | ((u32)host[1] << 16) | ((u32)host[2] << 8) | host[3];
    if ((insn >> 26) != 2) {
        return 0; // not a "j entry" header
    }
    pc = 0x80000000u | ((insn & 0x03FFFFFF) << 2);
    for (steps = 0; steps < 512; steps++) {
        const u8* p = (const u8*)Memmap_GetFragmentVaddr((void*)(uintptr_t)pc);
        u32 op, rs, rt, rd, sa, fn;
        s32 imm;
        u32 next = pc + 4;
        s32 willBranch = 0;
        u32 target = 0;

        if ((uintptr_t)p < 0x100000000ULL) {
            return 0;
        }
        insn = ((u32)p[0] << 24) | ((u32)p[1] << 16) | ((u32)p[2] << 8) | p[3];
        op = insn >> 26;
        rs = (insn >> 21) & 31;
        rt = (insn >> 16) & 31;
        rd = (insn >> 11) & 31;
        sa = (insn >> 6) & 31;
        fn = insn & 63;
        imm = (s16)(insn & 0xFFFF);
        switch (op) {
            case 0: // SPECIAL
                switch (fn) {
                    case 0x00: r[rd] = r[rt] << sa; break;
                    case 0x02: r[rd] = r[rt] >> sa; break;
                    case 0x03: r[rd] = (u32)((s32)r[rt] >> sa); break;
                    case 0x08: willBranch = 1; target = r[rs]; break; // jr
                    case 0x21: r[rd] = r[rs] + r[rt]; break;
                    case 0x23: r[rd] = r[rs] - r[rt]; break;
                    case 0x24: r[rd] = r[rs] & r[rt]; break;
                    case 0x25: r[rd] = r[rs] | r[rt]; break;
                    case 0x2A: r[rd] = (s32)r[rs] < (s32)r[rt]; break;
                    case 0x2B: r[rd] = r[rs] < r[rt]; break;
                    default: return 0;
                }
                break;
            case 0x02: willBranch = 1; target = (pc & 0xF0000000u) | ((insn & 0x03FFFFFF) << 2); break;
            case 0x04: if (r[rs] == r[rt]) { willBranch = 1; target = next + (imm << 2); } break;
            case 0x05: if (r[rs] != r[rt]) { willBranch = 1; target = next + (imm << 2); } break;
            case 0x06: if ((s32)r[rs] <= 0) { willBranch = 1; target = next + (imm << 2); } break;
            case 0x07: if ((s32)r[rs] > 0) { willBranch = 1; target = next + (imm << 2); } break;
            case 0x09: r[rt] = r[rs] + (u32)imm; break;
            case 0x0A: r[rt] = (s32)r[rs] < imm; break;
            case 0x0B: r[rt] = r[rs] < (u32)imm; break;
            case 0x0C: r[rt] = r[rs] & (insn & 0xFFFF); break;
            case 0x0D: r[rt] = r[rs] | (insn & 0xFFFF); break;
            case 0x0F: r[rt] = (insn & 0xFFFF) << 16; break;
            case 0x23: case 0x21: case 0x25: case 0x20: case 0x24: { // lw lh lhu lb lbu
                u32 va = r[rs] + (u32)imm;
                const u8* m;
                if (va >= 0x7FFFFE00u && va <= 0x7FFFFFFCu) {
                    m = stack + (va - 0x7FFFFE00u);
                } else {
                    m = (const u8*)Memmap_GetFragmentVaddr((void*)(uintptr_t)va);
                    if ((uintptr_t)m < 0x100000000ULL) {
                        return 0;
                    }
                }
                if (op == 0x23) r[rt] = ((u32)m[0] << 24) | ((u32)m[1] << 16) | ((u32)m[2] << 8) | m[3];
                else if (op == 0x21) r[rt] = (u32)(s16)((m[0] << 8) | m[1]);
                else if (op == 0x25) r[rt] = (u32)((m[0] << 8) | m[1]);
                else if (op == 0x20) r[rt] = (u32)(s8)m[0];
                else r[rt] = m[0];
                break;
            }
            case 0x2B: case 0x29: case 0x28: { // sw sh sb: only to the scratch stack
                u32 va = r[rs] + (u32)imm;
                if (va >= 0x7FFFFE00u && va <= 0x7FFFFFFCu) {
                    u8* m = stack + (va - 0x7FFFFE00u);
                    if (op == 0x2B) { m[0] = r[rt] >> 24; m[1] = r[rt] >> 16; m[2] = r[rt] >> 8; m[3] = r[rt]; }
                    else if (op == 0x29) { m[0] = r[rt] >> 8; m[1] = r[rt]; }
                    else { m[0] = r[rt]; }
                }
                break;
            }
            default:
                return 0;
        }
        r[0] = 0;
        if (inDelay) {
            if (branchTarget == 0xFFFFFFF0u) {
                break; // returned
            }
            pc = branchTarget;
            inDelay = 0;
        } else if (willBranch) {
            branchTarget = target;
            inDelay = 1;
            pc = next;
        } else {
            pc = next;
        }
    }
    if (r[2] >= 0x81000000u && r[2] < 0x90000000u) {
        return Memmap_GetFragmentVaddr((void*)(uintptr_t)r[2]);
    }
    return r[2];
}

void* Port_ResolveCode(uintptr_t addr) {
    static int depth;
    unsigned rom = 0;
    unsigned lo = 0;
    unsigned hi = gPortFuncTableCount;

    if (addr >= 0x100000000ULL || addr == 0) {
        return (void*)addr; // already a host pointer
    }
    if (addr >= 0x81000000 && addr < 0x90000000) {
        rom = sPortSlotRom[((addr & 0x0FF00000) >> 20) - 0x10];
    }
    while (lo < hi) {
        unsigned mid = (lo + hi) / 2;
        const struct PortFuncEntry* e = &gPortFuncTable[mid];
        if (e->rom < rom || (e->rom == rom && e->addr < addr)) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    if (lo < gPortFuncTableCount && gPortFuncTable[lo].rom == rom && gPortFuncTable[lo].addr == addr) {
        return gPortFuncTable[lo].fn;
    }
    // Fragment 31 (particle_data_library) starts with a table of trampolines ("j target; nop") that ROM data uses as
    // stable entry points into other fragments (e.g. 0x810001D0 -> ParticleGfx_GraphNodeBuildDescriptorMaterialList,
    // the arena floor's material callback). Follow a jump at the address to its target.
    if (addr >= 0x81000000 && addr < 0x90000000 && depth < 4) {
        uintptr_t host = Memmap_GetFragmentVaddr((void*)addr);
        if (host >= 0x100000000ULL) {
            const u8* p = (const u8*)host;
            u32 insn = ((u32)p[0] << 24) | ((u32)p[1] << 16) | ((u32)p[2] << 8) | p[3];
            if ((insn >> 26) == 2) {
                // (all 60 targets read their callback data ROM-aware: big-endian records with 32-bit addresses)
                u32 target = (u32)((addr & 0xF0000000) | ((insn & 0x03FFFFFF) << 2));
                void* fn;
                depth++;
                fn = Port_ResolveCode(target);
                depth--;
                if (fn != NULL) {
                    return fn;
                }
                if (getenv("PORT_TRACE_GEO")) fprintf(stderr, "[code] trampoline %08lx -> %08x unresolved\n", (unsigned long)addr, target);
            }
        }
    }
    if (getenv("PORT_TRACE_GEO")) fprintf(stderr, "[code] unresolved %08lx (slot rom 0x%x)\n", (unsigned long)addr, rom);
    return NULL; // MIPS code with no host counterpart
}

// PORT: ROM-format structs keep 32-bit slots for runtime pointers (BinArchiveFile.unk_08 caches a loaded file). A
// handle table stands in: the slot holds an index and the table holds the 64-bit pointer.
#define PORT_HANDLE_MAX 0x10000
static void* sPortHandlePtrs[PORT_HANDLE_MAX];
static u32 sPortHandleCount = 1;

u32 Port_PtrToHandle(void* p) {
    u32 i;
    if (p == NULL) {
        return 0;
    }
    for (i = 1; i < sPortHandleCount; i++) {
        if (sPortHandlePtrs[i] == p) {
            return i;
        }
    }
    if (sPortHandleCount >= PORT_HANDLE_MAX) {
        return 0;
    }
    sPortHandlePtrs[sPortHandleCount] = p;
    return sPortHandleCount++;
}

void* Port_HandleToPtr(u32 h) {
    return (h != 0 && h < sPortHandleCount) ? sPortHandlePtrs[h] : NULL;
}

// PORT: ROM data is big-endian. Archive headers and file tables are arrays of u32 (the first word holds two u16
// flags that are zero in the ROM), so byte-swapping them as words is exact.
static void Port_SwapWords(void* p, u32 count) {
    u32* w = (u32*)p;
    while (count-- > 0) {
        u32 v = *w;
        *w++ = (v >> 24) | ((v >> 8) & 0xFF00) | ((v << 8) & 0xFF0000) | (v << 24);
    }
}

static void BinArchive_SwapHeader(BinArchive* a) {
    u8* b = (u8*)a;
    // the first word is two big-endian u16 (flags, fragment slot id), the other header words are u32
    u16 flags = (u16)((b[0] << 8) | b[1]);
    u16 id = (u16)((b[2] << 8) | b[3]);
    a->unk_00 = flags;
    a->unk_02 = id;
    Port_SwapWords((u32*)a + 1, 3);
}

static void BinArchive_SwapTable(BinArchive* a) {
    Port_SwapWords((u32*)a + 4, a->num_files * 4); // header already swapped by the caller
}

BinArchive* BinArchive_Open(u8* romStart, u8* romEnd, s32 arg2, s32 arg3) {
    BinArchive* ret = NULL;
    u32 sp28;
    u32 sp28_2;
    u16* newaddr;

    switch (arg2) {
        case 0:
            newaddr = Asset_LoadCompressed(romStart, romEnd, 0, 0);
            ret = newaddr;
            if (newaddr != NULL) {
                BinArchive_SwapHeader(ret);
                BinArchive_SwapTable(ret);
                newaddr[0] |= 1;
            }
            break;

        case 1:
            newaddr = Asset_LoadCompressed(romStart, romStart + 0x20, 0, 0);
            ret = newaddr;
            if (newaddr != NULL) {
                BinArchive_SwapHeader(ret);
                sp28 = (ret->num_files * 0x10) + 0x10;
                main_pool_try_free(ret);
                newaddr = Asset_LoadCompressed(romStart, romStart + sp28, 0, 0);
                ret = newaddr;
                if (newaddr != NULL) {
                    BinArchive_SwapHeader(ret);
                    BinArchive_SwapTable(ret);
                    *newaddr |= 2;
                    if (arg3 == 1) {
                        *newaddr |= 4;
                    }
                }
            }
            break;

        case 2:
            newaddr = Asset_LoadCompressed(romStart, romStart + 0x20, 1, 0);
            ret = newaddr;
            if (newaddr != NULL) {
                BinArchive_SwapHeader(ret);
                sp28_2 = (ret->num_files * 0x10) + 0x10;
                main_pool_try_free(ret);
                newaddr = Asset_LoadCompressed(romStart, romStart + sp28_2, 1, 0);
                ret = newaddr;
                if (newaddr != NULL) {
                    BinArchive_SwapHeader(ret);
                    BinArchive_SwapTable(ret);
                }
                if ((newaddr != NULL) && (arg3 == 1)) {
                    *newaddr |= 4;
                }
            }
            break;
    }

    if (ret != NULL) {
        ret->unk_04 = romStart;
    }

    return ret;
}

void* BinArchive_LoadByLbaRange(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    BinArchive* var_a0;
    unk_func_800041C0 sp20;

    switch (arg2) {
        default:
            break;

        case 0:
            Storage_SetLbaRange(&sp20, arg0, arg1);
            var_a0 = Storage_LoadAssetByLbaRange(&sp20, 0);
            if (var_a0 != NULL) {
                main_pool_realloc(var_a0, var_a0->total_size);
                var_a0->unk_00 |= 1;
            }
            break;

        case 1:
            Storage_SetLbaRange(&sp20, arg0, arg0 + 1);
            var_a0 = Storage_LoadAssetByLbaRange(&sp20, 0);
            if (var_a0 != NULL) {
                main_pool_realloc(var_a0, (var_a0->num_files * sizeof(BinArchiveFile)) + sizeof(BinArchive));
                var_a0->unk_00 |= 2;
                if (arg3 == 1) {
                    var_a0->unk_00 |= 4;
                }
            }
            break;

        case 2:
            Storage_SetLbaRange(&sp20, arg0, arg0 + 1);
            var_a0 = Storage_LoadAssetByLbaRange(&sp20, 1);
            if (arg3 == 1) {
                var_a0->unk_00 |= 4;
            }
            break;
    }

    if (var_a0 != NULL) {
        var_a0->unk_00 |= 0x80;
        var_a0->unk_04 = arg0;
    }

    return var_a0;
}

void BinArchive_Free(void* arg0) {
    main_pool_try_free(arg0);
}

Fragment* BinArchive_LoadFileFromRom(BinArchive* arg0, BinArchiveFile* arg1) {
    u8* temp_a0 = arg0->unk_04 + arg1->offset;

    return Asset_LoadCompressed(temp_a0, arg1->size + temp_a0, 0, 0);
}

void* BinArchive_LoadFileBuffered(BinArchive* arg0, BinArchiveFile* arg1) {
    unk_func_800041C0 sp1C;

    Storage_SetByteRange(&sp1C, arg0->unk_04, arg1->offset, arg1->size);
    return Storage_LoadAssetByLbaRange(&sp1C, 0);
}

void* BinArchive_GetFile(BinArchive* archive, s32 file_number) {
    Fragment* var_a2 = NULL;
    UNUSED s32 pad;
    BinArchiveFile* sp18 = (u8*)archive + sizeof(BinArchive) + (file_number * sizeof(BinArchiveFile));

    if (archive == NULL) {
        return NULL; // PORT: archive failed to open
    }

    if (getenv("PORT_TRACE")) fprintf(stderr, "[archive] %p file=%d flags=0x%x id=%d n=%u unk04=0x%x off=0x%x size=0x%x u08=0x%x\n", (void*)archive, file_number, archive->unk_00, archive->unk_02, archive->num_files, archive->unk_04, sp18->offset, sp18->size, sp18->unk_08);
    if (file_number < archive->num_files) {
        if (archive->unk_00 & 1) {
            var_a2 = (u8*)archive + sp18->offset;
            if (getenv("PORT_DUMP_FRAGS")) {
                char path[512];
                snprintf(path, sizeof path, "%s/mem%04x_file%03d.bin", getenv("PORT_DUMP_FRAGS"), archive->unk_02, file_number);
                FILE* f = fopen(path, "wb");
                if (f) { fwrite(var_a2, 1, sp18->size, f); fclose(f); }
            }
        } else if (!(archive->unk_00 & 4) && (sp18->unk_08 != 0)) {
            var_a2 = Port_HandleToPtr(sp18->unk_08);
        } else {
            if (archive->unk_00 & 0x80) {
                var_a2 = BinArchive_LoadFileBuffered(archive, sp18);
            } else {
                var_a2 = BinArchive_LoadFileFromRom(archive, sp18);
            }

            if (var_a2 != NULL) {
                if ((PORT_BE32(((u32*)var_a2->magic)[0]) == 'FRAG') && (PORT_BE32(((u32*)var_a2->magic)[1]) == 'MENT')) {
                    // Debug aid: PORT_DUMP_FRAGS=<dir> saves each fragment file as loaded (before relocation).
                    if (getenv("PORT_DUMP_FRAGS")) {
                        char path[512];
                        u32 n = PORT_BE32(var_a2->relocOffset) + 4 + 4 * PORT_BE32(*(u32*)((u8*)var_a2 + PORT_BE32(var_a2->relocOffset)));
                        snprintf(path, sizeof path, "%s/arch%04x_file%03d.bin", getenv("PORT_DUMP_FRAGS"), archive->unk_02, file_number);
                        FILE* f = fopen(path, "wb");
                        if (f) { fwrite(var_a2, 1, n, f); fclose(f); }
                    }
                    gPortRelocateDataBlob = 1;
                    Fragment_Register(archive->unk_02, var_a2);
                    gPortRelocateDataBlob = 0;
                }

                sp18->unk_08 = Port_PtrToHandle(var_a2);
            }
        }
    }

    return var_a2;
}

void* BinArchive_GetCachedFile(BinArchive* archive, s32 file_number) {
    BinArchiveFile* sp18 = (u8*)archive + sizeof(BinArchive) + (file_number * sizeof(BinArchiveFile));
    void* var_v1 = NULL;

    if (file_number < archive->num_files) {
        var_v1 = Port_HandleToPtr(sp18->unk_08);
    }
    return var_v1;
}
