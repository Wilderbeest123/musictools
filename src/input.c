#include "input.h"


void input_init(input_t *i, screen_t *s)
{
    i->s = s;
}

void input_handle(input_t *i)
{
    SDL_Event ev;

    while(SDL_PollEvent(&ev))
    {
        if (ev.type == SDL_WINDOWEVENT)
        {
            switch (ev.window.event)
            {
            case SDL_WINDOWEVENT_CLOSE:
                i->s->close = true;
                break;
            }
        }
    }

}
