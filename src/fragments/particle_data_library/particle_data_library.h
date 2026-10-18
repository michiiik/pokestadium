#ifndef _FRAGMENT31_H_
#define _FRAGMENT31_H_

#include "global.h"
#include "gallery.h"

#include <stddef.h>

// PORT: the callbacks of this fragment are named by ROM model layouts (through the jump table at the start of the
// fragment): their data is big-endian with 32-bit addresses. Port_PdlWords converts the address lists (display lists,
// textures) to host addresses, cached; segmented addresses stay as they are (the renderer resolves them).
typedef struct DisplayListAddresses {
    /* 0x00 */ uintptr_t segments[2];
    /* 0x08 */ uintptr_t textures[8];
} DisplayListAddresses; // size = 0xC (N64: 2 + n words)

uintptr_t* Port_PdlWords(void* arg);

// A display-list graph node (unk_D_86002F34_alt7): the callback data is GraphNode.callbackArg, the built display list
// follows the node header. (Was a flat N64 view {char[0x14]; data; gfx}.)
#define PDL_NODE_WITH_DATA(type, name) \
    union { \
        GraphNode node; \
        struct { \
            u8 pad_[offsetof(GraphNode, callbackArg)]; \
            type name; \
        }; \
    }

typedef struct DisplayListState {
    PDL_NODE_WITH_DATA(DisplayListAddresses*, addresses);
    /* 0x18 */ Gfx* gfx;
} DisplayListState; // size = 0x1C

typedef struct unk_arg0_func_81002260 {
    /* 0x00 */ char unk00[0x1A];
    /* 0x1A */ s16 unk_1A;
} unk_arg0_func_81002260; // size >= 0x1C

extern const Gfx D_810041D0[];

void Particle31_ResetAllSlots(DisplayObject*);
void Particle31_ResetCounters(void);
void Particle31_PackSlotData(u8*);
void Particle31_UnpackSlotData(unk_arg0_func_81002260*, u8*);

#endif // _FRAGMENT31_H_
