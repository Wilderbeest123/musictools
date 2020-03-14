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
#define VERT_UNIFORM_MODEL 2
#define VERT_UNIFORM_COLOR 3
#define VERT_UNIFORM_COLOR_EN 4

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef struct {
  GLuint p;                    /* Reference to the OpenGL program */
  GLuint pos;                 /* Reference to pos attribute in vertex shader */
  GLuint color;               /* Reference to color attribute in vertex shader */
} gl_program_t;

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
