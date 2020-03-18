#include "shapes.h"
#include <cglm/cglm.h>
#include <math.h>

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

static gl_model_t model_init_square(void)
{
    gl_model_t s = MODEL_INIT();

    GLfloat vpos[] = { -1.0f, 1.0f, 0.0f,
                       1.0f, 1.0f, 0.0f,
                       1.0f, -1.0f, 0.0f,
                       -1.0f, -1.0f, 0.0f };

    GLuint elements[] = { 0, 1, 2, 2, 3, 0 };

    GLfloat tex[] = { 0.0f, 1.0f,
                      1.0f, 1.0f,
                      1.0f, 0.0f,
                      0.0f, 0.0f };

    s.v_len = 4;
    s.v_type = GL_TRIANGLES;
    s.e_len = sizeof(elements);

    //Init VAO
    glGenVertexArrays(1, &s.vao);
    glBindVertexArray(s.vao);

    //Init VBOs
    model_init_vpos(&s.vpos, vpos, sizeof(vpos));

    //Init tcoord buffer
    glGenBuffers(1, &s.tcoord);
    glBindBuffer(GL_ARRAY_BUFFER, s.tcoord);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex)*sizeof(GL_FLOAT), tex, GL_STATIC_DRAW);

    //Map tcoord buffer to binded VAO
    glEnableVertexAttribArray(VERT_ATTR_TCOORD);
    glVertexAttribPointer(VERT_ATTR_TCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    //Init EBO
    glGenBuffers(1, &s.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glBindVertexArray(0); //Unbind VAO
    return s;
}

static gl_model_t model_init_triangle(void)
{
    gl_model_t s = MODEL_INIT();

    GLfloat vpos[] = { 0.0f, 0.5f, 0.0f,
                       -1.0f, -1.0f, 0.0f,
                       1.0f, -1.0f, 0.0f };
    s.v_len = 3;
    s.v_type = GL_TRIANGLES;

    //Init VAO
    glGenVertexArrays(1, &s.vao);
    glBindVertexArray(s.vao);

    //Init VBO
    model_init_vpos(&s.vpos, vpos, sizeof(vpos));

    glBindVertexArray(0); //Unbind VAO
    return s;
}

static gl_model_t model_init_circle(void)
{
    gl_model_t c = MODEL_INIT();
    int num_steps;
    float step;
    GLfloat *vpos;

    num_steps = 45;
    step = (2.0*M_PI)/num_steps;

    c.v_len = num_steps+2;
    c.v_type = GL_TRIANGLE_FAN;
    vpos = (GLfloat *)malloc(sizeof(GLfloat)*3*(c.v_len));

    vpos[0] = 0.0f;
    vpos[1] = 0.0f;
    vpos[2] = 0.0f;

    for(int i=1; i<num_steps+2; i++)
    {
        (vpos+3*i)[0] = cos(i*step);
        (vpos+3*i)[1] = sin(i*step);
        (vpos+3*i)[2] = 0.0f;
    }

    //Init VAO
    glGenVertexArrays(1, &c.vao);
    glBindVertexArray(c.vao);

    //Init VBO
    model_init_vpos(&c.vpos, vpos, sizeof(GLfloat)*3*(c.v_len));

    glBindVertexArray(0); //Unbind VAO

    free(vpos);
    return c;
}


static void model_uniform_pos(int x, int y, int width, int height)
{
    vec3 pos = { x, y, 0 };
    vec3 scale = { width, height, 0 };
    mat4 ortho;

    glm_ortho(0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 0.0, 1.0, ortho);
    glm_translate(ortho, pos);
    glm_scale(ortho, scale);

    glProgramUniformMatrix4fv(gShapes->gl->p, VERT_UNIFORM_MODEL, 1, GL_FALSE, (GLfloat*)ortho);
}

static inline void model_uniform_color(gl_color_t c)
{
    glProgramUniform1i(gShapes->gl->p, VERT_UNIFORM_COLOR_EN, true);
    glProgramUniform4fv(gShapes->gl->p, VERT_UNIFORM_COLOR, 1, (GLfloat*)&c);
}

static inline void model_draw(gl_model_t s, int x, int y, int width, int height)
{
    glBindVertexArray(s.vao);

    model_uniform_pos(x, y, width, height);

    if(s.ebo) {
        glDrawElements(s.v_type, s.e_len, GL_UNSIGNED_INT, 0);
        return;
    }

    glDrawArrays(s.v_type, 0, s.v_len);
}

void shapes_init(shapes_t *s)
{
    s->gl = gl_program();
    s->square = model_init_square();
    s->triangle = model_init_triangle();
    s->circle = model_init_circle();

    /*
    printf("%u, %u, %u, %u\n", s->square.vao, s->square.vpos, s->square.vcolor, s->square.ebo);
    printf("%u, %u, %u, %u\n", s->triangle.vao, s->triangle.vpos, s->triangle.vcolor, s->triangle.ebo);
    printf("%u, %u, %u, %u\n", s->circle.vao, s->circle.vpos, s->circle.vcolor, s->circle.ebo);
    */

    gShapes = s;
}

void square_draw(int x, int y, int width, int height, gl_color_t c)
{
    model_uniform_color(c);
    model_draw(gShapes->square, x, y, width, height);
}

void tri_draw(int x, int y, int width, int height, gl_color_t c)
{
    model_uniform_color(c);
    model_draw(gShapes->triangle, x, y, width, height);
}

void circle_draw(int x, int y, int width, int height, gl_color_t c)
{
    model_uniform_color(c);
    model_draw(gShapes->circle, x, y, width, height);
}
