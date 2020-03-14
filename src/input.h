#ifndef INPUT_H_
#define INPUT_H_

#include "screen.h"

typedef struct
{
  screen_t *s;

} input_t;

void input_init(input_t *i, screen_t *s);
void input_handle(input_t *i);

#endif  /* INPUT_H_ */
