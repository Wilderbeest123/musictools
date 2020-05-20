#include "textbox.h"

static ui_node_t* textbox_select(ui_node_t *node, v2 mpos);
static void textbox_unselect(ui_node_t *node);
static void textbox_draw(ui_node_t *node, v2 pos);
static void textbox_update(ui_node_t *node, input_t *in, void *data);
static void textbox_free(ui_node_t *node);

static ui_ops_t textbox_ops = { .select=textbox_select,
                                .unselect=textbox_unselect,
                                .draw=textbox_draw,
                                .update=textbox_update,
                                .free=textbox_free };

static ui_node_t* textbox_select(ui_node_t *node, v2 mpos)
{
    textbox_t *this = CONTAINER_OF(node, textbox_t, n);

    if(input_check_sel(mpos, this->pos, this->size)) {
        this->sel = true;
        this->l_show = true;
        return node;
    }

    return NULL;
}


static void textbox_unselect(ui_node_t *node)
{
    textbox_t *this = CONTAINER_OF(node, textbox_t, n);
    this->sel = false;
    this->l_show = false;
}

static void textbox_draw(ui_node_t *node, v2 pos)
{
    textbox_t *this = CONTAINER_OF(node, textbox_t, n);
    gl_color_t white = COLOR_INIT(255,255,255,255);
    gl_color_t black = COLOR_INIT(0,0,0,255);
    gl_color_t border = black;
    int adv;
    v2 p,s;

    if(this->sel) {
        border.g = 10;
        border.b = 250;
    }

    p.x = this->pos.x+pos.x;
    p.y = this->pos.y+pos.y;
    s.x = this->size.x;
    s.y = this->size.y;

    glBindTexture(GL_TEXTURE_2D, this->tid);
    square_draw(p.x, p.y, s.x, s.y, border);

    square_draw(p.x+this->border, p.y+this->border,
                s.x-(2*this->border), s.y-(2*this->border), white);

    adv = text_draw(&this->cset, p.x+this->border, p.y-(2*this->border),
                    this->text, black);

    if(timer_check(&this->timer) && this->sel)
        this->l_show = !this->l_show;

    if(this->l_show) {
        glBindTexture(GL_TEXTURE_2D, this->tid);
        square_draw(p.x+adv+2, p.y+this->border+2, 1, s.y-(2*this->border)-4, black);
    }
}

static void textbox_update(ui_node_t *node, input_t *in, void *data)
{
    textbox_t *this = CONTAINER_OF(node, textbox_t, n);

    if((in->ev & INEVENT_KDOWN) != INEVENT_KDOWN)
        return;

    //Reset the timer
    timer_reset(&this->timer);
    this->l_show = true;

    //Backspace and Delete chars
    if(in->key == 0x7f || in->key == 0x08) {
        if(this->text[this->idx] == 0 && this->idx > 0)
            this->idx--;

        this->text[this->idx] = '\0';
        return;
    }

    //Enter char
    if(in->key == 0x0d) {
        printf("Enter: %s\n", this->text);
        return;
    }

    //Filter any non supported chars being inserted.
    if(in->key < this->cset.chars[0].c || in->key > this->cset.chars[this->cset.num-1].c)
        return;

    this->text[this->idx++] = in->key;
}

static void textbox_free(ui_node_t *node)
{
    textbox_t *this = CONTAINER_OF(node, textbox_t, n);
}

ui_node_t* textbox_init(v2 pos, v2 size, gl_charset_t cset, char *str)
{
    textbox_t *this = (textbox_t*)malloc(sizeof(textbox_t));

    this->pos = pos;
    this->size = size;
    this->tid = gl_load_image("res/white.png");
    this->cset = cset;
    this->border = 1;
    timer_init(&this->timer, 1000);

    memset(this->text, 0, 64);
    memcpy(this->text, str, strlen(str));
    this->idx = strlen(str);

    this->n.ops = &textbox_ops;
    this->n.next = NULL;
    return &this->n;
}

void textbox_add(v2 pos, v2 size, char *str)
{
    ui_node_t *nptr;
    gl_charset_t *cset = calloc(1, sizeof(gl_charset_t));

    *cset = gl_load_charset("res/OpenSans-Regular.ttf", 24, 30, 128);
    nptr = textbox_init(pos, size, *cset, str);
    uisys_append(nptr);
}

