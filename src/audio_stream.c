#include "src/port_audio.h"
#include <stdio.h>
#include <stdlib.h>
#include "audio_stream.h"

u8 D_800785F0 = 0;

s32 D_800FCED0;
unk_D_800FCED8* D_800FCED8[2];
s16* D_800FCEE0[2];
u32 D_800FCEE8;
u32 D_800FCEEC;
u8 D_800FCEF0[2];
s32 D_800FCEF8[2];
u32 D_800FCF00[2];
s32 D_800FCF08[2];
s32 D_800FCF10[2];
s32 pad_D_800FCF18;
s16 D_800FCF1C[2];
u8 D_800FCF20[2];
OSPiHandle* D_800FCF24;
unk_D_800FCF28* D_800FCF28[2];
u32 D_800FCF30[2];
s32 D_800FCF38[2];
OSMesgQueue D_800FCF40;
OSMesg D_800FCF58;

void AudioStream_Init(ALHeap* arg0, u32 arg1, u32 arg2) {
    if (PORT_AUDIO_OFF()) {
        return;
    }
    s32 i;

    D_800FCEE8 = arg1 >> 1;
    D_800FCEEC = arg2 >> 1;

    for (i = 0; i < 2; i++) {
        D_800FCED8[i] = alHeapDBAlloc(NULL, 0, arg0, 1, sizeof(unk_D_800FCED8));
        D_800FCF28[i] = alHeapDBAlloc(NULL, 0, arg0, sizeof(*D_800FCF28[0]) / 2, 2);

        D_800FCEE0[i] = (s16*)D_800FCED8[i]->unk_0198.unk_0000; // PORT: was + 0x198 (the N64 layout)
        D_800FCF30[i] = 0;
        D_800FCF38[i] = 0x400;
        D_800FCEF0[i] = 2;

        bzero(D_800FCF28[i], 0x1140);
        bzero(D_800FCEE0[i], 0x1400);

        AudioStream_OpenChannel(D_800FCED8[i], 0, 0);
        D_800FCF20[i] = 0;
    }

    D_800FCF24 = osCartRomInit();
    osCreateMesgQueue(&D_800FCF40, &D_800FCF58, 1);
    D_800FCED0 = 0;
    D_800785F0 = 0;
}

void AudioStream_Start(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    if (PORT_AUDIO_OFF()) {
        return;
    }
    u32 temp_a0;

    if ((arg0 < 2) && (D_800785F0 == 0)) {
        if (arg2 >= 0x81) {
            arg2 = 0x80;
        }

        temp_a0 = osSetIntMask(1);

        if (arg3 == 0) {
            D_800FCEF0[arg0] = 0;
        } else {
            D_800FCEF0[arg0] = 3;
        }

        if (getenv("PORT_TRACE_SFX")) {
            extern unsigned long long Port_ViTick(void);
            fprintf(stderr, "[stream] frame %llu start %u rom 0x%x vol %u delay %u\n", Port_ViTick(), (unsigned)arg0,
                    (unsigned)arg1, (unsigned)arg2, (unsigned)arg3);
        }
        D_800FCEF8[arg0] = arg1;
        D_800FCF10[arg0] = arg3;
        D_800FCF08[arg0] = arg2;

        osSetIntMask(temp_a0);
    }
}

void AudioStream_Stop(u32 arg0, u32 arg1) {
    if (PORT_AUDIO_OFF()) {
        return;
    }
    u32 temp_a0;

    if (arg0 < 2) {
        if (arg1 != 0) {
            D_800FCF1C[arg0] = (f32)D_800FCF00[arg0] / arg1;
        }
        temp_a0 = osSetIntMask(1);
        if (arg1 != 0) {
            D_800FCEF0[arg0] = 5;
        } else {
            D_800FCEF0[arg0] = 1;
        }
        osSetIntMask(temp_a0);
    }
}

s32 AudioStream_IsPlaying(u32 arg0) {
    if (PORT_AUDIO_OFF()) {
        return 0;
    }
    if (D_800FCEF0[arg0] == 2) {
        return 0;
    }
    return 1;
}

void AudioStream_SetEnabled(s32 arg0) {
    if (PORT_AUDIO_OFF()) {
        return;
    }
    if (arg0 != 0) {
        D_800785F0 = 0;
    } else {
        D_800785F0 = 1;
    }
}

void func_80044EA4(void) {
    if (PORT_AUDIO_OFF()) {
        return;
    }
    UNUSED s32 pad;
    u32 j;
    u32 var_s7;
    s32 i;
    u32 temp_a2;
    s32 temp_v0_9;
    s16 sample;

    for (i = 0; i < 2; i++) {
        if (D_800FCEF0[i] == 2U) {
            continue;
        }
        {
            if (D_800FCF30[i] < D_800FCF38[i]) {
                if ((D_800FCF38[i] - D_800FCF30[i]) < D_800FCEEC) {
                    var_s7 = D_800FCEEC;
                } else {
                    var_s7 = D_800FCEE8;
                }
            } else if ((0x8A0 - D_800FCEEC) < (D_800FCF30[i] - D_800FCF38[i])) {
                var_s7 = D_800FCEEC;
            } else {
                var_s7 = D_800FCEE8;
            }

            AudioStream_RefillAndDecode(D_800FCED8[i]);
            if ((u8)D_800FCED8[i]->unk_25D1 == 2) {
                osWritebackDCache(D_800FCF28[i], 0x1140);
                AudioStream_MarkLoaded(D_800FCED8[i]);
                D_800FCED0 = 1;
                D_800FCF20[i] = 0;
            } else if ((D_800FCEF0[i] != 0) && (D_800FCEF0[i] != 3) && ((u8)D_800FCED8[i]->unk_25D1 == 4)) {
                if (D_800FCF20[i] == 0) {
                    AudioStream_Abort(D_800FCED8[i]);
                    D_800FCF20[i] = 1;
                }
                D_800FCED0 = 0;
                D_800FCEF0[i] = 2;
                bzero(D_800FCEE0[i], 0x1400);
                bzero(D_800FCF28[i], 0x1140);
            }

            AudioStream_ConsumeSamples(D_800FCED8[i], var_s7);

            temp_a2 = D_800FCED8[i]->unk_25C8 - var_s7;
            for (j = 0; j < var_s7; j++) {
                sample = (D_800FCEE0[i][(temp_a2 + j) % 2560] * D_800FCF00[i]) >> 7;
                D_800FCF28[i]->unk_000[(D_800FCF38[i] + (j << 1) + 1) % 2208] = sample;
                D_800FCF28[i]->unk_000[(D_800FCF38[i] + (j << 1)) % 2208] = sample;
            }

            D_800FCF38[i] += var_s7 * 2;
            D_800FCF38[i] = (u32)D_800FCF38[i] % 2208;
            osWritebackDCache(D_800FCF28[i], 0x1140);

            switch (D_800FCEF0[i]) {
                case 2:
                case 4:
                    break;

                case 0:
                    AudioStream_OpenChannel(D_800FCED8[i], D_800FCEF8[i], AudioStream_DmaReadChunk);
                    D_800FCEF0[i] = 4;
                    if (D_800FCED0 != 0) {
                        D_800FCED0 = 0;
                    }
                    D_800FCF00[i] = D_800FCF08[i];
                    break;

                case 1:
                    AudioStream_Abort(D_800FCED8[i]);
                    bzero(D_800FCEE0[i], 0x1400);
                    D_800FCF20[i] = 1;
                    break;

                case 3:
                    if (--D_800FCF10[i] == 0) {
                        D_800FCEF0[i] = 0;
                    }
                    break;

                case 5:
                    temp_v0_9 = D_800FCF00[i] - D_800FCF1C[i];
                    if (temp_v0_9 <= 0) {
                        D_800FCF00[i] = 0;
                        D_800FCEF0[i] = 1;
                    } else {
                        D_800FCF00[i] = temp_v0_9;
                    }
                    break;
            }
        }
    }
}


void AudioStream_DmaReadChunk(uintptr_t arg0, uintptr_t arg1, u32 arg2, OSMesgQueue* arg3) { // PORT: arg1 was u32
    if (PORT_AUDIO_OFF()) {
        return;
    }
    OSIoMesg sp48;
    u32 var_s1;
    u32 var_s2;
    uintptr_t var_s3; // PORT: a host pointer (was u32)

    var_s2 = arg0;
    var_s3 = arg1;

    while (arg2) {
        if (arg2 >= 0x1001) {
            var_s1 = 0x1000;
        } else {
            var_s1 = arg2;
        }

        sp48.hdr.pri = 0;
        sp48.hdr.retQueue = &D_800FCF40;
        sp48.dramAddr = var_s3;
        sp48.devAddr = var_s2;
        sp48.size = var_s1;

        osInvalDCache(arg1, arg2);
        osEPiStartDma(D_800FCF24, &sp48, 0);
        osRecvMesg(&D_800FCF40, NULL, 1);

        arg2 -= var_s1;
        var_s2 += var_s1;
        var_s3 += var_s1;
    }

    osSendMesg(arg3, NULL, 1);
}

void AudioStream_OpenChannel(unk_D_800FCED8* arg0, s32 arg1, unk_D_800FCED8_190 arg2) { // PORT: arg2 was s32
    if (PORT_AUDIO_OFF()) {
        return;
    }
    bzero(&arg0->unk_0000, sizeof(unk_D_800FCED8_0000));

    arg0->unk_0000.gsm.nrp = 0x28;
    arg0->unk_0190 = arg2;
    arg0->unk_0194 = arg1;

    osCreateMesgQueue(&arg0->unk_25A0, &arg0->unk_25B8, 1);

    if ((arg1 == 0) || (arg2 == 0)) {
        arg0->unk_25D1 = 0;
    } else {
        arg0->unk_25D1 = 1;
    }

    arg0->unk_25D0 = 0;
    arg0->unk_2598 = arg0->unk_259C = arg0->unk_259A = 0;
    arg0->unk_25BC = 0;
    arg0->unk_25C4 = 0;
    arg0->unk_25C0 = 0;
    arg0->unk_25CC = 0;
    arg0->unk_25C8 = 0;
    arg0->unk_25D2 = 2;
}

void AudioStream_RefillAndDecode(unk_D_800FCED8* arg0) {
    if (PORT_AUDIO_OFF()) {
        return;
    }
    s32 var_a2;
    s32 var_s2;
    u32 var_v1;

    var_s2 = arg0->unk_25D2;

    if ((arg0->unk_25D0 != 0) && (osRecvMesg(&arg0->unk_25A0, NULL, 0) >= 0)) {
        if (arg0->unk_25C4 == 0) {
            // PORT: the stream header is big-endian
            arg0->unk_2598 = (PortA_BE32(&arg0->unk_1598[0].unk_04) >> 0x10) & 0xFFFF;
            arg0->unk_259A = PortA_BE32(&arg0->unk_1598[0].unk_04) & 0xFFFF;
            arg0->unk_259C = PortA_BE32(&arg0->unk_15A0) & 0xFFFFFF;
            arg0->unk_25BC = 0x60;
        }

        arg0->unk_25C4 += 0x400;
        if (arg0->unk_259C >= 0x100) {
            arg0->unk_259C -= 0x100;
        } else {
            arg0->unk_259C = 0;
        }
        arg0->unk_25D0 = 0;
    }

    if ((arg0->unk_25D0 == 0) && ((arg0->unk_25D1 == 1) || (arg0->unk_25D1 == 2) || (arg0->unk_25D1 == 3))) {
        if ((arg0->unk_25C4 - arg0->unk_25C0) < 0xC01) {
            if (arg0->unk_25C4 == 0) {
                var_a2 = 0x400;
            } else if (arg0->unk_259C >= 0x101) {
                var_a2 = 0x400;
            } else {
                var_a2 = arg0->unk_259C * 4;
            }

            if (var_a2 > 0) {
                arg0->unk_25D0 = 1;
                arg0->unk_0190(arg0->unk_0194 + arg0->unk_25C4, &arg0->unk_1598[(arg0->unk_25C4 & 0xFFF) >> 3], var_a2,
                               &arg0->unk_25A0);
            }
        }
    }

    var_v1 = arg0->unk_25CC - arg0->unk_25C8;
    while ((var_v1 < 0x960) && ((arg0->unk_25D1 == 1) || (arg0->unk_2598 > 0)) &&
           ((arg0->unk_25C4 - arg0->unk_25C0) >= 0x25) && (var_s2 > 0)) {
        Gsm_Decode_Stream(arg0, arg0->unk_0198.unk_0000 + (((arg0->unk_25CC % 2560) >> 2) * 8));
        arg0->unk_25C0 = (arg0->unk_25BC >> 3) & ~3;
        arg0->unk_25CC += 0xA0;
        arg0->unk_2598--;
        var_v1 = arg0->unk_25CC - arg0->unk_25C8;
        var_s2--;
    }

    if ((arg0->unk_25D1 == 1) && (var_v1 >= 0x8C0)) {
        arg0->unk_25D1 = 2;
    }
}

u32 AudioStream_ConsumeSamples(unk_D_800FCED8* arg0, u32 arg1) {
    if (PORT_AUDIO_OFF()) {
        return 0;
    }
    u32 temp_v0;
    u32 var_a2;

    if (arg0->unk_25D1 == 3) {
        temp_v0 = arg0->unk_25CC - arg0->unk_25C8;
        if (arg1 < temp_v0) {
            var_a2 = arg1;
        } else {
            var_a2 = temp_v0;
            if (arg0->unk_2598 <= 0) {
                arg0->unk_25D1 = 4;
            }
        }
        arg0->unk_25C8 += var_a2;
        return var_a2;
    }

    return 0;
}

void AudioStream_MarkLoaded(unk_D_800FCED8* arg0) {
    if (PORT_AUDIO_OFF()) {
        return;
    }
    if (arg0->unk_25D1 == 2) {
        arg0->unk_25D1 = 3;
    }
}

void AudioStream_Abort(unk_D_800FCED8* arg0) {
    if (PORT_AUDIO_OFF()) {
        return;
    }
    arg0->unk_25D1 = 4;
}
