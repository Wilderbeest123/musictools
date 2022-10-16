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
    uint32_t utex; // Uniform Texture for Fret Holdings

} fretboard_t;

void fretboard_init(fretboard_t *this)
{
    this->ftex = gl_load_image("res/fretboard.png");
    this->utex = gl_load_image("res/white.png");
    this->s = model_init_square();
}

// This is a dummy function used to determine where
// to render the appropriate locations for the fretboard
void fretboard_draw(fretboard_t *this)
{
    int xval = 33; // Offset for each note
    int yval = 20;
    v2 fsize = { 180, 120 }; // Total size of render
    v2 nsize = { 18, 18 }; // Size of each Fret Note
    v2 soff = { 16, 2 }; // The start point offset when rendering

    // Render the Base Fretboard
    glBindTexture(GL_TEXTURE_2D, this->ftex);
    square_draw(100, 100, fsize.x, fsize.y, COLOR_INIT(255,255,255,255));

    // Render the Finger Holdings
    glBindTexture(GL_TEXTURE_2D, this->utex);

    for(int i=0; i<5; i++) {
        for(int j=0; j<6; j++) {
            circle_draw(100 + soff.x + (i*xval), 100 + soff.y + (j*yval),
                        nsize.x, nsize.y, COLOR_INIT(255,0,255,100));
        }
    }
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

