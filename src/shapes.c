#include "shapes.h"
#include <cglm/cglm.h>
#include <assert.h>
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

static void model_init_tcoord(GLuint *vbo, GLfloat *tcoord, int len)
{
    //Init texture coord buffer
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, len*sizeof(GL_FLOAT), tcoord, GL_STATIC_DRAW);

    //Map tcoord buffer to currently binded VAO
    glEnableVertexAttribArray(VERT_ATTR_TCOORD);
    glVertexAttribPointer(VERT_ATTR_TCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

static gl_model_t model_init_square(void)
{
    gl_model_t s = MODEL_INIT();

    GLfloat vpos[] = { -1.0f, 1.0f, 0.0f,
                       1.0f, 1.0f, 0.0f,
                       1.0f, -1.0f, 0.0f,
                       -1.0f, -1.0f, 0.0f };

    GLuint elements[] = { 0, 1, 2, 2, 3, 0 };

    GLfloat tcoord[] = { 0.0f, 1.0f,
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
    model_init_tcoord(&s.tcoord, tcoord, sizeof(tcoord));

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
    GLfloat *vpos, *tcoord;

    num_steps = 45;
    step = (2.0*M_PI)/num_steps;

    c.v_len = num_steps+2;
    c.v_type = GL_TRIANGLE_FAN;
    vpos = (GLfloat *)malloc(sizeof(GLfloat)*3*(c.v_len));
    tcoord = (GLfloat *)malloc(sizeof(GLfloat)*2*(c.v_len));

    vpos[0] = 0.0f;
    vpos[1] = 0.0f;
    vpos[2] = 0.0f;

    tcoord[0] = 0.5f;
    tcoord[1] = 0.5f;

    for(int i=1; i<num_steps+2; i++)
    {
        (vpos+3*i)[0] = cos(i*step);
        (vpos+3*i)[1] = sin(i*step);
        (vpos+3*i)[2] = 0.0f;

        (tcoord+2*i)[0] = 0.5*(cos(i*step)+1.0);
        (tcoord+2*i)[1] = 0.5*(sin(i*step)+1.0);
    }

    //Init VAO
    glGenVertexArrays(1, &c.vao);
    glBindVertexArray(c.vao);

    //Init VBOs
    model_init_vpos(&c.vpos, vpos, sizeof(GLfloat)*3*(c.v_len));
    model_init_tcoord(&c.tcoord, tcoord, sizeof(GLfloat)*2*(c.v_len));

    glBindVertexArray(0); //Unbind VAO

    free(vpos);
    free(tcoord);
    return c;
}


static void model_uniform_pos(int x, int y, int width, int height)
{
    vec3 pos = { x + width/2, y + height/2, 0 };
    vec3 scale = { (float)width/2, (float)height/2, 0 };
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

static inline void model_draw(gl_model_t s, int x, int y, int width, int height, bool use_tcoord)
{
    glBindVertexArray(s.vao);

    if(use_tcoord)
        glEnableVertexAttribArray(VERT_ATTR_TCOORD);
    else
        glDisableVertexAttribArray(VERT_ATTR_TCOORD);

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

static inline void __square_draw(int x, int y, int width, int height, gl_color_t c, bool use_tcoord)
{
    model_uniform_color(c);
    model_draw(gShapes->square, x, y, width, height, use_tcoord);
}

void square_draw(int x, int y, int width, int height, gl_color_t c)
{
    __square_draw(x, y, width, height, c, true);
}

void img_draw(int x, int y, int width, int height)
{
    gl_color_t c = COLOR_INIT(255,255,255,255);
    __square_draw(x, y, width, height, c, true);
}

void tri_draw(int x, int y, int width, int height, gl_color_t c)
{
    model_uniform_color(c);
    model_draw(gShapes->triangle, x, y, width, height, false);
}

void circle_draw(int x, int y, int width, int height, gl_color_t c)
{
    model_uniform_color(c);
    model_draw(gShapes->circle, x, y, width, height, true);
}

uint32_t gl_load_font(char *filename, int fontsize)
{
    unsigned int t;
    TTF_Font *f;
    SDL_Surface *s;

    SDL_Color c = {255, 255, 255, 255};

    f = TTF_OpenFont(filename, fontsize);
    assert(f);

    /* NOTE(jack): These two functions were required since the below
       TTF_RenderText_Blended always generates alpha values as 0x00
       and the pixels are in GL_RGBA format... */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    s = TTF_RenderText_Blended(f, "This is a test", c);
    assert(s);

    printf("Width: %d, Height: %d pixtype: %u\n", s->w, s->h, s->format->BitsPerPixel);

    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    SDL_FreeSurface(s);
    TTF_CloseFont(f);
    return t;
}

uint32_t gl_load_image(char *filename)
{
    uint32_t t;
    SDL_Surface *s = IMG_Load(filename);
    assert(s);

    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s->w, s->h, 0, GL_RGB, GL_UNSIGNED_BYTE, s->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    SDL_FreeSurface(s);
    return t;
}

static inline gl_char_t charset_get_char(gl_charset_t *cset, char c)
{
    for(int i=0; i<cset->num; i++)
        if(cset->chars[i].c == c)
            return cset->chars[i];

    printf("ERROR: Char %c not supported\n", c);
}

void text_draw(gl_charset_t *cset, int x, int y, char *text, gl_color_t color)
{
    gl_char_t c;
    v2 pos = V2(x,y);

    for(char *tptr=text; *tptr; tptr++)
    {
        c = charset_get_char(cset, *tptr);

        glBindTexture(GL_TEXTURE_2D, c.tid);
        square_draw(pos.x, pos.y, c.size.x, c.size.y, color);
        pos.x += c.advance;
    }
}

gl_charset_t gl_load_charset(char *filename, int size, uint8_t start, uint8_t finish)
{
    TTF_Font *f;
    SDL_Surface *s;
    SDL_Color color = {255, 255, 255, 255};
    gl_charset_t cset;
    gl_char_t *c_ptr;
    int minx, miny, maxx, maxy, advance;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    f = TTF_OpenFont(filename, size);
    assert(f);

    cset.filename = filename;
    cset.num = finish-start;
    cset.chars = malloc((cset.num)*sizeof(gl_char_t));
    c_ptr = cset.chars;

    for(uint8_t i=start; i<finish; i++)
    {
        if(TTF_GlyphMetrics(f, i, &minx, &maxx, &miny, &maxy, &advance) == -1)
            printf("%s\n", TTF_GetError());

        glGenTextures(1, &c_ptr->tid);
        glBindTexture(GL_TEXTURE_2D, c_ptr->tid);

        s = TTF_RenderGlyph_Blended(f, i, color);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        c_ptr->c = (char)i;
        c_ptr->size = V2(s->w, s->h);
        c_ptr->offset = V2(minx, miny);
        c_ptr->advance = advance;

        free(s);
        c_ptr++;
    }

    free(f);
    return cset;
}
