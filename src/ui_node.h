#ifndef UI_NODE_H_
#define UI_NODE_H_

#include "input.h"

typedef struct ui_node_t
{
  struct ui_ops_t *ops;
  struct ui_node_t *next;
} ui_node_t;

typedef struct
{
  struct ui_node_t *first;
} ui_head_t;

typedef struct ui_ops_t
{
  ui_node_t* (*select)(ui_node_t *, v2); /* Checks if UI node has been selected */
  void (*unselect)(ui_node_t *);          /* Called to handle when unselected */
  void (*update)(ui_node_t *, input_t *, void *);
  void (*draw)(ui_node_t *, v2);
  void (*free)(ui_node_t *);
} ui_ops_t;

#endif  /* UI_NODE_H_ */
