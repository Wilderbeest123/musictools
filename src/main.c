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

void render_flats_sharps_test(gl_charset_t *cset, uint32_t flat, uint32_t sharp)
{
    char notes[8] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G' };
    int xpos = 0;
    int ypos = 300;
    gl_char_t c;
    int i, fs_size, sharp_x, flat_x, y_offset;
    gl_color_t col = COLOR_INIT(0,0,0,255);

    fs_size = 2*cset->rsize/3;
    sharp_x = fs_size/8;
    flat_x = 3*fs_size/8;
    y_offset = 5*fs_size/8;

    for(i=0; i<7; i++)
    {
        c = charset_get_char(cset, notes[i]);

        glBindTexture(GL_TEXTURE_2D, c.tid);
        square_draw(xpos, ypos, c.size.x, c.size.y, col);
        xpos += c.advance;

        glBindTexture(GL_TEXTURE_2D, sharp);
        square_draw(xpos-sharp_x, ypos+y_offset, fs_size, fs_size, col);
        xpos += fs_size-sharp_x;

        glBindTexture(GL_TEXTURE_2D, c.tid);
        square_draw(xpos, ypos, c.size.x, c.size.y, col);
        xpos += c.advance;

        glBindTexture(GL_TEXTURE_2D, flat);
        square_draw(xpos-flat_x, ypos+y_offset, fs_size, fs_size, col);
        xpos += fs_size-flat_x;

        if(xpos > SCREEN_WIDTH) {
            xpos = 0;
            ypos += cset->rsize;
        }
    }
}

int main(void)
{
    bool frite_midi = true;

    //Screen/UI
    screen_t s;
    input_t in;
    shapes_t sh;
    ui_system_t uisys;

    //Sound
    frite_t hw;
    keyboard_t k;

    //Screen/UI init
    screen_init(&s, SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init(&in, &s, !frite_midi);

    if(!frite_midi)
        in.midi_ev = &k.ev;

    shapes_init(&sh);
    uisys_init(&uisys, &in);

    //Sound init
    keys_init(&k);
    frite_open(&hw, &k.ev, frite_midi);

    waveform_t *w1 = waveform_init(10, 1000);
    int16_t buf[2205] = {0};

    textbox_add(V2(10,10), V2(200,30), "");
    gl_charset_t cset = gl_load_charset("res/OpenSans-Bold.ttf", 40, 30, 128);
    gl_char_t c = charset_get_char(&cset, 'A');

    //uint32_t sharp = gl_load_image_rgba("res/sharp.png");
    //uint32_t flat = gl_load_image_rgba("res/flat.png");

    while(s.close == false)
    {
        input_update(&in);
        uisys_update(&uisys);

        if(frite_midi)
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

        //render_flats_sharps_test(&cset, flat, sharp);

        model_uniform_color(COLOR_INIT(255, 0, 127, 255));
        model_draw(w1->m, 0, 0, 800, 600, false);
        screen_swap_buffer(&s);
    }
}
