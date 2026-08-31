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
  /* 0x2F */ char unk2F[0x1];
  /* 0x30 */ u32 unk_30;
  /* 0x34 */ u32 unk_34;
  /* 0x38 */ u8 unk_38;
  /* 0x39 */ u8 unk_39;
  /* 0x3A */ u8 unk_3A;
  /* 0x3B */ u8 unk_3B;
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
extern s32 D_8120EAD0;
extern u8 D_8120EB14[];
extern s32 D_8120EB78;
extern s32 D_8120EB7C;

// .rodata
extern f32 D_8122B0A0;
extern f32 D_8122B0A4;
extern f32 D_8122B0A8;
extern f32 D_8122B0AC;

// .bss
extern s16 D_8122C790;
extern s16 D_8122C792;
extern u8* D_8122C794;
extern GbApuChannelState gGbApuSquare1;
extern GbApuChannelState gGbApuSquare2;
extern GbApuChannelState gGbApuWave;
extern GbApuChannelState gGbApuNoise;
extern u8 D_8122EE58[40];
extern s32 D_8122EE98;
extern unk_D_8122EEA8 D_8122EEA8;
extern OSMesgQueue D_8122EEB0;
extern OSMesg D_8122EEC8;
extern void* D_812346E0;

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

u32 GbApu_FreqToStep2x(u16 arg0) {
  u32 temp;
  temp = ((131072.0f / (0x800 - arg0)) / D_8120EAC0) * 65536;
  return temp;
}

void func_81207690(void) {
}

#ifdef NON_MATCHING
// Matching needs rodata mapping
s32 func_81207698(u32 arg0, s32 arg1) {
    switch (arg0) {
    case 0:
        if ((arg1 >= 4) && (arg1 < 8)) {
            return 0x7F;
        }
        return 0;
    case 1:
        if (arg1 < 8) {
            return 0x7F;
        }
        return 0;
    case 2:
        if (arg1 < 0x10) {
            return 0x7F;
        }
        return 0;
    case 3:
        if (arg1 < 8) {
            return 0;
        }
        return 0x7F;
    case 4:
        return (u8)(D_8122C794[arg1 + D_8122EE98] & 0x7F);
    case 5:
        return (u8)(D_8122EE58[arg1] << 3);
    case 6:
    case 7:
        break;
    }
}
#else
s32 func_81207698(u32, s32);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/func_81207698.s")
#endif

u16 GbApu_UpdateSquare1Channel(void) {
    u16 var_t0;

    if (gGbApuSquare1.unk_00 == 0) {
        if (gGbApuSquare1.unk_3C == 0) {
            return 0U;
        }
        gGbApuSquare1.unk_3C--;
        gGbApuSquare1.unk_20 = gGbApuSquare1.unk_3C;
        gGbApuSquare1.unk_0C = (s32) gGbApuSquare1.unk_40;
    }
    if ((s32) gGbApuSquare1.unk_10 <= 0) {
        gGbApuSquare1.unk_4C = 1U;
    } else if (((s32) gGbApuSquare1.unk_10 >= 0x7FF) && (gGbApuSquare1.unk_39 == 0)) {
        gGbApuSquare1.unk_4C = 1U;
    } else {
        gGbApuSquare1.unk_4C = 0U;
    }
    {
        s32 temp_v0 = func_81207698(gGbApuSquare1.unk_3B, gGbApuSquare1.unk_08.unk_00);
        gGbApuSquare1.unk_08.unk_02 += gGbApuSquare1.unk_0C;
        gGbApuSquare1.unk_08.unk_02 &= 0x1FFFFF;
        if (gGbApuSquare1.unk_4C == 1) {
            if (gGbApuSquare1.unk_3C != 0) {
                gGbApuSquare1.unk_3C--;
            }
            var_t0 = ((temp_v0 & 0xFFFF) * gGbApuSquare1.unk_3C);
        } else {
            gGbApuSquare1.unk_40 = (s32) gGbApuSquare1.unk_0C;
            gGbApuSquare1.unk_3C = (u8) gGbApuSquare1.unk_20;
            var_t0 = ((temp_v0 & 0xFFFF) * gGbApuSquare1.unk_20);
        }
    }
    if ((gGbApuSquare1.unk_3A != 0) && (((++gGbApuSquare1.unk_30 % (u32) gGbApuSquare1.unk_34) == 0))) {
        s32 temp_a2 = ((s32) gGbApuSquare1.unk_10 >> gGbApuSquare1.unk_38) & 0xFFFF;
        switch (gGbApuSquare1.unk_39) {
        case 0:
            gGbApuSquare1.unk_10 += temp_a2;
            break;
        case 1:
            gGbApuSquare1.unk_10 = (gGbApuSquare1.unk_10 - temp_a2) - 1;;
            break;
        }
        if (gGbApuSquare1.unk_10 <= 0) {
            gGbApuSquare1.unk_00 = 0U;
            return var_t0;
        } else {
            if ((gGbApuSquare1.unk_10 >= 0x800) && (gGbApuSquare1.unk_39 == 0)) {
                gGbApuSquare1.unk_10 = 0x7FFU;
                gGbApuSquare1.unk_00 = 0U;
            }
            gGbApuSquare1.unk_0C = GbApu_FreqToStep2x(gGbApuSquare1.unk_10);
        }
    }
    if (gGbApuSquare1.unk_28 != 0) {
        gGbApuSquare1.unk_24++;
        if ((gGbApuSquare1.unk_24 % (u32) gGbApuSquare1.unk_28) == 0) {
            switch (gGbApuSquare1.unk_2E) {
            case 0:
                if (gGbApuSquare1.unk_2D != 0) {
                    gGbApuSquare1.unk_2D--;
                }
                if (!gGbApuSquare1.unk_2D) {
                    gGbApuSquare1.unk_00 = 0;
                }
                break;
            
            case 1:
                if (gGbApuSquare1.unk_2D < 0xF) {
                    gGbApuSquare1.unk_2D++;
                }
                break;
            
            default:
                break;
            }
        gGbApuSquare1.unk_20 = D_8120EB14[gGbApuSquare1.unk_2D];
        }
    } else {
        gGbApuSquare1.unk_20 = D_8120EB14[gGbApuSquare1.unk_2D];
    }
    if (gGbApuSquare1.unk_18 == 1U) {
        if (gGbApuSquare1.unk_14 != 0) {
            gGbApuSquare1.unk_14--;
        }
        if (gGbApuSquare1.unk_14 == 0) {
            gGbApuSquare1.unk_00 = 0U;
        }
    }
    return var_t0;
}

s32 GbApu_UpdateSquare2Channel(void) {
    s32 temp_v0;
    s32 var_a3;
    u8 var_v0;

    if (gGbApuSquare2.unk_00 == 0) {
        if (gGbApuSquare2.unk_3C == 0) {
            return 0;
        }
        gGbApuSquare2.unk_3C--;
        gGbApuSquare2.unk_20 = gGbApuSquare2.unk_3C;
        gGbApuSquare2.unk_0C = (s32) gGbApuSquare2.unk_40;
    }
    if ((s32) gGbApuSquare2.unk_10 >= 0x7FF) {
        gGbApuSquare2.unk_4C = 1U;
    } else {
        gGbApuSquare2.unk_4C = 0U;
    }
    temp_v0 = func_81207698(gGbApuSquare2.unk_3B, gGbApuSquare2.unk_08.unk_00);
    gGbApuSquare2.unk_08.unk_02 += gGbApuSquare2.unk_0C;
    gGbApuSquare2.unk_08.unk_02 &= 0x1FFFFF;
    if (gGbApuSquare2.unk_4C == 1) {
        if (gGbApuSquare2.unk_3C != 0) {
            gGbApuSquare2.unk_3C--;
        }
        var_a3 = ((temp_v0 & 0xFFFF) * gGbApuSquare2.unk_3C) & 0xFFFF;
    } else {
        if (gGbApuSquare2.unk_20 != gGbApuSquare2.unk_3C) {
            gGbApuSquare2.unk_3C = (u8) gGbApuSquare2.unk_20;
        }
        gGbApuSquare2.unk_40 = (s32) gGbApuSquare2.unk_0C;
        var_a3 = ((temp_v0 & 0xFFFF) * gGbApuSquare2.unk_20) & 0xFFFF;
    }
    if (gGbApuSquare2.unk_28 != 0) {
        gGbApuSquare2.unk_24++;
        if ((gGbApuSquare2.unk_24 % (u32) gGbApuSquare2.unk_28) == 0) {
            switch (gGbApuSquare2.unk_2E) {
                case 0:
                    if (gGbApuSquare2.unk_2D != 0) {
                        gGbApuSquare2.unk_2D--;
                    }
                    if (!gGbApuSquare2.unk_2D) {
                        gGbApuSquare2.unk_00 = 0;
                    }
                    break;
                
                case 1:
                    if (gGbApuSquare2.unk_2D < 0xF) {
                        gGbApuSquare2.unk_2D++;
                    }
                    break;
                
                default:
                    break;
                }
            gGbApuSquare2.unk_20 = D_8120EB14[gGbApuSquare2.unk_2D];
        }
    } else {
        gGbApuSquare2.unk_20 = D_8120EB14[gGbApuSquare2.unk_2D];
    }
    if (gGbApuSquare2.unk_18 == 1U) {
        if (gGbApuSquare2.unk_14 != 0) {
            gGbApuSquare2.unk_14--;
        }
        if (gGbApuSquare2.unk_14 == 0) {
            gGbApuSquare2.unk_00 = 0U;
        }
    }
    return var_a3;
}

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
    temp_a0 = func_81207698(7, gGbApuWave.unk_08.unk_00);
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
u16 func_81207DF8(void) {
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
    temp_v0 = func_81207698(6, gGbApuNoise.unk_08.unk_00);
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
u16 func_81207DF8(void);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/func_81207DF8.s")
#endif

void func_8120806C(u16, u8);
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/func_8120806C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/func_81208828.s")

#ifdef NON_MATCHING
// Matching needs rodata mapping
void func_81208C08(u16 arg0, u8 arg1, u16 arg2) {
    s32 temp_v1;

    temp_v1 = arg0 + 0xFFFF0100;
    osSendMesg(&D_8122EEB0, (temp_v1 << 0x18) | (arg1 << 0x10) | arg2, 0);
    D_8120EAD0 = (s32) arg2;
    switch (temp_v1) {
    case 20:
        if (arg1 & 0x80) {
            D_8122EEA8.unk_00 = 2;
            return;
        }
    default:
        return;
    case 25:
        if (arg1 & 0x80) {
            D_8122EEA8.unk_01 = 2;
            return;
        }
        break;
    case 30:
        if (arg1 & 0x80) {
            D_8122EEA8.unk_02 = 2;
            return;
        }
        break;
    case 35:
        if (arg1 & 0x80) {
            D_8122EEA8.unk_03 = 2;
            return;
        }
        break;
    case 38:
        if (!(arg1 & 1)) {
            D_8122EEA8.unk_00 = 1;
        }
        if (!(arg1 & 2)) {
            D_8122EEA8.unk_01 = 1;
        }
        if (!(arg1 & 4)) {
            D_8122EEA8.unk_02 = 1;
        }
        if (!(arg1 & 8)) {
            D_8122EEA8.unk_03 = 1;
        }
        if (!(arg1 & 0x80)) {
            D_8122EEA8.unk_00 = 1;
            D_8122EEA8.unk_01 = 1;
            D_8122EEA8.unk_02 = 1;
            D_8122EEA8.unk_03 = 1;
        }
        break;
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/func_81208C08.s")
#endif

void GbApu_ResetChannels(void) {
  s32 pad[6];
  s32 i;
  OSMesgQueue sp24;
  OSMesg sp20;

  osCreateMesgQueue(&sp24, &sp20, 1);
  osCreateMesgQueue(&D_8122EEB0, &D_8122EEC8, 0x960);
  for (i = 0x50; i > 0xF; i--) {
      func_8120806C(i, 0);
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

void func_81208E4C(void) {
    void* sp34 = D_812346E0;
    f32 x = (f32) D_8122C790;
    f32 y = (f32) D_8122C792;
    f32 dx = x / 640.0f;
    f32 dy = y / 640.0f;
    s32 i;
    s32 idx;

    GbApu_ResetChannels();
    for (i = 0, idx = 0; i != 0x280; i++) {
        ((s16*)sp34)[idx + 0] = x;
        ((s16*)sp34)[idx + 1] = y;
        x -= dx;
        y -= dy;
        idx += 2;
    }
     do {
     } while (osAiGetStatus() & 0x80000000);
    osWritebackDCache(sp34, 0xA00);
    osGbSetNextBuffer(sp34, 0xA00);
}

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

#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/func_81209078.s")

void GbAudio_SetAlternateRendererEnabled(s32 arg0) {
  D_8120EB78 = arg0;
}

void GbAudio_SetAlternateStreamId(s32 arg0) {
  D_8120EB7C = arg0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/fragments/1/fragment1_86CB0/func_81209374.s")

void func_81209688(UNUSED s32 arg0) {
}
