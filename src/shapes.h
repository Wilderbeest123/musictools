#ifndef SHAPES_H_
#define SHAPES_H_

#include <stdio.h>
#include <stdlib.h>
#include <float.h>

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

/* TO BE IMPLEMENTED */
typedef struct
{
    char *name;                  /* Name of the texture asset */
    uint32_t tid;                /* ID which represents a stored OpenGL texture */
} gl_texture_t;

typedef struct
{
    char c;
    uint32_t tid;
    v2 size;
    v2 offset;
    uint16_t advance;
} gl_char_t;

typedef struct
{
    char *filename;
    gl_char_t *chars;
    uint8_t num;                /* Number of chars loaded */
} gl_charset_t;

typedef struct
{
    gl_program_t *gl;
    gl_model_t square;
    gl_model_t triangle;
    gl_model_t circle;
} shapes_t;

void shapes_init(shapes_t *s);

void img_draw(int x, int y, int width, int height);
void square_draw(int x, int y, int width, int height, gl_color_t c);
void tri_draw(int x, int y, int width, int height, gl_color_t c);
void circle_draw(int x, int y, int width, int height, gl_color_t c);
void text_draw(gl_charset_t *cset, int x, int y, char *text);

uint32_t gl_load_font(char *filename, int fontsize);
uint32_t gl_load_image(char *filename);
gl_charset_t gl_load_charset(char *filename, uint8_t start, uint8_t finish);

#endif  /* SHAPES_H_ */
