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

    //Reset input events bitmask
    i->ev = 0;

    while(SDL_PollEvent(&ev))
    {
        if(ev.type == SDL_WINDOWEVENT) {
            switch (ev.window.event)
            {
            case SDL_WINDOWEVENT_CLOSE:
                i->s->close = true;
                break;
            }
        }

        if(ev.type == SDL_MOUSEMOTION) {
            i->ev |= INEVENT_MMOTION;
            i->m.ppos = i->m.pos;
            SDL_GetMouseState(&i->m.pos.x, &i->m.pos.y);
            SDL_GetRelativeMouseState(&i->m.dpos.x, &i->m.dpos.y);
        }

        if(ev.type == SDL_MOUSEBUTTONDOWN) {
            if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
                i->m.lPress = true;
                i->ev |= INEVENT_LDOWN;
            }
        }

        if(ev.type == SDL_MOUSEBUTTONUP) {
            if((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) == 0) {
                i->m.lPress = false;
                i->ev |= INEVENT_LUP;
            }
        }
    }
}
