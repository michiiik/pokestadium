#include "global.h"

void Audio_CopyDataWithCacheSync(s32 arg0, void* arg1, s32 arg2) {
    osInvalICache(arg1, arg2);
    osInvalDCache(arg1, arg2);
    func_80050BA0(arg0, arg1);
    osWritebackDCache(arg1, arg2);
}
