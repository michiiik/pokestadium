#ifndef _38BB0_H_
#define _38BB0_H_

#include "global.h"
#include "src/libnaudio/n_libaudio_sc.h"
#include "src/libnaudio/n_libaudio_sn_sc.h"
#include "src/libnaudio/n_synthInternals.h"

typedef struct sp38_func_800373D8 {
    /* 0x00 */ u32 voiceCount; // Audio_InitializeVoiceManager: AudioVoice pool size
    /* 0x04 */ u32 virtualVoiceCount; // ALSynConfig.maxVVoices; also AudioHardwareVoice pool size
    /* 0x08 */ u32 physicalVoiceCount; // ALSynConfig.maxPVoices
    /* 0x0C */ void* heapBase; // alHeapInit's arg
    /* 0x10 */ s32 heapSize;
    /* 0x14 */ u32 maxUpdates; // ALSynConfig.maxUpdates
    /* 0x18 */ u32 outputRate; // osAiSetFrequency's arg; amConfig.outputRate
    /* 0x1C */ u32 maxAcmdSize; // amConfig.maxACMDSize
    /* 0x20 */ u32 framesPerField; // amConfig.framesPerField
    /* 0x24 */ u32 numDmaBuffers; // amCreateAudioMgr's arg2
    /* 0x28 */ u32 unk_28; // amCreateAudioMgr's arg3
    /* 0x2C */ u32 commandQueueSize; // gAudioCommandQueue's alHeapDBAlloc size
} sp38_func_800373D8; // size >= 0x30

typedef struct unk_D_800FC7D0_08C_010 {
    /* 0x00 */ char unk00[0x7];
} unk_D_800FC7D0_08C_010; // size = 0x7

typedef struct unk_D_800FC7D0_08C_008 {
    /* 0x00 */ u8 unk_00[4];
} unk_D_800FC7D0_08C_008; // size = 0x4

typedef struct AudioInstrumentBank {
    /* 0x00 */ u32 unk_00; // base address that subEnvelopePointerOffsets entries are added to (Audio_SetVoiceSubEnvelopePointer)
    /* 0x04 */ u32* commandData; // per-index command-stream pointers, indexed by note/patch id; mirrored into commandStreamCursor/Base
    /* 0x08 */ unk_D_800FC7D0_08C_008** subEnvelopeDataA; // mirrored into subEnvelopeCursorA/BaseA
    /* 0x0C */ u32* subEnvelopeDataB; // mirrored into subEnvelopeCursorB/BaseB
    /* 0x10 */ unk_D_800FC7D0_08C_010* volumeEnvelopeTable; // preset envelopes indexed by Audio_SetVoiceEnvelopeFromTable
    /* 0x14 */ s32* subEnvelopePointerOffsets; // Audio_SetVoiceSubEnvelopePointer's per-index offset from unk_00
    /* 0x18 */ char unk18[0x10];
    /* 0x28 */ f32* basePitchTable; // per-sample base pitch, indexed by sampleIndex (added to the portamento target in Audio_ProcessVoiceCommands)
    /* 0x2C */ u32* waveTableTable; // per-sample ALWaveTable handle, indexed by sampleIndex, feeds AudioVoice.waveTable
} AudioInstrumentBank; // size >= 0x18

typedef struct unk_D_800FC7D0_140 {
    /* 0x00 */ char unk00[0x8];
    /* 0x08 */ unk_D_800FC7D0_08C_008** unk_08;
    /* 0x0C */ u32* subVoiceTable; // indexed by Audio_AllocateSubVoiceFromCommand's decoded sub-voice id
} unk_D_800FC7D0_140; // size >= 0x10

typedef struct AudioHardwareVoice {
    /* 0x00 */ ALLink node;
    /* 0x08 */ N_ALVoice voice;
    /* 0x24 */ s32 expirationTime; // scheduled tick (D_800FC7F8 + delay [+1]) at which Audio_ReclaimExpiredVoices frees this hardware voice
    /* 0x28 */ u16 releaseRate; // mirror of AudioVoice.releaseRate, read by n_alSynSetVol's fade-out calc in Audio_ReleaseVoice
    /* 0x2A */ u8 envelopeStage; // mirror of AudioVoice.envelopeStage
} AudioHardwareVoice; // size = 0x2C

typedef struct AudioVoice {
    /* 0x000 */ struct AudioVoice* unk_000;
    /* 0x004 */ char unk004[0x4];
    /* 0x008 */ u64 playbackPosition; // master Q8 fixed-point tick counter, advanced by tempoIncrement every process step
    /* 0x010 */ u64 pitchEnvelopeTimer; // advances by 0x100 in Audio_AdvancePitchEnvelope, compared against playbackPosition
    /* 0x018 */ u64 volumeEnvelopeTimer; // advances by 0x100 in Audio_AdvanceVolumeEnvelope, compared against playbackPosition
    /* 0x020 */ u64 noteEndPosition; // playbackPosition value at which the current command-stream note ends
    /* 0x028 */ u64 noteStartPosition; // playbackPosition value when the current note began (previous noteEndPosition)
    /* 0x030 */ u64 releaseTriggerPosition; // playbackPosition threshold that triggers Audio_ReleaseVoice's note-off handling
    /* 0x038 */ u8* commandStreamCursor; // active read position in the voice's command byte-stream
    /* 0x03C */ s32 waveTable; // ALWaveTable handle for the active sample, from AudioInstrumentBank.waveTableTable[sampleIndex]; NULL = no active note, passed directly to n_alSynStartVoiceParams
    /* 0x040 */ s32 fadeCounter; // Audio_FadeSoundHandler/FadeCategoryHandler; -1 disables the fade scale in Audio_ComputeVoiceVolume
    /* 0x044 */ s32 fadeDuration; // Audio_FadeSoundHandler/FadeCategoryHandler; divisor for fadeCounter in Audio_ComputeVoiceVolume
    /* 0x048 */ f32 vibratoDepth; // Audio_Enable/DisableVoiceVibrato*; scaled semitone depth (raw byte / 50), consumed by Audio_ComputeSinePitchModulation
    /* 0x04C */ f32 pitchModulation; // vibrato contribution added into Audio_AdvanceVoicePitch's total
    /* 0x050 */ f32 lastAppliedPitch; // change-detection cache in Audio_AdvanceVoicePitch
    /* 0x054 */ f32 basePitch; // Audio_AdvanceVoicePitch's slide target; also read directly by Audio_EnableVoicePitchSlide
    /* 0x058 */ f32 finePitchAccumulator; // running total adjusted by finePitchOffset deltas in Audio_SetVoiceFinePitch
    /* 0x05C */ u8* subEnvelopeCursorB; // Audio_AdvanceVolumeEnvelope; consumes duration/value pairs from subEnvelopeBaseB
    /* 0x060 */ u8* subEnvelopeCursorA; // Audio_AdvancePitchEnvelope; consumes duration/value pairs from subEnvelopeBaseA -- despite the function's name, this stream writes into `volume`, not a pitch field; the pitch/volume-envelope function naming from an earlier session may be swapped, not verified further this pass
    /* 0x064 */ s32 soundId; // external command-targeting id (Audio_Fade*/ApplyVoice*Command match against this to route a command to the right voice)
    /* 0x067 */ s32 priority; // n_alSynSetPriority/StartVoiceParams priority value, Audio_InitializeVoice's arg4
    /* 0x06C */ f32 portamentoStartPitch; // Audio_AdvanceVoicePitch's slide-from value
    /* 0x070 */ f32 currentPitch; // Audio_AdvanceVoicePitch's live interpolated portamento output
    /* 0x074 */ f32 decayRate; // Audio_SetVoiceVolumeEnvelope / Audio_ReleaseVoice envelope processing
    /* 0x078 */ f32 sustainRate; // Audio_SetVoiceVolumeEnvelope / Audio_ReleaseVoice envelope processing
    /* 0x07C */ s32 attackIncrement; // Audio_SetVoiceVolumeEnvelope: 0x400 / attackRate
    /* 0x080 */ f32 vibratoOutput; // live sine-wave output of Audio_ComputeSinePitchModulation, cleared by Audio_DisableVoiceVibrato
    /* 0x084 */ f32 pitchModulationScale; // multiplier applied to pitchModulationRaw to produce pitchModulation
    /* 0x088 */ f32 pitchModulationRaw; // set from a sub-envelope stream byte (-64 offset) or loopSavedPitchScale; pitchModulation = pitchModulationRaw * pitchModulationScale
    /* 0x08C */ AudioInstrumentBank* instrumentBank; // Audio_InitializeVoice/AllocateVoiceFromCommand
    /* 0x090 */ AudioInstrumentBank* subVoiceInstrumentBank; // propagated to sub-voices alongside subVoiceContext
    /* 0x094 */ u8* commandStreamBase; // base pointer mirrored into commandStreamCursor at voice init
    /* 0x098 */ u8* subEnvelopePointer; // Audio_Set/ClearVoiceSubEnvelopePointer
    /* 0x09C */ u8* subEnvelopeBaseB; // base pointer for subEnvelopeCursorB, set by Audio_SetVoiceEnvelopePointers
    /* 0x0A0 */ u8* subEnvelopeBaseA; // base pointer for subEnvelopeCursorA, set by Audio_SetVoiceEnvelopePointers
    /* 0x0A4 */ f32 finePitchOffset; // Audio_SetVoiceFinePitch
    /* 0x0A8 */ u32 panModulationTimer; // advances by 0x100 per tick in Audio_AdvancePanModulation
    /* 0x0AC */ s16 tempoScale; // Q7 fixed-point per-voice tempo multiplier, default 0x80 (1.0)
    /* 0x0AE */ u16 noteDuration; // decoded from the command stream or durationOverride; 0x7FFF = sustain/hold
    /* 0x0B0 */ u16 tempoIncrement; // added to playbackPosition every tick; = (tempoTicksRaw * tempoScale) >> 7
    /* 0x0B2 */ s16 volumeScale; // Audio_ApplyVoiceVolumeScaleCommand; multiplies into Audio_ComputeVoiceVolume's total, default 0x80 (Q7 1.0)
    /* 0x0B4 */ u16 priorityCache; // change-detection cache for the effective priority passed to n_alSynStartVoiceParams
    /* 0x0B6 */ u16 subEnvelopeCounterA; // hold-ticks countdown before Audio_AdvancePitchEnvelope reads the next subEnvelopeCursorA value
    /* 0x0B8 */ u16 subEnvelopeCounterB; // hold-ticks countdown before Audio_AdvanceVolumeEnvelope reads the next subEnvelopeCursorB value
    /* 0x0BA */ u16 independentTempo; // when set, Audio_SetVoiceSequenceTempo updates only this voice instead of every voice sharing the same sequence
    /* 0x0BC */ u16 tempoTicksRaw; // unscaled base tempo shared by every voice of the same sequence
    /* 0x0BE */ u16 ticksSinceNoteStart; // = (playbackPosition - noteStartPosition) >> 8, a general elapsed-time counter reused by the portamento and vibrato calculations
    /* 0x0C0 */ u16 durationOverride; // Audio_SetVoiceDurationOverride
    /* 0x0C2 */ u16 sampleIndex; // Audio_SetVoiceSampleIndex
    /* 0x0C4 */ s16 currentPan; // live pan-modulation output, oscillates 0-0xFE, 0x80 = center
    /* 0x0C6 */ u16 fixedDuration; // Audio_SetVoiceFixedDuration; overrides releaseDuration's calc in Audio_ReleaseVoice when nonzero
    /* 0x0C8 */ u16 releaseDuration; // Audio_SetVoiceReleaseDuration
    /* 0x0CA */ u8 vibratoRate; // Audio_Enable/DisableVoiceVibrato*
    /* 0x0CB */ u8 variableDurationEnabled; // Audio_EnableVoiceVariableDuration
    /* 0x0CC */ u8 portamentoDuration; // Audio_Enable/DisableVoicePitchSlide; despite the boolean-looking setter, Audio_AdvanceVoicePitch uses it as a tick-count divisor -- renamed from the session's earlier, less accurate 'pitchSlideEnabled' guess
    /* 0x0CD */ s8 detune; // Audio_SetVoiceDetune
    /* 0x0CE */ u8 detuneOnce; // Audio_ApplyVoiceDetuneOnce
    /* 0x0CF */ u8 currentVelocity; // read from the command stream when velocityFromStreamEnabled, else copied from defaultVelocity
    /* 0x0D0 */ u8 volume; // Audio_SetVoiceVolume/SetRandomVoiceVolume
    /* 0x0D1 */ u8 pan; // Audio_SetVoicePan/SetRandomVoicePan
    /* 0x0D2 */ u8 lastAppliedPan; // change-detection cache for the combined pan*panModulation output, reset to 0xFF/-1 to force a recompute
    /* 0x0D3 */ u8 attackRate; // Audio_SetVoiceVolumeEnvelope
    /* 0x0D4 */ u8 attackStartLevel; // always reset to 0; the floor decayRate ramps from
    /* 0x0D5 */ u8 decayLevel; // target level at the end of the decay ramp (Audio_ReleaseVoice stage 1->2)
    /* 0x0D6 */ u8 sustainLevel; // target level at the end of the sustain ramp (Audio_ReleaseVoice stage 2->3)
    /* 0x0D7 */ u8 envelopeStage; // 1=decay, 2=sustain-hold; distinct from the underlying voice's own hardwareVoice->envelopeStage counter
    /* 0x0D8 */ u8 currentLevel; // the live computed envelope output, recalculated every tick in Audio_ReleaseVoice
    /* 0x0D9 */ u8 attackRateSnapshot; // copy of attackRate taken when the envelope resets
    /* 0x0DA */ u8 decayDuration; // tick-count threshold for the decay stage
    /* 0x0DB */ u8 sustainDuration; // tick-count threshold for the sustain-hold stage; -1 (0xFF) = hold forever until note-off
    /* 0x0DC */ u32 voiceIdle; // 1 = no hardware voice attached (freed/not yet started); complements voiceActive
    /* 0x0E0 */ u8 releaseRate; // applied to the underlying voice's hardwareVoice->releaseRate at note-off (Audio_ReleaseVoice), a separate mechanism from the attack/decay/sustain ramp above
    /* 0x0E1 */ u8 voiceActive; // 1 = hardware voice started and playing; complements voiceIdle
    /* 0x0E2 */ u8 fxMix; // Audio_SetVoiceFxMix
    /* 0x0E3 */ u8 fxMixOverride; // Audio_ApplyVoiceFxMixCommand's per-soundId target, blended toward fxMix each tick
    /* 0x0E4 */ u8 appliedFxMix; // change-detection cache for the value last sent to n_alSynSetFXMix
    /* 0x0E5 */ u8 periodicPitchModDuration; // Audio_SetVoicePeriodicPitchModulation: tick-count for the 'depth applied' half-cycle
    /* 0x0E6 */ u8 periodicPitchModDurationOff; // tick-count for the 'no depth' half-cycle
    /* 0x0E7 */ u8 periodicPitchModCounter; // countdown timer, Audio_AdvancePeriodicPitchModulation
    /* 0x0E8 */ s8 periodicPitchModOutput; // alternates between 0 and periodicPitchModDepth, Audio_AdvancePeriodicPitchModulation
    /* 0x0E9 */ u8 velocityFromStreamEnabled; // Audio_Enable/DisableVoiceVelocityFromStream
    /* 0x0EA */ u8 defaultVelocity; // Audio_SetVoiceDefaultVelocity
    /* 0x0EB */ u8 panModulationRate; // Audio_SetVoicePanModulation; 0 disables modulation and resets currentPan to center
    /* 0x0EC */ u8 vibratoDepthRaw; // Audio_Enable/DisableVoiceVibrato*
    /* 0x0ED */ u8 unk_0ED;
    /* 0x0EE */ u8 unk_0EE;
    /* 0x0EF */ s8 periodicPitchModDepth; // Audio_SetVoicePeriodicPitchModulation
    /* 0x0F0 */ u8 panModulationPhase; // fractional phase accumulator, 0-0x7F
    /* 0x0F1 */ u8 panModulationDirection; // 0 = rising, 1 = falling
    /* 0x0F2 */ u8 loopStackDepth; // Audio_Push/PopVoiceLoopState
    /* 0x0F4 */ u8* loopReturnPtr[4]; // command-stream position saved by Audio_PushVoiceLoopState, restored on loop-back
    /* 0x104 */ u8* loopSavedEnvPtrA[4]; // saved/restored copy of unk_060 across a loop iteration
    /* 0x114 */ u8* loopSavedEnvPtrB[4]; // saved/restored copy of unk_05C across a loop iteration
    /* 0x124 */ u16 loopSavedEnvPtrAValid[4]; // saved/restored copy of unk_0B6 across a loop iteration
    /* 0x12C */ u16 loopSavedEnvPtrBValid[4]; // saved/restored copy of unk_0B8 across a loop iteration
    /* 0x134 */ u8 loopCounter[4]; // remaining-iterations countdown per loop-stack level; 0xFF = infinite
    /* 0x138 */ u8 loopSavedVolume[4]; // saved/restored copy of volume across a loop iteration
    /* 0x13C */ u8 loopSavedPitchScale[4]; // saved/restored copy of pitchModulationRaw across a loop iteration
    /* 0x140 */ unk_D_800FC7D0_140* subVoiceContext; // holds subVoiceTable; propagated to sub-voices with subVoiceInstrumentBank
    /* 0x144 */ char unk144[0x4];
    /* 0x148 */ AudioHardwareVoice* hardwareVoice;
    /* 0x14C */ char unk14C[0x4];
} AudioVoice; // size = 0x150

typedef struct unk_func_800397BC {
    /* 0x00 */ u32 num_entries;
    /* 0x04 */ u32 flags;
    /* 0x08 */ u32* unk_offset;
    /* 0x0C */ u32* entries_offset;
} unk_func_800397BC; // size = 0x10

extern ALHeap* D_800FC810;
extern u32 D_800FC814;
extern s32 D_800FC818;
extern s32 D_800FC81C;

u8* Audio_ClearVoicePlaybackState(AudioVoice* arg0, UNUSED u8* arg1);
u8* Audio_SetVoiceSampleIndex(AudioVoice* arg0, u8* arg1);
u8* Audio_EnableVoicePitchSlide(AudioVoice* arg0, u8* arg1);
u8* Audio_DisableVoicePitchSlide(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceVolumeEnvelope(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceSequenceTempo(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceReleaseDuration(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceFixedDuration(AudioVoice* arg0, u8* arg1);
u8* Audio_EnableVoiceVibrato(AudioVoice* arg0, u8* arg1);
u8* Audio_EnableVoiceVibratoDown(AudioVoice* arg0, u8* arg1);
u8* Audio_DisableVoiceVibrato(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceDurationOverride(AudioVoice* arg0, u8* arg1);
u8* Audio_EnableVoiceVariableDuration(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceDetune(AudioVoice* arg0, u8* arg1);
u8* Audio_ApplyVoiceDetuneOnce(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceFinePitch(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceEnvelopeFromTable(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceFlagED(AudioVoice* arg0, u8* arg1);
u8* Audio_ClearVoiceFlagED(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceFlagEE(AudioVoice* arg0, u8* arg1);
u8* Audio_ClearVoiceFlagEE(AudioVoice* arg0, u8* arg1);
u8* Audio_PushVoiceLoopState(AudioVoice* arg0, u8* arg1);
u8* Audio_PopVoiceLoopState(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoicePeriodicPitchModulation(AudioVoice* arg0, u8* arg1);
u8* Audio_DisableVoicePeriodicPitchModulation(AudioVoice* arg0, u8* arg1);
u8* Audio_EnableVoiceVelocityFromStream(AudioVoice* arg0, u8* arg1);
u8* Audio_DisableVoiceVelocityFromStream(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceDefaultVelocity(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoicePan(AudioVoice* arg0, u8* arg1);
u8* Audio_SkipVoiceCommandWord(UNUSED AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceSubEnvelopePointer(AudioVoice* arg0, u8* arg1);
u8* Audio_ClearVoiceSubEnvelopePointer(AudioVoice* arg0, u8* arg1);
u8* Audio_SkipVoiceCommandByte(UNUSED AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceEnvelopePointers(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceFxMix(AudioVoice* arg0, u8* arg1);
u8* Audio_SetRandomVoiceDetune(AudioVoice* arg0, u8* arg1);
u8* Audio_SetRandomVoiceVolume(AudioVoice* arg0, u8* arg1);
u8* Audio_SetRandomVoicePan(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceVolume(AudioVoice* arg0, u8* arg1);
u8* Audio_AllocateSubVoiceFromCommand(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoiceVolumeScale(AudioVoice* arg0, u8* arg1);
u8* Audio_SetVoicePanModulation(AudioVoice* arg0, u8* arg1);
u8* Audio_AllocateVoiceFromCommand(AudioVoice* arg0, u8* arg1);
void Audio_WriteU32BE(u8* arg0, u32 arg1);
void Audio_WriteU16BE(u8* arg0, u32 arg1);
s32 Audio_InitializeVoiceManager(sp38_func_800373D8* arg0);
void Audio_QueueSetVolumeCommand(s32 arg0, u32 arg1);
s32 Audio_PlayMusicSequence(SoundBank* arg0, s32 arg1);
s32 Audio_PlaySoundEffect(SoundBank* arg0, s32 arg1, s32 arg2, s32 arg3, u32 arg4, s32 arg5);
void Audio_QueueFadeCategoryCommand(s32 arg0, u32 arg1);
s32 Audio_CountActiveSoundsByCategory(s32 arg0);
void Audio_QueueFadeSoundCommand(s32 arg0, u32 arg1);
s32 Audio_IsSoundPlaying(s32 arg0);
void Audio_QueueVoiceVolumeScale(s32 arg0, u32 arg1);
void Audio_QueueVoicePan(s32 arg0, u16 arg1);
void Audio_QueueVoiceDetune(s32 arg0, f32 arg1);
void Audio_QueueVoiceVolume(s32 arg0, s32 arg1);
void Audio_QueueVoiceVolumeByte(u32 arg0, s32 arg1);
void Audio_RelocateSoundBankWrapper(s32 arg0, s32 arg1);
void Audio_RelocateSoundBankTable(unk_func_800397BC* arg0);
void Audio_QueueSetMonoOutputCommand(s32 arg0);
void Audio_SetVoiceStartTimingMode(s32 arg0);
void Audio_StopAllSoundEffects(void);
s32 Audio_PlayMusicSequenceLooped(SoundBank* arg0, s32 arg1, s32 arg2);
s32 Audio_StartVolumeTransition(s32 arg0, u8 arg1, u8 arg2, u32 arg3);
u32 Audio_ReadU32BE(u8* arg0);
u16 Audio_ReadU16BE(u8* arg0);
void Audio_ProcessCommandQueue(void);
u8* Audio_PlayMusicSequenceHandler(u8* arg0);
u8* Audio_PlayMusicSequenceLoopedHandler(u8* arg0);
u8* Audio_PlaySoundEffectHandler(u8* arg0);
u8* Audio_ApplyVoiceDetuneCommand(u8* arg0);
u8* Audio_ApplyVoiceVolumeCommand(u8* arg0);
u8* Audio_ApplyVoiceVolumeScaleCommand(u8* arg0);
u8* Audio_ApplyVoicePanCommand(u8* arg0);
u8* Audio_ApplyVoiceFxMixCommand(u8* arg0);
u8* Audio_FadeSoundHandler(u8* arg0);
u8* Audio_FadeCategoryHandler(u8* arg0);
u8* Audio_SetChannelVolumeHandler(u8* arg0);
u8* Audio_SetMonoOutputHandler(u8* arg0);
s32 Audio_PlayerFrameHandler(void* arg0);
void Audio_DecodeVoiceCommandStream(AudioVoice* arg0);
s32 Audio_ComputeVoiceVolume(AudioVoice* arg0);
f32 Audio_AdvanceVoicePitch(AudioVoice* arg0, f32 arg1);
void Audio_UpdateVoicePlaybackEnd(AudioVoice* arg0);
void Audio_ReleaseVoice(AudioVoice* arg0);
void Audio_ResetPanModulation(AudioVoice* arg0);
void Audio_AdvancePanModulation(AudioVoice* arg0);
f32 Audio_AdvancePeriodicPitchModulation(AudioVoice* arg0);
f32 Audio_ComputeSinePitchModulation(AudioVoice* arg0);
void Audio_AdvancePitchEnvelope(AudioVoice* arg0);
void Audio_AdvanceVolumeEnvelope(AudioVoice* arg0);
f32 Audio_ExpApproximation(f32 arg0);
void Audio_RelocateSoundBank(u8* arg0, s32 arg1);
s32 Audio_RandomScale(s32 arg0);
void Audio_ResetVoiceState(AudioVoice* arg0);
s32 Audio_FindFreeVoice(AudioInstrumentBank* arg0, s32 arg1);
void Audio_FillBytes(u8* arg0, u8 arg1, u32 arg2);
void Audio_RelocatePointerArray(u8* arg0, u32 arg1, u32 arg2);
s32 Audio_InitializeVoice(AudioVoice* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
void Audio_QueueFreeVoice(AudioHardwareVoice* arg0);
AudioHardwareVoice* Audio_DequeueFreeVoice(void);
void Audio_QueueActiveVoice(AudioHardwareVoice* arg0);
void Audio_ReclaimExpiredVoices(void);

#endif // _38BB0_H_
