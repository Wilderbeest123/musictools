#ifndef SCREEN_H_
#define SCREEN_H_

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <wayland-client.h>
#include <wayland-egl.h>


typedef struct
{
  int width;
  int height;

  struct wl_display *d;
  struct wl_surface *s;
  struct wl_registry *r;
  struct wl_registry_listener l;
  struct wl_compositor *c;
  struct wl_shell *sh;

  struct wl_egl_window *egl_w;  /* Interface between wayland and EGL */

  EGLDisplay egl_d;
  EGLSurface egl_s;

  struct {
    GLuint p;
    GLuint pos;
    GLuint color;
  } gl;
} screen_t;

void screen_init(screen_t *s);
void screen_swap_buffer(screen_t *s);

#endif  /* SCREEN_H_ */
