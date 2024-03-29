#include "ui.h"
#include "frite.h"

static ui_system_t *gsys;

void ui_node_insert(ui_head_t *h, ui_node_t *n)
{
    ui_node_t *ptr;

    if(h->first == NULL) {
        h->first = n;
        return;
    }

    for(ptr=h->first; ptr->next; ptr=ptr->next);

    ptr->next = n;
}

void uisys_init(ui_system_t *uisys, input_t *in)
{
    uisys->in = in;
    uisys->h.first = NULL;
    uisys->psel = NULL;
    uisys->select = NULL;
    gsys = uisys;
}

ui_node_t* uisys_box_append(ui_system_t *uisys, v2 pos, v2 size)
{
    gl_color_t c = COLOR_INIT(255,255,127,255);
    ui_node_t *nptr;

    nptr = box_init(pos, size, c);
    ui_node_insert(&uisys->h, nptr);
    return nptr;
}

ui_node_t* uisys_append(ui_node_t *n)
{
    ui_node_insert(&gsys->h, n);
    return n;
}

void ui_draw(ui_node_t *node, v2 pos)
{
    ui_t *this = CONTAINER_OF(node, ui_t, n);
    ui_node_t *nptr;

    glBindTexture(GL_TEXTURE_2D, this->tid);
    square_draw(this->pos.x, this->pos.y, this->size.x, this->size.y, this->color);

    /*
      printf("X: %d Y: %d SX: %d SY: %d\n",
      this->pos.x, this->pos.y, this->size.x, this->size.y);
    */

    for(nptr=this->h.first; nptr; nptr=nptr->next)
        nptr->ops->draw(nptr, this->pos);
}

ui_node_t* ui_select(ui_node_t *node, v2 mpos)
{
    ui_t *this = CONTAINER_OF(node, ui_t, n);
    ui_node_t *nptr;
    v2 rpos;

    if(input_check_sel(mpos, this->pos, this->size))
    {
        rpos = V2_DEL(mpos, this->pos);

        for(nptr=this->h.first; nptr; nptr=nptr->next)
        {
            if(!nptr->ops->select)
                continue;

            if(nptr->ops->select(nptr, rpos)) {
                //printf("select: %p\n", nptr);
                return nptr;
            }
        }

        //printf("select: %p\n", node);
        return node;
    }

    return NULL;
}

static void uisys_handle_ldown(ui_system_t *uisys, v2 pos)
{
    bool append = true;
    ui_node_t *nptr;
    v2 size;

    uisys->select = NULL;

    for(nptr=uisys->h.first; nptr; nptr=nptr->next)
    {
        uisys->select = nptr->ops->select(nptr, pos);

        if(uisys->select) {
            append = false;
            break;
        }
    }

    if(uisys->psel && uisys->psel != uisys->select && uisys->psel->ops->unselect)
        uisys->psel->ops->unselect(uisys->psel);

    if(append) {
        size.x = 160;
        size.y = 120;
        //uisys->select = uisys_box_append(uisys, pos, size);
    }
}

void uisys_update(ui_system_t *uisys)
{
    input_t *in = uisys->in;
    ui_node_t *nptr;
    v2 pos = V2(0,0);

    //Handle input events
    if(in->ev & INEVENT_LDOWN) {
        uisys_handle_ldown(uisys, in->m.pos);
    }

    if(in->ev & INEVENT_LUP) {
        uisys->psel = uisys->select;
    }

    //Handle selected UI element.
    if(uisys->select != NULL && uisys->select->ops->update != NULL)
        uisys->select->ops->update(uisys->select, in, uisys);

    for(nptr=uisys->h.first; nptr; nptr=nptr->next)
        nptr->ops->draw(nptr, pos);
}

void ui_free(ui_node_t *node)
{
    ui_t *this = CONTAINER_OF(node, ui_t, n);
    ui_node_t *nptr;

    for(nptr=this->h.first; nptr; nptr=nptr->next)
        nptr->ops->free(nptr);
}
