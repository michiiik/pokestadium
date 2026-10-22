#include "gb_audio.h"
#include "gb_audio_render.h"

typedef struct EnvelopeState {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
    /* 0x03 */ u8 unk_03;
    /* 0x04 */ u8 unk_04;
} EnvelopeState; // size = 0x5

typedef struct PitchSlideState {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ u8 unk_0C;
} PitchSlideState; // size = 0x10

static u8 D_800FCD40[8];
static u8 gChannelActiveFlags[8];
static u8 gChannelWaitCounters[8];
static u8 gChannelStatus[8];
static u8 gChannelControl[8];
static u8 gTrackTransitionFlag;
static EnvelopeState gEnvelopeState[8];
static PitchSlideState gPitchSlideState[8];
static u8 gChannelDutyRotationValues[8];
static u8 gChannelDutyValues[8];
static u8 gChannelVolumeValues[8];
static u8 gChannelJumpCounters[8];
static u8 gChannelOctaves[8];
static u8 gMasterVolume[8];
static u8* gChannelProgramCounters[8];
static s32 gChannelPrevProgramCounters[8];
static s32 gChannelDataBases[8];
static u8 gChannelNoteLengths[8];
static u16 gBpmLow;
static u16 gBpmHigh;
static u8 gCurrentTrackId;
static u8 gPitchModifiers[2];
static u8* gCustomWaveforms[2];
static u8 gChannelNoteOffsets[8];
static u32 gCurrentTrackData;
static u8 gTrackPitchOffset;
static u8 gTrackBpmOffset;

static u8 gChannelPanLow[] = {
    0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88,
};
static u8 gChannelPanHigh[] = {
    0xEE, 0xDD, 0xBB, 0x77, 0xEE, 0xDD, 0xBB, 0x77,
};
static u8 gTrackRangeStart = 0;
static u8 gTrackRangeEnd = 0xF0;
static u8 gTrackIdLimit = 0xF1;
static u8 gTrackIdMax = 0xFF;
static u8 gTrackMinId = 0;
static u8 gTrackLoadStatus = 0;
static u8 gDefaultSequenceEnd = 0xFF;

void GbMusic_UpdateChannels(void) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (gChannelActiveFlags[i] != 0) {
            GbMusic_UpdateChannel(i);
        }
    }
}

void GbMusic_StartSequenceById(u8 arg0) {
    gCurrentTrackId = arg0;
    if (arg0 == 0xFF) {
        GbMusic_ResetApu();
    } else if ((gTrackIdLimit >= arg0) || (gTrackIdMax < arg0)) {
        GbMusic_StartSfx(arg0);
    } else {
        GbMusic_StartMusic(arg0);
    }
}

s32 GbMusic_StartTableSequence(u16 arg0) {
    s32 a;
    s32 offset;

    if (GbMusic_CountActiveHighChannels() != 0) {
        return 0;
    }

    Music_ResolveTrackOffsets(gAudioSfxBankExtra);

    if (gAudioSfxBankExtra->num_entries < arg0) {
        return 0;
    }

    offset = gAudioSfxBankExtra->offset;
    GbMusic_ResetApu();

    arg0--;

    a = gAudioSfxBankExtra->entries[arg0].unk_00;
    gCurrentTrackId = a;
    gTrackPitchOffset = gAudioSfxBankExtra->entries[arg0].unk_02;
    gTrackBpmOffset = gAudioSfxBankExtra->entries[arg0].unk_03;
    gCurrentTrackData = offset + a * 0x18;

    GbMusic_StartSfx(a);

    return 1;
}

void GbMusic_UpdateChannel(u32 arg0) {
    if (gChannelWaitCounters[arg0] < 2) {
        gEnvelopeState[arg0].unk_03 = gEnvelopeState[arg0].unk_04;
        gChannelStatus[arg0] &= (u8)~0x30;
        GbMusic_DecodeChannelCommands(arg0);
        return;
    }

    gChannelWaitCounters[arg0]--;

    if ((arg0 < 4) && (gChannelActiveFlags[arg0 + 4] != 0)) {
        return;
    }

    if (gChannelStatus[arg0] & 0x40) {
        GbMusic_RotateChannelDuty(arg0);
    }

    if ((gChannelControl[arg0] & 1) || (gChannelStatus[arg0] & 4)) {
        if (gChannelStatus[arg0] & 0x10) {
            GbMusic_AdvancePitchSlide(arg0);
        } else if (gEnvelopeState[arg0].unk_03 != 0) {
            gEnvelopeState[arg0].unk_03--;
        } else if (gEnvelopeState[arg0].unk_00 != 0) {
            if (gEnvelopeState[arg0].unk_01 & 0xF) {
                gEnvelopeState[arg0].unk_01--;
            } else {
                u8 sp2F = gEnvelopeState[arg0].unk_01;
                sp2F = (sp2F << 4) | ((sp2F >> 4) & 0xF);

                gEnvelopeState[arg0].unk_01 |= sp2F;

                if ((gChannelStatus[arg0] & 8) != 0) {
                    sp2F = gEnvelopeState[arg0].unk_00 & 0xF;
                    gChannelStatus[arg0] &= ~8;
                    if (gEnvelopeState[arg0].unk_02 < sp2F) {
                        sp2F = gChannelStatus[arg0] * 0;
                    } else {
                        sp2F = gEnvelopeState[arg0].unk_02 - sp2F;
                    }
                } else {
                    sp2F = (gEnvelopeState[arg0].unk_00 & 0xF0) >> 4;
                    gChannelStatus[arg0] &= 8;
                    if ((0xFF - sp2F) < gEnvelopeState[arg0].unk_02) {
                        sp2F = 0xFF;
                    } else {
                        sp2F += gEnvelopeState[arg0].unk_02;
                    }
                }
                GbAudio_QueueRegisterWrite(GbMusic_GetChannelRegister(arg0, 3), sp2F, 0);
            }
        }
    }
}

void GbMusic_DecodeChannelCommands(u32 arg0) {
    u32 var_v0;
    u8 temp_v0;

    do {
        temp_v0 = GbMusic_ReadStreamByte(arg0);
        switch (temp_v0) {
            case 0xFF:
                var_v0 = GbMusic_CmdEndOrReturn(arg0);
                break;
            case 0xFD:
                var_v0 = GbMusic_CmdCall(arg0);
                break;
            case 0xFE:
                var_v0 = GbMusic_CmdLoop(arg0);
                break;
            case 0xE8:
                var_v0 = GbMusic_CmdToggleFinePitch(arg0);
                break;
            case 0xEA:
                var_v0 = GbMusic_CmdSetEnvelope(arg0);
                break;
            case 0xEB:
                var_v0 = GbMusic_CmdPitchSlide(arg0);
                break;
            case 0xEC:
                var_v0 = GbMusic_CmdSetDutyCycle(arg0);
                break;
            case 0xED:
                var_v0 = GbMusic_CmdSetTempo(arg0);
                break;
            case 0xEE:
                var_v0 = GbMusic_CmdSetPanMask(arg0);
                break;
            case 0xEF:
                var_v0 = GbMusic_CmdPlaySequence(arg0);
                break;
            case 0xFC:
                var_v0 = GbMusic_CmdSetDutyRotation(arg0);
                break;
            case 0xF0:
                var_v0 = GbMusic_CmdSetMasterVolume(arg0);
                break;
            case 0xF8:
                var_v0 = GbMusic_CmdEnableEnvelope(arg0);
                break;
            default:
                switch (temp_v0 & 0xF0) {
                    case 0xD0:
                        var_v0 = GbMusic_CmdSetNoteLength(arg0, temp_v0);
                        break;
                    case 0xE0:
                        var_v0 = GbMusic_CmdSetOctave(arg0, temp_v0);
                        break;
                    case 0x20:
                        var_v0 = GbMusic_CmdPlayNoteWithDuty(arg0, temp_v0);
                        break;
                    default:
                        var_v0 = GbMusic_CmdPlayNote(arg0, temp_v0);
                        break;
                }
                break;
        }
    } while (!var_v0 ^ 0);
}

u8 GbMusic_CmdEndOrReturn(u32 arg0) {
    if (gChannelStatus[arg0] & 2) {
        gChannelStatus[arg0] &= ~2;
        gChannelPrevProgramCounters[arg0] = gChannelProgramCounters[arg0];
        return 0;
    }

    if (arg0 >= 3) {
        gChannelStatus[arg0] &= ~4;
        gChannelControl[arg0] &= ~1;
        if (arg0 == 6) {
            GbAudio_QueueRegisterWrite(0xFF1A, 0, 0);
            GbAudio_QueueRegisterWrite(0xFF1A, 0x80, 0);
        }

        if (gTrackTransitionFlag != 0) {
            gTrackTransitionFlag = 0;
        }
        GbAudio_QueueRegisterWrite(0xFF25, gChannelPanHigh[arg0] & GbAudio_GetRegisterValue(0xFF25), 0);
    }

    gChannelActiveFlags[arg0] = 0;

    if ((gChannelActiveFlags[4] >= gTrackRangeStart) && (gChannelActiveFlags[4] < gTrackRangeEnd)) {
        if ((arg0 != 4) && (GbMusic_RewindIfSfxActive(arg0) != 0)) {
            return 1;
        } else {
            GbAudio_QueueRegisterWrite(0xFF24, gMasterVolume[0], 0);
            gMasterVolume[0] = 0;
            gChannelActiveFlags[arg0] = 0;
        }
    }
    return 1;
}

u8 GbMusic_CmdCall(u32 arg0) {
    u8 sp1F;
    u8 temp_v0;
    s32 tmp;

    sp1F = GbMusic_ReadStreamByte(arg0);
    temp_v0 = GbMusic_ReadStreamByte(arg0);
    tmp = (temp_v0 << 8) + sp1F;

    gChannelPrevProgramCounters[arg0] = gChannelProgramCounters[arg0];
    gChannelProgramCounters[arg0] = tmp + gChannelDataBases[arg0];
    gChannelStatus[arg0] |= 2;
    return 0;
}

u8 GbMusic_CmdLoop(u32 arg0) {
    u8 sp27;
    u8 sp26;
    u32 temp_v0 = GbMusic_ReadStreamByte(arg0);

    if (temp_v0) {
        if (gChannelJumpCounters[arg0] == temp_v0) {
            gChannelJumpCounters[arg0] = 1;
            GbMusic_ReadStreamByte(arg0);
            GbMusic_ReadStreamByte(arg0);
            return 0;
        }
        gChannelJumpCounters[arg0]++;
    }

    sp27 = GbMusic_ReadStreamByte(arg0);
    sp26 = GbMusic_ReadStreamByte(arg0);

    gChannelProgramCounters[arg0] = (u8*)gChannelDataBases + ((sp26 << 8) | sp27);
    return 0;
}

u8 GbMusic_CmdToggleFinePitch(u32 arg0) {
    gChannelStatus[arg0] ^= 1;
    return 0;
}

u8 GbMusic_CmdSetEnvelope(u32 arg0) {
    u8 temp_a0;
    u8 temp_a1;
    u8 temp_v0;
    u8 temp_t9;

    temp_v0 = GbMusic_ReadStreamByte(arg0);
    gEnvelopeState[arg0].unk_03 = temp_v0;
    gEnvelopeState[arg0].unk_04 = temp_v0;

    temp_v0 = GbMusic_ReadStreamByte(arg0);
    temp_a0 = temp_v0 & 0xF0;
    temp_a0 >>= 4;
    temp_a1 = temp_a0;
    temp_a1 >>= 1;
    temp_t9 = temp_a0 & 1;
    temp_a0 = temp_a1 + temp_t9;
    temp_a0 <<= 4;
    temp_t9 = temp_a0 | temp_a1;
    gEnvelopeState[arg0].unk_00 = temp_t9;

    temp_t9 = temp_v0 & 0xF;
    temp_v0 = temp_t9 << 4;
    temp_v0 |= temp_t9;
    gEnvelopeState[arg0].unk_01 = temp_v0;
    return 0;
}

u8 GbMusic_CmdPitchSlide(u32 arg0) {
    UNUSED s32 pad;
    u8 temp_v0;

    gPitchSlideState[arg0].unk_0C = GbMusic_ReadStreamByte(arg0);
    temp_v0 = GbMusic_ReadStreamByte(arg0);
    gPitchSlideState[arg0].unk_04 = GbMusic_GetNoteFrequency(temp_v0 & 0xF, (temp_v0 & 0xF0) >> 4);
    gChannelStatus[arg0] |= 0x10;
    GbMusic_ScheduleNote(arg0, GbMusic_ReadStreamByte(arg0));
    return 1;
}

u8 GbMusic_CmdSetDutyCycle(u32 arg0) {
    gChannelDutyValues[arg0] = GbMusic_ReadStreamByte(arg0) << 6;
    return 0;
}

u8 GbMusic_CmdSetTempo(u32 arg0) {
    u8 sp1F;
    u8 sp1E;

    if (arg0 < 4) {
        sp1F = GbMusic_ReadStreamByte(arg0);
        sp1E = GbMusic_ReadStreamByte(arg0);
        gBpmLow = (sp1E << 8) + sp1F;
        gChannelNoteOffsets[0] = 0;
        gChannelNoteOffsets[1] = 0;
        gChannelNoteOffsets[2] = 0;
        gChannelNoteOffsets[3] = 0;
    } else {
        sp1F = GbMusic_ReadStreamByte(arg0);
        sp1E = GbMusic_ReadStreamByte(arg0);
        gBpmHigh = (sp1E << 8) + sp1F;
        gChannelNoteOffsets[4] = 0;
        gChannelNoteOffsets[5] = 0;
        gChannelNoteOffsets[6] = 0;
        gChannelNoteOffsets[7] = 0;
    }
    return 0;
}

u8 GbMusic_CmdSetPanMask(u32 arg0) {
    gMasterVolume[1] = GbMusic_ReadStreamByte(arg0);
    return 0;
}

u8 GbMusic_CmdPlaySequence(u32 arg0) {
    GbMusic_StartSequenceById(GbMusic_ReadStreamByte(arg0));
    if (gTrackTransitionFlag == 0) {
        gTrackTransitionFlag = gChannelActiveFlags[7];
        gChannelActiveFlags[7] = 0;
    }
    return 0;
}

u8 GbMusic_CmdSetDutyRotation(u32 arg0) {
    u8 temp_v0 = GbMusic_ReadStreamByte(arg0);

    gChannelDutyRotationValues[arg0] = temp_v0;
    gChannelDutyValues[arg0] = temp_v0 & 0xC0;
    gChannelStatus[arg0] |= 0x40;
    return 0;
}

u8 GbMusic_CmdSetMasterVolume(u32 arg0) {
    GbAudio_QueueRegisterWrite(0xFF24, GbMusic_ReadStreamByte(arg0), 0);
    return 0;
}

u8 GbMusic_CmdEnableEnvelope(u32 arg0) {
    gChannelControl[arg0] |= 1;
    return 0;
}

u8 GbMusic_CmdSetNoteLength(u32 arg0, u8 arg1) {
    u8 var_a1;

    gChannelNoteLengths[arg0] = arg1 & 0xF;
    if (arg0 != 3) {
        var_a1 = GbMusic_ReadStreamByte(arg0);
        if (arg0 == 2) {
            gPitchModifiers[0] = var_a1 & 0xF;
            var_a1 = (var_a1 & 0x30) * 2;
        } else if (arg0 == 6) {
            gPitchModifiers[1] = var_a1 & 0xF;
            var_a1 = (var_a1 & 0x30) * 2;
        }
        gChannelVolumeValues[arg0] = var_a1;
    }
    return 0;
}

u8 GbMusic_CmdSetOctave(u32 arg0, u8 arg1) {
    gChannelOctaves[arg0] = arg1 & 0xF;
    return 0;
}

s32 GbMusic_CmdPlayNoteWithDuty(u32 arg0, u8 arg1) {
    u8 sp27;
    u8 sp26;
    u8 sp25;
    u32 var_v1;

    if (((arg1 & 0xF0) != 0x20) || (arg0 < 3) || (gChannelControl[arg0] & 1)) {
        var_v1 = GbMusic_CmdPlayNote(arg0, arg1);
    } else {
        sp25 = gChannelDutyValues[arg0] | GbMusic_ScheduleNote(arg0, arg1);
        GbAudio_QueueRegisterWrite(GbMusic_GetChannelRegister(arg0, 1), sp25, 0);
        sp27 = GbMusic_ReadStreamByte(arg0);
        GbAudio_QueueRegisterWrite(GbMusic_GetChannelRegister(arg0, 2), sp27, 0);
        sp26 = GbMusic_ReadStreamByte(arg0);
        sp27 = 0;
        if (arg0 != 7) {
            sp27 = GbMusic_ReadStreamByte(arg0);
        }
        GbMusic_WriteChannelDutyLength(arg0);
        GbMusic_UpdateChannelPanning(arg0);
        GbMusic_WriteChannelFrequency(arg0, (sp27 << 8) + sp26);
        var_v1 = 1;
    }
    return var_v1;
}

u8 GbMusic_CmdPlayNote(u32 arg0, u8 arg1) {
    if (arg0 < 4) {
        GbMusic_TriggerNoteFromByte(arg0, arg1);
    }

    if (arg1 != 0x10) {
        GbMusic_TriggerNoteFromByte(arg0, arg1);
        return 1;
    }

    if (gChannelControl[arg0] & 1) {
        GbMusic_TriggerNoteFromByte(arg0, arg1);
        return 1;
    }

    GbAudio_QueueRegisterWrite(0xFF10, GbMusic_ReadStreamByte(arg0), 0);
    return 0;
}

u8 GbMusic_TriggerNoteFromByte(u32 arg0, u8 arg1) {
    u8 sp1F;
    s32 temp_v0 = arg1 & 0xF0;

    if ((arg0 != 3) || (temp_v0 > 0xB0)) {
        GbMusic_ScheduleNote(arg0, arg1);
    } else {
        if (temp_v0 == 0xB0) {
            sp1F = GbMusic_ReadStreamByte(arg0);
        } else {
            sp1F = temp_v0 >> 4;
        }

        if (gTrackTransitionFlag == 0) {
            GbMusic_StartSequenceById(gCurrentTrackId);
        }
    }

    GbMusic_ScheduleNote(arg0, sp1F);
    return 1;
}

s32 GbMusic_ScheduleNote(u32 arg0, u8 arg1) {
    u16 sp26;
    u16 tmp;
    u16 var_t0;
    u16 sp20;
    u8 sp1F;

    sp26 = (gChannelNoteLengths[arg0] * ((arg1 & 0xF) + 1));
    var_t0 = 0x100;
    if (arg0 >= 4) {
        if (arg0 != 7) {
            GbMusic_UpdateHighChannelTempo();
            var_t0 = gBpmHigh;
        }
    } else {
        var_t0 = gBpmLow;
    }

    tmp = (gChannelNoteOffsets[arg0] + (sp26 * var_t0));
    gChannelNoteOffsets[arg0] = tmp & 0xFFFF;
    gChannelWaitCounters[arg0] = (tmp & 0xFF00) >> 8;

    if (!(gChannelControl[arg0] & 1) && (gChannelStatus[arg0] & 4)) {
        return gChannelWaitCounters[arg0];
    } else if ((arg1 & 0xF0) != 0xC0) {
        return GbMusic_TriggerChannelNote(arg0, arg1, tmp);
    } else if ((arg0 < 4) && (gChannelActiveFlags[4 + arg0] != 0)) {
        return gChannelActiveFlags[4 + arg0];
    } else if ((arg0 == 2) || (arg0 == 6)) {
        sp1F = gChannelPanHigh[arg0] | GbAudio_GetRegisterValue(0xFF25);
        GbAudio_QueueRegisterWrite(0xFF25, sp1F, 0);
        return sp1F;
    } else {
        sp20 = GbMusic_GetChannelRegister(arg0, 2);
        GbAudio_QueueRegisterWrite(sp20, 8, 0);
        GbAudio_QueueRegisterWrite(sp20 + 2, 0x80, 0);
    }

    return 0x80;
}

s32 GbMusic_TriggerChannelNote(u32 arg0, u16 arg1, u16 arg2) {
    u16 var_s1;
    s32 tmp;
    u8 tmp1;

    arg1 = (arg1 & 0xF0) >> 8;

    tmp1 = gChannelOctaves[arg0];
    var_s1 = GbMusic_GetNoteFrequency(arg1, tmp1);
    tmp = gChannelStatus[arg0] == 0;

    if (tmp & 0x10) {
        GbMusic_StartPitchSlide(arg0, var_s1);
    }

    if ((arg0 < 4) && (gChannelActiveFlags[4 + arg0] != 0)) {
        return 1;
    }

    GbAudio_QueueRegisterWrite(GbMusic_GetChannelRegister(arg0, 2), gChannelVolumeValues[arg0], 0);
    GbMusic_WriteChannelDutyLength(arg0);
    GbMusic_UpdateChannelPanning(arg0);
    if (gChannelStatus[arg0] & 1) {
        var_s1++;
    }
    gEnvelopeState[arg0].unk_02 = var_s1 & 0xFF;
    GbMusic_WriteChannelFrequency(arg0, var_s1);
    return 1;
}

s32 GbMusic_UpdateChannelPanning(u32 arg0) {
    u8 tmp;
    u8 sp1E;
    u8 temp_v1 = gChannelPanLow[arg0] | GbAudio_GetRegisterValue(0xFF25);

    if ((arg0 >= 4) && (arg0 != 7)) {
        GbAudio_QueueRegisterWrite(0xFF25, temp_v1, 0);
        return 1;
    }

    if ((arg0 != 7) && (gChannelActiveFlags[4 + arg0] != 0)) {
        GbAudio_QueueRegisterWrite(0xFF25, temp_v1, 0);
        return 1;
    }

    sp1E = (gMasterVolume[1] & gChannelPanLow[arg0]);
    tmp = gChannelPanHigh[arg0] & GbAudio_GetRegisterValue(0xFF25);
    tmp |= sp1E;
    GbAudio_QueueRegisterWrite(0xFF25, tmp, 0);
    return 1;
}

s32 GbMusic_WriteChannelDutyLength(u32 arg0) {
    u8 sp1F = gChannelWaitCounters[arg0];
    u8 tmp;

    if ((arg0 != 2) && (arg0 != 6)) {
        tmp = gChannelDutyValues[arg0];

        sp1F &= 0x3F;
        sp1F |= tmp;
    }
    GbAudio_QueueRegisterWrite(GbMusic_GetChannelRegister(arg0, 1), sp1F, 0);
    return 1;
}

s32 GbMusic_WriteChannelFrequency(u32 arg0, u16 arg1) {
    u8 sp2E[4];
    s32 var_s2;
    u16 temp_v0;

    if ((arg0 == 2) || (arg0 == 6)) {
        s32 i;

        if (arg0 == 2) {
            var_s2 = 0;
        } else {
            var_s2 = 1;
        }
        GbAudio_QueueRegisterWrite(0xFF1A, 0, 0);

        for (i = 0; i < 16; i++) {
            GbAudio_QueueRegisterWrite(i + 0x20, gCustomWaveforms[var_s2][i], 0);
        }

        GbAudio_QueueRegisterWrite(0xFF1A, 0x80, 0);
    }

    {
        u32 new_var2 = arg1 & 0xFF;
        u32 new_var = (((arg1 >> 8) & 0xFF) | 0x80) & 0xC7;

        sp2E[2] = new_var2;
        sp2E[3] = new_var;

        temp_v0 = GbMusic_GetChannelRegister(arg0, 3);
        GbAudio_QueueRegisterWrite(temp_v0, new_var2, 0);
        GbAudio_QueueRegisterWrite(temp_v0 + 1, new_var, 0);

        if (gChannelActiveFlags[4] != 0) {
            GbMusic_WriteTransposedFrequency(sp2E[2] + (sp2E[3] << 8), temp_v0);
        }
    }
    return 1;
}

void GbMusic_UpdateHighChannelTempo(void) {
    s32 tmp;

    if (GbMusic_IsSfxActive() != 0) {
        tmp = gTrackBpmOffset + 0x80;
        gBpmHigh = tmp;
    } else {
        gBpmHigh = 0x100;
    }
}

void GbMusic_WriteTransposedFrequency(u16 arg0, u16 arg1) {
    if (GbMusic_IsSfxActive() != 0) {
        u16 sp18 = gTrackPitchOffset;

        arg0 += sp18;
        GbAudio_QueueRegisterWrite(arg1, arg0 & 0xFF, 0);
        GbAudio_QueueRegisterWrite(arg1 + 1, (arg0 >> 8) & 0xFF, 0);
    }
}

s32 GbMusic_RewindIfSfxActive(u32 arg0) {
    if (GbMusic_IsSfxActive() != 0) {
        gChannelProgramCounters[arg0]--;
        return 1;
    }
    return 0;
}

s32 GbMusic_IsSfxActive(void) {
    if ((gChannelActiveFlags[4] < gTrackRangeEnd) && (gChannelActiveFlags[4] >= gTrackRangeStart)) {
        return 1;
    }
    return 0;
}

void GbMusic_AdvancePitchSlide(u32 arg0) {
    u8 sp1F;
    u8 sp1E;
    u16 sp1C;

    gPitchSlideState[arg0].unk_00 += gPitchSlideState[arg0].unk_08;
    if (gChannelStatus[arg0] & 0x20) {
        if (gPitchSlideState[arg0].unk_04 < gPitchSlideState[arg0].unk_00) {
            gChannelStatus[arg0] &= ~0x30;
            return;
        }
    } else if (gPitchSlideState[arg0].unk_00 < gPitchSlideState[arg0].unk_04) {
        gChannelStatus[arg0] &= ~0x30;
        return;
    }

    sp1F = gPitchSlideState[arg0].unk_00;
    sp1E = ((u16)gPitchSlideState[arg0].unk_00 & 0xFF00) >> 8;
    sp1C = GbMusic_GetChannelRegister(arg0, 3);
    GbAudio_QueueRegisterWrite(sp1C, sp1F, 0);
    GbAudio_QueueRegisterWrite(sp1C + 1, sp1E, 0);
}

s32 GbMusic_StartPitchSlide(u32 arg0, u16 arg1) {
    gPitchSlideState[arg0].unk_00 = arg1;
    if (arg1 < gPitchSlideState[arg0].unk_04) {
        gChannelStatus[arg0] &= ~0x20;
    } else {
        gChannelStatus[arg0] |= 0x20;
    }
    gPitchSlideState[arg0].unk_08 = (gPitchSlideState[arg0].unk_04 - gPitchSlideState[arg0].unk_00) / gPitchSlideState[arg0].unk_0C;
    return 1;
}

void GbMusic_RotateChannelDuty(u32 arg0) {
    u8 sp1F;
    u16 sp1C;

    sp1F = gChannelDutyRotationValues[arg0];
    sp1F &= 0xC0;
    gChannelDutyRotationValues[arg0] *= 4;
    gChannelDutyRotationValues[arg0] |= (sp1F >> 6);
    sp1F &= 0xC0;
    sp1C = GbMusic_GetChannelRegister(arg0, 1);
    GbAudio_QueueRegisterWrite(sp1C, (sp1F | (GbAudio_GetRegisterValue(sp1C) & 0x3F) & 0xFF), 0);
}

u8 GbMusic_ReadStreamByte(u32 arg0) {
    return *gChannelProgramCounters[arg0]++;
}

u16 GbMusic_GetChannelRegister(u32 arg0, s32 arg1) {
    UNUSED s32 pad;
    u16 gChannelRegisters[] = {
        0xFF10, 0XFF15, 0xFF1A, 0XFF1F, 0xFF10, 0XFF15, 0xFF1A, 0XFF1F,
    };
    s32 ret = gChannelRegisters[arg0] + arg1;

    return ret;
}

u16 GbMusic_GetNoteFrequency(u8 arg0, u8 arg1) {
    s16 sp8[] = {
        0xF82C, 0xF89D, 0xF907, 0xF96B, 0xF9CA, 0xFA23, 0xFA77, 0xFAC7, 0xFB12, 0xFB58, 0xFB9B, 0xFBDA,
    };
    s16 ret = sp8[arg0];

    ret >>= 7 - arg1;
    ret += 0x800;
    return ret;
}

void GbMusic_StartMusic(u8 arg0) {
    s32 i;

    gTrackTransitionFlag = 0;
    gBpmLow = 0x100;
    gPitchModifiers[0] = 0;
    gPitchModifiers[1] = 0;

    for (i = 0; i < 8; i++) {
        gChannelPrevProgramCounters[i] = 0;
        gChannelProgramCounters[i] = 0;
    }

    for (i = 0; i < 4; i++) {
        gChannelActiveFlags[i] = 0;
        gChannelStatus[i] = 0;
        gChannelDutyValues[i] = 0;
        gChannelDutyRotationValues[i] = 0;

        gEnvelopeState[i].unk_03 = 0;
        gEnvelopeState[i].unk_00 = 0;
        gEnvelopeState[i].unk_01 = 0;
        gEnvelopeState[i].unk_02 = 0;
        gEnvelopeState[i].unk_04 = 0;

        gChannelControl[i] = 0;

        gPitchSlideState[i].unk_0C = 0;
        gPitchSlideState[i].unk_00 = 0;
        gPitchSlideState[i].unk_04 = 0;
        gPitchSlideState[i].unk_08 = 0;

        gChannelJumpCounters[i] = 1;
        gChannelWaitCounters[i] = 1;
        gChannelNoteLengths[i] = 1;
    }

    gMasterVolume[1] = 0xFF;
    GbAudio_QueueRegisterWrite(0xFF24, 0, 0);
    GbAudio_QueueRegisterWrite(0xFF10, 8, 0);
    GbAudio_QueueRegisterWrite(0xFF25, 0, 0);
    GbAudio_QueueRegisterWrite(0xFF1A, 0, 0);
    GbAudio_QueueRegisterWrite(0xFF1A, 0x80, 0);
    GbAudio_QueueRegisterWrite(0xFF24, 0x77, 0);
    GbMusic_BindSequenceTracks(arg0);
}

void GbMusic_StartSfx(u8 arg0) {
    u8 i;

    gMasterVolume[1] = 0xFF;

    for (i = 4; i < 7; i++) {
        if (i == 6) {
            continue;
        }

        if (gChannelActiveFlags[i]) {
            if (i == 7) {
                if (arg0 < gTrackMinId) {
                    return;
                }
                if ((gChannelActiveFlags[i] < gTrackMinId) && (gChannelActiveFlags[i] < arg0)) {
                    return;
                }
            } else if (gChannelActiveFlags[i] < arg0) {
                return;
            }
        }

        gChannelPrevProgramCounters[i] = 0;
        gChannelProgramCounters[i] = 0;
        gChannelActiveFlags[i] = 0;

        gChannelStatus[i] = 0;
        gChannelDutyValues[i] = 0;
        gChannelDutyRotationValues[i] = 0;

        gEnvelopeState[i].unk_03 = 0;
        gEnvelopeState[i].unk_00 = 0;
        gEnvelopeState[i].unk_01 = 0;
        gEnvelopeState[i].unk_02 = 0;
        gEnvelopeState[i].unk_04 = 0;

        gPitchSlideState[i].unk_0C = 0;
        gPitchSlideState[i].unk_00 = 0.0f;
        gPitchSlideState[i].unk_04 = 0.0f;
        gPitchSlideState[i].unk_08 = 0.0f;

        gChannelJumpCounters[i] = 1;
        gChannelWaitCounters[i] = 1;
        gChannelNoteLengths[i] = 1;

        if (i == 4) {
            GbAudio_QueueRegisterWrite(0xFF10, 8, 0);
        }
    }

    GbMusic_BindSequenceTracks(arg0);
}

void GbMusic_ResetApu(void) {
    s32 i;

    GbAudio_QueueRegisterWrite(0xFF26, 0x80, 0);
    GbAudio_QueueRegisterWrite(0xFF1A, 0x80, 0);
    GbAudio_QueueRegisterWrite(0xFF25, 0, 0);
    GbAudio_QueueRegisterWrite(0xFF1C, 0, 0);
    GbAudio_QueueRegisterWrite(0xFF10, 8, 0);
    GbAudio_QueueRegisterWrite(0xFF12, 8, 0);
    GbAudio_QueueRegisterWrite(0xFF17, 8, 0);
    GbAudio_QueueRegisterWrite(0xFF21, 8, 0);
    GbAudio_QueueRegisterWrite(0xFF14, 0x40, 0);
    GbAudio_QueueRegisterWrite(0xFF19, 0x40, 0);
    GbAudio_QueueRegisterWrite(0xFF23, 0x40, 0);
    GbAudio_QueueRegisterWrite(0xFF24, 0x77, 0);

    gTrackTransitionFlag = 0;
    gTrackLoadStatus = 0;
    gBpmLow = 0x100;
    gBpmHigh = 0x100;
    gPitchModifiers[0] = 0;
    gPitchModifiers[1] = 0;

    for (i = 0; i < 8; i++) {
        gChannelWaitCounters[i] = 1;
        gChannelJumpCounters[i] = 1;
        gChannelNoteLengths[i] = 1;
    }

    gMasterVolume[1] = 0xFF;
}

void GbMusic_BindSequenceTracks(u8 arg0) {
    u32 i;
    u8 x;
    u8 temp;
    u8 temp_v1;

    temp_v1 = ((unk_D_800FC6D4_008*)gCurrentTrackData)->unk_04;
    temp = (temp_v1 >> 6) & 3;
    x = temp_v1 & 0xF;
    i = 0;
    while (1) {
        gChannelActiveFlags[x] = arg0 + 1;
        if (x >= 3) {
            gChannelStatus[x] |= 4;
        }

        gChannelProgramCounters[x] = ((unk_D_800FC6D4_008*)gCurrentTrackData + i)->unk_00;
        gChannelDataBases[x] = gChannelProgramCounters[x];
        i++;
        
        if (i > temp) {
            break;
        }
        x = ((unk_D_800FC6D4_008*)gCurrentTrackData + i)->unk_04 & 0xF;
    }

    if ((arg0 >= gTrackRangeStart) && (arg0 < gTrackRangeEnd)) {
        gChannelActiveFlags[4] = arg0 + 1;
        gChannelActiveFlags[5] = arg0 + 1;
        gChannelActiveFlags[6] = arg0 + 1;
        gChannelActiveFlags[7] = arg0 + 1;
        gChannelProgramCounters[6] = &gDefaultSequenceEnd;
        if (gMasterVolume[0] == 0) {
            gMasterVolume[0] = GbAudio_GetRegisterValue(0xFF24);
            GbAudio_QueueRegisterWrite(0xFF24, 0x77, 0);
        }
    }
}

s32 GbMusic_CountActiveHighChannels(void) {
    s32 ret = 0;
    s32 i;

    for (i = 0; i < 4; i++) {
        if (gChannelActiveFlags[4 + i] != 0) {
            ret++;
        }
    }

    return ret;
}

typedef struct unk_80044A50_008 {
    u32 unk_00;
    u32 unk_04;
} unk_80044A50_008;

void Music_ResolveTrackOffsets(GbTrackHeader* arg0) {
    s32 i, end;
    u32 base;
    u8* temp_v1;
    unk_80044A50_008* temp_v0;
    if (arg0->flags != 0) return;
    end = arg0->unk_04;
    base = (u32)arg0;
    temp_v1 = arg0->offset;
    temp_v1 += base;
    arg0->offset = (u32)temp_v1;
    for (i = 0; i != (end * 3); i++) {
        temp_v0 = (unk_80044A50_008*)((u32)i * 8 + (u32)temp_v1);
        temp_v0 += 0;
        temp_v0->unk_00 += base;
    }
    arg0->flags = 1;
}
