#ifndef _E1C0_H_
#define _E1C0_H_

#include "global.h"

typedef struct GbEmuThreadInfo {
  /* 0x0000 */ OSThread thread;
  /* 0x01B0 */ char unk01B0[0x2030];
} GbEmuThreadInfo; // size = 0x21E0

extern GbEmuState* gGbEmuState;

void GbTower_Start(UnkInputStruct8000D738* arg0);
OSMesg* GbTower_RecvExitMesg(void);
void GbTower_Stop(UnkInputStruct8000D738* arg0);

#endif // _E1C0_H_
