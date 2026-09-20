#include "minigame_dispatch.h"
#include "src/game_state.h"
#include "src/jpeg_stream.h"
#include "src/memory.h"

s16 D_80075E40[160] = {
    0, 105, 150, 220, 70, 120, 188, 75, 110, 160,
    60, 80, 120, 60, 70, 100, 70, 130, 170, 80,
    110, 65, 160, 120, 180, 85, 105, 100, 130, 80,
    100, 160, 100, 120, 160, 100, 140, 80, 130, 90,
    120, 110, 160, 90, 100, 160, 80, 125, 130, 150,
    60, 120, 70, 105, 105, 170, 90, 120, 100, 190,
    100, 140, 170, 90, 150, 150, 80, 120, 160, 80,
    165, 170, 140, 160, 120, 140, 160, 110, 170, 140,
    160, 80, 110, 100, 140, 150, 110, 90, 140, 220,
    85, 170, 150, 180, 150, 250, 120, 160, 102, 160,
    100, 130, 110, 160, 60, 100, 150, 140, 150, 110,
    140, 130, 200, 150, 120, 220, 80, 140, 80, 140,
    90, 110, 140, 160, 140, 130, 130, 150, 160, 90,
    270, 250, 100, 65, 120, 120, 110, 120, 70, 120,
    90, 140, 200, 180, 170, 180, 180, 100, 200, 180,
    180, 85, 100, 40, 0, 0, 0, 0, 0, 0,
};

void MiniGame_Dispatch(void) {
    u32 var_s0;
    u32 sp60;
    void (*func)(void);
    s32 sp58;
    UNUSED s32 pad[2];

    main_pool_push_state('KDCM');

    sp60 = 0;
    var_s0 = 0;
    sp58 = 1;

    func = FRAGMENT_LOAD(fragment2);
    func();

    while (sp58 != 0) {
        if ((var_s0 == 0) || (var_s0 == 1) || (var_s0 == 4)) {
            sp60 = FRAGMENT_LOAD_AND_CALL(fragment39, var_s0, sp60);
        }

        if (sp60 != 0) {
            main_pool_push_state('mini');

            ASSET_LOAD(D_3000000, kids_club_game_ui, 0);
            FRAGMENT_LOAD(fragment34);

            switch (sp60) {
                case 1: // magikarp minigame
                    FRAGMENT_LOAD(fragment4);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment6, 0, 0);
                    break;
                case 2: // clefairy minigame
                    FRAGMENT_LOAD(fragment4);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment7, 0, 0);
                    break;
                case 3: // rattata minigame
                    FRAGMENT_LOAD(fragment4);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment8, 0, 0);
                    break;
                case 4: // unfinished jigglypuff minigame
                    FRAGMENT_LOAD(fragment4);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment9, 0, 0);
                    break;
                case 5: // drowzee minigame
                    FRAGMENT_LOAD(fragment5);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment10, 0, 0);
                    break;
                case 6: // pikachu minigame
                    FRAGMENT_LOAD(fragment5);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment11, 0, 0);
                    break;
                case 7: // "minigame 7 sample" model viewer. debug placeholder?
                    FRAGMENT_LOAD(fragment5);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment12, 0, 0);
                    break;
                case 8: // "minigame 8 sample" model viewer. debug placeholder?
                    FRAGMENT_LOAD(fragment5);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment13, 0, 0);
                    break;
                case 9: // sushi go round
                    FRAGMENT_LOAD(fragment3);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment14, 0, 0);
                    break;
                case 10: // seems to be the "snap mode" code. However, the asset UI isnt loaded, so this call will
                         // crash.
                    FRAGMENT_LOAD(fragment3);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment15, 0, 0);
                    break;
                case 11: // credits? why from the kids club?
                    FRAGMENT_LOAD(fragment5);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment16, 0, 0);
                    break;
                case 12: // crashes. This fragment is the "Demo" cutscene bin, but this code passes an invalid demo ID,
                         // which doesnt exist.
                    FRAGMENT_LOAD(fragment3);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment17, 0, 0);
                    break;
                case 13: // ekans minigame
                    FRAGMENT_LOAD(fragment3);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment18, 0, 0);
                    break;
                case 14: // crashes...
                    FRAGMENT_LOAD(fragment3);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment19, 0, 0);
                    break;
                case 15: // metapod minigame
                    FRAGMENT_LOAD(fragment3);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment20, 0, 0);
                    break;
                case 16: // sandshrew minigame
                    FRAGMENT_LOAD(fragment3);
                    var_s0 = FRAGMENT_LOAD_AND_CALL(fragment21, 0, 0);
                    break;
            }

            main_pool_pop_state('mini');

            switch (var_s0) {
                case 0:
                case 1:
                case 3:
                case 4:
                    break;

                case 2:
                    sp58 = 0;
                    break;
            }
        } else {
            sp58 = 0;
        }
    }

    main_pool_pop_state('KDCM');
}
