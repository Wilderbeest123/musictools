#include "button.h"

void button_draw(button_t *b)
{
    gl_color_t c = COLOR_INIT(255,255,255,255);

    glBindTexture(GL_TEXTURE_2D, b->tid);
    circle_draw(b->pos.x, b->pos.y, b->size.x, b->size.y, c);
}

