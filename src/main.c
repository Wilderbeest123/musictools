#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "jtime.h"
#include "frite.h"
#include "sound.h"
#include "asoundlib.h"
#include "keys.h"
#include "screen.h"
#include "shapes.h"
#include "input.h"
#include "box.h"
#include "button.h"
#include "ui.h"
#include "toolbar.h"
#include "wform.h"
#include "textbox.h"
#include "chords.h"

int main(void)
{
    bool frite_midi = false;

    //Screen/UI
    screen_t s;
    input_t in;
    shapes_t sh;
    ui_system_t uisys;

    // Sound
    frite_t hw;
    keyboard_t k;

    //Screen/UI init
    screen_init(&s, SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init(&in, &s, &hw, !frite_midi);

    shapes_init(&sh);
    uisys_init(&uisys, &in);

    while(s.close == false)
    {
        input_update(&in);
        uisys_update(&uisys);

        // --- ENTER RENDER LOGIC HERE ---

        model_uniform_color(COLOR_INIT(255, 0, 127, 255));
        screen_swap_buffer(&s);
    }
}

