#ifndef UI_H_
#define UI_H_

#include "box.h"
#include "screen.h"

typedef struct
{
    struct ui_node_t *first;
} ui_head_t;

typedef struct
{
    box_t b;
    ui_head_t h;
    uint8_t state;
} ui_t;

typedef struct ui_node_t
{
    struct ui_ops_t *ops;
    struct ui_node_t *next;
} ui_node_t;

typedef struct ui_ops_t
{
    bool (*select)(ui_node_t *, v2);
    void (*draw)(ui_node_t *, v2);
    void (*free)(ui_node_t *);
} ui_ops_t;

void ui_node_insert(ui_head_t *h, ui_node_t *n);

#endif  /* UI_H_ */
