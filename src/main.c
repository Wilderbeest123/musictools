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

    //Sound
    frite_t hw;
    keyboard_t k;

    keys_init(&k);
    in.midi_ev = &k.ev;
    frite_open(&hw, NULL, false);

    screen_init(&s, SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init(&in, &s);
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

    return 0;
}

void music_loop(void)
{
    keyboard_t k;
    frite_t hw;
    jtime_t timer;
    uint16_t *out_buffer;

    keys_init(&k);
    frite_open(&hw, &k.ev, false);
    print_pback_settings();

    timer_init(&timer, 20);
    out_buffer = calloc(hw.pback_out.period_size, sizeof(uint16_t));

    while(1)
    {
        if(timer_check(&timer)){
            //print_time();
            frite_read(&hw);
            keys_print_notes(&k);

            //Only write if space availabe in audio buffer.
            if(snd_pcm_avail(hw.audio_out) > hw.pback_out.period_size)
            {
                //Zeroize buffer prior to populating with data.
                memset(out_buffer, 0, hw.pback_out.period_size*sizeof(uint16_t));
                keys_populate_buffer(&k, out_buffer, hw.pback_out.period_size);
                snd_pcm_writei(hw.audio_out, out_buffer, hw.pback_out.period_size);
            }
        }
    }

}
