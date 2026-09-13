#include "audio_io.h"
#include "src/3D140.h"
#include "src/libnaudio/n_synsetpriority.h"

typedef u8* (*VoiceCommandHandler)(AudioVoice*, u8*);
typedef u8* (*AudioCommandHandler)(u8*);

static s32 D_80077C90 = -1;
static u8 D_80077C94 = 0;
static VoiceCommandHandler gVoiceCommandHandlers[] = {
    Audio_ClearVoicePlaybackState, Audio_SetVoiceSampleIndex, Audio_EnableVoicePitchSlide, Audio_DisableVoicePitchSlide, Audio_SetVoiceVolumeEnvelope, Audio_SetVoiceSequenceTempo, Audio_SetVoiceFixedDuration,
    Audio_SetVoiceReleaseDuration, Audio_EnableVoiceVibrato, Audio_EnableVoiceVibratoDown, Audio_DisableVoiceVibrato, Audio_SetVoiceDurationOverride, Audio_EnableVoiceVariableDuration, Audio_SetVoiceDetune,
    Audio_ApplyVoiceDetuneOnce, Audio_SetVoiceFinePitch, Audio_SetVoiceEnvelopeFromTable, Audio_SetVoiceFlagED, Audio_ClearVoiceFlagED, Audio_SetVoiceFlagEE, Audio_ClearVoiceFlagEE,
    Audio_PushVoiceLoopState, Audio_PopVoiceLoopState, Audio_SetVoicePeriodicPitchModulation, Audio_DisableVoicePeriodicPitchModulation, Audio_EnableVoiceVelocityFromStream, Audio_DisableVoiceVelocityFromStream, Audio_SetVoiceDefaultVelocity,
    Audio_SetVoicePan, Audio_SkipVoiceCommandWord, Audio_SetVoiceSubEnvelopePointer, Audio_ClearVoiceSubEnvelopePointer, Audio_SkipVoiceCommandByte, Audio_SetVoiceEnvelopePointers, Audio_SetVoiceFxMix,
    Audio_SetRandomVoiceDetune, Audio_SetRandomVoiceVolume, Audio_SetRandomVoicePan, Audio_SetVoiceVolume, Audio_AllocateSubVoiceFromCommand, Audio_SetVoiceVolumeScale, Audio_SetVoicePanModulation,
    NULL,          NULL,          NULL,          NULL,          NULL,          NULL,          Audio_AllocateVoiceFromCommand,
};
static AudioCommandHandler gAudioCommandHandlers[] = {
    Audio_PlayMusicSequenceHandler, Audio_PlaySoundEffectHandler, Audio_ApplyVoiceDetuneCommand, Audio_ApplyVoiceVolumeCommand, Audio_ApplyVoiceVolumeScaleCommand, Audio_ApplyVoicePanCommand, Audio_ApplyVoiceFxMixCommand,
    Audio_FadeSoundHandler, Audio_FadeCategoryHandler, Audio_SetChannelVolumeHandler, Audio_SetMonoOutputHandler, Audio_PlayMusicSequenceLoopedHandler, NULL,
};

static ALPlayer D_800FC7A0;
static ALHeap D_800FC7B8;
static s32 pad_D_800FC7C8;
static u32 D_800FC7CC;
static AudioVoice* D_800FC7D0;
static u32 D_800FC7D4;
static u32 D_800FC7D8;
static u16 D_800FC7DC;
static u16 D_800FC7DE;
static u32 D_800FC7E0;
static s32 D_800FC7E4;
static u8* gAudioCommandQueue;
static s32 gAudioCommandQueueLength;
static u32 D_800FC7F0;
static u8 D_800FC7F4;
static u32 D_800FC7F8;
static AudioHardwareVoice* D_800FC7FC;
static AudioHardwareVoice* D_800FC800;
static AudioHardwareVoice* D_800FC804;
static AudioHardwareVoice* D_800FC808;
static AudioHardwareVoice* D_800FC80C;
ALHeap* D_800FC810;
u32 D_800FC814;
s32 D_800FC818;
s32 D_800FC81C;

u8* Audio_ClearVoicePlaybackState(AudioVoice* arg0, UNUSED u8* arg1) {
    arg0->subEnvelopeCursorA = 0;
    arg0->subEnvelopeCursorB = 0;
    arg0->instrumentBank = 0;
    arg0->independentTempo = 0;
    arg0->soundId = 0;
    return NULL;
}

u8* Audio_SetVoiceSampleIndex(AudioVoice* arg0, u8* arg1) {
    u16 var_v1 = *arg1++;

    if (var_v1 & 0x80) {
        var_v1 = (var_v1 & 0x7F) << 8;
        var_v1 |= *arg1++;
    }

    arg0->sampleIndex = var_v1;
    return arg1;
}

u8* Audio_EnableVoicePitchSlide(AudioVoice* arg0, u8* arg1) {
    arg0->portamentoDuration = *arg1++;
    if (arg0->portamentoDuration) {
        arg0->currentPitch = arg0->basePitch;
    }
    return arg1;
}

u8* Audio_DisableVoicePitchSlide(AudioVoice* arg0, u8* arg1) {
    arg0->portamentoDuration = 0;
    return arg1;
}

u8* Audio_SetVoiceVolumeEnvelope(AudioVoice* arg0, u8* arg1) {
    u8 var_v0;

    var_v0 = *arg1++;
    if (var_v0 == 0) {
        var_v0 = 1;
    }
    arg0->attackRate = var_v0;
    arg0->attackIncrement = 0x400 / var_v0;
    arg0->attackStartLevel = 0;

    arg1++;

    var_v0 = *arg1++;
    if (var_v0 == 0) {
        var_v0 = 1;
    }
    arg0->decayDuration = var_v0;
    arg0->decayLevel = *arg1++;
    arg0->decayRate = (1.0 / (f32)var_v0) * (f32)(arg0->decayLevel - arg0->attackStartLevel);

    var_v0 = *arg1++;
    if (var_v0 == 0) {
        var_v0 = 1;
    }
    arg0->sustainDuration = var_v0;
    arg0->sustainLevel = *arg1++;
    arg0->sustainRate = (1.0f / var_v0) * ((f32)arg0->sustainLevel - arg0->decayLevel);

    var_v0 = *arg1++;
    if (var_v0 == 0) {
        var_v0 = 1;
    }
    arg0->releaseRate = var_v0;

    return arg1;
}

u8* Audio_SetVoiceSequenceTempo(AudioVoice* arg0, u8* arg1) {
    s32 i;
    s32 temp_lo;
    s32 temp2;
    AudioVoice* var_a3;

    temp_lo = ((*arg1++ * 0x6000) / 120) / D_800FC7D4;
    temp2 = (arg0->tempoScale * temp_lo) >> 7;

    if (arg0->independentTempo != 0) {
        arg0->tempoIncrement = temp_lo;
    } else {
        for (i = 0, var_a3 = D_800FC7D0; i < D_800FC7CC; i++, var_a3++) {
            if (var_a3->instrumentBank == arg0->instrumentBank) {
                var_a3->tempoTicksRaw = temp_lo;
                var_a3->tempoIncrement = temp2;
            }
        }
    }

    return arg1;
}

u8* Audio_SetVoiceReleaseDuration(AudioVoice* arg0, u8* arg1) {
    arg0->releaseDuration = *arg1++;
    arg0->fixedDuration = 0;
    return arg1;
}

u8* Audio_SetVoiceFixedDuration(AudioVoice* arg0, u8* arg1) {
    s16 tmp = *arg1++ << 8;

    tmp |= *arg1++;

    arg0->fixedDuration = tmp;
    arg0->releaseDuration = 0;

    return arg1;
}

u8* Audio_EnableVoiceVibrato(AudioVoice* arg0, u8* arg1) {
    arg0->vibratoRate = *arg1++;
    arg0->vibratoDepthRaw = *arg1++;
    arg0->vibratoDepth = (f32)*arg1++ / 50.0;
    return arg1;
}

u8* Audio_EnableVoiceVibratoDown(AudioVoice* arg0, u8* arg1) {
    arg0->vibratoRate = *arg1++;
    arg0->vibratoDepthRaw = *arg1++;
    arg0->vibratoDepth = -(f32)*arg1++ / 50.0;

    return arg1;
}

u8* Audio_DisableVoiceVibrato(AudioVoice* arg0, u8* arg1) {
    arg0->vibratoDepthRaw = 0;
    arg0->vibratoOutput = 0.0f;
    return arg1;
}

u8* Audio_SetVoiceDurationOverride(AudioVoice* arg0, u8* arg1) {
    u8 temp_v0 = *arg1++;

    if (temp_v0 < 0x80) {
        arg0->durationOverride = temp_v0;
    } else {
        arg0->durationOverride = (temp_v0 & 0x7F) << 8;
        arg0->durationOverride += *arg1++;
    }
    return arg1;
}

u8* Audio_EnableVoiceVariableDuration(AudioVoice* arg0, u8* arg1) {
    arg0->variableDurationEnabled = 1;
    return arg1;
}

u8* Audio_SetVoiceDetune(AudioVoice* arg0, u8* arg1) {
    arg0->detune = *arg1++;
    return arg1;
}

u8* Audio_ApplyVoiceDetuneOnce(AudioVoice* arg0, u8* arg1) {
    arg0->detuneOnce = 1;
    return arg1;
}

u8* Audio_SetVoiceFinePitch(AudioVoice* arg0, u8* arg1) {
    s32 var_v1 = *arg1++;

    if (var_v1 & 0x80) {
        var_v1 |= ~0xFF;
    }
    arg0->finePitchAccumulator -= arg0->finePitchOffset;
    arg0->finePitchOffset = (f32)var_v1 / 100.0;
    arg0->finePitchAccumulator += arg0->finePitchOffset;
    return arg1;
}

u8* Audio_SetVoiceEnvelopeFromTable(AudioVoice* arg0, u8* arg1) {
    s32 var_v0 = *arg1++;

    if (var_v0 & 0x80) {
        var_v0 = (var_v0 & 0x7F) << 8;
        var_v0 |= *arg1++;
    }

    Audio_SetVoiceVolumeEnvelope(arg0, (u8*)&arg0->instrumentBank->volumeEnvelopeTable[var_v0]);
    return arg1;
}

u8* Audio_SetVoiceFlagED(AudioVoice* arg0, u8* arg1) {
    arg0->unk_0ED = 1;
    return arg1;
}

u8* Audio_ClearVoiceFlagED(AudioVoice* arg0, u8* arg1) {
    arg0->unk_0ED = 0;
    return arg1;
}

u8* Audio_SetVoiceFlagEE(AudioVoice* arg0, u8* arg1) {
    arg0->unk_0EE = 1;
    return arg1;
}

u8* Audio_ClearVoiceFlagEE(AudioVoice* arg0, u8* arg1) {
    arg0->unk_0EE = 0;
    return arg1;
}

u8* Audio_PushVoiceLoopState(AudioVoice* arg0, u8* arg1) {
    u8 temp_v1 = arg0->loopStackDepth;

    arg0->loopCounter[temp_v1] = *arg1++;

    arg0->loopReturnPtr[temp_v1] = arg1;
    arg0->loopSavedEnvPtrA[temp_v1] = arg0->subEnvelopeCursorA;
    arg0->loopSavedEnvPtrB[temp_v1] = arg0->subEnvelopeCursorB;

    arg0->loopSavedVolume[temp_v1] = arg0->volume;
    arg0->loopSavedPitchScale[temp_v1] = arg0->pitchModulationRaw;

    arg0->loopSavedEnvPtrAValid[temp_v1] = arg0->subEnvelopeCounterA;
    arg0->loopSavedEnvPtrBValid[temp_v1] = arg0->subEnvelopeCounterB;

    arg0->loopStackDepth++;
    return arg1;
}

u8* Audio_PopVoiceLoopState(AudioVoice* arg0, u8* arg1) {
    s32 var_v0 = arg0->loopStackDepth - 1;

    if (arg0->loopCounter[var_v0] != 0xFF) {
        arg0->loopCounter[var_v0]--;
        if (!arg0->loopCounter[var_v0]) {
            arg0->loopStackDepth = var_v0;
            var_v0 = -1;
        }
    }

    if (var_v0 >= 0) {
        arg1 = arg0->loopReturnPtr[var_v0];

        arg0->subEnvelopeCursorA = arg0->loopSavedEnvPtrA[var_v0];
        arg0->subEnvelopeCursorB = arg0->loopSavedEnvPtrB[var_v0];
        arg0->volume = arg0->loopSavedVolume[var_v0];

        arg0->pitchModulationRaw = arg0->loopSavedPitchScale[var_v0];
        arg0->subEnvelopeCounterA = arg0->loopSavedEnvPtrAValid[var_v0];
        arg0->subEnvelopeCounterB = arg0->loopSavedEnvPtrBValid[var_v0];
        arg0->pitchModulationScale = 0.03125f;
        arg0->pitchModulation = arg0->pitchModulationRaw * arg0->pitchModulationScale;
    }

    return arg1;
}

u8* Audio_SetVoicePeriodicPitchModulation(AudioVoice* arg0, u8* arg1) {
    arg0->periodicPitchModDepth = *arg1++;
    arg0->periodicPitchModDuration = *arg1++;
    arg0->periodicPitchModDurationOff = *arg1++;
    return arg1;
}

u8* Audio_DisableVoicePeriodicPitchModulation(AudioVoice* arg0, u8* arg1) {
    arg0->periodicPitchModDuration = 0;
    return arg1;
}

u8* Audio_EnableVoiceVelocityFromStream(AudioVoice* arg0, u8* arg1) {
    arg0->velocityFromStreamEnabled = 1;
    return arg1;
}

u8* Audio_DisableVoiceVelocityFromStream(AudioVoice* arg0, u8* arg1) {
    arg0->velocityFromStreamEnabled = 0;
    return arg1;
}

u8* Audio_SetVoiceDefaultVelocity(AudioVoice* arg0, u8* arg1) {
    arg0->defaultVelocity = *arg1++;
    arg0->velocityFromStreamEnabled = 0;
    return arg1;
}

u8* Audio_SetVoicePan(AudioVoice* arg0, u8* arg1) {
    arg0->pan = *arg1++ / 2;
    return arg1;
}

u8* Audio_SkipVoiceCommandWord(UNUSED AudioVoice* arg0, u8* arg1) {
    arg1++;
    arg1++;
    return arg1;
}

u8* Audio_SetVoiceSubEnvelopePointer(AudioVoice* arg0, u8* arg1) {
    u8* ptr = (u8*)&arg0->instrumentBank->unk_00 + arg0->instrumentBank->subEnvelopePointerOffsets[*arg1++];

    arg0->subEnvelopePointer = ptr;
    return arg1;
}

u8* Audio_ClearVoiceSubEnvelopePointer(AudioVoice* arg0, u8* arg1) {
    arg0->subEnvelopePointer = NULL;
    return arg1;
}

u8* Audio_SkipVoiceCommandByte(UNUSED AudioVoice* arg0, u8* arg1) {
    arg1++;
    return arg1;
}

u8* Audio_SetVoiceEnvelopePointers(AudioVoice* arg0, u8* arg1) {
    s32 tmp0;
    s32 tmp2;

    tmp0 = *arg1++ << 8;
    tmp0 += *arg1++;

    tmp2 = *arg1++ << 8;
    tmp2 += *arg1++;

    arg0->subEnvelopeCursorA = &arg0->subEnvelopeBaseA[tmp2];
    arg0->subEnvelopeCounterA = 1;

    tmp2 = *arg1++ << 8;
    tmp2 += *arg1++;

    arg0->subEnvelopeCursorB = &arg0->subEnvelopeBaseB[tmp2];
    arg0->subEnvelopeCounterB = 1;

    return &arg0->commandStreamBase[tmp0];
}

u8* Audio_SetVoiceFxMix(AudioVoice* arg0, u8* arg1) {
    arg0->fxMix = *arg1++;
    return arg1;
}

u8* Audio_SetRandomVoiceDetune(AudioVoice* arg0, u8* arg1) {
    arg0->detune = Audio_RandomScale(*arg1++);
    arg0->detune += *arg1++;
    return arg1;
}

u8* Audio_SetRandomVoiceVolume(AudioVoice* arg0, u8* arg1) {
    arg0->volume = Audio_RandomScale(*arg1++);
    arg0->volume += *arg1++;
    return arg1;
}

u8* Audio_SetRandomVoicePan(AudioVoice* arg0, u8* arg1) {
    arg0->pan = Audio_RandomScale(*arg1++);
    arg0->pan += *arg1++;
    return arg1;
}

u8* Audio_SetVoiceVolume(AudioVoice* arg0, u8* arg1) {
    arg0->volume = *arg1++;
    return arg1;
}

u8* Audio_AllocateSubVoiceFromCommand(AudioVoice* arg0, u8* arg1) {
    s32 i;
    s32 var_s1;
    AudioVoice* var_a0;
    u32* sp30;

    var_s1 = *arg1++;
    if (var_s1 >= 0x80) {
        var_s1 = *arg1++ + ((var_s1 & 0x7F) << 8);
    }

    sp30 = arg0->subVoiceContext->subVoiceTable;
    for (i = 0, var_a0 = D_800FC7D0; i < D_800FC7CC; i++, var_a0++) {
        if (var_a0->commandStreamCursor == NULL) {
            Audio_InitializeVoice(var_a0, var_s1, arg0->volumeScale, arg0->currentPan, arg0->priority);

            var_a0->soundId = arg0->soundId;
            var_a0->subVoiceInstrumentBank = arg0->subVoiceInstrumentBank;
            var_a0->subVoiceContext = arg0->subVoiceContext;
            var_a0->fadeCounter = arg0->fadeCounter;
            var_a0->fadeDuration = arg0->fadeDuration;

            var_a0->commandStreamCursor = var_a0->commandStreamBase = sp30[var_s1];
            break;
        }
    }

    return arg1;
}

u8* Audio_SetVoiceVolumeScale(AudioVoice* arg0, u8* arg1) {
    arg0->pitchModulationScale = (f32)*arg1++ * 0.015625;
    arg0->pitchModulation = SQ(arg0->pitchModulationRaw);
    return arg1;
}

u8* Audio_SetVoicePanModulation(AudioVoice* arg0, u8* arg1) {
    arg0->panModulationRate = *arg1++;
    if (!arg0->panModulationRate) {
        arg0->currentPan = 0x80;
    }
    return arg1;
}

u8* Audio_AllocateVoiceFromCommand(AudioVoice* arg0, u8* arg1) {
    s32 i;
    s32 var_s1;
    AudioVoice* var_a0;
    s32* sp30;

    var_s1 = *arg1++;
    if (var_s1 >= 0x80) {
        var_s1 = *arg1++ + ((var_s1 & 0x7F) << 8);
    }

    sp30 = arg0->subVoiceContext->subVoiceTable;
    for (i = 0, var_a0 = D_800FC7D0; i < D_800FC7CC; i++, var_a0++) {
        if (var_a0->commandStreamCursor == NULL) {
            Audio_InitializeVoice(var_a0, var_s1, arg0->volumeScale, arg0->currentPan, arg0->priority);
            var_a0->soundId = arg0->soundId;
            var_a0->subVoiceInstrumentBank = arg0->subVoiceInstrumentBank;
            var_a0->subVoiceContext = arg0->subVoiceContext;
            var_a0->fadeCounter = arg0->fadeCounter;
            var_a0->fadeDuration = arg0->fadeDuration;
            var_a0->commandStreamCursor = var_a0->commandStreamBase = sp30[var_s1];
            break;
        }
    }

    return arg1;
}

void Audio_WriteU32BE(u8* arg0, u32 arg1) {
    *arg0++ = (arg1 >> 0x18) & 0xFF;
    *arg0++ = (arg1 >> 0x10) & 0xFF;
    *arg0++ = (arg1 >> 8) & 0xFF;
    *arg0++ = (arg1 >> 0) & 0xFF;
}

void Audio_WriteU16BE(u8* arg0, u32 arg1) {
    *arg0++ = (arg1 >> 8u) & 0xFF;
    *arg0++ = arg1 & 0xFF;
}

s32 Audio_InitializeVoiceManager(sp38_func_800373D8* arg0) {
    UNUSED s32 pad[1];
    s32 i;
    ALSynConfig sp4C;
    amConfig sp40;

    sp4C.outputRate = osAiSetFrequency(arg0->outputRate);
    D_800FC7CC = arg0->voiceCount;
    if (osTvType == OS_TV_PAL) {
        D_800FC7D4 = 50;
    } else {
        D_800FC7D4 = 60;
    }
    D_800FC7D8 = 1000000 / D_800FC7D4;

    Audio_FillBytes(arg0->heapBase, 0, arg0->heapSize);
    alHeapInit(&D_800FC7B8, arg0->heapBase, arg0->heapSize);
    D_800FC810 = &D_800FC7B8;
    D_800FC7D0 = alHeapDBAlloc(NULL, 0, &D_800FC7B8, 1, D_800FC7CC * sizeof(AudioVoice));
    Audio_FillBytes((u8*)D_800FC7D0, 0, D_800FC7CC * sizeof(AudioVoice));

    gAudioCommandQueue = alHeapDBAlloc(NULL, 0, &D_800FC7B8, 1, arg0->commandQueueSize);
    gAudioCommandQueueLength = 0;
    D_800FC7F0 = arg0->commandQueueSize;

    D_800FC7FC = alHeapDBAlloc(NULL, 0, &D_800FC7B8, arg0->virtualVoiceCount, sizeof(AudioHardwareVoice));
    D_800FC800 = D_800FC804 = NULL;
    D_800FC808 = D_800FC80C = NULL;
    D_800FC818 = 0;
    D_800FC81C = 0;

    for (i = 0; i < arg0->virtualVoiceCount; i++) {
        Audio_QueueFreeVoice(D_800FC7FC++);
    }

    sp4C.maxVVoices = arg0->virtualVoiceCount;
    sp4C.maxPVoices = arg0->physicalVoiceCount;
    sp4C.maxUpdates = arg0->maxUpdates;
    sp4C.dmaproc = NULL;
    sp4C.fxType = AL_FX_BIGROOM;
    sp4C.heap = &D_800FC7B8;

    sp40.outputRate = arg0->outputRate;
    sp40.framesPerField = arg0->framesPerField;
    sp40.maxACMDSize = arg0->maxAcmdSize;

    amCreateAudioMgr(&sp4C, &sp40, arg0->numDmaBuffers, arg0->unk_28, D_800FC7D4);

    D_800FC814 = arg0->virtualVoiceCount;
    D_800FC7E0 = 1;
    D_800FC7F8 = 0;
    D_800FC7DE = 0x7FFF;
    D_800FC7DC = 0x7FFF;

    D_80077C90 = -1;
    D_80077DE4 = 0;
    D_80077C94 = 1;

    D_800FC7A0.next = NULL;
    D_800FC7A0.handler = Audio_PlayerFrameHandler;
    D_800FC7A0.clientData = &D_800FC7A0;

    n_alSynAddPlayer(&D_800FC7A0);

    for (i = 0; i < D_800FC7CC; i++) {
        D_800FC7D0[i].voiceActive = 0;
        D_800FC7D0[i].commandStreamCursor = 0;
        Audio_ResetVoiceState(&D_800FC7D0[i]);
    }

    return D_800FC7B8.cur - D_800FC7B8.base;
}

void Audio_QueueSetVolumeCommand(s32 arg0, u32 arg1) {
    u8* temp_a0;
    u32 sp18 = osSetIntMask(1);

    temp_a0 = gAudioCommandQueue;
    temp_a0 += gAudioCommandQueueLength;

    *temp_a0++ = 9;
    *temp_a0++ = arg0;
    Audio_WriteU16BE(temp_a0, arg1);
    temp_a0 += 2;

    gAudioCommandQueueLength += 4;
    if (gAudioCommandQueueLength >= D_800FC7F0) {
        gAudioCommandQueueLength -= 4;
    }

    osSetIntMask(sp18);
}

s32 Audio_PlayMusicSequence(SoundBank* arg0, s32 arg1) {
    u8* temp_a0;
    u32 sp20;
    UNUSED s32 pad;
    u32 sp18;

    if (arg0 == 0) {
        return 0;
    }

    if (!(arg0->flags & 1)) {
        return 0;
    }

    sp18 = osSetIntMask(1);

    temp_a0 = gAudioCommandQueue;
    temp_a0 += gAudioCommandQueueLength;

    sp20 = D_800FC7E0;
    D_800FC7E0++;

    *temp_a0++ = 0;
    Audio_WriteU32BE(temp_a0, arg0);
    temp_a0 += 4;
    Audio_WriteU32BE(temp_a0, arg1);
    temp_a0 += 4;
    Audio_WriteU32BE(temp_a0, sp20);
    temp_a0 += 4;

    gAudioCommandQueueLength += 0xD;
    if (gAudioCommandQueueLength >= D_800FC7F0) {
        gAudioCommandQueueLength -= 0xD;
    }

    osSetIntMask(sp18);
    return sp20;
}

s32 Audio_PlaySoundEffect(SoundBank* arg0, s32 arg1, s32 arg2, s32 arg3, u32 arg4, s32 arg5) {
    u32 sp2C;
    UNUSED s32 pad[2];
    u32 sp20;
    u8* temp_s0;

    if (D_80077DE4 != 0) {
        return 0;
    }

    if (arg0 == NULL) {
        return 0;
    }

    if (!(arg0->flags & 1)) {
        return 0;
    }

    sp20 = osSetIntMask(1);

    temp_s0 = gAudioCommandQueue;
    temp_s0 += gAudioCommandQueueLength;

    sp2C = D_800FC7E0;
    D_800FC7E0++;

    *temp_s0++ = 1;
    Audio_WriteU32BE(temp_s0, arg0);
    temp_s0 += 4;
    Audio_WriteU32BE(temp_s0, arg1);
    temp_s0 += 4;
    Audio_WriteU32BE(temp_s0, sp2C);
    temp_s0 += 4;
    Audio_WriteU32BE(temp_s0, arg2);
    temp_s0 += 4;

    *temp_s0++ = arg3;
    *temp_s0++ = arg4;
    *temp_s0++ = arg5;

    gAudioCommandQueueLength += 0x14;
    if (gAudioCommandQueueLength >= D_800FC7F0) {
        gAudioCommandQueueLength -= 0x14;
    }

    osSetIntMask(sp20);
    return sp2C;
}

void Audio_QueueFadeCategoryCommand(s32 arg0, u32 arg1) {
    u8* temp_a0;
    u32 sp18 = osSetIntMask(1);

    temp_a0 = gAudioCommandQueue;
    temp_a0 += gAudioCommandQueueLength;

    *temp_a0++ = 8;
    *temp_a0++ = arg0;
    Audio_WriteU16BE(temp_a0, arg1);
    temp_a0 += 2;

    gAudioCommandQueueLength += 4;
    if (gAudioCommandQueueLength >= D_800FC7F0) {
        gAudioCommandQueueLength -= 4;
    }

    osSetIntMask(sp18);
}

s32 Audio_CountActiveSoundsByCategory(s32 arg0) {
    s32 i;
    s32 sp20;
    AudioVoice* var_v1;
    u32 sp18;

    sp18 = osSetIntMask(1);

    for (i = 0, sp20 = 0, var_v1 = D_800FC7D0; i < D_800FC7CC; i++, var_v1++) {
        if (var_v1->commandStreamCursor != NULL) {
            if (((var_v1->independentTempo != 0) && (arg0 & 1)) || ((var_v1->independentTempo == 0) && (arg0 & 2))) {
                sp20++;
            }
        }
    }

    osSetIntMask(sp18);
    return sp20;
}

void Audio_QueueFadeSoundCommand(s32 arg0, u32 arg1) {
    UNUSED s32 pad;
    u8* temp_a0;
    u32 sp1C;

    if (arg0 != 0) {
        sp1C = osSetIntMask(1);

        temp_a0 = gAudioCommandQueue;
        temp_a0 += gAudioCommandQueueLength;

        *temp_a0++ = 7;
        Audio_WriteU32BE(temp_a0, arg0);
        temp_a0 += 4;
        Audio_WriteU16BE(temp_a0, (u16)arg1);

        gAudioCommandQueueLength += 7;
        if (gAudioCommandQueueLength >= D_800FC7F0) {
            gAudioCommandQueueLength -= 7;
        }

        osSetIntMask(sp1C);
    }
}

s32 Audio_IsSoundPlaying(s32 arg0) {
    AudioVoice* var_a0;
    s32 i;
    s32 sp1C;
    u32 sp18;

    if (arg0 == 0) {
        return 0;
    }

    sp18 = osSetIntMask(1);

    for (i = 0, sp1C = 0, var_a0 = D_800FC7D0; i < D_800FC7CC; i++, var_a0++) {
        if (arg0 == var_a0->soundId) {
            sp1C++;
        }
    }

    osSetIntMask(sp18);

    return sp1C;
}

void Audio_QueueVoiceVolumeScale(s32 arg0, u32 arg1) {
    u8* temp_a0;
    UNUSED s32 pad[2];
    u32 sp18;

    if (arg0 != 0) {
        sp18 = osSetIntMask(1);

        temp_a0 = gAudioCommandQueue;
        temp_a0 += gAudioCommandQueueLength;

        *temp_a0++ = 4;
        Audio_WriteU32BE(temp_a0, arg0);
        temp_a0 += 4;
        Audio_WriteU16BE(temp_a0, (u16)arg1);

        gAudioCommandQueueLength += 7;
        if (gAudioCommandQueueLength >= D_800FC7F0) {
            gAudioCommandQueueLength -= 7;
        }

        osSetIntMask(sp18);
    }
}

void Audio_QueueVoicePan(s32 arg0, u16 arg1) {
    u8* temp_a0;
    UNUSED s32 pad[2];
    u32 sp18;

    if (arg0 != 0) {
        sp18 = osSetIntMask(1);

        temp_a0 = gAudioCommandQueue;
        temp_a0 += gAudioCommandQueueLength;

        *temp_a0++ = 5;
        Audio_WriteU32BE(temp_a0, arg0);
        temp_a0 += 4;
        Audio_WriteU16BE(temp_a0, arg1);
        temp_a0 += 2;

        gAudioCommandQueueLength += 7;
        if (gAudioCommandQueueLength >= D_800FC7F0) {
            gAudioCommandQueueLength -= 7;
        }
        osSetIntMask(sp18);
    }
}

void Audio_QueueVoiceDetune(s32 arg0, f32 arg1) {
    u8* temp_a0;
    UNUSED s32 pad[3];
    u32 sp1C;
    UNUSED s32 pad2;

    if (arg0 != 0) {
        sp1C = osSetIntMask(1);

        temp_a0 = gAudioCommandQueue;
        temp_a0 += gAudioCommandQueueLength;

        *temp_a0++ = 2;
        Audio_WriteU32BE(temp_a0, arg0);
        temp_a0 += 4;
        Audio_WriteU16BE(temp_a0, (s32)(255.0f * arg1) & 0xFFFF);

        gAudioCommandQueueLength += 7;
        if (gAudioCommandQueueLength >= D_800FC7F0) {
            gAudioCommandQueueLength -= 7;
        }

        osSetIntMask(sp1C);
    }
}

void Audio_QueueVoiceVolume(s32 arg0, s32 arg1) {
    u8* temp_a0;
    UNUSED s32 pad;
    u32 sp1C;

    if (arg0 != 0) {
        if (arg1 <= 0) {
            arg1 = 1;
        } else if (arg1 > 0x100) {
            arg1 = 0x100;
        }
        arg1--;

        sp1C = osSetIntMask(1);
        temp_a0 = gAudioCommandQueue;
        temp_a0 += gAudioCommandQueueLength;

        *temp_a0++ = 3;
        Audio_WriteU32BE(temp_a0, arg0);
        temp_a0 += 4;
        Audio_WriteU16BE(temp_a0, arg1 & 0xFFFF);

        gAudioCommandQueueLength += 7;
        if (gAudioCommandQueueLength >= D_800FC7F0) {
            gAudioCommandQueueLength -= 7;
        }

        osSetIntMask(sp1C);
    }
}

void Audio_QueueVoiceVolumeByte(u32 arg0, s32 arg1) {
    u8* temp_a0;
    UNUSED s32 pad;
    u32 sp1C;

    if (arg0 != 0) {
        if (arg1 < 0) {
            arg1 = 0;
        } else if (arg1 >= 0x80) {
            arg1 = 0x7F;
        }

        sp1C = osSetIntMask(1);

        temp_a0 = gAudioCommandQueue;
        temp_a0 += gAudioCommandQueueLength;

        *temp_a0++ = 3;
        Audio_WriteU32BE(temp_a0, arg0);
        temp_a0 += 4;

        *temp_a0++ = arg1;

        gAudioCommandQueueLength += 6;
        if (gAudioCommandQueueLength >= D_800FC7F0) {
            gAudioCommandQueueLength -= 6;
        }

        osSetIntMask(sp1C);
    }
}

void Audio_RelocateSoundBankWrapper(s32 arg0, s32 arg1) {
    Audio_RelocateSoundBank(arg0, arg1);
}

void Audio_RelocateSoundBankTable(unk_func_800397BC* arg0) {
    u32 i;
    u32* ptr;
    u32 num_entries;
    u32 val;
    u32 a0 = (u32)arg0;

    if (arg0->flags & 2) {
        return;
    }

    num_entries = arg0->num_entries;

    val = arg0->unk_offset;
    arg0->unk_offset = a0 + val;

    val = arg0->entries_offset;
    arg0->entries_offset = a0 + val;

    ptr = arg0->entries_offset;
    if (0) {}

    for (i = 0; i < num_entries; i++, ptr++) {
        *ptr += a0;
    }

    arg0->flags |= 2;
}

void Audio_QueueSetMonoOutputCommand(s32 arg0) {
    u32 temp_v0 = osSetIntMask(1);
    u8* temp_v1 = gAudioCommandQueue;

    temp_v1 += gAudioCommandQueueLength;

    *temp_v1++ = 0xA;
    *temp_v1++ = arg0;

    gAudioCommandQueueLength += 2;
    if (gAudioCommandQueueLength >= D_800FC7F0) {
        gAudioCommandQueueLength -= 2;
    }

    osSetIntMask(temp_v0);
}

void Audio_SetVoiceStartTimingMode(s32 arg0) {
    u32 temp_a0 = osSetIntMask(1);

    if (arg0 != 0) {
        D_80077C94 = 1;
    } else {
        D_80077C94 = 0;
    }

    osSetIntMask(temp_a0);
}

void Audio_StopAllSoundEffects(void) {
    UNUSED s32 pad[4];
    u32 sp34 = osSetIntMask(1);
    u32 i;
    AudioVoice* ptr = D_800FC7D0;

    for (i = 0; i < D_800FC7CC; i++, ptr++) {
        if ((ptr->independentTempo != 0) || (ptr->commandStreamCursor == 0)) {
            continue;
        }

        ptr->commandStreamCursor = Audio_ClearVoicePlaybackState(ptr, 0);

        if (ptr->voiceActive == 0) {
            ptr->waveTable = 0;
            continue;
        }

        ptr->hardwareVoice->envelopeStage = 4;
        ptr->hardwareVoice->expirationTime = D_800FC7F8 + 2;

        n_alSynSetVol(&ptr->hardwareVoice->voice, 0, D_800FC7D8 * 2);

        n_alSynSetPriority(&ptr->hardwareVoice->voice, 0);
        Audio_QueueActiveVoice(ptr->hardwareVoice);

        ptr->hardwareVoice = NULL;
        ptr->voiceIdle = 1;
        ptr->voiceActive = 0;
        ptr->waveTable = 0;
    }

    osSetIntMask(sp34);
}

s32 Audio_PlayMusicSequenceLooped(SoundBank* arg0, s32 arg1, s32 arg2) {
    u8* temp_s0;
    s32 sp28;
    UNUSED s32 pad;
    u32 sp20;

    if (arg0 == NULL) {
        return 0;
    }

    if (!(arg0->flags & 1)) {
        return 0;
    }

    sp20 = osSetIntMask(1);

    temp_s0 = gAudioCommandQueue;
    temp_s0 += gAudioCommandQueueLength;

    sp28 = D_800FC7E0++;

    *temp_s0++ = 11;
    Audio_WriteU32BE(temp_s0, arg0->header_name);
    temp_s0 += sizeof(u32);
    Audio_WriteU32BE(temp_s0, arg1);
    temp_s0 += sizeof(u32);
    Audio_WriteU32BE(temp_s0, sp28);
    temp_s0 += sizeof(u32);
    Audio_WriteU32BE(temp_s0, arg2);

    gAudioCommandQueueLength += 17;
    if (gAudioCommandQueueLength >= D_800FC7F0) {
        gAudioCommandQueueLength -= 17;
    }

    osSetIntMask(sp20);

    return sp28;
}

s32 Audio_StartVolumeTransition(s32 arg0, u8 arg1, u8 arg2, u32 arg3) {
    u32 temp_v0;

    if (arg1 == arg2) {
        return 0;
    }

    if (D_80077DC4 != 0) {
        return 0;
    }

    temp_v0 = osSetIntMask(1);

    D_80077DC4 = arg0;
    D_80077DD0 = ((f32)arg2 - arg1) / arg3;
    D_80077DC8 = arg1;
    D_80077DCC = arg2;

    osSetIntMask(temp_v0);

    return 1;
}

u32 Audio_ReadU32BE(u8* arg0) {
    u32 val = *arg0++ << 0x18;
    val += *arg0++ << 0x10;
    val += *arg0++ << 8;
    val += *arg0++;
    return val;
}

u16 Audio_ReadU16BE(u8* arg0) {
    u32 val = *arg0++ << 8;
    val += *arg0++;
    return val;
}

void Audio_ProcessCommandQueue(void) {
    u8* var_s0 = gAudioCommandQueue;
    s32 idx;

    while (gAudioCommandQueueLength) {
        idx = *var_s0++;
        gAudioCommandQueueLength--;
        var_s0 = gAudioCommandHandlers[idx](var_s0);
    }
}

u8* Audio_PlayMusicSequenceHandler(u8* arg0) {
    AudioInstrumentBank* sp4C;
    AudioInstrumentBank* temp_s2;
    AudioInstrumentBank* sp44;
    AudioVoice* temp_s0;
    u32 i;
    u32 count;
    u32 temp_v0;

    sp44 = Audio_ReadU32BE(arg0);
    arg0 += 4;
    temp_s2 = Audio_ReadU32BE(arg0);
    arg0 += 4;
    sp4C = Audio_ReadU32BE(arg0);
    arg0 += 4;

    gAudioCommandQueueLength -= 0xC;

    count = temp_s2->unk_00;
    if ((u32)temp_s2->commandData < 0x400) {
        Audio_RelocatePointerArray(&temp_s2->commandData, temp_s2, 5);
        Audio_RelocatePointerArray(temp_s2->commandData, temp_s2, count);
        Audio_RelocatePointerArray(temp_s2->subEnvelopeDataA, temp_s2, count);
        Audio_RelocatePointerArray(temp_s2->subEnvelopeDataB, temp_s2, count);
    }

    for (i = 0; i < count; i++) {
        if (temp_s2->commandData[i] == 0) {
            continue;
        }

        temp_v0 = Audio_FindFreeVoice(temp_s2, i);
        if (temp_v0 == -1) {
            continue;
        }

        temp_s0 = &D_800FC7D0[temp_v0];
        Audio_ResetVoiceState(temp_s0);
        temp_s0->instrumentBank = temp_s2;
        temp_s0->subVoiceInstrumentBank = sp44;
        temp_s0->subEnvelopeCursorA = temp_s0->subEnvelopeBaseA = temp_s0->instrumentBank->subEnvelopeDataA[i];
        temp_s0->subEnvelopeCursorB = temp_s0->subEnvelopeBaseB = temp_s0->instrumentBank->subEnvelopeDataB[i];
        temp_s0->commandStreamCursor = temp_s0->commandStreamBase = temp_s0->instrumentBank->commandData[i];
        temp_s0->soundId = sp4C;
        temp_s0->priority = 0x3C;
    }

    D_80077C90 = -1;
    return arg0;
}

u8* Audio_PlayMusicSequenceLoopedHandler(u8* arg0) {
    u32 sp5C;
    u32 i;
    u32 sp54;
    s32 temp_v0;
    u32 temp_fp;
    AudioInstrumentBank* temp_s2;
    AudioVoice* temp_s0;
    u32 sp40;

    sp54 = Audio_ReadU32BE(arg0);
    arg0 += 4;
    temp_s2 = Audio_ReadU32BE(arg0);
    arg0 += 4;
    sp5C = Audio_ReadU32BE(arg0);
    arg0 += 4;
    sp40 = Audio_ReadU32BE(arg0);
    arg0 += 4;

    gAudioCommandQueueLength -= 0x10;

    temp_fp = temp_s2->unk_00;
    if ((u32)temp_s2->commandData < 0x400) {
        Audio_RelocatePointerArray(&temp_s2->commandData, temp_s2, 5);
        Audio_RelocatePointerArray(temp_s2->commandData, temp_s2, temp_fp);
        Audio_RelocatePointerArray(temp_s2->subEnvelopeDataA, temp_s2, temp_fp);
        Audio_RelocatePointerArray(temp_s2->subEnvelopeDataB, temp_s2, temp_fp);
    }

    for (i = 0; i < temp_fp; i++) {
        if (temp_s2->commandData[i] == 0) {
            continue;
        }

        temp_v0 = Audio_FindFreeVoice(temp_s2, i);
        if (temp_v0 == -1) {
            continue;
        }

        temp_s0 = &D_800FC7D0[temp_v0];
        Audio_ResetVoiceState(temp_s0);
        temp_s0->instrumentBank = temp_s2;
        temp_s0->subVoiceInstrumentBank = sp54;
        temp_s0->subEnvelopeCursorA = temp_s0->subEnvelopeBaseA = temp_s2->subEnvelopeDataA[i];
        temp_s0->subEnvelopeCursorB = temp_s0->subEnvelopeBaseB = temp_s2->subEnvelopeDataB[i];
        temp_s0->commandStreamCursor = temp_s0->commandStreamBase = temp_s2->commandData[i];
        temp_s0->soundId = sp5C;
        temp_s0->priority = 0x3C;
    }

    D_80077C90 = sp40;
    return arg0;
}

u8* Audio_PlaySoundEffectHandler(u8* arg0) {
    u32 sp44;
    unk_D_800FC7D0_140* sp40;
    u32* sp3C;
    u32 sp38;
    u32 sp34;
    u32 i;
    u8 var_t0;
    u8 var_t1;
    u8 var_t2;
    AudioVoice* var_a0;
    unk_D_800FC7D0_08C_008* a;

    sp44 = Audio_ReadU32BE(arg0);
    arg0 += 4;
    sp40 = Audio_ReadU32BE(arg0);
    arg0 += 4;
    sp38 = Audio_ReadU32BE(arg0);
    arg0 += 4;
    sp34 = Audio_ReadU32BE(arg0);
    arg0 += 4;

    var_t2 = *arg0++;
    var_t1 = *arg0++;
    var_t0 = *arg0++;

    gAudioCommandQueueLength -= 0x13;
    sp3C = sp40->subVoiceTable;
    a = sp40->unk_08;

    if (var_t0 == 0xFF) {
        var_t0 = a[sp34].unk_00[3];
    }

    for (i = 0, var_a0 = D_800FC7D0; i < D_800FC7CC; i++, var_a0++) {
        if (var_a0->commandStreamCursor == NULL) {
            Audio_InitializeVoice(var_a0, sp34, var_t2, var_t1, var_t0);
            var_a0->soundId = sp38;
            var_a0->subVoiceInstrumentBank = sp44;
            var_a0->subVoiceContext = sp40;
            var_a0->commandStreamCursor = var_a0->commandStreamBase = sp3C[sp34];
            break;
        }
    }

    return arg0;
}

u8* Audio_ApplyVoiceDetuneCommand(u8* arg0) {
    u32 sp1C;
    s32 i;
    s16 temp_v0;
    AudioVoice* var_a0;

    sp1C = Audio_ReadU32BE(arg0);
    arg0 += 4;
    temp_v0 = Audio_ReadU16BE(arg0);
    arg0 += 2;

    gAudioCommandQueueLength -= 6;

    for (i = 0, var_a0 = D_800FC7D0; i < D_800FC7CC; i++, var_a0++) {
        if (sp1C == var_a0->soundId) {
            var_a0->finePitchAccumulator = var_a0->finePitchOffset + (temp_v0 / 255.0f);
        }
    }

    return arg0;
}

u8* Audio_ApplyVoiceVolumeCommand(u8* arg0) {
    u32 sp24;
    u16 temp_v0;
    AudioVoice* var_v1;
    u32 i;
    u8 val;

    sp24 = Audio_ReadU32BE(arg0);
    arg0 += 4;
    temp_v0 = Audio_ReadU16BE(arg0);
    arg0 += 2;

    gAudioCommandQueueLength -= 6;

    for (i = 0, var_v1 = D_800FC7D0; i < D_800FC7CC; i++, var_v1++) {
        if (sp24 == var_v1->soundId) {
            val = temp_v0;
            var_v1->tempoScale = val;
            var_v1->tempoIncrement = (var_v1->tempoTicksRaw * val) >> 7;
        }
    }

    return arg0;
}

u8* Audio_ApplyVoiceVolumeScaleCommand(u8* arg0) {
    u32 sp1C;
    u32 i;
    AudioVoice* var_v1;
    u16 temp_v0;
    u8 val;

    sp1C = Audio_ReadU32BE(arg0);
    arg0 += 4;
    temp_v0 = Audio_ReadU16BE(arg0);
    arg0 += 2;

    gAudioCommandQueueLength -= 6;

    for (i = 0, var_v1 = D_800FC7D0; i < D_800FC7CC; i++, var_v1++) {
        if (sp1C == var_v1->soundId) {
            val = temp_v0;
            var_v1->volumeScale = val;
        }
    }

    return arg0;
}

u8* Audio_ApplyVoicePanCommand(u8* arg0) {
    u32 sp24;
    u32 i;
    u16 temp_v0;
    AudioVoice* var_v1;
    u8 val;

    sp24 = Audio_ReadU32BE(arg0);
    arg0 += 4;
    temp_v0 = Audio_ReadU16BE(arg0);
    arg0 += 2;

    gAudioCommandQueueLength -= 6;

    for (i = 0, var_v1 = D_800FC7D0; i < D_800FC7CC; i++, var_v1++) {
        if (sp24 == var_v1->soundId) {
            val = temp_v0;
            var_v1->currentPan = val;
            var_v1->lastAppliedPan = 0xFF;
        }
    }

    return arg0;
}

u8* Audio_ApplyVoiceFxMixCommand(u8* arg0) {
    u32 i;
    u32 temp_v0;
    AudioVoice* var_v1;
    u8 val;

    temp_v0 = Audio_ReadU32BE(arg0);
    arg0 += 4;
    val = *arg0++;

    gAudioCommandQueueLength -= 5;

    for (i = 0, var_v1 = D_800FC7D0; i < D_800FC7CC; i++, var_v1++) {
        if (temp_v0 == var_v1->soundId) {
            var_v1->fxMixOverride = val;
            var_v1->appliedFxMix = 0xFF;
        }
    }

    return arg0;
}

u8* Audio_FadeSoundHandler(u8* arg0) {
    u32 sp24;
    u32 i;
    u32 var_a2;
    u16 temp_v0;
    AudioVoice* var_v1;

    sp24 = Audio_ReadU32BE(arg0);
    arg0 += 4;
    temp_v0 = Audio_ReadU16BE(arg0);
    arg0 += 2;

    gAudioCommandQueueLength -= 6;

    if (sp24 != 0) {
        if (temp_v0 != 0) {
            var_a2 = temp_v0 & 0xFFFF;
        } else {
            var_a2 = 1;
        }

        for (i = 0, var_v1 = D_800FC7D0; i < D_800FC7CC; i++, var_v1++) {
            if (sp24 == var_v1->soundId) {
                var_v1->fadeCounter = temp_v0;
                var_v1->fadeDuration = var_a2;
            }
        }
    }

    return arg0;
}

u8* Audio_FadeCategoryHandler(u8* arg0) {
    u8 sp27;
    u32 i;
    u32 var_t0;
    u32 temp_v0;
    AudioVoice* var_a0;

    sp27 = *arg0++;
    temp_v0 = Audio_ReadU16BE(arg0);
    arg0 += 2;

    gAudioCommandQueueLength -= 3;

    if (temp_v0 != 0) {
        var_t0 = temp_v0 & 0xFFFF;
    } else {
        var_t0 = 1;
    }

    for (i = 0, var_a0 = D_800FC7D0; i < D_800FC7CC; i++, var_a0++) {
        if (((var_a0->independentTempo != 0) && (sp27 & 1)) || ((var_a0->independentTempo == 0) && (sp27 & 2))) {
            var_a0->fadeCounter = temp_v0;
            var_a0->fadeDuration = var_t0;
        }
    }

    return arg0;
}

u8* Audio_SetChannelVolumeHandler(u8* arg0) {
    u8 sp1F;
    u16 temp_v0;

    sp1F = *arg0++;
    temp_v0 = Audio_ReadU16BE(arg0);
    arg0 += 2;

    gAudioCommandQueueLength -= 3;

    if (sp1F & 1) {
        D_800FC7DC = temp_v0;
    }

    if (sp1F & 2) {
        D_800FC7DE = temp_v0;
    }

    return arg0;
}

u8* Audio_SetMonoOutputHandler(u8* arg0) {
    u8 val = *arg0++;

    gAudioCommandQueueLength--;

    if (val == 1) {
        D_800FC7F4 = 1;
    } else {
        D_800FC7F4 = 0;
    }
    return arg0;
}

#ifdef NON_MATCHING
s32 Audio_PlayerFrameHandler(void* arg0) {
    s32 i;
    UNUSED s32 pad4[1];
    f32 spA4;
    UNUSED s32 pad[4];
    ALVoiceConfig sp8C;
    UNUSED s32 pad2[3];
    UNUSED s32 pad3[1];
    f32 var_fs0;
    s32 temp_s3;
    u32 var_s4;
    u32 temp_s1;
    s32 var_s2;
    AudioVoice* ptr;

    if (D_80077C90 > 0) {
        D_80077C90--;
    }

    if (D_80077C90 == 0) {
        Audio_StopAllSoundEffects();
        D_80077C90 = -1;
    }

    Audio_ProcessCommandQueue();

    ptr = D_800FC7D0;

    for (i = 0; i < D_800FC7CC; i++, ptr++) {
        if (ptr->commandStreamCursor == 0) {
            continue;
        }

        ptr->playbackPosition += ptr->tempoIncrement;

        if (ptr->noteDuration != 0x7FFF) {
            while ((ptr->playbackPosition > ptr->noteEndPosition) && (ptr->commandStreamCursor != 0)) {
                Audio_DecodeVoiceCommandStream(ptr);
            }
        }

        if (ptr->subEnvelopeCursorA != 0) {
            if (ptr->pitchEnvelopeTimer < ptr->playbackPosition) {
                Audio_AdvancePitchEnvelope(ptr);
            }
        }

        if (ptr->subEnvelopeCursorB != 0) {
            if (ptr->volumeEnvelopeTimer < ptr->playbackPosition) {
                Audio_AdvanceVolumeEnvelope(ptr);
            }
        }

        if (ptr->fadeCounter != -1) {
            ptr->fadeCounter--;
            if (ptr->fadeCounter == -1) {
                ptr->commandStreamCursor = Audio_ClearVoicePlaybackState(ptr, 0);
                if (ptr->voiceActive != 0) {
                    ptr->voiceActive = 0;
                    n_alSynStopVoice(&ptr->hardwareVoice->voice);
                    n_alSynFreeVoice(&ptr->hardwareVoice->voice);
                    ptr->hardwareVoice->envelopeStage = 5;
                    Audio_QueueFreeVoice(ptr->hardwareVoice);
                    ptr->hardwareVoice = NULL;
                    ptr->independentTempo = 0;
                }
                ptr->waveTable = 0;
            }
        }

        if (ptr->waveTable == 0) {
            continue;
        }

        Audio_ReleaseVoice(ptr);

        if (ptr->waveTable == 0) {
            continue;
        }

        spA4 = ptr->finePitchAccumulator;
        if (ptr->vibratoDepthRaw != 0) {
            spA4 += Audio_ComputeSinePitchModulation(ptr);
        }

        if (ptr->periodicPitchModDuration != 0) {
            spA4 += Audio_AdvancePeriodicPitchModulation(ptr);
        }

        var_fs0 = Audio_AdvanceVoicePitch(ptr, spA4);
        temp_s3 = Audio_ComputeVoiceVolume(ptr);

        if (ptr->panModulationRate != 0) {
            if (ptr->panModulationTimer < ptr->playbackPosition) {
                Audio_AdvancePanModulation(ptr);
            }
        }

        if (D_800FC7F4 == 1) {
            var_s2 = 0x40;
        } else {
            var_s2 = ((ptr->pan * ptr->currentPan) >> 7) & 0x7F;
        }

        var_s4 = ptr->playbackPosition - ptr->noteStartPosition;
        ptr->ticksSinceNoteStart = var_s4 >> 8;

        temp_s1 = ptr->fxMixOverride;
        temp_s1 += (((0x80 - temp_s1) * ptr->fxMix) >> 7);

        if (ptr->voiceActive != 0) {
            if (var_fs0 != 0.0f) {
                n_alSynSetPitch(&ptr->hardwareVoice->voice, var_fs0);
            }

            if (temp_s3 != ptr->priorityCache) {
                ptr->priorityCache = temp_s3;
                n_alSynSetVol(&ptr->hardwareVoice->voice, temp_s3, D_800FC7D8);
            }

            if (var_s2 != ptr->lastAppliedPan) {
                ptr->lastAppliedPan = var_s2;
                n_alSynSetPan(&ptr->hardwareVoice->voice, var_s2);
            }

            if (ptr->fxMix != ptr->appliedFxMix) {
                ptr->appliedFxMix = temp_s1;
                n_alSynSetFXMix(&ptr->hardwareVoice->voice, temp_s1);
            }
        } else {
            ptr->hardwareVoice = Audio_DequeueFreeVoice();
            if (ptr->hardwareVoice == NULL) {
                ptr->waveTable = 0;
                continue;
            }

            ptr->hardwareVoice->envelopeStage = ptr->envelopeStage;
            ptr->hardwareVoice->releaseRate = ptr->releaseRate;

            if (ptr->priority >= 0x80) {
                sp8C.priority = 0x7F;
            } else {
                sp8C.priority = ptr->priority;
            }

            sp8C.unityPitch = 0;
            sp8C.fxBus = 0;
            n_alSynAllocVoice(&ptr->hardwareVoice->voice, &sp8C);

            ptr->priorityCache = temp_s3;
            ptr->lastAppliedPan = var_s2;
            ptr->appliedFxMix = temp_s1;

            if (var_fs0 == 0.0f) {
                var_fs0 = Audio_ExpApproximation(ptr->lastAppliedPitch * 0.0833333333333333287);
                if (var_fs0 > 2.0) {
                    var_fs0 = 2.0f;
                }
            }

            if (D_80077C94 == 0) {
                n_alSynStartVoiceParams(&ptr->hardwareVoice->voice, ptr->waveTable, var_fs0, temp_s3, var_s2, temp_s1,
                                        D_800FC7D8);
            } else {
                n_alSynStartVoiceParams(&ptr->hardwareVoice->voice, ptr->waveTable, var_fs0, temp_s3, var_s2, temp_s1, 1);
            }

            ptr->voiceIdle = 0;
            ptr->voiceActive = 1;
            ptr->hardwareVoice->envelopeStage = 1;
        }
    }

    Audio_ReclaimExpiredVoices();
    D_800FC7F8++;

    return D_800FC7D8;
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/audio_io/Audio_PlayerFrameHandler.s")
#endif

void Audio_DecodeVoiceCommandStream(AudioVoice* arg0) {
    s32 pad[2];
    AudioInstrumentBank* temp_s0;
    u8* var_s0;
    u8 temp_a0;
    u8 tmp;
    u8 tmp1;
    u16 tmp2;
    u32 tmp3;

    var_s0 = arg0->commandStreamCursor;
    while (var_s0 != NULL && (tmp1 = *var_s0) >= 0x80) {
        var_s0 = gVoiceCommandHandlers[tmp1 & 0x7F](arg0, var_s0 + 1);
    }
    arg0->commandStreamCursor = var_s0;

    if (arg0->commandStreamCursor != NULL) {
        arg0->portamentoStartPitch = arg0->currentPitch;
        tmp = *arg0->commandStreamCursor++;

        if (arg0->velocityFromStreamEnabled != 0) {
            arg0->currentVelocity = *arg0->commandStreamCursor++;
        } else {
            arg0->currentVelocity = arg0->defaultVelocity;
        }

        if (arg0->durationOverride != 0) {
            if (arg0->variableDurationEnabled == 0) {
                arg0->noteDuration = arg0->durationOverride;
            } else {
                arg0->variableDurationEnabled = 0;
                temp_a0 = *arg0->commandStreamCursor++;
                if (temp_a0 < 0x80) {
                    arg0->noteDuration = temp_a0;
                } else {
                    arg0->noteDuration = *arg0->commandStreamCursor++ + ((temp_a0 & 0x7F) << 8);
                }
            }
        } else {
            temp_a0 = *arg0->commandStreamCursor++;
            if (temp_a0 < 0x80) {
                arg0->noteDuration = temp_a0 ^ 0;
            } else {
                arg0->noteDuration = *arg0->commandStreamCursor++ + ((temp_a0 & 0x7F) << 8);
            }
        }

        arg0->noteStartPosition = arg0->noteEndPosition;
        arg0->noteEndPosition += arg0->noteDuration << 8;

        arg0->ticksSinceNoteStart = 0;
        arg0->periodicPitchModOutput = 0;
        arg0->periodicPitchModCounter = arg0->periodicPitchModDurationOff;

        if (tmp != 0x60) {
            temp_s0 = arg0->subVoiceInstrumentBank;
            Audio_UpdateVoicePlaybackEnd(arg0);

            if (arg0->panModulationRate != 0) {
                Audio_ResetPanModulation(arg0);
            }

            if (arg0->voiceActive != 0) {
                arg0->waveTable = NULL;
                Audio_ReleaseVoice(arg0);
            }

            tmp2 = arg0->sampleIndex;
            tmp3 = temp_s0->waveTableTable[tmp2];
            arg0->waveTable = tmp3;
            arg0->basePitch = temp_s0->basePitchTable[tmp2] + tmp;
        } else {
            arg0->waveTable = NULL;
            if (arg0->voiceActive != 0) {
                arg0->releaseTriggerPosition = arg0->playbackPosition;
                Audio_ReleaseVoice(arg0);
            }
        }
    } else {
        arg0->waveTable = NULL;
        if (arg0->voiceActive != 0) {
            arg0->releaseTriggerPosition = arg0->playbackPosition;
            Audio_ReleaseVoice(arg0);
        }
    }
}

s32 Audio_ComputeVoiceVolume(AudioVoice* arg0) {
    u32 var_v1 = arg0->volume * arg0->currentLevel * arg0->currentVelocity * arg0->volumeScale;

    var_v1 >>= 0xD;
    if (arg0->independentTempo == 0) {
        var_v1 *= D_800FC7DE;
    } else {
        var_v1 *= D_800FC7DC;
    }

    var_v1 >>= 0xF;
    if (var_v1 >= 0x8000) {
        var_v1 = 0x7FFF;
    }

    if (arg0->fadeCounter != -1) {
        var_v1 = (arg0->fadeCounter * var_v1) / arg0->fadeDuration;
    }

    return var_v1;
}

f32 Audio_AdvanceVoicePitch(AudioVoice* arg0, f32 arg1) {
    f32 temp_fv1;
    f32 var_fv1;
    f32 var_fv1_2;
    s32 temp_lo;

    temp_fv1 = arg0->basePitch;
    if (arg0->portamentoDuration != 0) {
        if (arg0->portamentoDuration >= arg0->ticksSinceNoteStart) {
            var_fv1_2 = ((temp_fv1 - arg0->portamentoStartPitch) / arg0->portamentoDuration);
            var_fv1_2 *= arg0->ticksSinceNoteStart;
            temp_fv1 = arg0->portamentoStartPitch + var_fv1_2;
        }
        arg0->currentPitch = temp_fv1;
    }

    temp_lo = arg0->detune * (1 - arg0->detuneOnce);

    arg0->detuneOnce = 0;

    temp_fv1 += (arg1 + arg0->pitchModulation + temp_lo);
    if (temp_fv1 == arg0->lastAppliedPitch) {
        return 0.0f;
    }
    arg0->lastAppliedPitch = temp_fv1;

    temp_fv1 = Audio_ExpApproximation(temp_fv1 * 0.0833333333333333287);
    if (temp_fv1 > 2.0) {
        temp_fv1 = 2.0f;
        arg0->currentVelocity = 0;
    }

    return temp_fv1;
}

void Audio_UpdateVoicePlaybackEnd(AudioVoice* arg0) {
    s32 temp_t7;
    s32 temp_t7_2;
    u16 temp_v0;
    u32 temp_t3;
    u32 temp_t5;

    if (arg0->noteDuration != 0x7FFF) {
        if (arg0->fixedDuration != 0) {
            arg0->releaseTriggerPosition = arg0->noteStartPosition + (arg0->fixedDuration << 8);
        } else {
            arg0->releaseTriggerPosition = arg0->noteEndPosition - (arg0->releaseDuration << 8);
        }
    } else {
        arg0->releaseTriggerPosition = 0x7FFFFFFF;
    }

    arg0->currentLevel = arg0->attackStartLevel;
    arg0->attackRateSnapshot = arg0->attackRate;
    arg0->envelopeStage = 1;
}

void Audio_ReleaseVoice(AudioVoice* arg0) {
    u32 temp_ft4;
    s32 var_v1;
    u32 temp_a0;

    if (arg0->hardwareVoice != NULL) {
        if (((arg0->playbackPosition >= arg0->releaseTriggerPosition) && (arg0->hardwareVoice->envelopeStage < 4)) || (arg0->waveTable == 0)) {
            arg0->hardwareVoice->envelopeStage = 4;
            temp_ft4 = (f32)arg0->hardwareVoice->releaseRate / ((f32)arg0->tempoIncrement * 0.00390625);
            if (temp_ft4 == 0) {
                temp_ft4++;
            }
            arg0->hardwareVoice->expirationTime = D_800FC7F8 + temp_ft4 + 1;

            n_alSynSetVol(&arg0->hardwareVoice->voice, 0, D_800FC7D8 * temp_ft4);

            if (arg0->priority < 0x80) {
                if (arg0->independentTempo != 0) {
                    n_alSynSetPriority(&arg0->hardwareVoice->voice, 1);
                } else {
                    n_alSynSetPriority(&arg0->hardwareVoice->voice, 0);
                }
            }

            Audio_QueueActiveVoice(arg0->hardwareVoice);

            arg0->hardwareVoice = NULL;
            arg0->voiceIdle = 1;
            arg0->voiceActive = 0;
            arg0->waveTable = 0;
        } else {
            switch (arg0->hardwareVoice->envelopeStage) {
                case 1:
                    temp_a0 = (u32)(arg0->playbackPosition - arg0->noteStartPosition) >> 8;
                    if (temp_a0 < arg0->decayDuration) {
                        arg0->currentLevel = arg0->decayRate * temp_a0;
                    } else {
                        arg0->hardwareVoice->envelopeStage++;
                        arg0->currentLevel = arg0->decayLevel;
                    }
                    break;

                case 2:
                    temp_a0 = ((arg0->playbackPosition - arg0->noteStartPosition) >> 8) - arg0->decayDuration;
                    if (temp_a0 < arg0->sustainDuration) {
                        arg0->currentLevel = arg0->decayLevel + (s32)(arg0->sustainRate * temp_a0);
                    } else {
                        arg0->hardwareVoice->envelopeStage++;
                        arg0->currentLevel = arg0->sustainLevel;
                    }
                    break;

                case 3:
                case 4:
                    break;
            }
        }
    } else {
        temp_a0 = (u32)(arg0->playbackPosition - arg0->noteStartPosition) >> 8;
        if ((temp_a0 < arg0->decayDuration) && (arg0->decayDuration >= 2)) {
            arg0->envelopeStage = 1;
            arg0->currentLevel = arg0->decayRate * temp_a0;
        } else {
            arg0->envelopeStage = 2;
            arg0->currentLevel = arg0->decayLevel;
        }
    }
}

void Audio_ResetPanModulation(AudioVoice* arg0) {
    arg0->panModulationTimer = arg0->noteStartPosition;
    arg0->panModulationPhase = 0;
    arg0->panModulationDirection = arg0->currentPan & 0x80;
}

void Audio_AdvancePanModulation(AudioVoice* arg0) {
    u32 temp_v1;
    u32 var_v0;

    do {
        arg0->panModulationTimer += 0x100;

        temp_v1 = arg0->panModulationPhase + (arg0->panModulationRate * 2);
        if (temp_v1 < 0x80) {
            arg0->panModulationPhase = temp_v1;
            continue;
        }

        arg0->panModulationPhase = temp_v1 & 0x7F;
        temp_v1 >>= 7;
        if (arg0->panModulationDirection == 0) {
            var_v0 = arg0->currentPan;
            var_v0 += temp_v1;
            if (var_v0 >= 0xFF) {
                var_v0 = 0xFE;
                arg0->panModulationDirection = 1;
            }
            arg0->currentPan = var_v0;
        } else {
            var_v0 = arg0->currentPan;
            var_v0 -= temp_v1;
            if ((var_v0 >= 0xFF) || (var_v0 == 0)) {
                var_v0 = 1;
                arg0->panModulationDirection = 0;
            }
            arg0->currentPan = var_v0;
        }
    } while (arg0->panModulationTimer < arg0->playbackPosition);

    arg0->lastAppliedPan = 0xFF;
}

f32 Audio_AdvancePeriodicPitchModulation(AudioVoice* arg0) {
    arg0->periodicPitchModCounter--;
    if (!arg0->periodicPitchModCounter) {
        if (arg0->periodicPitchModOutput == 0) {
            arg0->periodicPitchModOutput = arg0->periodicPitchModDepth;
            arg0->periodicPitchModCounter = arg0->periodicPitchModDuration;
        } else {
            arg0->periodicPitchModOutput = 0;
            arg0->periodicPitchModCounter = arg0->periodicPitchModDurationOff;
        }
    }

    return arg0->periodicPitchModOutput;
}

f32 Audio_ComputeSinePitchModulation(AudioVoice* arg0) {
    u32 temp_v0 = arg0->ticksSinceNoteStart - arg0->vibratoRate;
    f32 tmp;

    if (temp_v0 != 0) {
        tmp = __sinf(((temp_v0 / (f32)arg0->vibratoDepthRaw) * 2) * 3.14159260000000007) * arg0->vibratoDepth;
        arg0->vibratoOutput = tmp;
    }
    return arg0->vibratoOutput;
}

void Audio_AdvancePitchEnvelope(AudioVoice* arg0) {
    s32 tmp;

    do {
        arg0->pitchEnvelopeTimer += 0x100;

        arg0->subEnvelopeCounterA--;
        if (arg0->subEnvelopeCounterA) {
            continue;
        }

        tmp = *arg0->subEnvelopeCursorA++;
        if (tmp >= 0x80u) {
            arg0->volume = tmp & 0x7F;

            tmp = *arg0->subEnvelopeCursorA++;
            if (tmp >= 0x80u) {
                arg0->subEnvelopeCounterA = (tmp & 0x7F) << 8;
                arg0->subEnvelopeCounterA += *arg0->subEnvelopeCursorA++ + 2;
            } else {
                arg0->subEnvelopeCounterA = tmp + 2;
            }
        } else {
            arg0->volume = tmp;
            arg0->subEnvelopeCounterA = 1;
        }
    } while (arg0->pitchEnvelopeTimer < arg0->playbackPosition);
}

void Audio_AdvanceVolumeEnvelope(AudioVoice* arg0) {
    u8 tmp;

    do {
        arg0->volumeEnvelopeTimer += 0x100;

        arg0->subEnvelopeCounterB--;
        if (arg0->subEnvelopeCounterB) {
            continue;
        }

        tmp = *arg0->subEnvelopeCursorB++;
        if (tmp >= 0x80) {
            arg0->pitchModulationRaw = (f32)(tmp & 0x7F) - 64.0;
            arg0->pitchModulation = arg0->pitchModulationRaw * arg0->pitchModulationScale;
            tmp = *arg0->subEnvelopeCursorB++;
            if (tmp >= 0x80) {
                arg0->subEnvelopeCounterB = (tmp & 0x7F) << 8;
                arg0->subEnvelopeCounterB += *arg0->subEnvelopeCursorB++ + 2;
            } else {
                arg0->subEnvelopeCounterB = tmp + 2;
            }
        } else {
            arg0->pitchModulationRaw = (f32)(tmp)-64.0;
            arg0->pitchModulation = arg0->pitchModulationRaw * arg0->pitchModulationScale;
            arg0->subEnvelopeCounterB = 1;
        }
    } while (arg0->volumeEnvelopeTimer < arg0->playbackPosition);
}

f32 Audio_ExpApproximation(f32 arg0) {
    f32 temp_fa1;
    f32 temp_fv1;

    if (arg0 == 0.0f) {
        return 1.0f;
    }

    if (arg0 > 0.0f) {
        temp_fv1 = SQ(arg0);
        temp_fa1 = SQ(temp_fv1);
        return (arg0 * 0.693147180559944953) + 1.0 + (temp_fv1 * 0.240226506959100999) +
               ((temp_fv1 * arg0) * 0.055504108664821597) + (temp_fa1 * 0.00961812910762848035) +
               ((temp_fa1 * arg0) * 0.00133335581464283999) + ((temp_fa1 * temp_fv1) * 0.000154035303933816006);
    } else {
        arg0 = -arg0;
        temp_fv1 = SQ(arg0);
        temp_fa1 = SQ(temp_fv1);
        return 1.0 / ((arg0 * 0.693147180559944953) + 1.0 + (temp_fv1 * 0.240226506959100999) +
                      ((temp_fv1 * arg0) * 0.055504108664821597) + (temp_fa1 * 0.00961812910762848035) +
                      ((temp_fa1 * arg0) * 0.00133335581464283999) + ((temp_fa1 * temp_fv1) * 0.000154035303933816006));
    }
}

void Audio_RelocateSoundBank(u8* arg0, s32 arg1) {
    UNUSED s32 pad;
    u32 i;
    f32 var_fv0;
    SoundBank* soundbank = arg0;
    u8 temp_v1;

    if (soundbank->flags & 1) {
        return;
    }

    soundbank->flags |= 1;

    Audio_RelocatePointerArray(&soundbank->basenote, arg0, 3);
    Audio_RelocatePointerArray(soundbank->wave_list, arg0, soundbank->count);

    for (i = 0; i < soundbank->count; i++) {
        f32* ptr = &soundbank->detune[i];

        temp_v1 = *(u8*)ptr;
        if (temp_v1 & 0x80) {
            var_fv0 = -(0x100 - temp_v1);
        } else {
            var_fv0 = (s32)temp_v1;
        }
        *ptr = var_fv0 / 100.0;

        temp_v1 = soundbank->basenote[i] - 0x30;
        if (temp_v1 & 0x80) {
            var_fv0 = -(0x100 - temp_v1);
        } else {
            var_fv0 = (s32)temp_v1;
        }
        *ptr += var_fv0;

        if (soundbank->wave_list[i]->flags == 0) {
            u32 tmp = soundbank->wave_list[i]->base;

            if ((tmp & 0xFF000000) != 0xFF000000) {
                tmp += arg1;
                soundbank->wave_list[i]->base = tmp;
            }

            soundbank->wave_list[i]->flags = 1;

            if (soundbank->wave_list[i]->waveInfo.rawWave.loop != NULL) {
                soundbank->wave_list[i]->waveInfo.rawWave.loop =
                    (u32)soundbank + (u32)soundbank->wave_list[i]->waveInfo.rawWave.loop;
            }

            if (soundbank->wave_list[i]->type == AL_ADPCM_WAVE) {
                soundbank->wave_list[i]->waveInfo.adpcmWave.book =
                    (u32)soundbank + (u32)soundbank->wave_list[i]->waveInfo.adpcmWave.book;
            }
        }
    }

    osWritebackDCacheAll();
}

s32 Audio_RandomScale(s32 arg0) {
    s32 i;
    s32 tmp;
    f32 tmpf;

    for (i = 0; i < 8; i++) {
        tmp = D_800FC7E4 & 0x48000000;
        D_800FC7E4 <<= 1;
        if ((tmp == 0x48000000) || (tmp == 0x08000000)) {
            D_800FC7E4 |= 1;
        }
    }

    tmpf = D_800FC7E4 / 65536.0f;
    tmpf /= 65536.0f;
    return arg0 * tmpf;
}

void Audio_ResetVoiceState(AudioVoice* arg0) {
    u8* ptr;
    s32 i;

    arg0->commandStreamCursor = 0;
    arg0->subVoiceInstrumentBank = 0;
    arg0->waveTable = 0;
    arg0->hardwareVoice = NULL;

    arg0->playbackPosition = 0;
    arg0->pitchEnvelopeTimer = 0;
    arg0->volumeEnvelopeTimer = 0;
    arg0->noteEndPosition = 0;
    arg0->noteStartPosition = 0;
    arg0->releaseTriggerPosition = 0;

    ptr = (u8*)&arg0->releaseTriggerPosition + 4;
    for (i = 0x34; i < 0x150; i++) {
        *ptr++ = 0;
    }

    arg0->priorityCache = 0xFFFF;
    arg0->appliedFxMix = -1;
    arg0->lastAppliedPan = -1;
    arg0->lastAppliedPitch = 99.90000153f;

    arg0->tempoIncrement = arg0->tempoTicksRaw = 0x6000 / D_800FC7D4;
    arg0->noteDuration = 1;

    arg0->velocityFromStreamEnabled = 0;
    arg0->defaultVelocity = 0x7F;

    if (1) {}

    arg0->durationOverride = 0;
    arg0->variableDurationEnabled = 0;
    arg0->volume = 0x7F;
    arg0->pan = 0x40;
    arg0->subEnvelopeCounterA = 1;
    arg0->subEnvelopeCounterB = 1;
    arg0->fadeCounter = -1;
    arg0->panModulationRate = 0;
    arg0->volumeScale = 0x80;
    arg0->currentPan = 0x80;
    arg0->tempoScale = 0x80;
    arg0->attackRate = 1;
    arg0->decayDuration = 1;
    arg0->decayLevel = 0x7F;
    arg0->sustainDuration = -1;
    arg0->pitchModulationScale = 0.03125f;

    arg0->decayRate = 1.0f;
    arg0->sustainRate = 0.003921568859f;
    arg0->pitchModulation = 0.0f;
    arg0->finePitchAccumulator = 0.0f;
    arg0->currentPitch = 0.0f;
    arg0->finePitchOffset = 0.0f;

    arg0->sustainLevel = 0x7F;
    arg0->releaseRate = 1;
    arg0->voiceIdle = 1;
    arg0->vibratoDepthRaw = 0;
    arg0->periodicPitchModDuration = 0;
    arg0->independentTempo = 0;
    arg0->voiceActive = 0;
}

s32 Audio_FindFreeVoice(AudioInstrumentBank* arg0, s32 arg1) {
    s32 i;
    AudioVoice* var_v0 = D_800FC7D0;

    for (i = 0; i < D_800FC7CC; i++, var_v0++) {
        if (var_v0->commandStreamCursor == NULL) {
            return i;
        }
    }

    return -1;
}

void Audio_FillBytes(u8* arg0, u8 arg1, u32 arg2) {
    u8* a0 = arg0;

    while (arg2--) {
        *a0++ = arg1;
    }
}

void Audio_RelocatePointerArray(u8* arg0, u32 arg1, u32 arg2) {
    u32* ptr = arg0;
    u32 i;

    for (i = 0; i < arg2; i++) {
        if (ptr[i] != 0) {
            ptr[i] += arg1;
        }
    }
}

s32 Audio_InitializeVoice(AudioVoice* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    Audio_ResetVoiceState(arg0);
    arg0->independentTempo = arg1;
    arg0->volumeScale = arg2;
    arg0->currentPan = arg3;
    arg0->priority = arg4;
    return arg0->soundId;
}

void Audio_QueueFreeVoice(AudioHardwareVoice* arg0) {
    if (D_800FC800 == NULL) {
        D_800FC800 = D_800FC804 = arg0;
        arg0->node.next = NULL;
    } else {
        D_800FC804->node.next = arg0;
        arg0->node.next = NULL;
        D_800FC804 = arg0;
    }

    arg0->envelopeStage = 5;
    D_800FC818++;
}

AudioHardwareVoice* Audio_DequeueFreeVoice(void) {
    AudioHardwareVoice* temp_v0;

    if (D_800FC800 == NULL) {
        return NULL;
    }

    temp_v0 = D_800FC800;
    D_800FC800 = D_800FC800->node.next;

    D_800FC818--;
    return temp_v0;
}

void Audio_QueueActiveVoice(AudioHardwareVoice* arg0) {
    if (D_800FC808 == NULL) {
        D_800FC808 = D_800FC80C = arg0;
        arg0->node.next = NULL;
        arg0->node.prev = NULL;
    } else {
        D_800FC80C->node.next = arg0;
        arg0->node.next = NULL;
        arg0->node.prev = D_800FC80C;
        D_800FC80C = arg0;
    }

    D_800FC81C++;
}

void Audio_ReclaimExpiredVoices(void) {
    AudioHardwareVoice* temp_s1;
    AudioHardwareVoice* var_s0;

    if (D_800FC808 != NULL) {
        var_s0 = D_800FC808;

        do {
            if (var_s0->expirationTime < D_800FC7F8) {
                n_alSynStopVoice(&var_s0->voice);
                n_alSynFreeVoice(&var_s0->voice);

                var_s0->envelopeStage = 5;
                if (var_s0->node.prev != NULL) {
                    if (var_s0->node.next != NULL) {
                        var_s0->node.prev->next = var_s0->node.next;
                        var_s0->node.next->prev = var_s0->node.prev;
                    } else {
                        var_s0->node.prev->next = NULL;
                        D_800FC80C = var_s0->node.prev;
                    }
                } else if (var_s0->node.next != NULL) {
                    D_800FC808 = var_s0->node.next;
                    if (!var_s0) {}
                    D_800FC808->node.prev = NULL;
                } else {
                    D_800FC808 = D_800FC80C = NULL;
                }
                temp_s1 = var_s0->node.next;
                D_800FC81C -= 1;
                Audio_QueueFreeVoice(var_s0);
                var_s0 = temp_s1;
            } else {
                var_s0 = var_s0->node.next;
            }
        } while (var_s0 != NULL);
    }
}
