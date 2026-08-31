#include "audio_category_fade.h"
#include "audio_stream.h"
#include "libnumus/player.h"
#include "3D140.h"

void Audio_FadeCategories(s32 arg0, u32 arg1) {
    s32 i;

    if (arg0 & 4) {
        for (i = 0; i < 2; i++) {
            AudioStream_Stop(i, arg1);
        }
        arg0 &= ~4;
    }
    Audio_QueueFadeCategoryCommand(arg0, arg1);
}

void Audio_FadeCategoriesAndStreams(s32 arg0) {
    s32 i;

    D_8007840C = 0;
    func_8003DB84(0);
    Audio_FadeCategories(3, arg0);

    for (i = 0; i < 2; i++) {
        AudioStream_Stop(i, arg0);
    }
}

void Audio_FadeCategoriesAndStreamsDefault(void) {
    Audio_FadeCategoriesAndStreams(1);
}

void Audio_FadeCategoriesAndStreamsWithDuration(s32 arg0) {
    Audio_FadeCategoriesAndStreams(arg0);
}
