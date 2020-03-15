#ifndef INPUT_H_
#define INPUT_H_

#include "screen.h"

typedef struct
{
    v2 pos;                     /* Current Mouse Location */
    v2 ppos;                    /* Previous Mouse Location */
    v2 dpos;                    /* Relative Mouse Location (Delta) */
    bool lPress;                /* Left Button is currenty pressed */
} mouse_t;

typedef enum
{
    INEVENT_LDOWN=0x01,
    INEVENT_LUP=0x02,
    INEVENT_MMOTION=0x04,

} input_events_t;

typedef struct
{
    screen_t *s;
    mouse_t m;
    uint8_t ev;                 /* Input events bitmask */
} input_t;

void input_init(input_t *i, screen_t *s);
void input_handle(input_t *i);

#endif  /* INPUT_H_ */
