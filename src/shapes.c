#include "shapes.h"
#include <cglm/cglm.h>

static shapes_t *gShapes;

static void model_init_vcolor(GLuint *vbo, gl_color_t c, int len)
{
    int i;
    gl_color_t *vcolor;

    vcolor = malloc(sizeof(gl_color_t) * len);

    for(i=0; i<len; i++)
        vcolor[i] = c;

    //Init vertex color buffer
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_color_t)*len, vcolor, GL_STATIC_DRAW);

    //Map vcolor VBO to binded VAO
    glEnableVertexAttribArray(VERT_ATTR_COLOR);
    glVertexAttribPointer(VERT_ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    free(vcolor);
}

static void model_init_vpos(GLuint *vbo, GLfloat *vpos, int len)
{
    //Init vertex pos buffer
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, len*sizeof(GL_FLOAT), vpos, GL_STATIC_DRAW);

    //Map vpos VBO to binded VAO
    glEnableVertexAttribArray(VERT_ATTR_POS);
    glVertexAttribPointer(VERT_ATTR_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

static gl_model_t model_init_square(gl_color_t c)
{
    gl_model_t s = MODEL_INIT();

    GLfloat vpos[] = { -1.0f, 1.0f, 0.0f,
                       1.0f, 1.0f, 0.0f,
                       1.0f, -1.0f, 0.0f,
                       -1.0f, -1.0f, 0.0f };

    GLuint elements[] = { 0, 1, 2, 2, 3, 0 };

    s.v_len = 4;
    s.e_len = sizeof(elements);

    //Init VAO
    glGenVertexArrays(1, &s.vao);
    glBindVertexArray(s.vao);

    //Init VBOs
    model_init_vpos(&s.vpos, vpos, sizeof(vpos));
    model_init_vcolor(&s.vcolor, c, s.v_len);

    //Init EBO
    glGenBuffers(1, &s.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glBindVertexArray(0); //Unbind VAO
    return s;
}

static gl_model_t model_init_triangle(gl_color_t c)
{
    gl_model_t s = MODEL_INIT();

    GLfloat vpos[] = { 0.0f, 0.5f, 0.0f,
                       -1.0f, -1.0f, 0.0f,
                       1.0f, -1.0f, 0.0f };
    s.v_len = 3;

    //Init VAO
    glGenVertexArrays(1, &s.vao);
    glBindVertexArray(s.vao);

    //Init VBOs
    model_init_vpos(&s.vpos, vpos, sizeof(vpos));
    model_init_vcolor(&s.vcolor, c, s.v_len);

    glBindVertexArray(0); //Unbind VAO
    return s;
}

static void update_model_uniform(int x, int y, int width, int height)
{
    vec3 pos = { x, y, 0 };
    vec3 scale = { width, height, 0 };
    mat4 ortho;

    glm_ortho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 0.0, 1.0, ortho);
    glm_translate(ortho, pos);
    glm_scale(ortho, scale);

    glProgramUniformMatrix4fv(gShapes->gl->p, VERT_UNIFORM_MODEL, 1, GL_FALSE, (GLfloat*)ortho);
}

static inline void model_draw(gl_model_t s, int x, int y, int width, int height)
{
    glBindVertexArray(s.vao);

    update_model_uniform(x, y, width, height);

    if(s.ebo) {
        glDrawElements(GL_TRIANGLES, s.e_len, GL_UNSIGNED_INT, 0);
        return;
    }

    glDrawArrays(GL_TRIANGLES, 0, s.v_len);
}

void shapes_init(shapes_t *s, gl_color_t c1, gl_color_t c2)
{
    s->gl = gl_program();
    s->square = model_init_square(c1);
    s->triangle = model_init_triangle(c2);
    gShapes = s;
}

void square_draw(int x, int y, int width, int height)
{
    model_draw(gShapes->square, x, y, width, height);
}

void tri_draw(int x, int y, int width, int height)
{
    model_draw(gShapes->triangle, x, y, width, height);
}
