#ifndef _MACROS_H_
#define _MACROS_H_

#define true 1
#define false 0

// Avoid compiler warnings for unused variables
#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED


#endif

#ifndef __sgi
#define GLOBAL_ASM(...)


#endif

#ifdef _LANGUAGE_C
// PORT: memory-mapped hardware registers have no host equivalent; they read/write a scratch page.
extern void* port_hw_reg(unsigned long addr);
#define HW_REG(reg, type) *(volatile type*)port_hw_reg((unsigned long)(reg))


#endif

#define assert

#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))

#define SQ(x) ((x) * (x))
#define CB(x) ((x) * (x) * (x))

// cast thru uintptr_t for integer to pointer casts
#define INT2VOID(i) ((void *)(uintptr_t)(i))

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// PORT: masks are pointer-width so these are safe on addresses.
#define ALIGN2(val) (((uintptr_t)(val) + 0x1) & ~(uintptr_t)0x1)
#define ALIGN4(val) (((uintptr_t)(val) + 0x3) & ~(uintptr_t)0x3)
#define ALIGN8(val) (((uintptr_t)(val) + 0x7) & ~(uintptr_t)0x7)
#define ALIGN16(val) (((uintptr_t)(val) + 0xF) & ~(uintptr_t)0xF)
#define ALIGN64(val) (((uintptr_t)(val) + 0x3F) & ~(uintptr_t)0x3F)

#define DECR(x) ((x) == 0 ? 0 : --(x))

//! checks min first
#define CLAMP(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))
//! checks max first
#define CLAMP_ALT(x, min, max) ((x) > (max) ? (max) : (x) < (min) ? (min) : (x))
#define CLAMP_MAX(x, max) ((x) > (max) ? (x) : (max))
#define CLAMP_MIN(x, min) ((x) < (min) ? (x) : (min))

#define ROUND_MAX(x) ((x) + ((x) > 0.0f ? 0.5f : -0.5f))

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define ABS_ALT(x) ((x) >= 0 ? (x) : -(x))

#define ABS_NORMALIZED(x) ((ABS_ALT((x)) < (0.000001f)) ? 0.0f : (x))

#define FRAGMENT_ID(fragment) ((((u32)fragment##_TEXT_START & 0x0FF00000) >> 0x14) - 0x10)
#define ASSET_ID(asset) (((u32)asset & 0x0F000000) >> 0x18)

#define FRAGMENT_LOAD(fragment) ( \
    Fragment_Load(FRAGMENT_ID(fragment), fragment##_ROM_START, fragment##_relocs_ROM_END) \
)

#define FRAGMENT_LOAD_AND_CALL(fragment, arg0, arg1) (\
    Fragment_LoadAndCall(FRAGMENT_ID(fragment), fragment##_ROM_START, fragment##_relocs_ROM_END, arg0, arg1) \
)

#define FRAGMENT_LOAD_AND_CALL2(fragment, arg0, arg1) (\
    Fragment_QueueCall(FRAGMENT_ID(fragment), fragment##_ROM_START, fragment##_relocs_ROM_END, arg0, arg1) \
)

#define ASSET_LOAD(vram, rom, arg0) (\
    Asset_LoadToSegment(ASSET_ID(vram), rom##_ROM_START, rom##_ROM_END, arg0) \
)

#define ASSET_LOAD2(rom, arg0, arg1) (\
    BinArchive_Open(rom##_ROM_START, rom##_ROM_END, arg0, arg1) \
)



// PORT: ROM-resident data is big-endian. Four-character tags and header words read from it as u32 must be byte
// swapped before they are compared with numeric constants or used as sizes.
static inline u32 Port_Swap32(u32 v) {
    return (v >> 24) | ((v >> 8) & 0xFF00) | ((v << 8) & 0xFF0000) | (v << 24);
}
#define PORT_BE32(x) Port_Swap32((u32)(x))
static inline u16 Port_Swap16(u16 v) {
    return (u16)((v >> 8) | (v << 8));
}

#endif
