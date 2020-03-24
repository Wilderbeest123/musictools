#include "box.h"
#include "input.h"
#include "frite.h"
#include "ui.h"
#include "button.h"

static ui_node_t* box_ui_select(ui_node_t *node, v2 mpos);
static void box_ui_draw(ui_node_t *node, v2 pos);
static void box_ui_update(ui_node_t *node, input_t *in, void *data);
static void box_ui_free(ui_node_t *node);

static ui_ops_t box_ui_ops = { .select=box_ui_select,
                               .draw=box_ui_draw,
                               .update=box_ui_update,
                               .free=box_ui_free };

bound_t box_bound_get(v2 pos, v2 size)
{
    bound_t b;

    b.t = pos.y;
    b.b = pos.y + size.y;
    b.l = pos.x;
    b.r = pos.x + size.x;
    return b;
}

static void ui_button_callback(void *data, uint8_t event)
{
    ui_t *this = (ui_t*)data;
    this->b.col.r = (float)rand()/(float)(RAND_MAX/1.0);
    this->b.col.g = (float)rand()/(float)(RAND_MAX/1.0);
    this->b.col.b = (float)rand()/(float)(RAND_MAX/1.0);
}

ui_node_t* box_init(v2 pos, v2 size, unsigned int tid, gl_color_t c)
{
    ui_t *this = (ui_t*)malloc(sizeof(ui_t));
    ui_node_t *nptr;

    this->b.pos = pos;
    this->b.size = size;
    this->b.tid = tid;
    this->b.col = c;

    this->b.tid = gl_load_image("res/brick.png");
    this->b.n.ops = &box_ui_ops;
    this->b.n.next = NULL;
    this->h.first = NULL;

    nptr = button_init(V2(0,0),V2(32,32), gl_load_image("res/yoda.png"), ui_button_callback, this);
    ui_node_insert(&this->h, nptr);

    return &this->b.n;
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

static ui_node_t* box_ui_select(ui_node_t *node, v2 mpos)
{
    ui_t *this = CONTAINER_OF(CONTAINER_OF(node, box_t, n), ui_t, b);
    ui_node_t *nptr;
    v2 rpos;

    if(input_check_sel(mpos, this->b.pos, this->b.size))
    {
        rpos = V2_DEL(mpos, this->b.pos);

        for(nptr=this->h.first; nptr; nptr=nptr->next)
        {
            if(nptr->ops->select(nptr, rpos))
                return nptr;
        }

        return node;
    }

    return NULL;
}

static inline void box_draw(box_t b)
{
    square_draw(b.pos.x, b.pos.y, b.size.x, b.size.y, b.col);
}

static void box_ui_draw(ui_node_t *node, v2 pos)
{
    ui_t *this = CONTAINER_OF(CONTAINER_OF(node, box_t, n), ui_t, b);
    ui_node_t *nptr;

    glBindTexture(GL_TEXTURE_2D, this->b.tid);
    box_draw(this->b);

    for(nptr=this->h.first; nptr; nptr=nptr->next)
        nptr->ops->draw(nptr, this->b.pos);
}

static void box_ui_update(ui_node_t *node, input_t *in, void *data)
{
    box_t *this = CONTAINER_OF(node, box_t, n);
    ui_system_t *bsys = (ui_system_t*)data;
    ui_node_t *nptr;
    box_t *bptr;

    v2 ppos;
    int col;

    if((in->ev & INEVENT_MMOTION) != INEVENT_MMOTION)
        return;

    ppos = this->pos;
    this->pos.x += in->m.dpos.x;
    this->pos.y += in->m.dpos.y;

    //Check for collisions
    for(nptr=bsys->h.first; nptr; nptr=nptr->next)
    {
        bptr = CONTAINER_OF(nptr, box_t, n);

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

static void box_ui_free(ui_node_t *node)
{
    ui_t *this = CONTAINER_OF(CONTAINER_OF(node, box_t, n), ui_t, b);
    ui_node_t *nptr;

    for(nptr=this->h.first; nptr; nptr=nptr->next)
        nptr->ops->free(nptr);
}
