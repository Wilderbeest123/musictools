#ifndef UI_H_
#define UI_H_

#include "screen.h"
#include "ui_node.h"
#include "shapes.h"
#include "input.h"

typedef struct
{
    v2 pos;
    v2 size;
    unsigned int tid;
    gl_color_t color;

    ui_node_t n;
    ui_head_t h;

    /* general purpose bitmask for keeping
     * track of current state. */
    uint8_t state;

} ui_t;

typedef struct
{
  input_t *in;
  ui_head_t h;                /* Head of linked box nodes */
  ui_node_t *select;          /* Reference to currently selected node. */
  ui_node_t *psel;            /* Reference to the previous selected node. */

} ui_system_t;

void ui_node_insert(ui_head_t *h, ui_node_t *n);
void ui_draw(ui_node_t *node, v2 pos);
ui_node_t* ui_select(ui_node_t *node, v2 mpos);
void ui_free(ui_node_t *node);

void uisys_init(ui_system_t *uisys, input_t *in);
void uisys_update(ui_system_t *uisys);

ui_node_t* uisys_box_append(ui_system_t *uisys, v2 pos, v2 size);
ui_node_t* uisys_append(ui_node_t *n);

#endif  /* UI_H_ */
