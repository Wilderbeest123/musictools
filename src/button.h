#ifndef BUTTON_H_
#define BUTTON_H_

#include "screen.h"
#include "shapes.h"

typedef struct
{
    v2 pos;
    v2 size;
    unsigned int tid;           /* ID of the texture to render */

    void (*fn)(void *);          /* Function to call when button pressed */
    void *data;                 /* Data to pass when function called */

} button_t;

#define BUTTON_INIT(X,Y,W,H) { .pos={.x=X, .y=Y}, .size={.x=W, .y=H}, .tid=0 }

void button_draw(button_t *b);

#endif  /* BUTTON_H_ */
