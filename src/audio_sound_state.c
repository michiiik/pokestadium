#include <stdio.h>
#include <stdlib.h>
#include "audio_sound_state.h"
#include "src/libnumus/player.h"
#include "src/audio_stream.h"
#include "src/3D140.h"
#include "src/audio_commands.h"

void Audio_IsSoundPlayingWrapper(s32 arg0) {
    if (PORT_AUDIO_OFF()) {
        return;
    }
    Audio_IsSoundPlaying(arg0);
}

s32 Audio_CountCategorySounds(s32 arg0) {
    if (PORT_AUDIO_OFF()) {
        return 0;
    }
    return Audio_CountActiveSoundsByCategory(arg0);
}

s32 Audio_GetActivityScore(void) {
    if (PORT_AUDIO_OFF()) {
        return 0;
    }
    s32 i;
    s32 temp_v0;
    s32 var_s1;

    var_s1 = 0;
    if (mus_free_voices_count != mus_num_vvoices) {
        var_s1 = mus_num_vvoices - mus_free_voices_count;
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
    if (getenv("PORT_TRACE_ACTIVITY")) {
        extern unsigned long long Port_ViTick(void);
        static unsigned long long last = 0;
        if (Port_ViTick() >= last + 60) {
            last = Port_ViTick();
            fprintf(stderr, "[activity] frame %llu score %d (voices %d free %d releasing %d, streams %d %d, cry %d)\n",
                    Port_ViTick(), (int)var_s1, (int)mus_num_vvoices, (int)mus_free_voices_count,
                    (int)mus_releasing_voices_count, (int)AudioStream_IsPlaying(0), (int)AudioStream_IsPlaying(1),
                    (int)temp_v0);
            {
                extern void Audio_PortDumpChannels(void);
                Audio_PortDumpChannels();
            }
        }
    }

    return var_s1;
}

s32 Audio_GetFreeVoiceCount(void) {
    if (PORT_AUDIO_OFF()) {
        return 0;
    }
    return mus_free_voices_count;
}

s32 Audio_GetActiveVoiceCount(void) {
    if (PORT_AUDIO_OFF()) {
        return 0;
    }
    return mus_releasing_voices_count;
}
