#ifndef INPUT_H_
#define INPUT_H_

#include "screen.h"
#include "frite.h"

typedef struct
{
    v2 pos;                     /* Current Mouse Location */
    v2 ppos;                    /* Previous Mouse Location */
    v2 dpos;                    /* Relative Mouse Location (Delta) */
    bool lPress;                /* Left Button is currenty pressed */
    bool rPress;                /* Right Button is currenty pressed */
} mouse_t;

typedef enum
{
    INEVENT_LDOWN=0x01,
    INEVENT_LUP=0x02,
    INEVENT_MMOTION=0x04,
    INEVENT_RDOWN=0x08,
    INEVENT_RUP=0x16,

} input_events_t;

typedef struct
{
    screen_t *s;
    mouse_t m;
    uint8_t ev;                 /* Input events bitmask */
    midi_events_t *midi_ev;
    bool midi_en;
} input_t;

void input_init(input_t *i, screen_t *s, bool midi_en);
void input_update(input_t *i);

bool input_check_sel(v2 mpos, v2 pos, v2 size);

#endif  /* INPUT_H_ */
