#include "particle_data_library.h"
#include "src/gfx_buffer.h"
#include "src/fragments/snorlax_game/snorlax_game.h"

void DisplayList_InitToggledBlendModeSegment(s32 arg0, DisplayListState* state) {
    Gfx* gfx;

    if (arg0 == 2) {
        gfx = (Gfx*)Gfx_AllocDisplayList(0x10);
        state->gfx = gfx;
        if (D_86402848 == 0) {
            {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFC119604;    _gfx->words.w1 = 0xFF13FFFF;    }
        } else {
            {    Gfx *_gfx = (Gfx *)(gfx++);    _gfx->words.w0 = 0xFCFFFFFF;    _gfx->words.w1 = 0xFFFDF638;    }
        }
        gSPEndDisplayList(gfx++);
    }
}
