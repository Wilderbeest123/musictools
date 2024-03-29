#include "toolbar.h"
#include "button.h"

static ui_ops_t toolbar_ops = { .select=ui_select,
                                .unselect=NULL,
                                .draw=ui_draw,
                                .update=NULL,
                                .free=NULL };

void toolbar_init(ui_system_t *uisys)
{
    ui_t *this = (ui_t*)malloc(sizeof(ui_t));
    ui_node_t *nptr;

    this->pos = V2(0,0);
    this->size = V2(180, 40);
    this->tid = gl_load_image("res/white.png");
    this->color = COLOR_INIT(255,0,0,255);

    this->n.ops = &toolbar_ops;
    this->n.next = NULL;
    this->h.first = NULL;

    for(int i=0; i<5; i++)
    {
        nptr = button_init(V2(36*i,4), V2(32,32), gl_load_image("res/yoda.png"), NULL, this);
        ui_node_insert(&this->h, nptr);
    }

    ui_node_insert(&uisys->h, &this->n);
}
