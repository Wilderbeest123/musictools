#include "box.h"
#include "input.h"
#include "frite.h"
#include "ui.h"
#include "button.h"
#include "textbox.h"

static void box_ui_update(ui_node_t *node, input_t *in, void *data);
static void box_ui_free(ui_node_t *node);

static ui_ops_t box_ui_ops = { .select=ui_select,
                               .draw=ui_draw,
                               .update=box_ui_update,
                               .free=ui_free };

bound_t box_bound_get(v2 pos, v2 size)
{
    bound_t b;

    b.t = pos.y;
    b.b = pos.y + size.y;
    b.l = pos.x;
    b.r = pos.x + size.x;
    return b;
}

ui_t* box_init(v2 pos, v2 size, gl_color_t c)
{
    ui_t *this = (ui_t*)malloc(sizeof(ui_t));
    this->pos = pos;
    this->size = size;
    this->color = c;

    this->tid = gl_load_image("res/white.png");
    this->n.ops = &box_ui_ops;

    this->n.next = NULL;
    this->h.first = NULL;
    return this;
}

static uint8_t box_check_collision(v2 ppos, ui_t *box1, ui_t *box2)
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
        box1->color.g = 1.0f;
        box2->color.g = 1.0f;

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

    box1->color.g = 0.5f;
    box2->color.g = 0.5f;
    return BOX_COL_NONE;
}

static void box_ui_update(ui_node_t *node, input_t *in, void *data)
{
    ui_t *this = CONTAINER_OF(node, ui_t, n);
    ui_system_t *bsys = (ui_system_t*)data;
    ui_node_t *nptr;
    ui_t *bptr;

    v2 ppos;
    int col;

    if((in->ev & INEVENT_MMOTION) != INEVENT_MMOTION)
        return;

    if(in->m.lPress == false)
        return;

    ppos = this->pos;
    this->pos.x += in->m.dpos.x;
    this->pos.y += in->m.dpos.y;

    /* TODO(jack): Below code assumes all nodes in list are ui_t. Need
     * to fix at some stage. */
    return;

    //Check for collisions
    for(nptr=bsys->h.first; nptr; nptr=nptr->next)
    {
        bptr = CONTAINER_OF(nptr, ui_t, n);

        if(bptr == this)
            continue;

        col = box_check_collision(ppos, this, bptr);

        if(col & BOX_COL_L)
            this->pos.x = bptr->pos.x - bptr->size.x/2 - this->size.x/2;

        if(col & BOX_COL_R)
            this->pos.x = bptr->pos.x + bptr->size.x/2 + this->size.x/2;

        if(col & BOX_COL_T)
            this->pos.y = bptr->pos.y - bptr->size.y/2 - this->size.y/2;

        if(col & BOX_COL_B)
            this->pos.y = bptr->pos.y + bptr->size.y/2 + this->size.y/2;
    }
}
