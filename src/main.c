#include <stdbool.h>
#include <stdint.h>
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

static void genbox_handle(box_system_t *b, v2 pos, int size)
{
    gl_color_t c = COLOR_INIT(0,127,255,255);

    box_t box;
    box.pos.x = pos.x;
    box.pos.y = pos.y;
    box.size.x = size;
    box.size.y = size;
    box.col = c;
    boxsys_append(b, box);
}

int main(void)
{
    screen_t s;
    input_t in;
    shapes_t sh;
    box_system_t b;

    screen_init(&s, SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init(&in, &s);
    shapes_init(&sh);
    boxsys_init(&b);

    jtime_t timer;
    timer_init(&timer, 200);
    gl_color_t c1 = COLOR_INIT(0,127,255,255);

    while(s.close == false)
    {
        input_handle(&in);

        if(in.ev & INEVENT_LDOWN)
            genbox_handle(&b, in.m.pos, 25);

        boxsys_draw(&b);
        screen_swap_buffer(&s);
        usleep(100);
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
    frite_open(&hw, &k.ev);
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

    snd_rawmidi_drain(hw.midi_in);
    snd_rawmidi_close(hw.midi_in);
}
