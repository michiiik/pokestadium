#include "audio_stored_fade.h"
#include "audio_io.h"

extern s32 D_80078A18;
extern u8 D_80078A1C;

#pragma GLOBAL_ASM("asm/us/nonmatchings/audio_stored_fade/func_80048060.s")

void Audio_FadeStoredSound(void) {
  if (D_80078A1C != 0) {
      Audio_QueueFadeSoundCommand(D_80078A18, (s32)D_80078A1C);
  }
}
