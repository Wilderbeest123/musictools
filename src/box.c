#include "box.h"
#include "input.h"

void boxsys_init(box_system_t *b, input_t *in)
{
    b->num = 0;
    b->in = in;
}

static void boxsys_append(box_system_t *b, v2 pos, v2 size)
{
    box_t box;
    gl_color_t c = COLOR_INIT(255,255,127,255);

    if(b->num == MAX_BOX) {
        printf("ERROR: Already reached max box\n");
        return;
    }

    box.pos = pos;
    box.size = size;
    box.col = c;

    b->b[b->num] = box;
    b->num++;
}

static void boxsys_handle_ldown(box_system_t *bsys, v2 pos)
{
    box_t *box;
    bool append = true;
    v2 size;

    //printf("X: %d Y: %d\n", pos.x, pos.y);

    for(int i=0; i<bsys->num; i++)
    {
        box = &bsys->b[i];

        if(input_check_sel(pos, box->pos, box->size) == true)
        {
            box->sel = true;
            box->col.r = 1.0;
            append = false;
        }
    }

    if(append) {
        size.x = SCREEN_WIDTH/10;
        size.y = SCREEN_HEIGHT/10;
        boxsys_append(bsys, bsys->in->m.pos, size);
    }
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

bound_t box_bound_get(v2 pos, v2 size)
{
    bound_t b;

    b.t = pos.y;
    b.b = pos.y + size.y;
    b.l = pos.x;
    b.r = pos.x + size.x;
    return b;
}

static uint8_t box_check_collision(v2 ppos, box_t *box1, box_t *box2)
{
    bound_t b1,b2,b1_prev;
    uint8_t result;

    result = BOX_COL_NONE;

    b1 = box_bound_get(box1->pos, box1->size);
    b2 = box_bound_get(box2->pos, box2->size);
    b1_prev = box_bound_get(ppos, box1->size);

    //First check if there is a collision
    if(b1.b >= b2.t && b1.t <= b2.b && b1.r >= b2.l && b1.l <= b2.r)
    {
        box1->col.g = 1.0f;
        box2->col.g = 1.0f;

        //Top Collision
        if(b1.b >= b2.t && b1.t <= b2.t && b1_prev.b <= b2.t)
            return BOX_COL_T;

        //Bottom Collision
        if(b1.t <= b2.b && b1.b >= b2.b && b1_prev.t >= b2.b)
            return BOX_COL_B;

        //Left Collision
        if(b1.r >= b2.l && b1.l <= b2.l && b1_prev.r <= b2.l)
            return BOX_COL_L;

        //Right Collision
        if(b1.l <= b2.r && b1.r >= b2.r && b1_prev.l >= b2.r)
            return BOX_COL_R;
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
    box->pos.x += in->m.dpos.x;
    box->pos.y += in->m.dpos.y;

    //Check for collisions
    for(int i=0; i<bsys->num; i++)
    {
        bptr = &bsys->b[i];

        if(bptr == box)
            continue;

        col = box_check_collision(ppos, box, bptr);

        if(col & BOX_COL_L)
            box->pos.x = bptr->pos.x - bptr->size.x/2 - box->size.x/2;

        if(col & BOX_COL_R)
            box->pos.x = bptr->pos.x + bptr->size.x/2 + box->size.x/2;

        if(col & BOX_COL_T)
            box->pos.y = bptr->pos.y - bptr->size.y/2 - box->size.y/2;

        if(col & BOX_COL_B)
            box->pos.y = bptr->pos.y + bptr->size.y/2 + box->size.y/2;
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
