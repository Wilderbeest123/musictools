#ifndef WFORM_H_
#define WFORM_H_

#include "shapes.h"

typedef struct
{
  gl_model_t m;
  int p_num;
  int p_size;
  int p_idx;
  float x_step;
  int maxval;
  GLfloat *vpos;
} waveform_t;

waveform_t* waveform_init(int p_num, int bsize);
void waveform_update(waveform_t *w, int16_t *buf, uint16_t period);

#endif  /* WFORM_H_ */

