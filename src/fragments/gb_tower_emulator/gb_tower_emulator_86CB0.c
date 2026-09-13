#include "global.h"

typedef struct GbApuChannelState {
  /* 0x00 */ u8 unk_00;
  /* 0x02 */ char unk02[0x02];
  /* 0x04 */ s32 unk_04;
  /* 0x08 */ union {
      u16 unk_00;
      s32 unk_02;
  } unk_08;
  /* 0x0C */ u32 unk_0C;
  /* 0x10 */ u16 unk_10;
  /* 0x12 */ u16 unk_12;
  /* 0x14 */ s32 unk_14;
  /* 0x18 */ s32 unk_18;
  /* 0x1C */ char unk1C[0x4];
  /* 0x20 */ u8 unk_20;
  /* 0x21 */ u8 unk_21;
  /* 0x22 */ char unk22[0x2];
  /* 0x24 */ u32 unk_24;
  /* 0x28 */ u32 unk_28;
  /* 0x2C */ char unk2C[0x1];
  /* 0x2D */ u8 unk_2D;
  /* 0x2E */ u8 unk_2E;
  /* 0x2F */ char unk2F[0xD];
  /* 0x3C */ u8 unk_3C;
  /* 0x3D */ char unk3D[0x3];
  /* 0x40 */ u32 unk_40;
  /* 0x44 */ u16 unk_44;
  /* 0x46 */ u16 unk46;
  /* 0x48 */ u32 unk_48;
  /* 0x4C */ u8 unk_4C;
  /* 0x4D */ char unk4D[0x3];
} GbApuChannelState; // size = 0x50

typedef struct unk_D_8122EEA8 {
  /* 0x00 */ u8 unk_00;
  /* 0x01 */ u8 unk_01;
  /* 0x02 */ u8 unk_02;
  /* 0x03 */ u8 unk_03;
} unk_D_8122EEA8; // size = 0x4

extern u8 gGbMemoryMap[];
extern u8 D_8120EB24[];
extern u8 D_8120EB28[];
extern u8 D_8120EB38[];
extern u16 D_8120EA86;
extern f32 D_8120EAC0;
extern s32 D_8120EAC8;
extern s32 D_8120EACC;
extern s32 D_8120EAC4;
extern s32 D_8120EA60;
extern u32 D_8120EA80;
extern s32 D_8120EB78;
extern s32 D_8120EB7C;
extern f32 D_8122B0A0;
extern f32 D_8122B0A4;
extern s32 D_8122C794;

extern GbApuChannelState gGbApuSquare1;
extern GbApuChannelState gGbApuSquare2;
extern GbApuChannelState gGbApuWave;
extern GbApuChannelState gGbApuNoise;
extern unk_D_8122EEA8 D_8122EEA8;

extern OSMesgQueue D_8122EEB0;
extern OSMesg D_8122EEC8;

void func_81207330(void) {
}

void GbApu_SetActiveFlag(void) {
    D_8120EA60 = 1;
}

void GbApu_ClearActiveFlag(void) {
    D_8120EA60 = 0;
}

void func_81207354(void) {
}

void GbApu_SelectRegionClock(s32 arg0) {
    if (arg0 == 0) {
        D_8120EAC4 = 0x4578;
        D_8120EAC0 = D_8122B0A0;
        return;
    }
    D_8120EAC4 = 0x8AF0;
    D_8120EAC0 = D_8122B0A4;
}

void GbApu_SetNoisePeriodBounds(s32 arg0, s32 arg1) {
    D_8120EAC8 = arg0;
    D_8120EACC = arg1;
}

u32 GbApu_FreqToStep(u16 arg0) {
  u32 temp;
  if (arg0 == 0) {
      return 0;
  }
  temp = ((65536.0f / (0x800 - arg0)) / D_8120EAC0) * 65536;
  return temp;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/func_81207494.s")

s32 GbApu_FreqToStep2x(s32);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/GbApu_FreqToStep2x.s")

void func_81207690(void) {
}

s32 GbApu_SampleWaveform(u32, u16);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/GbApu_SampleWaveform.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/GbApu_UpdateSquare1Channel.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/GbApu_UpdateSquare2Channel.s")

u16 GbApu_UpdateWaveChannel(void) {
    u16 temp_v0;
    u16 temp_a0;
    u16 temp_a1;
    u16 var_a0;

    if (gGbApuWave.unk_00 == 0) {
        if (gGbApuWave.unk_3C == 0) {
            return 0U;
        }
        gGbApuWave.unk_3C--;
        gGbApuWave.unk_20 = gGbApuWave.unk_3C;
        gGbApuWave.unk_0C = (s32) gGbApuWave.unk_40;
    } else {    
        if (gGbApuWave.unk_04 != 0) {
            gGbApuWave.unk_04--;
            if (gGbApuWave.unk_04 >= 0x21) {
                if (gGbApuWave.unk_3C == 0) {
                    gGbApuWave.unk_04 = 0x20;
                    gGbApuWave.unk_08.unk_02 = 0;
                    gGbApuWave.unk_0C = GbApu_FreqToStep(gGbApuWave.unk_10);
                    gGbApuWave.unk_20 = D_8120EB24[gGbApuWave.unk_21];
                } else {
                    if (!(gGbApuWave.unk_04 & 7)) {
                        gGbApuWave.unk_3C--;
                    }
                    gGbApuWave.unk_20 = gGbApuWave.unk_3C;
                    gGbApuWave.unk_0C = (s32) gGbApuWave.unk_40;
                }
            }
        }
    }
    temp_a0 = GbApu_SampleWaveform(7, gGbApuWave.unk_08.unk_00);
    gGbApuWave.unk_08.unk_02 += gGbApuWave.unk_0C;
    gGbApuWave.unk_08.unk_02 &= 0x1FFFFF;
    if (gGbApuWave.unk_4C - 1 == 0) {

        if (gGbApuWave.unk_3C != 0) {
            gGbApuWave.unk_3C--;
        }
        temp_a1 = gGbApuWave.unk_3C;
    } else {
        gGbApuWave.unk_3C = gGbApuWave.unk_20;
        gGbApuWave.unk_40 = gGbApuWave.unk_0C;
        temp_a1 = gGbApuWave.unk_20;
    }

    temp_a0 *= temp_a1;
    temp_a0 = (gGbApuWave.unk_44 + temp_a0) >> 1;
    gGbApuWave.unk_44 = temp_a0;
    if (gGbApuWave.unk_18 == 1) {
        if (gGbApuWave.unk_14 != 0) {
            gGbApuWave.unk_14--;
        }
        if (gGbApuWave.unk_14 == 0) {
            gGbApuWave.unk_00 = 0U;
        }
    }
    return temp_a0;
}

u16 func_81207C5C_Empty(void) {
    
}

#ifdef NON_MATCHING
// Matching but won't generate correct checksum
u16 GbApu_UpdateNoiseChannel(void) {
    static s32 D_8120EB6C;
    u16 temp_v0;
    u32 var_a2;

    if (gGbApuNoise.unk_0C < gGbApuNoise.unk_40) {
        gGbApuNoise.unk_40 -= gGbApuNoise.unk_48;
        var_a2 = gGbApuNoise.unk_40;
    } else if (gGbApuNoise.unk_40 < gGbApuNoise.unk_0C) {
        gGbApuNoise.unk_40 += gGbApuNoise.unk_48;
        var_a2 = gGbApuNoise.unk_40;
    } else {
        var_a2 = gGbApuNoise.unk_40;
    }

    if (gGbApuNoise.unk_00 == 0) {
        if (gGbApuNoise.unk_3C == 0) {
            return 0;
        }
        gGbApuNoise.unk_3C--;
        gGbApuNoise.unk_20 = gGbApuNoise.unk_3C;
        gGbApuNoise.unk_0C = gGbApuNoise.unk_40;
    }
    temp_v0 = GbApu_SampleWaveform(6, gGbApuNoise.unk_08.unk_00);
    gGbApuNoise.unk_08.unk_02 += var_a2;
    if (gGbApuNoise.unk_08.unk_00  >= (u32)D_8120EACC) {
        gGbApuNoise.unk_08.unk_00  = D_8120EAC8;
    }
    temp_v0 *= gGbApuNoise.unk_3C;
    if ((gGbApuNoise.unk_3C != gGbApuNoise.unk_20) && ((D_8120EB6C % 95) == 0)) {
        if (gGbApuNoise.unk_3C > gGbApuNoise.unk_20) {
            gGbApuNoise.unk_3C--;
        } else {
            gGbApuNoise.unk_3C++;
        }
    }
    D_8120EB6C++;
    if (gGbApuNoise.unk_28 != 0) {
        gGbApuNoise.unk_24++;
        if ((gGbApuNoise.unk_24 % (u32) gGbApuNoise.unk_28) == 0) {
            switch (gGbApuNoise.unk_2E) {             /* irregular */
            case 0:
                if (gGbApuNoise.unk_2D) {
                    gGbApuNoise.unk_2D--;
                }
                if (gGbApuNoise.unk_2D == 0) {
                    gGbApuNoise.unk_00 = 0;
                }
                break;
            case 1:
                if (gGbApuNoise.unk_2D < 0xF) {
                    gGbApuNoise.unk_2D++;
                }
                break;
            }
            switch ((D_8120EA80 << 0x14) >> 0x1F) {                    /* switch 1; irregular */
            case 0:                                 /* switch 1 */
                gGbApuNoise.unk_20 = D_8120EB28[gGbApuNoise.unk_2D];
                break;
            case 1:                                 /* switch 1 */
                gGbApuNoise.unk_20 = D_8120EB38[gGbApuNoise.unk_2D];
                break;
            }
        }
    }
    if (gGbApuNoise.unk_18 == 1) {
        if (gGbApuNoise.unk_14 != 0) {
            gGbApuNoise.unk_14--;
        }
        if (gGbApuNoise.unk_14 == 0) {
            gGbApuNoise.unk_00 = 0U;
        }
    }
    
    return (temp_v0 << 1);
}
#else
u16 GbApu_UpdateNoiseChannel(void);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/GbApu_UpdateNoiseChannel.s")
#endif

void GbApu_WriteRegister(u16, u8);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/GbApu_WriteRegister.s")

void GbApu_RenderAudioBuffer(s32 sampleCount, s16* samples);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/GbApu_RenderAudioBuffer.s")

void GbApu_QueueRegisterWrite(u16 address, u8 value, u16 timestamp);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/GbApu_QueueRegisterWrite.s")

void GbApu_ResetChannels(void) {
  s32 pad[6];
  s32 i;
  OSMesgQueue sp24;
  OSMesg sp20;

  osCreateMesgQueue(&sp24, &sp20, 1);
  osCreateMesgQueue(&D_8122EEB0, &D_8122EEC8, 0x960);
  for (i = 0x50; i > 0xF; i--) {
      GbApu_WriteRegister(i, 0);
  }
  gGbApuSquare1.unk_3C = 0;
  gGbApuSquare2.unk_3C = 0;
  gGbApuWave.unk_3C = 0;
  gGbApuNoise.unk_3C = 0;
  gGbApuSquare1.unk_08.unk_02 = 0;
  gGbApuSquare2.unk_08.unk_02 = 0;
  gGbApuWave.unk_08.unk_02 = 0;
  gGbApuNoise.unk_08.unk_02 = 0;
}

void GbApu_Reset(s32 arg0) {
  D_8122C794 = arg0;
  GbApu_ResetChannels();
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/func_81208E4C.s")

void func_81208F94(void) {
}

u8 GbMem_ReadIoRegister(u16 arg0) {
  u8 var_a0;
  u8 var_a1;
  u8 var_a2;
  u8 var_v1;
  u8 temp;
  if ((arg0 & 0xFFFF) == 0xFF26) {
      if (D_8122EEA8.unk_00 != 0) {
          var_v1 = (D_8122EEA8.unk_00 - 1) & 0xFF;
      } else {
          var_v1 = gGbApuSquare1.unk_00;
      }
      if (D_8122EEA8.unk_01 != 0) {
          var_a0 = (D_8122EEA8.unk_01 - 1) & 0xFF;
      } else {
          var_a0 = gGbApuSquare2.unk_00;
      }
      if (D_8122EEA8.unk_02 != 0) {
          var_a1 = (D_8122EEA8.unk_02 - 1) & 0xFF;
      } else {
          var_a1 = gGbApuWave.unk_00;
      }
      if (D_8122EEA8.unk_03 != 0) {
          var_a2 = (D_8122EEA8.unk_03 - 1) & 0xFF;
      } else {
          var_a2 = gGbApuNoise.unk_00;
      }
      temp = ((((u32)D_8120EA86 >> 0xF) << 7) | (var_a2 * 8) | (var_a1 * 4) | (var_a0 * 2) | var_v1);
      return temp;
  }
  return gGbMemoryMap[arg0 & 0xFFFF];
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/GbAudio_ServiceOutputBuffer.s")

/* Schedules the next N64 audio buffer and renders either the GB APU or an alternate stream. */
void GbAudio_ServiceOutputBuffer(void);

void GbAudio_SetAlternateRendererEnabled(s32 arg0) {
  D_8120EB78 = arg0;
}

void GbAudio_SetAlternateStreamId(s32 arg0) {
  D_8120EB7C = arg0;
}

void GbAudio_RenderAlternateBuffer(s32 sampleCount, s16* samples);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/GbAudio_RenderAlternateBuffer.s")

void func_81209688(UNUSED s32 arg0) {
}

// Decrypting this function causes issues
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/func_81209690.s")
