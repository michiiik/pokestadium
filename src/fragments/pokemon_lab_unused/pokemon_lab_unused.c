#include "src/game_state.h"

#include "pokemon_lab_unused.h"

//  unused wrapper to load the pokelab?
s32 PokemonLab_UnusedEntryWrapper(UNUSED s32 arg0, UNUSED s32 arg1) {
    FRAGMENT_LOAD_AND_CALL2(fragment22, 1, 0);
    return 0x25;
}
