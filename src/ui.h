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

void ui_node_insert(ui_head_t *h, ui_node_t *n);

#endif  /* UI_H_ */
