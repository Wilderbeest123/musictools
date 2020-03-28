#include "ui.h"
#include "frite.h"

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
}

ui_node_t* uisys_append(ui_system_t *uisys, v2 pos, v2 size)
{
    gl_color_t c = COLOR_INIT(255,255,127,255);
    ui_node_t *nptr;

    nptr = box_init(pos, size, 0, c);
    ui_node_insert(&uisys->h, nptr);
    return nptr;
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

    if(append) {
        size.x = SCREEN_WIDTH/10;
        size.y = SCREEN_HEIGHT/10;
        uisys->select = uisys_append(uisys, pos, size);
    }
}

void uisys_update(ui_system_t *uisys)
{
    input_t *in = uisys->in;
    ui_node_t *nptr;
    v2 pos = V2(0,0);

    //Handle input events
    if(in->ev & INEVENT_LDOWN)
        uisys_handle_ldown(uisys, in->m.pos);

    if(in->ev & INEVENT_LUP)
        uisys->select = NULL;

    //Handle selected UI element.
    if(uisys->select != NULL && uisys->select->ops->update != NULL)
        uisys->select->ops->update(uisys->select, in, uisys);

    for(nptr=uisys->h.first; nptr; nptr=nptr->next)
        nptr->ops->draw(nptr, pos);
}
