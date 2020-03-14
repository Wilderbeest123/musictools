#include "input.h"


void input_init(input_t *i, screen_t *s)
{
    i->s = s;

    i->m.pos.x = 0;
    i->m.pos.y = 0;
    i->m.ppos = i->m.pos;
}

void input_handle(input_t *i)
{
    SDL_Event ev;

    while(SDL_PollEvent(&ev))
    {
        if(ev.type == SDL_WINDOWEVENT)
        {
            switch (ev.window.event)
            {
            case SDL_WINDOWEVENT_CLOSE:
                i->s->close = true;
                break;
            }
        }

        if(ev.type == SDL_MOUSEMOTION)
        {
            i->m.ppos = i->m.pos;
            SDL_GetMouseState(&i->m.pos.x, &i->m.pos.y);
        }
    }

}
