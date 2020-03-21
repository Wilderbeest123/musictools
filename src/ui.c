#include "ui.h"

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
