#ifndef SHAPES_H_
#define SHAPES_H_

#include "screen.h"

typedef struct
{
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    int e_len;                   /* No. of elements in EBO */
    int v_len;                   /* No. of vertices in VBO */

} shape_t;

#define SHAPE_INIT() { .vao=0, .vbo=0, .ebo=0, .e_len=0 }

shape_t shape_init_square(void);
shape_t shape_init_triangle(void);
void shape_draw(shape_t *s);

#endif  /* SHAPES_H_ */
