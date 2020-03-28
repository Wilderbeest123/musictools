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

    while(s.close == false)
    {
        input_update(&in);
        uisys_update(&uisys);
        screen_swap_buffer(&s);
    }

    return 0;
}


void main_loop()
{
    //Screen/UI
    screen_t s;
    input_t in;
    shapes_t sh;
    ui_system_t uisys;

    //Sound
    frite_t hw;
    keyboard_t k;

    //Sound init
    keys_init(&k);
    in.midi_ev = &k.ev;
    frite_open(&hw, NULL, false);

    //Screen/UI init
    screen_init(&s, SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init(&in, &s, true);
    shapes_init(&sh);
    uisys_init(&uisys, &in);

    while(s.close == false)
    {
        input_update(&in);
        uisys_update(&uisys);
        screen_swap_buffer(&s);

        //Only write if space availabe in audio buffer.
        if(snd_pcm_avail(hw.audio_out) > hw.pback_out.period_size)
        {
            //Zeroize buffer prior to populating with data.
            memset(hw.out_buffer, 0, hw.pback_out.period_size*sizeof(uint16_t));
            keys_populate_buffer(&k, hw.out_buffer, hw.pback_out.period_size);
            snd_pcm_writei(hw.audio_out, hw.out_buffer, hw.pback_out.period_size);
        }
    }

    if(hw.midi_ev) {
        snd_rawmidi_drain(hw.midi_in);
        snd_rawmidi_close(hw.midi_in);
    }
}
