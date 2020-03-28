#ifndef UI_H_
#define UI_H_

#include "screen.h"
#include "box.h"
#include "ui_node.h"

typedef struct
{
    box_t b;
    ui_head_t h;
    uint8_t state;
} ui_t;

typedef struct
{
  input_t *in;
  ui_head_t h;                /* Head of linked box nodes */
  ui_node_t *select;          /* Reference to currently selected node. */

} ui_system_t;

void ui_node_insert(ui_head_t *h, ui_node_t *n);

void uisys_init(ui_system_t *uisys, input_t *in);
void uisys_update(ui_system_t *uisys);

ui_node_t* uisys_append(ui_system_t *uisys, v2 pos, v2 size);

#endif  /* UI_H_ */
