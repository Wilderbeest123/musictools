#include "box.h"


void boxsys_init(box_system_t *b, input_t *in)
{
    b->num = 0;
    b->in = in;
}

static inline void box_draw(box_t b)
{
    square_draw(b.pos.x, b.pos.y, b.size.x, b.size.y, b.col);
}

static void boxsys_append(box_system_t *b, v2 pos, int size)
{
    box_t box;
    gl_color_t c = COLOR_INIT(255,127,255,255);

    if(b->num == MAX_BOX) {
        printf("ERROR: Already reached max box\n");
        return;
    }

    box.pos = pos;
    box.size.x = size;
    box.size.y = size;
    box.col = c;

    b->b[b->num] = box;
    b->num++;
}

static void boxsys_handle_ldown(box_system_t *bsys, v2 pos)
{
    int t,b,l,r;
    box_t *box;
    bool append = true;

    //printf("X: %d Y: %d\n", pos.x, pos.y);

    for(int i=0; i<bsys->num; i++)
    {
        box = &bsys->b[i];
        t = box->pos.y - box->size.y;
        b = box->pos.y + box->size.y;
        l = box->pos.x - box->size.x;
        r = box->pos.x + box->size.x;

        //printf("T: %d B: %d L: %d R: %d\n", t, b, l, r);

        if(pos.y >= t && pos.y <= b && pos.x >= l && pos.x <= r)
        {
            box->sel = true;
            box->col.r = 1.0;
            append = false;
        }
    }

    if(append)
        boxsys_append(bsys, bsys->in->m.pos, 30);
}

static void boxsys_handle_lup(box_system_t *bsys)
{
    box_t *box;

    for(int i=0; i<bsys->num; i++)
    {
        box = &bsys->b[i];
        box->sel = false;
        box->col.r = 0.0;
    }
}

void boxsys_update(box_system_t *b)
{
    input_t *in = b->in;
    box_t *box;

    if(in->ev & INEVENT_LDOWN)
        boxsys_handle_ldown(b, in->m.pos);

    if(in->ev & INEVENT_LUP)
        boxsys_handle_lup(b);

    for(int i=0; i<b->num; i++)
    {
        box = &b->b[i];

        if(box->sel)
            box->pos = in->m.pos;

        box_draw(*box);
    }
}

