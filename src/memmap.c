#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "memmap.h"

struct MemoryMap {
    uintptr_t vaddr;
    size_t size;
};

extern void Port_GfxFlushCache(void);

/*
 * PORT: virtual 32-bit address space for data blobs (models) that the game relocates into RAM and keeps several of at
 * once. All of them are linked at the same slot address (e.g. 0x8FF00000), so without relocation their addresses
 * collide. Each loaded blob gets a pseudo base here; same-slot pointers in its data are rewritten to it (see
 * Port_RelocateDataBlob) and Port_VResolve maps pseudo addresses back to host memory.
 */
#define PORT_VBASE 0x30000000u
#define PORT_VBLOBS 512
struct PortVBlob {
    u8* host;
    u32 size;
    u32 link; // slot base the blob was linked at
};
static struct PortVBlob sVBlobs[PORT_VBLOBS];

u32 Port_VAlloc(void* host, u32 size, u32 link) {
    int i;
    if (size > 0x100000) {
        return 0;
    }
    // The pool hands the same memory out again after a blob is released; drop a stale entry that starts at the same address
    // so lookups by host address (Port_VAddrOfLink) and by pseudo address cannot find the old blob.
    for (i = 0; i < PORT_VBLOBS; i++) {
        if (sVBlobs[i].host == (u8*)host) {
            sVBlobs[i].host = NULL;
        }
    }
    for (i = 0; i < PORT_VBLOBS; i++) {
        if (sVBlobs[i].host == NULL) {
            sVBlobs[i].host = host;
            sVBlobs[i].size = size;
            sVBlobs[i].link = link;
            if (getenv("PORT_TRACE_VBLOB")) fprintf(stderr, "[vblob] alloc slot %d host %p size 0x%x link 0x%08x\n", i, host, size, link);
            return PORT_VBASE + (u32)i * 0x100000;
        }
    }
    return 0;
}

void Port_VFree(void* host) {
    int i;
    for (i = 0; i < PORT_VBLOBS; i++) {
        if (sVBlobs[i].host == (u8*)host) {
            if (getenv("PORT_TRACE_VBLOB")) fprintf(stderr, "[vblob] free slot %d host %p\n", i, host);
            sVBlobs[i].host = NULL;
        }
    }
    Port_GfxFlushCache();
}

void* Port_VResolve(uintptr_t addr) {
    if (addr >= PORT_VBASE && addr < PORT_VBASE + PORT_VBLOBS * 0x100000u) {
        struct PortVBlob* b = &sVBlobs[(addr - PORT_VBASE) >> 20];
        u32 off = addr & 0xFFFFF;
        if (b->host != NULL && off < b->size) {
            return b->host + off;
        }
    }
    return NULL;
}

int Port_VIsRaw(const void* p) {
    int i;
    for (i = 0; i < PORT_VBLOBS; i++) {
        if (sVBlobs[i].host != NULL && (const u8*)p >= sVBlobs[i].host && (const u8*)p < sVBlobs[i].host + sVBlobs[i].size) {
            return 1;
        }
    }
    return 0;
}

// Pseudo address of a link address inside the blob that starts at 'host' (0 if the blob is not registered).
u32 Port_VAddrOfLink(void* host, u32 linkAddr) {
    int i;
    for (i = PORT_VBLOBS - 1; i >= 0; i--) {
        if (sVBlobs[i].host == (u8*)host) {
            return PORT_VBASE + (u32)i * 0x100000 + (linkAddr & 0xFFFFF);
        }
    }
    return 0;
}

// Rewrites the same-slot pointer words of a freshly loaded data blob (R_MIPS_32 entries of its relocation table) to the
// blob's pseudo base. Code relocations are skipped: the blob's MIPS code never runs here.
u32 Port_RelocateDataBlob(struct Fragment* f) {
    struct RelocTable* tbl;
    u32 link;
    u32 vbase;
    u32 i;
    u32 n;

    Memmap_NormalizeFragmentHeader(f);
    {
        // entry instruction is `j target`; the target's high bits are the slot base
        u32 insn = PORT_BE32(f->inst[0]);
        link = (((insn & 0x03FFFFFF) << 2) | 0x80000000) & 0xFFF00000;
    }
    vbase = Port_VAlloc(f, f->sizeInRam, link);
    if (vbase == 0) {
        return 0;
    }
    tbl = (struct RelocTable*)((u8*)f + f->relocOffset);
    n = PORT_BE32(tbl->nRelocations);
    for (i = 0; i < n; i++) {
        u32 reloc = PORT_BE32(tbl->relocations[i]);
        if (((reloc & 0x7F000000) >> 24) == R_MIPS_32) {
            u8* w = (u8*)f + (reloc & 0xFFFFFF);
            u32 v = ((u32)w[0] << 24) | ((u32)w[1] << 16) | ((u32)w[2] << 8) | w[3];
            if ((v & 0xFFF00000) == link) {
                v = vbase + (v & 0xFFFFF);
                w[0] = v >> 24;
                w[1] = v >> 16;
                w[2] = v >> 8;
                w[3] = v;
            }
        }
    }
    return vbase;
}

struct MemoryMap gSegments[16];
struct MemoryMap gFragments[240];

/*
 * Set the base virtual address and size of the specified segment memmap.
 */
void Memmap_SetSegmentMap(u32 id, uintptr_t vaddr, size_t size) {
    Port_GfxFlushCache(); // PORT: translated display lists depend on these maps
    gSegments[id].vaddr = vaddr;
    gSegments[id].size = size;
}

/*
 * Return the base address of the specified segment number.
 */
uintptr_t Memmap_GetSegmentBaseVaddr(u32 id) {
    return gSegments[id].vaddr;
}

/*
 * Return the current virtual address of a given offset into a segment. This is
 * calculated based on the input bitmask constructed of the following values:
 *
 * Input: XYZZZZZZ
 * X: Safety nibble. Only process if 0.
 * Y: Segment number.
 * Z: Segment offset.
 */
uintptr_t Memmap_GetSegmentVaddr(uintptr_t mask) {
    if ((mask >> 0x1C) == 0) {
        u32 id = (mask & 0x0F000000) >> 0x18;
        u32 value = (mask & 0x00FFFFFF) >> 0x00;
        if (gSegments[id].vaddr != NULL) {
            mask = gSegments[id].vaddr + value;
        }
    }
    return mask;
}

/*
 * Returns the input bitmask equivalent for Memmap_GetSegmentVaddr's input. This function
 * appears to be unused.
 */
u32 Memmap_GetSegmentVaddrMask(u32 i, uintptr_t addr) {
    struct MemoryMap* entry = &gSegments[i];
    size_t diff = addr - entry->vaddr;

    if (diff >= entry->size) {
        return 0;
    }
    return (i << 0x18) | diff;
}

/*
 * Clear the memmap for the segment ID specified.
 */
void Memmap_ClearSegmentMemmap(u32 id) {
    Port_GfxFlushCache(); // PORT: translated display lists depend on these maps
    gSegments[id].vaddr = NULL;
    gSegments[id].size = 0;
}

/*
 * Initialize the 16 segments with the segment memmap virtual addresses for the
 * display list head.
 */
void Memmap_SetSegments(Gfx** gfxDl) {
    u32 i;
    Gfx* gfx = *gfxDl;

    for (i = 0; i < 16; i++) {
        gSPSegment(gfx++, i, (uintptr_t)gSegments[i].vaddr); // PORT: full host pointer, not a 29-bit physical address
    }
    *gfxDl = gfx;
}

/*
 * PORT: fragment headers in ROM data are big-endian. The first word after the jump is always a nop (0), which is
 * reused as the "already byte swapped" marker so this can be called from every entry point.
 */
#define PORT_FRAGMENT_SWAPPED 0x50525453
void Memmap_NormalizeFragmentHeader(struct Fragment* f) {
    if (f->inst[1] != PORT_FRAGMENT_SWAPPED) {
        f->headerSize = PORT_BE32(f->headerSize);
        f->relocOffset = PORT_BE32(f->relocOffset);
        f->sizeInRom = PORT_BE32(f->sizeInRom);
        f->sizeInRam = PORT_BE32(f->sizeInRam);
        f->inst[1] = PORT_FRAGMENT_SWAPPED;
    }
}

/*
 * Perform fragment relocation on a given Fragment.
 */
void Memmap_RelocateFragment(u32 id, struct Fragment* fragment) {
    Memmap_NormalizeFragmentHeader(fragment);
    u32 isLoNeg;
    u32* luiRefs[32];
    u32 luiVals[32];
    u32* luiInstRef;
    u32* relocDataP;
    u32 relocSize;
    struct RelocTable* relocInfo;
    UNUSED u32 relocOffset;
    u32 reloc;
    u32 temp_v0_5;
    u32 i;
    u32* regValP;
    UNUSED s32 pad;

    relocOffset = fragment->relocOffset;
    relocSize = fragment->sizeInRam - fragment->relocOffset;
    relocInfo = (struct RelocTable*)((uintptr_t)fragment->relocOffset + (uintptr_t)fragment);

    osInvalICache(fragment, fragment->sizeInRam);
    osInvalDCache(fragment, fragment->sizeInRam);
    Memmap_SetFragmentMap(id, (uintptr_t)fragment, fragment->sizeInRam);
#ifdef PORT_STATIC_FRAGMENTS
    // PORT: fragment code is linked into the executable, so there is nothing to relocate. The blob stays
    // as loaded (its data words still hold link-time addresses; Memmap_GetFragmentVaddr maps them).
    return;
#endif

    for (i = 0; i < relocInfo->nRelocations; i++) {
        reloc = relocInfo->relocations[i];
        relocDataP = (u32*)((reloc & 0xFFFFFF) + (uintptr_t)fragment);

        switch ((reloc & 0x7F000000) >> 24) {
            case R_MIPS_32:
                // Handles 32-bit address relocation, used for things such as jump tables and pointers in data.
                // Just relocate the full address.
                *relocDataP = Memmap_GetFragmentVaddr(*relocDataP);
                break;
            case R_MIPS_26:
                // Handles 26-bit address relocation, used for jumps and jals.
                // Extract the address from the target field of the J-type MIPS instruction.
                // Relocate the address and update the instruction.
                *relocDataP =
                    (((u32)(Memmap_GetFragmentVaddr(((*relocDataP * 4) & 0x0FFFFFFC) + 0x80000000) & 0x0FFFFFFF) >> 2) |
                     (*relocDataP & 0xFC000000));
                break;
            case R_MIPS_HI16:
                // Handles relocation for a hi/lo pair, part 1.
                // Store the reference to the LUI instruction (hi) using the `rt` register of the instruction.
                // This will be updated later in the `R_MIPS_LO16` section.
                luiRefs[(*relocDataP >> 0x10) & 0x1F] = relocDataP;
                luiVals[(*relocDataP >> 0x10) & 0x1F] = *relocDataP;
                break;
            case R_MIPS_LO16:
                // Handles relocation for a hi/lo pair, part 2.
                // Grab the stored LUI (hi) from the `R_MIPS_HI16` section using the `rs` register of the instruction.
                // The full address is calculated, relocated, and then used to update both the LUI and lo instructions.
                // If the lo part is negative, add 1 to the LUI value.
                // Note: The lo instruction is assumed to have a signed immediate.
                luiInstRef = luiRefs[(*relocDataP >> 0x15) & 0x1F];
                regValP = &luiVals[(*relocDataP >> 0x15) & 0x1F];

                temp_v0_5 = Memmap_GetFragmentVaddr((*regValP << 0x10) + (s16)*relocDataP);
                isLoNeg = (temp_v0_5 & 0x8000) ? 1 : 0;
                *luiInstRef = (u32)((*luiInstRef & 0xFFFF0000) | ((temp_v0_5 >> 16) + isLoNeg));
                *relocDataP = (u32)((*relocDataP & 0xFFFF0000) | (temp_v0_5 & 0xFFFF));
                break;
        }
    }
    if (relocSize != 0) {
        bzero((void*)((uintptr_t)fragment->relocOffset + (uintptr_t)fragment), relocSize);
    }
    osWritebackDCache(fragment, fragment->sizeInRam);
}

/*
 * Set the base virtual address and size of the specified fragment memmap.
 */
void Memmap_SetFragmentMap(u32 id, uintptr_t vaddr, size_t size) {
    Port_GfxFlushCache(); // PORT: translated display lists depend on these maps
    gFragments[id].vaddr = vaddr;
    gFragments[id].size = size;
}

/*
 * Return the base vaddress of the fragment ID specified.
 */
uintptr_t Memmap_GetFragmentBaseVaddr(u32 id) {
    return gFragments[id].vaddr;
}

/*
 * Return the current virtual address of a given offset into a fragment. This is
 * calculated based on the input bitmask constructed of the following values:
 *
 * Input: XYYZZZZZ
 * X: Safety nibble. Only process if 0. This is handled by the address range check
 *    at the start.
 * Y: Fragment number.
 * Z: Fragment offset.
 */
uintptr_t Memmap_GetFragmentVaddr(void* addr) {
    void* virt = Port_VResolve((uintptr_t)addr);
    if (virt != NULL) {
        return (uintptr_t)virt;
    }
    if (((uintptr_t)addr >= 0x81000000U) && ((uintptr_t)addr < 0x90000000U)) {
        u32 id = (((uintptr_t)addr & 0x0FF00000) >> 0x14) - 0x10;
        u32 value = (((uintptr_t)addr & 0x000FFFFF));

        if (gFragments[id].vaddr != NULL) {
            addr = gFragments[id].vaddr + value;
        }
    }
    return (uintptr_t)addr;
}

/*
 * Returns the input bitmask equivalent for Memmap_GetFragmentVaddr's input. This function
 * appears to be unused.
 */
u32 Memmap_GetFragmentVaddrMask(u32 i, uintptr_t addr) {
    struct MemoryMap* entry = &gFragments[i];
    size_t diff = addr - entry->vaddr;

    if (diff >= entry->size) {
        return 0;
    }
    return ((i + 0x10) << 0x14) | 0x80000000 | diff;
}

/*
 * Clear the memmap for the fragment ID specified.
 */
void Memmap_ClearFragmentMemmap(u32 id) {
    Port_GfxFlushCache(); // PORT: translated display lists depend on these maps
    gFragments[id].vaddr = NULL;
    gFragments[id].size = 0;
}

/*
 * Given a specified address, check to see if it is located inside of a fragment
 * currently loaded. Return a calculated address.
 */
uintptr_t Memmap_GetLoadedFragmentVaddr(uintptr_t addr) {
    struct MemoryMap* fraglist = gFragments;
    int i, UNUSED j;

    for (i = 0, fraglist = gFragments; i < 0xF0; i++, fraglist++) {
        if (addr >= fraglist->vaddr) {
            size_t diff = addr - fraglist->vaddr;
            if (diff < fraglist->size) {
                return ((i + 0x10) << 0x14) | 0x80000000 | diff;
            }
        }
    }
    return 0;
}

// PORT: some data archives (stadium_models) hold relocatable fragments whose entry point is a small MIPS function the
// game calls to look up its data (e.g. "return &geoLayout[arg0]"). Their code was never decompiled, so it is run
// here by a minimal interpreter for leaf functions: integer ALU ops, branches, loads from the blob itself; no calls.
// Link addresses in the result are returned as the blob's pseudo address, like the pointers inside the blob.
static u32 Port_BlobLoad32(struct Fragment* f, u32 link, u32 addr, int* ok) {
    u8* p = (u8*)f + (addr - link);
    if (addr < link || addr + 4 > link + f->sizeInRam) {
        *ok = 0;
        return 0;
    }
    return ((u32)p[0] << 24) | ((u32)p[1] << 16) | ((u32)p[2] << 8) | p[3];
}

uintptr_t Port_CallBlobEntry(void* frag, s32 a0, s32 a1) {
    static int sTraceBlob = -1;
    struct Fragment* f = (struct Fragment*)frag;
    u32 r[32] = { 0 };
    u32 insn0, link, pc, next, size;
    int steps, ok = 1;

    if (f == NULL) {
        return 0;
    }
    if (sTraceBlob < 0) {
        sTraceBlob = getenv("PORT_TRACE_BLOB") != NULL; // debug: one line per interpreted instruction
    }
    insn0 = PORT_BE32(f->inst[0]);
    link = (((insn0 & 0x03FFFFFF) << 2) | 0x80000000) & 0xFFF00000;
    pc = ((insn0 & 0x03FFFFFF) << 2) | (link & 0xF0000000);
    size = f->sizeInRam;
    r[4] = (u32)a0;
    r[5] = (u32)a1;
    r[29] = 0xFFFFFF00; // stack writes are ignored
    r[31] = 0xDEAD0000;
    next = pc + 4;
    for (steps = 0; steps < 100000; steps++) {
        u32 in = Port_BlobLoad32(f, link, pc, &ok);
        u32 op = in >> 26, rs = (in >> 21) & 31, rt = (in >> 16) & 31, rd = (in >> 11) & 31, sa = (in >> 6) & 31;
        s32 imm = (s16)(in & 0xFFFF);
        u32 target = next;
        int branch = 0;

        if (!ok) {
            break;
        }
        if (sTraceBlob) fprintf(stderr, "[blob] %08x: %08x  a0=%08x v0=%08x v1=%08x at=%08x t6=%08x\n", pc, in, r[4], r[2], r[3], r[1], r[14]);
        switch (op) {
            case 0x00:
                switch (in & 0x3F) {
                    case 0x00: r[rd] = r[rt] << sa; break;
                    case 0x02: r[rd] = r[rt] >> sa; break;
                    case 0x03: r[rd] = (u32)((s32)r[rt] >> sa); break;
                    case 0x04: r[rd] = r[rt] << (r[rs] & 31); break;
                    case 0x06: r[rd] = r[rt] >> (r[rs] & 31); break;
                    case 0x08: target = r[rs]; branch = 1; break; // jr
                    case 0x21: case 0x20: r[rd] = r[rs] + r[rt]; break;
                    case 0x23: case 0x22: r[rd] = r[rs] - r[rt]; break;
                    case 0x24: r[rd] = r[rs] & r[rt]; break;
                    case 0x25: r[rd] = r[rs] | r[rt]; break;
                    case 0x26: r[rd] = r[rs] ^ r[rt]; break;
                    case 0x27: r[rd] = ~(r[rs] | r[rt]); break;
                    case 0x2A: r[rd] = (s32)r[rs] < (s32)r[rt]; break;
                    case 0x2B: r[rd] = r[rs] < r[rt]; break;
                    default: ok = 0; break;
                }
                break;
            case 0x01: // bltz / bgez
                if ((rt == 0 && (s32)r[rs] < 0) || (rt == 1 && (s32)r[rs] >= 0)) target = next + (imm << 2);
                if (rt > 1) ok = 0;
                branch = 1;
                break;
            case 0x02: target = (next & 0xF0000000) | ((in & 0x03FFFFFF) << 2); branch = 1; break; // j
            case 0x04: if (r[rs] == r[rt]) target = next + (imm << 2); branch = 1; break;
            case 0x05: if (r[rs] != r[rt]) target = next + (imm << 2); branch = 1; break;
            case 0x06: if ((s32)r[rs] <= 0) target = next + (imm << 2); branch = 1; break;
            case 0x07: if ((s32)r[rs] > 0) target = next + (imm << 2); branch = 1; break;
            case 0x08: case 0x09: r[rt] = r[rs] + imm; break;
            case 0x0A: r[rt] = (s32)r[rs] < imm; break;
            case 0x0B: r[rt] = r[rs] < (u32)imm; break;
            case 0x0C: r[rt] = r[rs] & (in & 0xFFFF); break;
            case 0x0D: r[rt] = r[rs] | (in & 0xFFFF); break;
            case 0x0E: r[rt] = r[rs] ^ (in & 0xFFFF); break;
            case 0x0F: r[rt] = (in & 0xFFFF) << 16; break;
            case 0x23: r[rt] = Port_BlobLoad32(f, link, r[rs] + imm, &ok); break; // lw
            case 0x20: case 0x24: case 0x21: case 0x25: { // lb lbu lh lhu
                u32 a = r[rs] + imm;
                u32 w = Port_BlobLoad32(f, link, a & ~3u, &ok);
                u32 v = (op == 0x20 || op == 0x24) ? (w >> (24 - 8 * (a & 3))) & 0xFF : (w >> (16 - 8 * (a & 2))) & 0xFFFF;
                if (op == 0x20) v = (u32)(s8)v;
                if (op == 0x21) v = (u32)(s16)v;
                r[rt] = v;
                break;
            }
            case 0x2B: case 0x29: case 0x28: // stores: only to the stack
                if (rs != 29) ok = 0;
                break;
            default: ok = 0; break;
        }
        r[0] = 0;
        if (!ok) {
            break;
        }
        if (branch) { // execute the delay slot, then jump
            u32 ds = Port_BlobLoad32(f, link, next, &ok);
            u32 dop = ds >> 26, drs = (ds >> 21) & 31, drt = (ds >> 16) & 31, drd = (ds >> 11) & 31;
            s32 dimm = (s16)(ds & 0xFFFF);
            switch (dop) {
                case 0x00:
                    if ((ds & 0x3F) == 0x25) r[drd] = r[drs] | r[drt];
                    else if ((ds & 0x3F) == 0x21) r[drd] = r[drs] + r[drt];
                    else if (ds != 0) ok = 0;
                    break;
                case 0x09: r[drt] = r[drs] + dimm; break;
                case 0x0D: r[drt] = r[drs] | (ds & 0xFFFF); break;
                case 0x0F: r[drt] = (ds & 0xFFFF) << 16; break;
                case 0x2B: if (drs != 29) ok = 0; break;
                case 0x23: r[drt] = Port_BlobLoad32(f, link, r[drs] + dimm, &ok); break;
                default: ok = 0; break;
            }
            r[0] = 0;
            if (!ok) {
                break;
            }
            if (target == 0xDEAD0000) {
                u32 v = r[2];
                if (v >= link && v < link + size) {
                    return Port_VAddrOfLink(f, v);
                }
                return v;
            }
            // jump tables inside the blob hold its pseudo addresses (Port_RelocateDataBlob rewrote them): map back
            if ((target < link || target >= link + size) && (target & 0xFFF00000) == (Port_VAddrOfLink(f, link) & 0xFFF00000)) {
                target = link | (target & 0xFFFFF);
            }
            pc = target;
            next = pc + 4;
            continue;
        }
        pc = next;
        next = pc + 4;
    }
    fprintf(stderr, "[blob-entry] cannot run fragment code at 0x%08x (insn 0x%08x, arg %d)\n", pc, Port_BlobLoad32(f, link, pc, &ok), a0);
    return 0;
}
