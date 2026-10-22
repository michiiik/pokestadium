#include "global.h"

typedef struct unk_func_81206FA0 {
  /* 0x00 */ s32 unk_00;
  /* 0x04 */ s32 unk_04;
  /* 0x08 */ s32 unk_08;
  /* 0x0C */ s32 unk_0C;
} unk_func_81206FA0; // size = 0x10

typedef struct unk_D_812346E0 {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
} unk_D_812346E0;// size = 0x8

typedef struct unk_D_81231450 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
} unk_D_81231450;// size = 0x8

typedef struct unk_D_81234650 {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
} unk_D_81234650;// size = 0x8

typedef struct GbApuConfig {
    /* 0x00 */ u8  pad0[2];
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ u8 pad0E[0x2A];
} GbApuConfig; // size = 0x38

// .data
extern s32 D_812286EC;
extern s32 D_812286F0;

// .bss
u64 gAudioArenaBuffer[0x640];
u64 gAudioWorkBuffer[0x6];
unk_func_81206FA0 gAudioArena;
GbApuConfig gAudioConfig;
volatile s32 gAudioTaskPending;
s32 D_812346CC;
s32 D_812346D0;
s32 gAudioTaskId;
f32 gAudioTimer;
s32 gAudioFramesPerSec;
s16* gApuSampleBuffers[3];
s16 gApuInitValues[3];
s16 gApuInitValuesEnd;
u32 D_812346F4;
s32 D_812346F8;
volatile s32 gAudioInitFlag;
s32 gAudioWorkBufferEnd;
s32 D_81234704;
s32 D_81234708;
s32 D_8123470C;

u32 GbApu_AllocZeroed(unk_func_81206FA0* arg0, s32 arg1) {
  s32 temp_v0;
  u32 temp_a2;
  u32 var_a1;
  temp_v0 = ALIGN16(arg1);
  temp_a2 = arg0->unk_04;
  if ((arg0->unk_00 + arg0->unk_08) >= (temp_a2 + temp_v0)) {
      arg0->unk_04 += temp_v0;
      var_a1 = temp_a2;
      while (var_a1 < arg0->unk_04) {
          *((u8*)var_a1++) = 0;
      }
  } else {
      return 0;
  }
  arg0->unk_0C++;
  return temp_a2;
}

s32 GbApu_Alloc(unk_func_81206FA0* arg0, s32 arg1) {
  s32 temp_a2;
  u32 temp_v0;

  temp_v0 = ALIGN16(arg1);
  temp_a2 = arg0->unk_04;
  if ((arg0->unk_00 + arg0->unk_08) >= (arg0->unk_04 + temp_v0)) {
      arg0->unk_04 = temp_v0 + temp_a2;
  } else {
      return 0;
  }
  arg0->unk_0C++;
  return temp_a2;
}


void GbApu_InitAllocator(unk_func_81206FA0* arg0, s32 arg1, s32 arg2) {
  arg0->unk_00 = ALIGN16(arg1);
  arg0->unk_04 = ALIGN16(arg1);
  arg0->unk_08 = (s32) (arg2 - (arg1 & 0xF));
  arg0->unk_0C = 0;
}

void GbAudio_Init(void) {
    u64* var_v0_2;
    s16* var_v1_2;
    s32 var_a0;
    u32 var_v1;
    s16* var_v1_3;
    s32 i;

    gAudioInitFlag = 0;
    for (i = 0; i < D_812286EC / 8; i++) {
        gAudioArenaBuffer[i] = 0;
    }
    var_v0_2 = gAudioWorkBuffer;
    var_v1 = (u32) ((s32) &gAudioWorkBufferEnd - (s32) gAudioWorkBuffer) >> 3;
    if ((s32) var_v1 >= 0) {
        do {
            var_v1--;
            *var_v0_2 = 0;
            ++var_v0_2;
        } while ((s32) var_v1 >= 0);
    }
    gAudioTimer = 16.713f;
    gAudioFramesPerSec = 0x3C;
    do {} while (0);
    gAudioConfig.unk_02 = 0xBB80;
    gAudioConfig.unk_04 = osAiSetFrequency(gAudioConfig.unk_02);
    gAudioConfig.unk_06 = (s16) (((gAudioConfig.unk_02 / gAudioFramesPerSec) + 0xF) & ~0xF);
    var_v1_2 = (s16*) &gApuInitValues;
    gAudioConfig.unk_0A = (s16) (gAudioConfig.unk_06 - 0x10);
    gAudioConfig.unk_08 = (s16) (gAudioConfig.unk_06 + 0x10);
    do {
        *var_v1_2 = 0xA0;
        var_v1_2++;
    } while (var_v1_2 < (s16*) &gApuInitValuesEnd);
    gAudioTaskPending = 0;
    gAudioTaskId = 0;
    GbApu_InitAllocator(&gAudioArena, (s32) gAudioArenaBuffer, D_812286F0);
    for (i = 0; i < 3; i++) {
        gApuSampleBuffers[i] = (s16*) GbApu_Alloc(&gAudioArena, 0xFC0);
        var_v1_3 = gApuSampleBuffers[i];
        for (var_a0 = 0; var_a0 != 0xFC0 / 2; var_a0++) {
            *var_v1_3 = 0;
            var_v1_3++;
        }
    }
}
