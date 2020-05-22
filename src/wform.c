#include "wform.h"
#include "frite.h"

waveform_t* waveform_init(int p_num, int bsize)
{
    int i;
    GLfloat *tcoord;
    waveform_t *w;

    w = (waveform_t*)calloc(1, sizeof(waveform_t));
    w->p_num = p_num;
    w->p_size = bsize/w->p_num;
    w->p_idx = 0;

    w->maxval = pback_maxval();
    w->x_step = 2.0/(float)bsize;

    w->m.v_len = bsize;
    w->m.v_type = GL_LINE_STRIP;

    w->vpos = (GLfloat *)calloc(3*(w->m.v_len), sizeof(GLfloat));
    tcoord = (GLfloat *)calloc(2*(w->m.v_len), sizeof(GLfloat));

    for(i=0; i<bsize; i++)
      (w->vpos+3*i)[0] = -1.0f;

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
    return w;
}

void waveform_update(waveform_t *w, int16_t *buf, uint16_t period)
{
    float y_step;
    int i, idx, offset;
    int start, pidx, size;

    y_step = period/(float)(w->p_size);
    offset = w->p_idx*w->p_size;

    //X Axis shift
    for(i=0; i<offset; i++)
        (w->vpos+3*i)[0] += w->x_step*w->p_size;

    for(i=offset; i<offset+w->p_size; i++)
    {
        //X Axis shift
        (w->vpos+3*i)[0] = -1.0 + w->x_step*(i-offset);

        //Y axis
        idx = round(y_step*(i-offset));
        (w->vpos+3*i)[1] = buf[idx]/(float)w->maxval;
    }

    //X Axis shift
    for(i=offset+w->p_size; i<w->m.v_len; i++)
        (w->vpos+3*i)[0] += w->x_step*w->p_size;

    glBindVertexArray(w->m.vao);
    glBindBuffer(GL_ARRAY_BUFFER, w->m.vpos);

    size = 3*sizeof(GL_FLOAT)*w->p_size;
    start = size*(w->p_idx+1);

    for(i=0; i<w->p_num; i++)
    {
        pidx = 3*(i*w->p_size);
        start -= size;

        if(start < 0)
            start = size*(w->p_num-1);

        glBufferSubData(GL_ARRAY_BUFFER, start, size, &w->vpos[pidx]);
    }

    w->p_idx++;

    if(w->p_idx >= w->p_num)
        w->p_idx = 0;
}
