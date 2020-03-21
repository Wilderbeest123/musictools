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
#include "button.h"
#include <stdlib.h>

typedef struct
{
    box_t b;
    uint8_t state;
    button_t en;
    button_t move;

} osc_t;

void osc_enable(void *data, uint8_t event)
{
    osc_t *o = (osc_t*)data;

    if(event & INEVENT_LDOWN)
    {
        o->b.col.r = (float)rand()/(float)(RAND_MAX/1.0);
        o->b.col.g = (float)rand()/(float)(RAND_MAX/1.0);
        o->b.col.b = (float)rand()/(float)(RAND_MAX/1.0);
    }
}

void osc_move(void *data, uint8_t event)
{
    osc_t *this = (osc_t*)data;
    printf("Move\n");

    if(event & INEVENT_LDOWN)
        this->b.sel = true;
}

void osc_init(osc_t *o)
{
    gl_color_t c = COLOR_INIT(127,127,255,255);
    o->b = box_init(300, 300, 200, 160, c);
    o->state = 0;

    o->en.size = V2(32,32);
    o->en.pos.x = 0;
    o->en.pos.y = 0;
    o->en.fn = osc_enable;
    o->en.data = o;
    o->en.tid = gl_load_image("res/brick.png");

    o->move.size = V2(32,32);
    o->move.pos.x = 32;
    o->move.pos.y = 0;
    o->move.fn = osc_move;
    o->move.data = o;
    o->move.tid = gl_load_image("res/yoda.png");
}

void osc_update(input_t *in, osc_t *o)
{
    if(in->ev & INEVENT_LDOWN)
    {
        if(input_check_sel(in->m.pos, V2_ADD(o->b.pos, o->en.pos), o->en.size))
            o->en.fn(o->en.data, in->ev);

        if(input_check_sel(in->m.pos, V2_ADD(o->b.pos, o->move.pos), o->move.size))
            o->move.fn(o->move.data, in->ev);
    }

    if(in->ev & INEVENT_LUP)
        o->b.sel = false;

    if(o->b.sel && (in->ev & INEVENT_MMOTION))
    {
        o->b.pos.x += in->m.dpos.x;
        o->b.pos.y += in->m.dpos.y;
    }

    box_draw(o->b);
    button_draw_rel(&o->en, o->b.pos);
    button_draw_rel(&o->move, o->b.pos);
}

int main(void)
{
    screen_t s;
    input_t in;
    shapes_t sh;
    box_system_t bsys;

    screen_init(&s, SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init(&in, &s);
    shapes_init(&sh);

    osc_t o;
    osc_init(&o);

    /*
      boxsys_init(&bsys, &in);
    button_t b = BUTTON_INIT(50,50,25,25);
    b.tid = gl_load_image("res/brick.png");
    b.fn = button_fn;
    b.data = &bsys;
    */
    gl_color_t c = COLOR_INIT(255,127,255,127);

    while(s.close == false)
    {
        input_update(&in);
        //boxsys_update(&bsys);
        //button_handle(&in, &b);
        osc_update(&in, &o);

        screen_swap_buffer(&s);
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
