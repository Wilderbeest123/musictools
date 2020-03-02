#include "screen.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

static void registry_handler(void *data, struct wl_registry *registry,
                             uint32_t id, const char *interface, uint32_t version)
{
    printf("Got a registry event for %s id %d\n", interface, id);

    if (strcmp(interface, "wl_compositor") == 0) {
        ((screen_t *)data)->c = wl_registry_bind(registry, id, &wl_compositor_interface,1);
    }

    if (strcmp(interface, "wl_shell") == 0) {
        ((screen_t *)data)->sh = wl_registry_bind(registry, id, &wl_shell_interface, 1);
    }
}

static void registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    printf("Got a registry losing event for %d\n", id);
}

static void screen_init_wl(screen_t *s)
{
    s->l.global = registry_handler;
    s->l.global_remove = registry_remover;
    s->d = wl_display_connect(NULL);

    if(s->d == NULL)
        printf("ERROR: Failed to connect to wayland display\n");

    s->r = wl_display_get_registry(s->d);

    s->c = NULL;
    wl_registry_add_listener(s->r, &s->l, s);
    wl_display_dispatch(s->d);
    wl_display_roundtrip(s->d);

    s->s = wl_compositor_create_surface(s->c);
    assert(s->s != NULL);

    struct wl_shell_surface* shellSurface = wl_shell_get_shell_surface(s->sh, s->s);
    wl_shell_surface_set_toplevel(shellSurface);
}

static void screen_init_egl(screen_t *s)
{
    s->egl_w = wl_egl_window_create(s->s, s->width, s->height);
    assert(s->egl_w != NULL);

    s->egl_d = eglGetDisplay((EGLNativeDisplayType)s->d);

    EGLBoolean initialized = eglInitialize(s->egl_d, NULL, NULL);
    assert(initialized == EGL_TRUE);

    EGLint number_of_config;
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLConfig configs[1];
    eglChooseConfig(s->egl_d, config_attribs, configs, 1, &number_of_config);

    EGLContext eglContext = eglCreateContext(s->egl_d, configs[0], EGL_NO_CONTEXT, context_attribs);

    s->egl_s = eglCreateWindowSurface(s->egl_d, configs[0], (EGLNativeWindowType)s->egl_w, NULL);
    assert(s->egl_s != EGL_NO_SURFACE);

    EGLBoolean makeCurrent = eglMakeCurrent(s->egl_d, s->egl_s, s->egl_s, eglContext);
    assert(makeCurrent == EGL_TRUE);

}

static GLuint compile_shader(GLenum type, const char* shader_src)
{
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);

    glShaderSource(shader, 1, &shader_src, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    assert(compiled);

    return shader;
}

static GLuint load_shader(GLenum type, const char* filename)
{
    char *buffer;
    FILE *fp;
    long fsize;
    size_t read_result;
    GLuint shader;

    fp= fopen(filename, "r");
    assert(fp);

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    rewind(fp);

    buffer = (char*)malloc(sizeof(char)*fsize);
    assert(buffer);

    read_result = fread(buffer, 1, fsize, fp);
    assert(read_result == fsize);
    fclose(fp);

    shader = compile_shader(type, buffer);
    free(buffer);
    return shader;
}

static void screen_init_opengl(screen_t *s)
{
    GLuint vertexShader = load_shader(GL_VERTEX_SHADER, "./src/shader.vert");
    GLuint fragmentShader = load_shader(GL_FRAGMENT_SHADER, "./src/shader.frag");

    s->gl.p = glCreateProgram();
    glAttachShader(s->gl.p, vertexShader);
    glAttachShader(s->gl.p, fragmentShader);
    glLinkProgram(s->gl.p);

    GLint linked;
    glGetProgramiv(s->gl.p, GL_LINK_STATUS, &linked);
    assert(linked);

    glUseProgram(s->gl.p);
    glViewport(0, 0, s->width, s->height);
}

void screen_init(screen_t *s)
{
    s->width = 320;
    s->height = 240;

    screen_init_wl(s);
    screen_init_egl(s);
    screen_init_opengl(s);
}

void screen_swap_buffer(screen_t *s)
{
    eglSwapBuffers(s->egl_d, s->egl_s);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
