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
#include "ui.h"

typedef struct
{
    box_t b;
    uint8_t state;
    button_t en;
    button_t move;

} osc_t;

void ui_enable(void *data, uint8_t event)
{
    ui_t *o = (ui_t*)data;

    o->b.col.r = (float)rand()/(float)(RAND_MAX/1.0);
    o->b.col.g = (float)rand()/(float)(RAND_MAX/1.0);
    o->b.col.b = (float)rand()/(float)(RAND_MAX/1.0);
}

void ui_move(void *data, uint8_t event)
{
    ui_t *this = (ui_t*)data;
    this->b.sel = true;
}

void ui_init(ui_t *o)
{
    gl_color_t c = COLOR_INIT(127,127,255,255);
    o->b = box_init(300, 300, 200, 160, c);
    o->state = 0;
    o->h.first = NULL;

    ui_node_t *nptr;

    nptr = button_init(V2(0,0),V2(32,32), gl_load_image("res/brick.png"), ui_enable, o);
    ui_node_insert(&o->h, nptr);

    nptr = button_init(V2(32,0),V2(32,32), gl_load_image("res/yoda.png"), ui_move, o);
    ui_node_insert(&o->h, nptr);

}

void ui_update(input_t *in, ui_t *o)
{
    ui_node_t *nptr;
    v2 rpos;

    if(in->ev & INEVENT_LDOWN)
    {
        rpos = V2_DEL(in->m.pos, o->b.pos);

        for(nptr=o->h.first; nptr; nptr=nptr->next)
        {
            if(nptr->ops->select(nptr, rpos) == true)
                break;
        }
    }

    if(in->ev & INEVENT_LUP)
        o->b.sel = false;

    if(o->b.sel && (in->ev & INEVENT_MMOTION))
    {
        o->b.pos.x += in->m.dpos.x;
        o->b.pos.y += in->m.dpos.y;
    }

    box_draw(o->b);

    for(nptr=o->h.first; nptr; nptr=nptr->next)
        nptr->ops->draw(nptr, o->b.pos);
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

    ui_t o;
    ui_init(&o);

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
        ui_update(&in, &o);

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
