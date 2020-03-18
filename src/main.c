#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "jtime.h"
#include "frite.h"
#include "sound.h"
#include "asoundlib.h"
#include "keys.h"
#include "screen.h"
#include "shapes.h"
#include "input.h"
#include "box.h"

/* NOTE: This is a WIP */
static void open_font(void)
{
    TTF_Font *f;
    SDL_Surface *s;
    unsigned int t;

    SDL_Color c = {255, 127, 255};

    f = TTF_OpenFont("res/OpenSans-Regular.ttf", 16);
    assert(f);

    s = TTF_RenderText_Blended(f, "This is a test", c);
    assert(s);

    printf("Width: %d, Height: %d pixtype: %u\n", s->w, s->h, s->format->BitsPerPixel);

    glGenTextures(1, &t);

    glBindTexture(GL_TEXTURE_2D, t);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);

    SDL_FreeSurface(s);
    TTF_CloseFont(f);
}

static void load_image(void)
{
    SDL_Surface *s = IMG_Load("res/brick.png");
    assert(s);

    unsigned int t;

    //glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s->w, s->h, 0, GL_RGB, GL_UNSIGNED_BYTE, s->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    SDL_FreeSurface(s);
}


int main(void)
{
    screen_t s;
    input_t in;
    shapes_t sh;
    box_system_t b;

    screen_init(&s, SCREEN_WIDTH, SCREEN_HEIGHT);
    input_init(&in, &s);
    shapes_init(&sh);
    boxsys_init(&b, &in);

    load_image();
    gl_color_t c = COLOR_INIT(255,127,255,255);

    while(s.close == false)
    {
        input_update(&in);
        boxsys_update(&b);

        circle_draw(100, 100, 50, 50, c);
        screen_swap_buffer(&s);
    }

    return 0;
}

void music_loop(void)
{
    keyboard_t k;
    frite_t hw;
    jtime_t timer;
    uint16_t *out_buffer;

    keys_init(&k);
    frite_open(&hw, &k.ev);
    print_pback_settings();

    timer_init(&timer, 20);
    out_buffer = calloc(hw.pback_out.period_size, sizeof(uint16_t));

    while(1)
    {
        if(timer_check(&timer)){
            //print_time();
            frite_read(&hw);
            keys_print_notes(&k);

            //Only write if space availabe in audio buffer.
            if(snd_pcm_avail(hw.audio_out) > hw.pback_out.period_size)
            {
                //Zeroize buffer prior to populating with data.
                memset(out_buffer, 0, hw.pback_out.period_size*sizeof(uint16_t));
                keys_populate_buffer(&k, out_buffer, hw.pback_out.period_size);
                snd_pcm_writei(hw.audio_out, out_buffer, hw.pback_out.period_size);
            }
        }
    }

    snd_rawmidi_drain(hw.midi_in);
    snd_rawmidi_close(hw.midi_in);
}
