#include "audio_sfx_wrapper.h"
#include "src/audio_sfx.h"

void Audio_PlaySoundEffectByIdWrapper(s32 arg0) {
#ifdef PORT_NO_AUDIO
    return;
#endif
    Audio_PlaySoundEffectById(arg0);
}

s32 func_80036790(void) {
#ifdef PORT_NO_AUDIO
    return 0;
#endif
    s32 arr[1];

    return arr[16];
}
