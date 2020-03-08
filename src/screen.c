#include "screen.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

static gl_program_t* gGL;

gl_program_t* gl_program(void)
{
    return gGL;
}

GLenum check_gl(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  printf("INVALID_ENUM "); break;
        case GL_INVALID_VALUE:                 printf("INVALID_VALUE "); break;
        case GL_INVALID_OPERATION:             printf("INVALID_OPERATION "); break;
        case GL_STACK_OVERFLOW:                printf("STACK_OVERFLOW "); break;
        case GL_STACK_UNDERFLOW:               printf("STACK_UNDERFLOW "); break;
        case GL_OUT_OF_MEMORY:                 printf("OUT_OF_MEMORY "); break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: printf("INVALID_FRAMEBUFFER_OPERATION "); break;
        }

        printf("| %s (%i)\n", file, line);
    }
    return errorCode;
}

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

    globalGL = (gl_program_t*)&s->gl.p;

    glUseProgram(s->gl.p);
    glViewport(0, 0, s->width, s->height);

    s->gl.pos = VERT_ATTR_POS;
    s->gl.color = VERT_ATTR_COLOR;

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

static void opengl_dbg_callback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity,
                                int length, const char *msg, const void *user)
{
    printf("gldbg[");

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             printf("API, "); break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   printf("WINSYS, "); break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: printf("COMPILER, "); break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     printf("THRD PTY, "); break;
    case GL_DEBUG_SOURCE_APPLICATION:     printf("APP, "); break;
    case GL_DEBUG_SOURCE_OTHER:           printf("OTHER, "); break;
    };

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         printf("HIGH, "); break;
    case GL_DEBUG_SEVERITY_MEDIUM:       printf("MED, "); break;
    case GL_DEBUG_SEVERITY_LOW:          printf("LOW, "); break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: printf("NOTICE, "); break;
    };

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               printf("Error"); break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: printf("Deprecated Behaviour"); break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  printf("Undefined Behaviour"); break;
    case GL_DEBUG_TYPE_PORTABILITY:         printf("Portability"); break;
    case GL_DEBUG_TYPE_PERFORMANCE:         printf("Performance"); break;
    case GL_DEBUG_TYPE_MARKER:              printf("Marker"); break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          printf("Push Group"); break;
    case GL_DEBUG_TYPE_POP_GROUP:           printf("Pop Group"); break;
    case GL_DEBUG_TYPE_OTHER:               printf("Other"); break;
    };

    printf("]: %s\n", msg);
}

static void screen_init_sdl(screen_t *s)
{
    GLint flags;

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

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5);

    //Uncomment for troubleshooting
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    s->sdl.context = SDL_GL_CreateContext(s->sdl.window);

    if(s->sdl.context == NULL)
        printf("Failed to init OpenGL Context: %s\n", SDL_GetError());

    if(SDL_GL_SetSwapInterval(1) < 0)
        printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

    glewExperimental = GL_TRUE;
    glewInit();
    glGetError(); //Here to handle known GL_INVALID_ENUM bug with glewInit

    //Get GL Context flags
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        printf("NOTE: GL DEBUG ENABLED\n");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(opengl_dbg_callback, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    }

    assert(glewIsSupported("GL_VERSION_4_5"));
}

void screen_init(screen_t *s, int width, int height)
{
    s->width = width;
    s->height = height;

    screen_init_sdl(s);
    screen_init_opengl(s);
}

void screen_swap_buffer(screen_t *s)
{
    SDL_GL_SwapWindow(s->sdl.window);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
