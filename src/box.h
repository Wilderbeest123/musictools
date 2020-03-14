#ifndef BOX_H_
#define BOX_H_

#include "screen.h"
#include "shapes.h"

typedef struct
{
  v2 pos;
  v2 size;
  gl_color_t col;
} box_t;

#define MAX_BOX 32

typedef struct
{
    box_t b[MAX_BOX];
    int num;                    /* Number of boxes currently in use */

} box_system_t;

void boxsys_init(box_system_t *b);
void boxsys_append(box_system_t *b, box_t box);
void boxsys_draw(box_system_t *b);

#endif  /* BOX_H_ */
