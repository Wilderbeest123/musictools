#include "screen.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

static GLuint compile_shader(GLenum type, const char* shader_src)
{
    GLuint shader;
    GLint compiled;

    shader = glCreateShader(type);

    glShaderSource(shader, 1, &shader_src, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if(compiled == 0)
        printf("Failed to compile shader: %s\n", glGetError());

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

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

static void screen_init_sdl(screen_t *s)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        printf("Failed to init SDL: %s\n", SDL_GetError());

    //Initialising SDL TTF
    if(TTF_Init() < 0)
        printf("Failed to init SDL TTF: %s\n", SDL_GetError());

    //Initialising SDL_Image
    if(IMG_Init(IMG_INIT_PNG) < 0)
        printf("Failed to init SDL Image: %s\n", SDL_GetError());

    //Configuring Window Parameters
    s->sdl.window = SDL_CreateWindow("musictools",
                                     SDL_WINDOWPOS_CENTERED, //Xscreen position
                                     SDL_WINDOWPOS_CENTERED, //Yaxis position
                                     s->width,    //Window size
                                     s->height,
                                     SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);      //Creation flags

    if(s->sdl.window == NULL)
        printf("Failed to init window: %s\n", SDL_GetError());

    assert(s->sdl.window);


    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

    //Use OpenGL 3.0
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0);

    s->sdl.context = SDL_GL_CreateContext(s->sdl.window);

    if(s->sdl.context == NULL)
        printf("Failed to init OpenGL Context: %s\n", SDL_GetError());

    if(SDL_GL_SetSwapInterval(1) < 0)
        printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

    glewExperimental = GL_TRUE;
    glewInit();
}

void screen_init(screen_t *s)
{
    s->width = 320;
    s->height = 240;

    screen_init_sdl(s);
    screen_init_opengl(s);
}

void screen_swap_buffer(screen_t *s)
{
    SDL_GL_SwapWindow(s->sdl.window);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
