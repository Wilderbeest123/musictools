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

/*
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

    toolbar_init(&uisys);

    while(s.close == false)
    {
        input_update(&in);
        uisys_update(&uisys);
        screen_swap_buffer(&s);
    }

    return 0;
}
*/

gl_model_t waveform_init(void)
{
    gl_model_t m = MODEL_INIT();
    tone_t t;
    int16_t buf[1000] = {0};
    float step;
    GLfloat *vpos, *tcoord;
    int i;

    tone_init(&t, 880.0);
    square_wave(&t, buf, 1000);
    tri_wave(&t, buf, 1000);

    m.v_len = 1000;
    m.v_type = GL_LINE_STRIP;

    step = 2.0/1000.0;

    vpos = (GLfloat *)malloc(sizeof(GLfloat)*3*(m.v_len));
    tcoord = (GLfloat *)malloc(sizeof(GLfloat)*2*(m.v_len));

    for(i=0; i<1000; i++)
    {
        (vpos+3*i)[0] =-1.0 + step*i;
        (vpos+3*i)[1] = (float)buf[i]/t.maxval;
        (vpos+3*i)[2] = 0.0f;

        (tcoord+2*i)[0] = 0.0f;
        (tcoord+2*i)[1] = 0.0f;
    }

    for(i=0; i<999; i++)
    {
        printf("%.2f %.2f %.2f\n",
               (vpos+3*i)[0],(vpos+3*i)[1],(vpos+3*i)[2]);
    }

    //Init VAO
    glGenVertexArrays(1, &m.vao);
    glBindVertexArray(m.vao);

    //Init VBOs
    model_init_vpos(&m.vpos, vpos, 3*m.v_len);
    model_init_tcoord(&m.tcoord, tcoord, 2*m.v_len);

    glBindVertexArray(0); //Unbind VAO
    free(vpos);
    free(tcoord);
    return m;
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

    gl_model_t w = waveform_init();

    while(s.close == false)
    {
        input_update(&in);
        uisys_update(&uisys);

        model_uniform_color(COLOR_INIT(255, 0, 0, 255));
        model_draw(w, 0, 0, 800, 600, true);

        screen_swap_buffer(&s);
    }
}
