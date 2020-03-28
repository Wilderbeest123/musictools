#ifndef BOX_H_
#define BOX_H_

#include "screen.h"
#include "shapes.h"
#include "input.h"
#include "ui_node.h"

typedef enum
{
    BOX_COL_NONE=0x00,
    BOX_COL_T=0x01,
    BOX_COL_B=0x02,
    BOX_COL_L=0x04,
    BOX_COL_R=0x08,
} box_col_t;

typedef struct
{
  int t;                      /* Top boundary */
  int b;                      /* Bottom boundary */
  int l;                      /* Left boundary */
  int r;                      /* Right boundary */
} bound_t;

typedef struct
{
    v2 pos;
    v2 size;
    unsigned int tid;           /* ID of the texture to render */
    gl_color_t col;
    ui_node_t n;
} box_t;

bound_t box_bound_get(v2 pos, v2 size);

ui_node_t* box_init(v2 pos, v2 size, unsigned int tid, gl_color_t c);

#endif  /* BOX_H_ */
