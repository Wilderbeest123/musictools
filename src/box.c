#include "box.h"

void boxsys_init(box_system_t *b)
{
    b->num = 0;
}

static inline void box_draw(box_t b)
{
    square_draw(b.pos.x, b.pos.y, b.size.x, b.size.y, b.col);
}


void boxsys_append(box_system_t *b, box_t box)
{
    if(b->num == MAX_BOX) {
        printf("ERROR: Already reached max box\n");
        return;
    }

    b->b[b->num] = box;
    b->num++;
}

void boxsys_draw(box_system_t *b)
{
    for(int i=0; i<b->num; i++)
        box_draw(b->b[i]);
}

