#include <PR/os_internal_reg.h>
#include "global.h"
#include "dp_intro.h"
#include "profiler.h"
#include "audio_loop_point.h"
#include "scheduler.h"
#include "gb_tower_audio.h"
#include "audio_commands.h"
#include "src/audio_category_fade.h"
#include "src/audio_sound_state.h"
#include "src/audio_channel.h"
#include "src/audio_loop_point.h"

typedef struct AudioThreadData {
  /* 0x0000 */ OSThread thread;
  /* 0x01B0 */ char unk01B0[0x2030];
} AudioThreadData; // size = 0x21E0

static s32 gAudioThreadEnabled;
static UnkStruct80001380 gAudioMessageQueues[2];
static s32 gAudioTaskBufferIndex;
static s32 gCurrentMusicId;
static AudioThreadData gAudioThreadData;

void Audio_NoopStub1(void) {
}

void Audio_NoopStub2(void) {
}

void Audio_NoopStub3(void) {
}

void Audio_NoopStub4(void) {
}

void Audio_ResetCurrentMusic(void) {
    gCurrentMusicId = -1;
}

void Audio_PlayMusicIfChanged(s32 arg0) {
    if (arg0 != gCurrentMusicId) {
        if (gCurrentMusicId >= 0) {
            Audio_FadeOutAll(0);
        }
        Audio_StartMusicTrack(arg0);
        gCurrentMusicId = arg0;
    }
}

void Audio_PlayMusicIfChangedImmediate(s32 arg0) {
    if (arg0 != gCurrentMusicId) {
        Audio_StartMusicTrack(arg0);
        gCurrentMusicId = arg0;
    }
}

void Audio_StopMusic(s32 arg0) {
    if (gCurrentMusicId >= 0) {
        Audio_FadeOutAll(arg0);
        gCurrentMusicId = -1;
    }
}

s32 Audio_WaitForIdle(s32 arg0) {
    s32 retvar = 0;

    if (arg0 != 0) {
        Audio_FadeCategoriesAndStreams(2);
    }
    while (Audio_GetActivityScore() != 0) {
        if (retvar < 1000000) {
            retvar++;
        } else if (retvar == 1000000) {
            Audio_FadeCategoriesAndStreamsDefault();
            retvar++;
        }
    }
    return retvar;
}

void Audio_FadeOutForShutdown(void) {
    Audio_FadeCategoriesAndStreamsWithDuration(0x10);
}

void Audio_Disable(void) {
    gAudioThreadEnabled = 0;
    Audio_SetEnabled(0);
}

void Audio_Enable(void) {
    Audio_SetEnabled(1);
    gAudioThreadEnabled = 1;
}

void Audio_ThreadMain(void* unused) {
    __osSetFpcCsr(0x01000C01);
    Sched_InitClientQueue(&gAudioThreadData.thread, 1, 1);
    Sched_AddClient(&gAudioThreadData.thread);
    gAudioThreadEnabled = 1;
    gCurrentMusicId = -1;
    gAudioTaskBufferIndex = 0;
    osCreateMesgQueue(&gAudioMessageQueues[0].queue, &gAudioMessageQueues[0].mesg, 1);
    osCreateMesgQueue(&gAudioMessageQueues[1].queue, &gAudioMessageQueues[1].mesg, 1);
    osSendMesg(&gAudioMessageQueues[0].queue, (void*)'DONE', 0);
    osSendMesg(&gAudioMessageQueues[1].queue, (void*)'DONE', 0);
    Audio_InitBanks();
    Audio_SetMonoOutput(0);
    Audio_SetChannelVolume(3, 4);

    // thread loop
    while (1) {
        Sched_WaitClientQueue(&gAudioThreadData.thread);
        profiler_log_thread4_time();
        if ((gAudioThreadEnabled != 0) && (gRspTaskManager.unk_A38 < 0x15)) {
            Audio_CreateFrameTaskWrapper(&gAudioMessageQueues[gAudioTaskBufferIndex].task);
            Sched_SubmitTask(&gAudioMessageQueues[gAudioTaskBufferIndex], 0);
        }
        gAudioTaskBufferIndex ^= 1;
        profiler_log_thread4_time();
    }
}

void Audio_StartThread(void) {
    osCreateThread(&gAudioThreadData.thread, 4, Audio_ThreadMain, NULL, &gGbEmuState, 0x50);
    osStartThread(&gAudioThreadData.thread);
}
