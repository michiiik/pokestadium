#include "audio_sfx_wrapper.h"
#include "src/audio_sfx.h"

void Audio_PlaySoundEffectByIdWrapper(s32 arg0) {
    if (PORT_AUDIO_OFF()) {
        return;
    }
    Audio_PlaySoundEffectById(arg0);
}

s32 func_80036790(void) {
    if (PORT_AUDIO_OFF()) {
        return 0;
    }
    s32 arr[1];

    return arr[16];
}
