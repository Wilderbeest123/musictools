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
    gl_color_t c = COLOR_INIT(255,127,127,255);

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

static uint8_t box_check_collision(box_t *box1, box_t *box2)
{
    int t1,b1,l1,r1;
    int t2,b2,l2,r2;
    uint8_t result;

    result = BOX_COL_NONE;

    t1 = box1->pos.y - box1->size.y;
    b1 = box1->pos.y + box1->size.y;
    l1 = box1->pos.x - box1->size.x;
    r1 = box1->pos.x + box1->size.x;

    t2 = box2->pos.y - box2->size.y;
    b2 = box2->pos.y + box2->size.y;
    l2 = box2->pos.x - box2->size.x;
    r2 = box2->pos.x + box2->size.x;

    //First check if collision
    if(b1 >= t2 && t1 <= b2 && r1 >= l2 && l1 <= r2)
    {
        //Top Collision
        if(b1 >= t2 && t1 <= t2)
            result |= BOX_COL_X;

        //Bottom Collision
        if(t1 <= b2 && b1 >= b2)
            result |= BOX_COL_X;

        //Left Collision
        if(r1 >= l2 && l1 <= l2)
            result |= BOX_COL_Y;

        //Right Collision
        if(l1 <= r2 && r1 >= r2)
            result |= BOX_COL_Y;

        box1->col.g = 1.0f;
        box2->col.g = 1.0f;
        return result;
    }

    box1->col.g = 0.5f;
    box2->col.g = 0.5f;
    return BOX_COL_NONE;
}

static void boxsys_handle_select(box_system_t *bsys, box_t *box)
{
    input_t *in = bsys->in;
    box_t *bptr;
    v2 ppos;
    int col;

    if((in->ev & INEVENT_MMOTION) != INEVENT_MMOTION)
        return;

    ppos = box->pos;
    box->pos.x += in->m.pos.x - in->m.ppos.x;
    box->pos.y += in->m.pos.y - in->m.ppos.y;

    //Check for collisions
    for(int i=0; i<bsys->num; i++)
    {
        bptr = &bsys->b[i];

        if(bptr == box)
            continue;

        col = box_check_collision(box, bptr);

        if(col & BOX_COL_Y)
            box->pos.y = ppos.y;

        if(col & BOX_COL_X)
            box->pos.x = ppos.x;
    }
}

void boxsys_update(box_system_t *bsys)
{
    input_t *in = bsys->in;
    box_t *bptr;

    if(in->ev & INEVENT_LDOWN)
        boxsys_handle_ldown(bsys, in->m.pos);

    if(in->ev & INEVENT_LUP)
        boxsys_handle_lup(bsys);

    for(int i=0; i<bsys->num; i++)
    {
        bptr = &bsys->b[i];

        if(bptr->sel)
            boxsys_handle_select(bsys, bptr);

        box_draw(*bptr);
    }
}
