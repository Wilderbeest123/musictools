#ifndef INPUT_H_
#define INPUT_H_

#include "screen.h"

typedef struct
{
    v2 pos;                     /* Current Mouse Location */
    v2 ppos;                    /* Previous Mouse Location */
} mouse_t;

typedef struct
{
    screen_t *s;
    mouse_t m;

} input_t;

void input_init(input_t *i, screen_t *s);
void input_handle(input_t *i);

#endif  /* INPUT_H_ */
