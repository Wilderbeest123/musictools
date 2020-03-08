#ifndef SHAPES_H_
#define SHAPES_H_

#include "screen.h"

typedef struct
{
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
} color_t;

#define COLOR_INIT(R,G,B,A) \
    {.r=(GLfloat)R/255.0, .g=(GLfloat)G/255.0, .b=(GLfloat)B/255.0, .a=(GLfloat)A/255.0}

typedef struct
{
    GLuint vao;
    GLuint vpos;                 /* ID of vertex pos buffer */
    GLuint vcolor;               /* ID of vertex color buffer */
    GLuint ebo;                 /* ID of element buffer */

    int v_len;                   /* No. of vertices in VBO */
    int e_len;                   /* No. of elements in EBO */

} shape_t;

#define SHAPE_INIT() { .vao=0, .vpos=0, .vcolor=0, .ebo=0, .e_len=0, .v_len=0 }

shape_t shape_init_square(color_t c);
shape_t shape_init_triangle(color_t c);
void shape_draw(shape_t *s, int x, int y, int width, int height);

#endif  /* SHAPES_H_ */
