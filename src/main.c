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

static void draw_shapes(int mod, gl_color_t c1, gl_color_t c2)
{
    int i,j;

    for(i=0; i<20; i++)
    {
        c1.r = 0.05*i;

        for(j=0; j<20; j++)
        {
            c2.g = 0.05*j;

            if(i%mod)
            {
                square_draw(50*i, 50*j, 20, 20, c1);
            }
            else
                tri_draw(50*i, 50*j, 15, 15, c2);
        }
    }
}

int main(void)
{
    screen_t s;
    screen_init(&s, SCREEN_WIDTH, SCREEN_HEIGHT);

    shapes_t sh;
    shapes_init(&sh);

    int i=0;
    int j=2;
    gl_color_t c1 = COLOR_INIT(0,127,255,255);
    gl_color_t c2 = COLOR_INIT(255,0,255,255);
    jtime_t timer;
    timer_init(&timer, 200);

    while(1)
    {
        if(timer_check(&timer)){
            if(c1.b >= 1.0)
                c1.b = 0.0;
            else
                c1.b += 0.1;

            if(c2.b <= 0.0)
                c2.b = 1.0;
            else
                c2.b -= 0.1;

            if(i==0) {
                draw_shapes(j, c1, c2);
                i++;
            }
            else {
                draw_shapes(j, c1, c2);
                i=0;
            }

            if(j==4)
            {
                j=1;
            }
            j++;
            screen_swap_buffer(&s);
        }

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
