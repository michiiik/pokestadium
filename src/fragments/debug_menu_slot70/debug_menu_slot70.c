#include "debug_menu_slot70.h"
#include "src/game_state.h"

s32 DebugMenu_ForwardSlot70(UNUSED s32 arg0, UNUSED s32 arg1) {
    FRAGMENT_LOAD_AND_CALL2(fragment67, 0, 0);
    return 0x80;
}
