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


int main(void)
{
    //Screen/UI
    screen_t s;
    input_t in;
    shapes_t sh;
    ui_system_t uisys;

    //Screen/UI init
    screen_init(&s, SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init(&in, &s, false);
    shapes_init(&sh);
    uisys_init(&uisys, &in);

    //Sound
    frite_t hw;
    keyboard_t k;

    //Sound init
    keys_init(&k);
    frite_open(&hw, &k.ev, true);

    waveform_t *w1 = waveform_init(10, 1000);
    int16_t buf[2205] = {0};

    textbox_add(V2(10,10), V2(200,30), "");

    gl_charset_t cset = gl_load_charset("res/OpenSans-Bold.ttf", 60, 30, 128);

    while(s.close == false)
    {
        input_update(&in);
        uisys_update(&uisys);
        frite_read(&hw);

        //Only write if space availabe in audio buffer.
        if(snd_pcm_avail(hw.audio_out) > hw.pback_out.period_size)
        {
            //Zeroize buffer prior to populating with data.
            memset(buf, 0, hw.pback_out.period_size*sizeof(uint16_t));

            keys_populate_buffer(&k, buf, hw.pback_out.period_size);
            snd_pcm_writei(hw.audio_out, buf, hw.pback_out.period_size);
            waveform_update(w1, buf, 2205);

        }

        keys_draw_notes(&k, V2(400,0), &cset);

        model_uniform_color(COLOR_INIT(255, 0, 127, 255));
        model_draw(w1->m, 0, 0, 800, 600, false);
        screen_swap_buffer(&s);
    }
}

