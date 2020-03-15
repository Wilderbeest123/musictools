#ifndef BOX_H_
#define BOX_H_

#include "screen.h"
#include "shapes.h"
#include "input.h"

typedef enum
{
    BOX_COL_NONE=0x00,
    BOX_COL_X=0x01,
    BOX_COL_Y=0x02,
} box_col_t;

typedef struct
{
    v2 pos;
    v2 size;
    gl_color_t col;
    bool sel;                   /* true if box has been selected */
} box_t;

#define MAX_BOX 32

typedef struct
{
    box_t b[MAX_BOX];
    int num;                    /* Number of boxes currently in use */
    input_t *in;

} box_system_t;

void boxsys_init(box_system_t *b, input_t *in);
void boxsys_update(box_system_t *b);

#endif  /* BOX_H_ */
