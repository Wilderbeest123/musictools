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

void waveform_init(waveform_t *w)
{
    int i, bsize;
    GLfloat *tcoord;

    bsize = 1000;
    w->num_periods = 10;
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
    in.midi_ev = &k.ev;
    frite_open(&hw, NULL, false);

    //Screen/UI init
    screen_init(&s, SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init(&in, &s, true);
    shapes_init(&sh);
    uisys_init(&uisys, &in);

    print_pback_settings();

    tone_t t;
    int16_t buf[2205] = {0};
    tone_init(&t, 100.0);
    waveform_t w;
    waveform_init(&w);

    while(s.close == false)
    {
        input_update(&in);
        uisys_update(&uisys);

        if(in.ev & INEVENT_KDOWN) {
            memset(buf, 0, 2205*sizeof(int16_t));
            tri_wave(&t, buf, 2205);
            square_wave(&t, buf, 2205);
            waveform_update(&w, buf, 2205);
        }

        if(1) {
            memset(buf, 0, 2205*sizeof(int16_t));
            tri_wave(&t, buf, 2205);
            waveform_update(&w, buf, 2205);
        }

        model_uniform_color(COLOR_INIT(255, 0, 0, 255));
        model_draw(w.m, 0, 0, 800, 600, false);

        screen_swap_buffer(&s);
    }
}
