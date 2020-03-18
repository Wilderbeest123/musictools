#ifndef SHAPES_H_
#define SHAPES_H_

#include "screen.h"

typedef struct
{
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
} gl_color_t;

#define COLOR_INIT(R,G,B,A) \
    {.r=(GLfloat)R/255.0, .g=(GLfloat)G/255.0, .b=(GLfloat)B/255.0, .a=(GLfloat)A/255.0}

typedef struct
{
    GLuint vao;
    GLuint vpos;                 /* ID of vertex pos buffer */
    GLuint vcolor;               /* ID of vertex color buffer */
    GLuint tcoord;               /* ID of texture coordinate buffer */
    GLuint ebo;                  /* ID of element buffer */

    GLenum v_type;               /* Primitive type used to render vertex data */

    int v_len;                   /* No. of vertices in VBO */
    int e_len;                   /* No. of elements in EBO */

} gl_model_t;

#define MODEL_INIT() { .vao=0, .vpos=0, .vcolor=0, .ebo=0, .e_len=0, .v_len=0 }

typedef struct
{
    gl_program_t *gl;
    gl_model_t square;
    gl_model_t triangle;
    gl_model_t circle;
} shapes_t;

void shapes_init(shapes_t *s);

void square_draw(int x, int y, int width, int height, gl_color_t c);
void tri_draw(int x, int y, int width, int height, gl_color_t c);
void circle_draw(int x, int y, int width, int height, gl_color_t c);

#endif  /* SHAPES_H_ */
