#ifndef SCREEN_H_
#define SCREEN_H_

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

typedef struct
{
  int width;
  int height;

  struct {
    SDL_Window *window;
    SDL_GLContext context;
  } sdl;

  struct {
    GLuint p;
    GLuint pos;
    GLuint color;
  } gl;
} screen_t;

void screen_init(screen_t *s);
void screen_swap_buffer(screen_t *s);

#endif  /* SCREEN_H_ */
