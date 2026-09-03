#ifndef _C030_H_
#define _C030_H_

#include "global.h"

void System_DelayMilliseconds(long ms);
s32 Controller_CheckAccessoryEeprom(void);
u8 Controller_ReadAccessoryStatus(void);
void Controller_WriteAccessoryStatus(u8 arg0);

#endif // _C030_H_
