#include "global.h"

extern s32 D_800789F4;
extern s32 D_800789F8;

#pragma GLOBAL_ASM("asm/us/nonmatchings/485C0/func_800479C0.s")

void Audio_FadeAndClearStoredSounds(void) {
  Audio_QueueFadeSoundCommand(D_800789F8, 0x5A);
  Audio_QueueFadeSoundCommand(D_800789F4, 0x5A);
  D_800789F8 = 0;
  D_800789F4 = 0;
}
