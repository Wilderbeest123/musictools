#ifndef BUTTON_H_
#define BUTTON_H_

#include "screen.h"
#include "shapes.h"

typedef struct
{
    v2 pos;                     /* position of button relative to parent (parent can be screen). */
    v2 size;
    unsigned int tid;           /* ID of the texture to render */

    void (*fn)(void *, uint8_t);          /* Function to call when button pressed */
    void *data;                  /* Data to pass when function called */

} button_t;

#define BUTTON_INIT(X,Y,W,H) { .pos={.x=X, .y=Y}, .size={.x=W, .y=H}, .tid=0 }

void button_draw(button_t *b);
void button_draw_rel(button_t *b, v2 pos);

#endif  /* BUTTON_H_ */
