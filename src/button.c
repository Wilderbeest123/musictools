#include "button.h"
#include "frite.h"

static ui_node_t* button_check_select(ui_node_t *node, v2 mpos);
static void button_do_draw(ui_node_t *node, v2 pos);
static void button_free(ui_node_t *node);

static ui_ops_t button_ops = { .select=button_check_select,
                               .draw=button_do_draw,
                               .free=button_free };

void button_draw(button_t *b)
{
    gl_color_t c = COLOR_INIT(255,255,255,255);

    glBindTexture(GL_TEXTURE_2D, b->tid);
    circle_draw(b->pos.x, b->pos.y, b->size.x, b->size.y, c);
}

void button_draw_rel(button_t *b, v2 pos)
{
    gl_color_t c = COLOR_INIT(255,255,255,255);

    glBindTexture(GL_TEXTURE_2D, b->tid);
    circle_draw(pos.x + b->pos.x, pos.y + b->pos.y, b->size.x, b->size.y, c);
}

static ui_node_t* button_check_select(ui_node_t *node, v2 mpos)
{
    button_t *this = CONTAINER_OF(node, button_t, n);

    if(input_check_sel(mpos, this->pos, this->size)) {
        this->fn(this->data, 0);
        return node;
    }

    return NULL;
}

static void button_do_draw(ui_node_t *node, v2 pos)
{
    button_t *this = CONTAINER_OF(node, button_t, n);
    gl_color_t c = COLOR_INIT(255,255,255,255);

    glBindTexture(GL_TEXTURE_2D, this->tid);

    circle_draw(this->pos.x+pos.x, this->pos.y+pos.y,
                this->size.x, this->size.y, c);
}

static void button_free(ui_node_t *node)
{
    button_t *this = CONTAINER_OF(node, button_t, n);
    free(this);
}

ui_node_t* button_init(v2 pos, v2 size, unsigned int tid, void (*fn)(void *, uint8_t), void *data)
{
    button_t *this = (button_t*)malloc(sizeof(button_t));
    this->pos = pos;
    this->size = size;
    this->tid = tid;
    this->fn = fn;
    this->data = data;

    this->n.ops = &button_ops;
    this->n.next = NULL;
    return &this->n;
}
