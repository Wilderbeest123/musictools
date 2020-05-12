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

typedef struct
{
    gl_model_t m;
    int num_periods;
    int psize;
    float x_step;
    int maxval;
    int idx;
    GLfloat *vpos;

} waveform_t;

void waveform_init(waveform_t *w, int num_periods, int size)
{
    int i, bsize;
    GLfloat *tcoord;

    bsize = size;
    w->num_periods = num_periods;
    w->psize = bsize/w->num_periods;
    w->maxval = pback_maxval();

    w->x_step = 2.0/(float)bsize;
    w->idx = 0;

    w->m.v_len = bsize;
    w->m.v_type = GL_LINE_STRIP;

    w->vpos = (GLfloat *)malloc(sizeof(GLfloat)*3*(w->m.v_len));
    tcoord = (GLfloat *)malloc(sizeof(GLfloat)*2*(w->m.v_len));

    for(i=0; i<bsize; i++)
    {
        (w->vpos+3*i)[0] = -1.0f;
        (w->vpos+3*i)[1] = 0.0f;
        (w->vpos+3*i)[2] = 0.0f;

        (tcoord+2*i)[0] = 0.0f;
        (tcoord+2*i)[1] = 0.0f;
    }

    //Init VAO
    glGenVertexArrays(1, &w->m.vao);
    glBindVertexArray(w->m.vao);

    //Init pos VBO
    glGenBuffers(1, &w->m.vpos);
    glBindBuffer(GL_ARRAY_BUFFER, w->m.vpos);
    glBufferData(GL_ARRAY_BUFFER, 3*w->m.v_len*sizeof(GL_FLOAT), &w->m.vpos, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(VERT_ATTR_POS);
    glVertexAttribPointer(VERT_ATTR_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //Init tcoord VBO
    model_init_tcoord(&w->m.tcoord, tcoord, 2*w->m.v_len);

    glBindVertexArray(0); //Unbind VAO

    free(tcoord);
}

void waveform_update(waveform_t *w, int16_t *buf, uint16_t period)
{
    float y_step;
    int i, idx, offset;
    int start, pidx, size;

    y_step = period/(float)(w->psize);
    offset = w->idx*w->psize;

    //X Axis shift
    for(i=0; i<offset; i++)
        (w->vpos+3*i)[0] += w->x_step*w->psize;

    for(i=offset; i<offset+w->psize; i++)
    {
        //X Axis shift
        (w->vpos+3*i)[0] = -1.0 + w->x_step*(i-offset);

        //Y axis
        idx = round(y_step*(i-offset));
        (w->vpos+3*i)[1] = buf[idx]/(float)w->maxval;
    }

    //X Axis shift
    for(i=offset+w->psize; i<w->m.v_len; i++)
        (w->vpos+3*i)[0] += w->x_step*w->psize;

    glBindVertexArray(w->m.vao);
    glBindBuffer(GL_ARRAY_BUFFER, w->m.vpos);

    size = 3*sizeof(GL_FLOAT)*w->psize;
    start = size*(w->idx+1);

    for(i=0; i<w->num_periods; i++)
    {
        pidx = 3*(i*w->psize);
        start -= size;

        if(start < 0)
            start = size*(w->num_periods-1);

        glBufferSubData(GL_ARRAY_BUFFER, start, size, &w->vpos[pidx]);
    }

    w->idx++;

    if(w->idx >= w->num_periods)
        w->idx = 0;
}

waveform_t w1,w2,w3,w4;

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

    //Sound init
    keys_init(&k);
    //in.midi_ev = &k.ev;
    frite_open(&hw, &k.ev, true);

    //Screen/UI init
    screen_init(&s, SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init(&in, &s, false);
    shapes_init(&sh);
    uisys_init(&uisys, &in);

    //waveform_t w1,w3,w4;

    //waveform_t w1;
    waveform_init(&w1, 100, 1000);
    waveform_init(&w2, 5, 10000);
    waveform_init(&w3, 2, 5000);
    waveform_init(&w4, 1, 500);

    int16_t buf[2205] = {0};

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
            waveform_update(&w1, buf, 2205);
            waveform_update(&w2, buf, 2205);
            waveform_update(&w3, buf, 2205);
            waveform_update(&w4, buf, 2205);
        }

        model_uniform_color(COLOR_INIT(255, 255, 0, 255));
        model_draw(w1.m, 0, 0, 400, 300, false);

        model_uniform_color(COLOR_INIT(255, 0, 0, 255));
        model_draw(w2.m, 400, 0, 400, 300, false);

        model_uniform_color(COLOR_INIT(255, 0, 255, 255));
        model_draw(w3.m, 400, 300, 400, 300, false);

        model_uniform_color(COLOR_INIT(0, 0, 255, 255));
        model_draw(w4.m, 0, 300, 400, 300, false);

        screen_swap_buffer(&s);
    }
}
