#include "audio_sound_state.h"
#include "src/audio_io.h"
#include "src/audio_stream.h"
#include "src/3D140.h"
#include "src/audio_commands.h"

void Audio_IsSoundPlayingWrapper(s32 arg0) {
    Audio_IsSoundPlaying(arg0);
}

s32 Audio_CountCategorySounds(s32 arg0) {
    return Audio_CountActiveSoundsByCategory(arg0);
}

s32 Audio_GetActivityScore(void) {
    s32 i;
    s32 temp_v0;
    s32 var_s1;

    var_s1 = 0;
    if (D_800FC818 != D_800FC814) {
        var_s1 = D_800FC814 - D_800FC818;
    }

    for (i = 0; i < 2; i++) {
        if (AudioStream_IsPlaying(i) != 0) {
            var_s1 += 100;
        }
    }

    temp_v0 = Cry_IsPlaying();
    if (temp_v0 != 0) {
        var_s1 += temp_v0 * 1000;
    }

    return var_s1;
}

s32 Audio_GetFreeVoiceCount(void) {
    return D_800FC818;
}

s32 Audio_GetActiveVoiceCount(void) {
    return D_800FC81C;
}
