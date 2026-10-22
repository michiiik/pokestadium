#include "audio_stream.h"

u8 gAudioPaused = 0;

s32 gAudioTransferPending;
GbAudioChannelState* gAudioChannel[2];
s16* gAudioSampleData[2];
u32 gAudioChunkSizeLarge;
u32 gAudioChunkSizeSmall;
u8 gAudioChannelState[2];
s32 gAudioChannelDmaStart[2];
u32 gAudioChannelVolume[2];
s32 gAudioChannelTargetVolume[2];
s32 gAudioChannelLoopCount[2];
s32 pad_D_800FCF18;
s16 gAudioChannelVolumeFadeStep[2];
u8 gAudioChannelStopFlag[2];
OSPiHandle* gCartRomHandle;
AudioBuffer* gAudioChannelBuffer[2];
u32 gAudioChannelReadPos[2];
s32 gAudioChannelWritePos[2];
OSMesgQueue gAudioDmaQueue;
OSMesg gAudioDmaMsg;

void AudioStream_Init(ALHeap* arg0, u32 arg1, u32 arg2) {
    s32 i;

    gAudioChunkSizeLarge = arg1 >> 1;
    gAudioChunkSizeSmall = arg2 >> 1;

    for (i = 0; i < 2; i++) {
        gAudioChannel[i] = alHeapDBAlloc(NULL, 0, arg0, 1, sizeof(GbAudioChannelState));
        gAudioChannelBuffer[i] = alHeapDBAlloc(NULL, 0, arg0, sizeof(*gAudioChannelBuffer[0]) / 2, 2);

        gAudioSampleData[i] = (u32)gAudioChannel[i] + 0x198;
        gAudioChannelReadPos[i] = 0;
        gAudioChannelWritePos[i] = 0x400;
        gAudioChannelState[i] = 2;

        bzero(gAudioChannelBuffer[i], 0x1140);
        bzero(gAudioSampleData[i], 0x1400);

        AudioStream_OpenChannel(gAudioChannel[i], 0, 0);
        gAudioChannelStopFlag[i] = 0;
    }

    gCartRomHandle = osCartRomInit();
    osCreateMesgQueue(&gAudioDmaQueue, &gAudioDmaMsg, 1);
    gAudioTransferPending = 0;
    gAudioPaused = 0;
}

void AudioStream_Start(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    u32 temp_a0;

    if ((arg0 < 2) && (gAudioPaused == 0)) {
        if (arg2 >= 0x81) {
            arg2 = 0x80;
        }

        temp_a0 = osSetIntMask(1);

        if (arg3 == 0) {
            gAudioChannelState[arg0] = 0;
        } else {
            gAudioChannelState[arg0] = 3;
        }

        gAudioChannelDmaStart[arg0] = arg1;
        gAudioChannelLoopCount[arg0] = arg3;
        gAudioChannelTargetVolume[arg0] = arg2;

        osSetIntMask(temp_a0);
    }
}

void AudioStream_Stop(u32 arg0, u32 arg1) {
    u32 temp_a0;

    if (arg0 < 2) {
        if (arg1 != 0) {
            gAudioChannelVolumeFadeStep[arg0] = (f32)gAudioChannelVolume[arg0] / arg1;
        }
        temp_a0 = osSetIntMask(1);
        if (arg1 != 0) {
            gAudioChannelState[arg0] = 5;
        } else {
            gAudioChannelState[arg0] = 1;
        }
        osSetIntMask(temp_a0);
    }
}

s32 AudioStream_IsPlaying(u32 arg0) {
    if (gAudioChannelState[arg0] == 2) {
        return 0;
    }
    return 1;
}

void AudioStream_SetEnabled(s32 arg0) {
    if (arg0 != 0) {
        gAudioPaused = 0;
    } else {
        gAudioPaused = 1;
    }
}

void Audio_ProcessChannels(void) {
    UNUSED s32 pad;
    u32 j;
    u32 var_s7;
    s32 i;
    u32 temp_a2;
    s32 temp_v0_9;
    s16 sample;

    for (i = 0; i < 2; i++) {
        if (gAudioChannelState[i] == 2U) {
            continue;
        }
        {
            if (gAudioChannelReadPos[i] < gAudioChannelWritePos[i]) {
                if ((gAudioChannelWritePos[i] - gAudioChannelReadPos[i]) < gAudioChunkSizeSmall) {
                    var_s7 = gAudioChunkSizeSmall;
                } else {
                    var_s7 = gAudioChunkSizeLarge;
                }
            } else if ((0x8A0 - gAudioChunkSizeSmall) < (gAudioChannelReadPos[i] - gAudioChannelWritePos[i])) {
                var_s7 = gAudioChunkSizeSmall;
            } else {
                var_s7 = gAudioChunkSizeLarge;
            }

            AudioStream_RefillAndDecode(gAudioChannel[i]);
            if ((u8)gAudioChannel[i]->unk_25D1 == 2) {
                osWritebackDCache(gAudioChannelBuffer[i], 0x1140);
                AudioStream_MarkLoaded(gAudioChannel[i]);
                gAudioTransferPending = 1;
                gAudioChannelStopFlag[i] = 0;
            } else if ((gAudioChannelState[i] != 0) && (gAudioChannelState[i] != 3) && ((u8)gAudioChannel[i]->unk_25D1 == 4)) {
                if (gAudioChannelStopFlag[i] == 0) {
                    AudioStream_Abort(gAudioChannel[i]);
                    gAudioChannelStopFlag[i] = 1;
                }
                gAudioTransferPending = 0;
                gAudioChannelState[i] = 2;
                bzero(gAudioSampleData[i], 0x1400);
                bzero(gAudioChannelBuffer[i], 0x1140);
            }

            AudioStream_ConsumeSamples(gAudioChannel[i], var_s7);

            temp_a2 = gAudioChannel[i]->unk_25C8 - var_s7;
            for (j = 0; j < var_s7; j++) {
                sample = (gAudioSampleData[i][(temp_a2 + j) % 2560] * gAudioChannelVolume[i]) >> 7;
                gAudioChannelBuffer[i]->unk_000[(gAudioChannelWritePos[i] + (j << 1) + 1) % 2208] = sample;
                gAudioChannelBuffer[i]->unk_000[(gAudioChannelWritePos[i] + (j << 1)) % 2208] = sample;
            }

            gAudioChannelWritePos[i] += var_s7 * 2;
            gAudioChannelWritePos[i] = (u32)gAudioChannelWritePos[i] % 2208;
            osWritebackDCache(gAudioChannelBuffer[i], 0x1140);

            switch (gAudioChannelState[i]) {
                case 2:
                case 4:
                    break;

                case 0:
                    AudioStream_OpenChannel(gAudioChannel[i], gAudioChannelDmaStart[i], AudioStream_DmaReadChunk);
                    gAudioChannelState[i] = 4;
                    if (gAudioTransferPending != 0) {
                        gAudioTransferPending = 0;
                    }
                    gAudioChannelVolume[i] = gAudioChannelTargetVolume[i];
                    break;

                case 1:
                    AudioStream_Abort(gAudioChannel[i]);
                    bzero(gAudioSampleData[i], 0x1400);
                    gAudioChannelStopFlag[i] = 1;
                    break;

                case 3:
                    if (--gAudioChannelLoopCount[i] == 0) {
                        gAudioChannelState[i] = 0;
                    }
                    break;

                case 5:
                    temp_v0_9 = gAudioChannelVolume[i] - gAudioChannelVolumeFadeStep[i];
                    if (temp_v0_9 <= 0) {
                        gAudioChannelVolume[i] = 0;
                        gAudioChannelState[i] = 1;
                    } else {
                        gAudioChannelVolume[i] = temp_v0_9;
                    }
                    break;
            }
        }
    }
}


void AudioStream_DmaReadChunk(u32 arg0, u32 arg1, u32 arg2, OSMesgQueue* arg3) {
    OSIoMesg sp48;
    u32 var_s1;
    u32 var_s2;
    u32 var_s3;

    var_s2 = arg0;
    var_s3 = arg1;

    while (arg2) {
        if (arg2 >= 0x1001) {
            var_s1 = 0x1000;
        } else {
            var_s1 = arg2;
        }

#if defined(VERSION_JP)
        sp48.hdr.pri = OS_MESG_PRI_HIGH;
#else
        sp48.hdr.pri = 0;
#endif
        sp48.hdr.retQueue = &gAudioDmaQueue;
        sp48.dramAddr = var_s3;
        sp48.devAddr = var_s2;
#if defined(VERSION_JP)
        // Each Japanese request spans from the current position to the end of the transfer instead
        // of one 0x1000-byte chunk, so later iterations reread bytes the previous ones copied.
        sp48.size = arg2;
#else
        sp48.size = var_s1;
#endif

        osInvalDCache(arg1, arg2);
        osEPiStartDma(gCartRomHandle, &sp48, 0);
        osRecvMesg(&gAudioDmaQueue, NULL, 1);

        arg2 -= var_s1;
        var_s2 += var_s1;
        var_s3 += var_s1;
    }

    osSendMesg(arg3, NULL, 1);
}
