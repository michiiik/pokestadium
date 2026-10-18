#include "model_animation_events.h"
#include "src/util.h"
#include "src/memmap.h"

static unk_D_800ABD00 D_800ABD00[2];
static s32 D_800ABD20;
// PORT: event tracks of models loaded from ROM are big-endian with 32-bit addresses (trackTable at 0xC, frameValues at
// 0x10); C-defined tracks use the host layout. The accessors pick the right form from the data's address.
static s32 ETD_RAW(const EventTrackData* d) {
    return Port_VIsRaw((const void*)d);
}
static s16 ETD_S16(const EventTrackData* d, s32 off) {
    const u8* b = (const u8*)d + off;
    return ETD_RAW(d) ? (s16)((b[0] << 8) | b[1]) : *(const s16*)b;
}
#define ETD_START(d) ETD_S16(d, 0x04)
#define ETD_LOOP(d) ETD_S16(d, 0x06)
#define ETD_COUNT(d) ((u16)ETD_S16(d, 0x08))
#define ETD_END(d) ((u16)ETD_S16(d, 0x0A))
#define ETD_FLAGS(d) ETD_S16(d, 0x00)
static void* ETD_PTR(const EventTrackData* d, s32 which) {
    if (ETD_RAW(d)) {
        return Util_ConvertAddrToVirtAddr(PORT_BE32(*(const u32*)((const u8*)d + 0x0C + which * 4)));
    }
    return Util_ConvertAddrToVirtAddr(which ? (uintptr_t)d->frameValues : (uintptr_t)d->trackTable);
}
static s32 sEventRaw;


s32 ModelAnim_ResolveEventIndex(s32 arg0, unk_func_80017540_arg1* arg1, s32 arg2) {
    unk_func_80017540_arg1* temp_v0 = &arg1[arg2];
    s32 ret;
    u16 n = temp_v0->unk_00, first = temp_v0->unk_02;

    if (sEventRaw) { // PORT: big-endian entries
        n = (u16)((n >> 8) | (n << 8));
        first = (u16)((first >> 8) | (first << 8));
    }
    if (arg0 < n) {
        ret = arg0 + first;
    } else {
        ret = (first + n) - 1;
    }
    return ret;
}

s16 ModelAnim_AdvanceEventFrame(EventTrackState* arg0, u16 arg1) {
    s16 var_v1;
    EventTrackData* temp_v0;

    var_v1 = arg0->frame;
    temp_v0 = arg0->data;

    if (arg0->lastRenderFrame != arg1) {
        var_v1 += 1;
        if (var_v1 >= ETD_END(temp_v0)) {
            if (ETD_FLAGS(temp_v0) & 2) {
                var_v1 = ETD_END(temp_v0) - 1;
            } else {
                var_v1 = ETD_LOOP(temp_v0);
            }
        }
    }
    return var_v1;
}

void ModelAnim_ResetEventContext(void) {
    D_800ABD20 = -1;
}

void ModelAnim_BeginEventContext(EventTrackState* arg0, u16 arg1, s32 arg2) {
    EventTrackData* sp24;
    unk_D_800ABD00* temp_s0;

    sp24 = arg0->data;
    D_800ABD20++;
    if (D_800ABD20 < 2) {
        temp_s0 = &D_800ABD00[D_800ABD20];
        if (arg0->data != NULL) {
            if (arg2 != 0) {
                arg0->frame = ModelAnim_AdvanceEventFrame(arg0, arg1);
            }
            arg0->lastRenderFrame = arg1;
            temp_s0->unk_00 = 1;
            temp_s0->unk_02 = arg0->frame;
            temp_s0->unk_04 = sp24;
            temp_s0->unk_08 = ETD_PTR(sp24, 0);
            temp_s0->unk_0C = ETD_PTR(sp24, 1);
            if (temp_s0->unk_02 < 0) {
                temp_s0->unk_02 = 0;
            }
        } else {
            temp_s0->unk_00 = 0;
        }
    }
}

void ModelAnim_EndEventContext(void) {
    if (D_800ABD20 >= 0) {
        D_800ABD20--;
    }
}

void ModelAnim_GetEventAtFrame(unk_D_86002F34_alt11_018** arg0, unk_D_86002F34_alt11_018* arg1, s32 arg2) {
    unk_D_800ABD00* sp1C;

    if ((D_800ABD20 >= 0) && (D_800ABD20 < 2)) {
        sp1C = &D_800ABD00[D_800ABD20];
        sEventRaw = ETD_RAW(sp1C->unk_04);
        if ((sp1C->unk_00 == 1) && (arg2 >= 0) && (arg2 < ETD_COUNT(sp1C->unk_04))) {
            // PORT: descriptors in ROM model data are 12 bytes apart (the host struct is larger): Port_DescAt
            extern void* Port_DescAt(void* base, int idx, size_t hostStride);
            *arg0 = Port_DescAt(arg1, ((u8*)sp1C->unk_0C)[ModelAnim_ResolveEventIndex(sp1C->unk_02, sp1C->unk_08, arg2)],
                                sizeof(unk_D_86002F34_alt11_018));
        }
    }
}

void ModelAnim_ClearEventTrack(DisplayObject* arg0) {
    arg0->eventTrack.trackId = -1;
    arg0->eventTrack.data = 0;
}

s16 ModelAnim_BindEventTrack(DisplayObject* arg0, s16 arg1, intptr_t arg2) { // PORT: arg2 is an address (was s32)
    EventTrackData* temp_v0 = Util_ConvertAddrToVirtAddr(arg2);
    EventTrackState* ptr = &arg0->eventTrack;

    if ((temp_v0 != ptr->data) || (arg1 != ptr->trackId)) {
        ptr->trackId = arg1;
        ptr->data = temp_v0;
        ptr->frame = ETD_START(temp_v0) - 1;
    }

    return ptr->frame;
}

void ModelAnim_SetEventFrame(DisplayObject* arg0, s16 arg1) {
    arg0->eventTrack.frame = arg1 - 1;
}

s32 ModelAnim_IsEventFrame(DisplayObject* arg0, s16 arg1) {
    return (arg0->eventTrack.frame + 1) == arg1;
}

s32 ModelAnim_IsEventTrackNearEnd(DisplayObject* arg0) {
    return (arg0->eventTrack.frame + 2) == ETD_END(arg0->eventTrack.data);
}

s32 ModelAnim_IsEventTrackDone(DisplayObject* arg0) {
    return (arg0->eventTrack.frame + 1) == ETD_END(arg0->eventTrack.data);
}
