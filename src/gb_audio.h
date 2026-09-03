#ifndef _435D0_H_
#define _435D0_H_

#include "global.h"
#include "src/audio_commands.h"

void GbMusic_UpdateChannels(void);
void GbMusic_StartSequenceById(u8 arg0);
s32 GbMusic_StartTableSequence(u16 arg0);
void GbMusic_UpdateChannel(u32 arg0);
void GbMusic_DecodeChannelCommands(u32 arg0);
u8 GbMusic_CmdEndOrReturn(u32 arg0);
u8 GbMusic_CmdCall(u32 arg0);
u8 GbMusic_CmdLoop(u32 arg0);
u8 GbMusic_CmdToggleFinePitch(u32 arg0);
u8 GbMusic_CmdSetEnvelope(u32 arg0);
u8 GbMusic_CmdPitchSlide(u32 arg0);
u8 GbMusic_CmdSetDutyCycle(u32 arg0);
u8 GbMusic_CmdSetTempo(u32 arg0);
u8 GbMusic_CmdSetPanMask(u32 arg0);
u8 GbMusic_CmdPlaySequence(u32 arg0);
u8 GbMusic_CmdSetDutyRotation(u32 arg0);
u8 GbMusic_CmdSetMasterVolume(u32 arg0);
u8 GbMusic_CmdEnableEnvelope(u32 arg0);
u8 GbMusic_CmdSetNoteLength(u32 arg0, u8 arg1);
u8 GbMusic_CmdSetOctave(u32 arg0, u8 arg1);
s32 GbMusic_CmdPlayNoteWithDuty(u32 arg0, u8 arg1);
u8 GbMusic_CmdPlayNote(u32 arg0, u8 arg1);
u8 GbMusic_TriggerNoteFromByte(u32 arg0, u8 arg1);
s32 GbMusic_ScheduleNote(u32 arg0, u8 arg1);
s32 GbMusic_TriggerChannelNote(u32 arg0, u16 arg1, u16 arg2);
s32 GbMusic_UpdateChannelPanning(u32 arg0);
s32 GbMusic_WriteChannelDutyLength(u32 arg0);
s32 GbMusic_WriteChannelFrequency(u32 arg0, u16 arg1);
void GbMusic_UpdateHighChannelTempo(void);
void GbMusic_WriteTransposedFrequency(u16 arg0, u16 arg1);
s32 GbMusic_RewindIfSfxActive(u32 arg0);
s32 GbMusic_IsSfxActive(void);
void GbMusic_AdvancePitchSlide(u32 arg0);
s32 GbMusic_StartPitchSlide(u32 arg0, u16 arg1);
void GbMusic_RotateChannelDuty(u32 arg0);
u8 GbMusic_ReadStreamByte(u32 arg0);
u16 GbMusic_GetChannelRegister(u32 arg0, s32 arg1);
u16 GbMusic_GetNoteFrequency(u8 arg0, u8 arg1);
void GbMusic_StartMusic(u8 arg0);
void GbMusic_StartSfx(u8 arg0);
void GbMusic_ResetApu(void);
void GbMusic_BindSequenceTracks(u8 arg0);
s32 GbMusic_CountActiveHighChannels(void);
void func_80044A50(unk_D_800FC6D4* arg0);

#endif // _435D0_H_
