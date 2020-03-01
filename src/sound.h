#ifndef SOUND_H_
#define SOUND_H_

#include "frite.h"
#include <math.h>

#define MAX_PHASE 2*M_PI

typedef struct
{
  int freq;
  float amplitude;
  double step;
  double current_phase;
  uint16_t maxval;
} tone_t;

void tone_init(tone_t *s, float freq);

void tri_wave(tone_t *t, int16_t *buffer, uint16_t period);
void sine_wave(tone_t *s, int16_t *buffer, uint16_t period);
void square_wave(tone_t *t, int16_t *buffer, uint16_t period);

float midi_to_freq(uint8_t note);

#endif  /* SOUND_H_ */
