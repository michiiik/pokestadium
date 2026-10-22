#include "global.h"
#include "input.h"

static s32 pad_D_800AC880[0x2];
static s32 gMenuInput_RepeatTimer;
static Controller* gMenuInput_Controller;

void Input_ResetRepeatState(void) {
}

void Input_SetRepeatController(Controller* arg0) {
    gMenuInput_RepeatTimer = 0xA;
    gMenuInput_Controller = arg0;
}

s32 Input_GetRepeatedDPad(void) {
    if (gMenuInput_Controller->buttonPressed & 0x800) {
        gMenuInput_RepeatTimer = 0xA;
        return 0x800;
    }

    if (gMenuInput_Controller->buttonPressed & 0x400) {
        gMenuInput_RepeatTimer = 0xA;
        return 0x400;
    }

    if (gMenuInput_Controller->buttonPressed & 0x200) {
        gMenuInput_RepeatTimer = 0xA;
        return 0x200;
    }

    if (gMenuInput_Controller->buttonPressed & 0x100) {
        gMenuInput_RepeatTimer = 0xA;
        return 0x100;
    }

    gMenuInput_RepeatTimer--;
    if (gMenuInput_RepeatTimer <= 0) {
        gMenuInput_RepeatTimer = 0;

        // clang-format off
        if (gMenuInput_Controller->buttonDown & 0x800) {
            gMenuInput_RepeatTimer = 2; return 0x800;
        }
        // clang-format on

        if (gMenuInput_Controller->buttonDown & 0x400) {
            gMenuInput_RepeatTimer = 2;
            return 0x400;
        }

        if (gMenuInput_Controller->buttonDown & 0x200) {
            gMenuInput_RepeatTimer = 2;
            return 0x200;
        }

        if (gMenuInput_Controller->buttonDown & 0x100) {
            gMenuInput_RepeatTimer = 2;
            return 0x100;
        }
    }

    return 0;
}
