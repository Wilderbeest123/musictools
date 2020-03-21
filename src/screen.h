#ifndef SCREEN_H_
#define SCREEN_H_

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <stdbool.h>

//VERTEX SHADER LOCATIONS
#define VERT_ATTR_POS 0
#define VERT_ATTR_COLOR 1
#define VERT_ATTR_TCOORD 2
#define VERT_UNIFORM_MODEL 3
#define VERT_UNIFORM_COLOR 4
#define VERT_UNIFORM_COLOR_EN 5

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef struct {
  GLuint p;                    /* Reference to the OpenGL program */
  GLuint pos;                 /* Reference to pos attribute in vertex shader */
  GLuint color;               /* Reference to color attribute in vertex shader */
} gl_program_t;

typedef struct {
    int x;
    int y;
} v2;

static inline v2 V2(int x, int y)
{
    v2 v = { .x=x, .y=y };
    return v;
}

static inline v2 V2_ADD(v2 a, v2 b)
{
    v2 v = { .x=a.x+b.x, .y=a.y+b.y };
    return v;
}

typedef struct
{
  int width;
  int height;
  bool close;

  struct {
    SDL_Window *window;
    SDL_GLContext context;
  } sdl;

  gl_program_t gl;

} screen_t;

void screen_init(screen_t *s, int width, int height);
void screen_swap_buffer(screen_t *s);

gl_program_t* gl_program(void);

GLenum check_gl(const char *file, int line);
#define gle() check_gl(__FILE__, __LINE__)

#endif  /* SCREEN_H_ */
