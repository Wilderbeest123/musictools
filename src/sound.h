#ifndef SOUND_H_
#define SOUND_H_

#include "frite.h"
#include <math.h>

#define MAX_PHASE 2*M_PI

typedef struct
{
  bool enabled;
  int freq;
  float amplitude;
  double step;
  double current_phase;
  uint16_t maxval;
} sinewave_t;

void sine_init(sinewave_t *s, float freq);
void sine_wave(sinewave_t *s, int16_t *buffer, uint16_t period);
void mix_output(int16_t *out, int16_t *b1, int16_t *b2, uint16_t period);

float midi_to_freq(uint8_t note);

#endif  /* SOUND_H_ */
