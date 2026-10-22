#include "battle_hud.h"
#include "model_renderer_callbacks.h"
#include "geo_render.h"
#include "src/util.h"
#include "src/memmap.h"
#include "src/fragments/particle_data_library/particle_data_library.h"

// .bss
u8 gPillarGlowValues[8];
s16 gHpWheelDigitOffsetOnes[4];
s16 gHpWheelDigitOffsetTens[4];
s16 gHpWheelDigitOffsetHundreds[4];
s16 gHpWheelDigitOffsetOnesSmooth[4];
s16 gHpWheelDigitOffsetTensSmooth[4];
s16 gHpWheelDigitOffsetHundredsSmooth[4];
s32 gPrevAnimationFrame;
s16 gAnimationFrameUnchangedFlag;
s16 gTextureOffsetScale;
s16 gModelUpdateFlag[2];
s16 gModelReadyFlag[2];

void BattleHud_SetDigitRotationTargets(u8 arg0, s16 arg1) {
    gHpWheelDigitOffsetOnes[arg0] = 0x167 - ((arg1 % 10) * 0x24);
    gHpWheelDigitOffsetTens[arg0] = 0x167 - (((arg1 / 10) % 10) * 0x24);
    gHpWheelDigitOffsetHundreds[arg0] = 0x167 - (((arg1 / 100) % 10) * 0x24);
}

void BattleHud_ApproachDigitRotations(u8 arg0, s16 arg1) {
    s16 tmp;
    s16 tmp2;

    gHpWheelDigitOffsetOnesSmooth[arg0] = 0x167 - ((arg1 % 10) * 0x24);
    gHpWheelDigitOffsetTensSmooth[arg0] = 0x167 - (((arg1 / 10) % 10) * 0x24);
    gHpWheelDigitOffsetHundredsSmooth[arg0] = 0x167 - (((arg1 / 100) % 10) * 0x24);

    tmp2 = gHpWheelDigitOffsetOnesSmooth[arg0];
    tmp = gHpWheelDigitOffsetOnes[arg0];
    if (((tmp2 < 0x5A) && (tmp >= 0x10F)) || ((tmp < 0x5A) && (tmp2 >= 0x10F))) {
        if (tmp < tmp2) {
            gHpWheelDigitOffsetOnes[arg0] -= 6;
        }

        if (tmp2 < tmp) {
            gHpWheelDigitOffsetOnes[arg0] += 6;
        }
    } else {
        if (tmp < tmp2) {
            gHpWheelDigitOffsetOnes[arg0] += 6;
        }

        if (tmp2 < tmp) {
            gHpWheelDigitOffsetOnes[arg0] -= 6;
        }
    }

    if (gHpWheelDigitOffsetOnes[arg0] < 0) {
        gHpWheelDigitOffsetOnes[arg0] += 0x168;
    }

    if (gHpWheelDigitOffsetOnes[arg0] >= 0x168) {
        gHpWheelDigitOffsetOnes[arg0] -= 0x168;
    }

    tmp2 = gHpWheelDigitOffsetTensSmooth[arg0];
    tmp = gHpWheelDigitOffsetTens[arg0];
    if (((tmp2 < 0x5A) && (tmp >= 0x10F)) || ((tmp < 0x5A) && (tmp2 >= 0x10F))) {
        if (tmp < tmp2) {
            gHpWheelDigitOffsetTens[arg0] -= 6;
        }

        if (tmp2 < tmp) {
            gHpWheelDigitOffsetTens[arg0] += 6;
        }
    } else {
        if (tmp < tmp2) {
            gHpWheelDigitOffsetTens[arg0] += 6;
        }

        if (tmp2 < tmp) {
            gHpWheelDigitOffsetTens[arg0] -= 6;
        }
    }

    if (gHpWheelDigitOffsetTens[arg0] < 0) {
        gHpWheelDigitOffsetTens[arg0] += 0x168;
    }

    if (gHpWheelDigitOffsetTens[arg0] >= 0x168) {
        gHpWheelDigitOffsetTens[arg0] -= 0x168;
    }

    tmp2 = gHpWheelDigitOffsetHundredsSmooth[arg0];
    tmp = gHpWheelDigitOffsetHundreds[arg0];
    if (((tmp2 < 0x5A) && (tmp >= 0x10F)) || ((tmp < 0x5A) && (tmp2 >= 0x10F))) {
        if (tmp < tmp2) {
            gHpWheelDigitOffsetHundreds[arg0] -= 6;
        }

        if (tmp2 < tmp) {
            gHpWheelDigitOffsetHundreds[arg0] += 6;
        }
    } else {
        if (tmp < tmp2) {
            gHpWheelDigitOffsetHundreds[arg0] += 6;
        }

        if (tmp2 < tmp) {
            gHpWheelDigitOffsetHundreds[arg0] -= 6;
        }
    }

    if (gHpWheelDigitOffsetHundreds[arg0] < 0) {
        gHpWheelDigitOffsetHundreds[arg0] += 0x168;
    }

    if (gHpWheelDigitOffsetHundreds[arg0] >= 0x168) {
        gHpWheelDigitOffsetHundreds[arg0] -= 0x168;
    }
}

void ModelAnim_UpdateFrameChangeState(void) {
    gAnimationFrameUnchangedFlag = (gPrevAnimationFrame == gAnimationFrame) ? 1 : 0;
    gPrevAnimationFrame = gAnimationFrame;
}

s32 ModelPhase_IsIdle(void) {
    return 0;
}

u8 ModelRenderer_GetActiveMode(void) {
    u8 ret = *(u8*)Memmap_GetSegmentVaddr(&gDefaultGraphRoot->effectSlot);
    return ret;
}

#if !defined(VERSION_JP)
s16 ModelRenderer_GetObjectType(void) {
    s16 ret = *(s16*)Util_ConvertAddrToVirtAddr(&gDefaultGraphRoot->objectType);
    return ret;
}
#endif

#if defined(VERSION_JP)
// The Japanese Model_InitDisplayObject passes DisplayObject::effectSlot instead of the object.
// 0xFF leaves the slot state untouched; slots 0 and 1 also clear their models and pass the
// slot index to Particle31_ResetAllSlots, whose Japanese parameter type is unresolved.
//@src/model_renderer.c
void ModelAnim_StartDisplayObjectAnimation(u32 arg0) {
    void (*func)(u32);

    gTextureOffsetScale = 0;

    if (arg0 != 0xFF) {
        gModelPhase[arg0 & 1] = 0;
        if (arg0 < 2) {
            ModelRenderer_ClearSlotModels(arg0);
            func = Util_ConvertAddrToVirtAddr(Particle31_ResetAllSlots);
            func(arg0);
        }
    }
}
#else
void ModelAnim_StartDisplayObjectAnimation(DisplayObject* arg0) {
    void (*func)(void*);
    s32 unk1A = arg0->objectType;
    s32 idx = arg0->effectSlot;

    gTextureOffsetScale = 0;

    if (unk1A < 0x98) {
        gModelPhase[idx & 1] = 0;
        ModelRenderer_ClearSlotModels(idx & 1);
        func = Util_ConvertAddrToVirtAddr(Particle31_ResetAllSlots);
        func(arg0);
    }
}
#endif

void ModelAnim_UpdateDisplayObjectAnimation(u32 arg0, u32 arg1) {
    s32 (*func)(void);

    gModelUpdateFlag[arg0 & 1] = 0;
    func = Util_ConvertAddrToVirtAddr(Particle31_ResetCounters);
    func();

    if ((arg0 < 2) && (arg1 != 0) && (arg1 < 0x98)) {
        switch (gModelPhase[arg0]) {
            default:
                gModelPhase[arg0] = 1;
                return;
            case 0:
                gModelPhase[arg0] = 1;
                return;
            case 1:
                gModelPhase[arg0] = 2;
                return;
            case 2:
                gModelPhase[arg0] = 3;
                break;
            case 3:
                break;
        }
    }
}

void ModelAnim_FinalizeDisplayObjectAnimation(u32 arg0, u32 arg1) {
    if (ModelPhase_IsIdle() == 0) {
        gTextureOffsetScale++;
    }

    if ((arg1 != 0) && (arg0 < 2)) {
        if (gModelUpdateFlag[arg0 & 1] == 0) {
            gModelReadyFlag[arg0] = 0;
        } else {
            gModelReadyFlag[arg0] = 1;
        }
    }
}

void Gfx_SetCombineMode(Gfx* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, s32 arg8, s32 arg9, s32 arg10, s32 arg11, s32 arg12, s32 arg13, s32 arg14, s32 arg15, s32 arg16) {
    gDPSetCombine(arg0++, GCCc0w0(arg1, arg3, arg5, arg7) | GCCc1w0(arg9, arg11), GCCc0w1(arg2, arg4, arg6, arg8) | GCCc1w1(arg10, arg13, arg15, arg12, arg14, arg16));
}
