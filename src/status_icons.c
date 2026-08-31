#include "status_icons.h"

u32 Status_ToDisplayIconIndex(s32 arg0) {
    s32 ret;

    if (arg0 & 7) {
        ret = 2;
    } else if (arg0 & 8) {
        ret = 3;
    } else if (arg0 & 0x10) {
        ret = 4;
    } else if (arg0 & 0x20) {
        ret = 5;
    } else if (arg0 & 0x40) {
        ret = 6;
    } else {
        ret = 0;
    }

    return ret;
}

u32 PokemonType_ToDisplayIndex(s32 arg0) {
    if (arg0 >= 0x1B) {
        arg0 = 0;
    } else if (arg0 >= 0x14) {
        arg0 -= 0xB;
    }
    return arg0;
}
