#include "input.h"
#include "box.h"
#include "frite.h"

#if 0
#define dbg(s, ...) printf(s, __VA_ARGS__);
#else
#define dbg(s, ...)
#endif

void input_init(input_t *i, screen_t *s, frite_t *frite, bool midi_en)
{
    i->s = s;

    i->m.pos.x = 0;
    i->m.pos.y = 0;
    i->m.ppos = i->m.pos;
    i->frite = frite;

    if(midi_en)
        i->midi_en = true;
    else
        i->midi_en = false;
}

static inline void input_update_mouse(input_t *in, SDL_Event ev)
{

    if(ev.type == SDL_MOUSEMOTION) {
        in->ev |= INEVENT_MMOTION;
        in->m.ppos = in->m.pos;
        SDL_GetMouseState(&in->m.pos.x, &in->m.pos.y);
        SDL_GetRelativeMouseState(&in->m.dpos.x, &in->m.dpos.y);
    }

    if(ev.type == SDL_MOUSEBUTTONDOWN) {
        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            in->m.lPress = true;
            in->ev |= INEVENT_LDOWN;
        }

        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            in->m.lPress = true;
            in->ev |= INEVENT_RDOWN;
        }

    }

    if(ev.type == SDL_MOUSEBUTTONUP) {
        if((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) == 0) {
            in->m.lPress = false;
            in->ev |= INEVENT_LUP;
        }

        if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
            in->m.rPress = false;
            in->ev |= INEVENT_RUP;
        }
    }
}

static inline void input_update_keyboard(input_t *in, SDL_Event ev)
{
    const uint8_t *state;

    if(ev.type == SDL_KEYDOWN) {
        in->ev |= INEVENT_KDOWN;
        dbg("DOWN: %hhx\n", ev.key.keysym.sym);
        in->key = ev.key.keysym.sym;

        if(ev.key.repeat == false && in->midi_en) {
            in->midi_ev->press_on(in->midi_ev, ev.key.keysym.sym, 0x00);
            in->ev |= INEVENT_MIDI_DOWN;
        }
    }

    if(ev.type == SDL_KEYUP) {
        in->ev |= INEVENT_KUP;
        dbg("UP: %c\n", ev.key.keysym.sym);

        if(ev.key.repeat == false && in->midi_en) {
            in->midi_ev->press_off(in->midi_ev, ev.key.keysym.sym, 0x00);
            in->ev |= INEVENT_MIDI_UP;
        }
    }
}

void input_update(input_t *in)
{
    SDL_Event ev;

    //Reset input events bitmask
    in->ev = 0;

    while(SDL_PollEvent(&ev))
    {
        input_update_mouse(in, ev);
        input_update_keyboard(in, ev);

        if(ev.type == SDL_WINDOWEVENT) {
            switch (ev.window.event)
            {
            case SDL_WINDOWEVENT_CLOSE:
                in->s->close = true;
                break;
            }
        }
    }

    if(!in->midi_en)
        in->ev |= frite_read(in->frite);
}

bool input_check_sel(v2 mpos, v2 pos, v2 size)
{
    bound_t box;

    box = box_bound_get(pos, size);

    if(mpos.y >= box.t && mpos.y <= box.b && mpos.x >= box.l && mpos.x <= box.r)
        return true;

    return false;
}
