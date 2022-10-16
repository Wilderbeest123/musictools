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


typedef struct {
    gl_model_t s;
    uint32_t ftex; // Texture for the Fretboard

} fretboard_t;

void fretboard_init(fretboard_t *this)
{
    this->ftex = gl_load_image("res/fretboard.png");
    this->s = model_init_square();
}

void fretboard_draw(fretboard_t *this)
{
    glBindTexture(GL_TEXTURE_2D, this->ftex);
    square_draw(100, 100, 180, 120, COLOR_INIT(255,255,255,255));
}


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

    fretboard_t f;
    fretboard_init(&f);

    while(s.close == false)
    {
        input_update(&in);
        uisys_update(&uisys);

        // --- ENTER RENDER LOGIC HERE ---
        fretboard_draw(&f);

        model_uniform_color(COLOR_INIT(255, 0, 127, 255));
        screen_swap_buffer(&s);
    }
}

