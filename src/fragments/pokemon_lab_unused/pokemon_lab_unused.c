#include "src/game_state.h"
#include "pokemon_lab_unused.h"

static u8 sFragment30Buffer[0x10];

//  unused wrapper to load the pokelab?
s32 Pokelab_LoadFragment(UNUSED s32 arg0, UNUSED s32 arg1) {
    FRAGMENT_LOAD_AND_CALL2(FRAGMENT_PROFESSOR_OAK_LAB, 1, 0);
    return 0x25;
}
