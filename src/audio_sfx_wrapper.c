#include "audio_sfx_wrapper.h"
#include "src/audio_sfx.h"

void Audio_PlaySoundEffectByIdWrapper(s32 arg0) {
    Audio_PlaySoundEffectById(arg0);
}

s32 Stub_ReturnInvalidIndex(void) {
    s32 arr[1];

    return arr[16];
}
